// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
// Código fornecido como parte da solução do projeto de Sistemas Operacionais
//
// -----  P O N T O   D E   E N T R A D A   D O   S I S T E M A  -----
// Este programa instancia o Sistema com todos os elementos mencionados:
//    HW: Memória e CPU (virtualiza o hardware)
//    SO: Tratadores de interrupções e chamadas de sistema
// Em seguida solicita a execução de algum programa com load_and_exec

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware.h"
#include "so.h"
#include "programs.h"
#include "utils.h"
#include "gp.h"

#define MAX_PROCESSOS 64
#define QUANTUM 5

static pthread_mutex_t processMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t processCond = PTHREAD_COND_INITIALIZER;
static bool systemExit = false;
static bool execAllRequested = false;
static int schedulerIndex = 0;
static int currentRunningPid = -1;

static CPU *sharedCpu = NULL;
static GM *sharedGm = NULL;

static PCB *processos[MAX_PROCESSOS] = {0};
static int totalProcessos = 0;

static const char *pcb_state_name(int state) {
    switch (state) {
        case NEW: return "NEW";
        case READY: return "READY";
        case RUNNING: return "RUNNING";
        case BLOCKED: return "BLOCKED";
        case TERMINATED: return "TERMINATED";
        default: return "UNKNOWN";
    }
}

static PCB *find_process(PCB **processos, int total, int pid) {
    for (int i = 0; i < total; ++i) {
        if (processos[i] && processos[i]->pid == pid) {
            return processos[i];
        }
    }
    return NULL;
}

static int find_process_index(PCB **processos, int total, int pid) {
    for (int i = 0; i < total; ++i) {
        if (processos[i] && processos[i]->pid == pid) {
            return i;
        }
    }
    return -1;
}

static void dump_pcb(const PCB *pcb) {
    if (!pcb) return;
    printf("PID: %d\n", pcb->pid);
    printf("Programa: %s\n", pcb->prog ? pcb->prog->name : "<sem programa>");
    printf("Estado: %s\n", pcb_state_name(pcb->state));
    printf("PC: %d\n", pcb->pc);
    printf("Paginas: %d\n", pcb->numPages);
    printf("Tabela de paginas:\n");
    for (int i = 0; i < pcb->numPages; ++i) {
        printf("  [%d] = %d\n", i, pcb->pageTable[i]);
    }
    printf("Registradores:\n");
    for (int i = 0; i < 10; ++i) {
        printf("  r[%d] = %d\n", i, pcb->regs[i]);
    }
}

static void dump_process_memory(const PCB *pcb, const GM *gm, const Memory *mem) {
    if (!pcb || !gm || !mem) return;
    for (int i = 0; i < pcb->numPages; ++i) {
        int frame = pcb->pageTable[i];
        if (frame < 0) continue;
        int ini = frame * gm->tamPg;
        int fim = ini + gm->tamPg;
        printf("Memoria do processo PID %d - pagina %d (frame %d, enderecos %d..%d)\n",
               pcb->pid, i, frame, ini, fim - 1);
        utils_dump_mem(mem, ini, fim);
    }
}

static void remove_process_at(PCB **processos, int *total, int index, GM *gm) {
    if (index < 0 || index >= *total) return;
    desalocaProcesso(processos[index], gm);
    for (int i = index; i < *total - 1; ++i) {
        processos[i] = processos[i + 1];
    }
    processos[*total - 1] = NULL;
    (*total)--;
}

static int find_process_index_locked(PCB **processos, int total, int pid) {
    for (int i = 0; i < total; ++i) {
        if (processos[i] && processos[i]->pid == pid) {
            return i;
        }
    }
    return -1;
}

static PCB *pick_next_ready_locked(void) {
    if (totalProcessos == 0) {
        return NULL;
    }

    for (int attempts = 0; attempts < totalProcessos; ++attempts) {
        int index = (schedulerIndex + attempts) % totalProcessos;
        PCB *pcb = processos[index];
        if (!pcb) continue;
        if (pcb->state == READY) {
            schedulerIndex = (index + 1) % totalProcessos;
            return pcb;
        }
    }

    return NULL;
}

static void *scheduler_thread(void *arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&processMutex);
        while (!systemExit && totalProcessos == 0) {
            pthread_cond_wait(&processCond, &processMutex);
        }
        if (systemExit) {
            pthread_mutex_unlock(&processMutex);
            break;
        }

        PCB *pcb = pick_next_ready_locked();
        if (!pcb) {
            pthread_cond_wait(&processCond, &processMutex);
            pthread_mutex_unlock(&processMutex);
            continue;
        }

        pcb->state = RUNNING;
        currentRunningPid = pcb->pid;
        pthread_mutex_unlock(&processMutex);

        cpu_set_page_table(sharedCpu, pcb->pageTable, pcb->numPages);
        recup_contexto(sharedCpu, pcb);
        sharedCpu->cpuStop = 0;
        printf("\n[sch] PID %d executando por quantum %d\n", pcb->pid, QUANTUM);
        cpu_run_steps(sharedCpu, QUANTUM);

        pthread_mutex_lock(&processMutex);
        currentRunningPid = -1;

        if (pcb->killRequested) {
            pcb->state = TERMINATED;
        } else if (sharedCpu->irpt != noInterrupt || sharedCpu->ir.opc == STOP || sharedCpu->cpuStop) {
            pcb->state = TERMINATED;
        } else {
            salva_contexto(sharedCpu, pcb);
            pcb->state = READY;
        }

        if (pcb->state == TERMINATED) {
            int idx = find_process_index_locked(processos, totalProcessos, pcb->pid);
            if (idx >= 0) {
                remove_process_at(processos, &totalProcessos, idx, sharedGm);
                if (schedulerIndex >= totalProcessos) {
                    schedulerIndex = 0;
                }
            }
        }

        if (execAllRequested && totalProcessos == 0) {
            execAllRequested = false;
        }

        pthread_mutex_unlock(&processMutex);
    }
    return NULL;
}

int main(void) {
    CPU cpu;
    Memory mem;
    GM gm;
    // -----  I N I C I A L I Z A Ç Ã O   D O   S I S T E M A  -----
    memory_init(&mem, 1024);      // Cria memória com 1024 palavras
    cpu_init(&cpu, &mem, 0);      // Cria CPU com debug desligado por padrao
    gm_init(&gm, &mem, 8);             // Inicializa gerente de memória (tamanho de pagina padrão 8)
    // Registra as funções de tratamento de interrupções e syscalls do SO
    cpu_set_handlers(&cpu, so_interrupt_handle, so_syscall_handle, so_syscall_stop);
    cpu_set_gm(&cpu, &gm); // associa gerenciador de memória à CPU

    sharedCpu = &cpu;
    sharedGm = &gm;

    pthread_t scheduler;
    if (pthread_create(&scheduler, NULL, scheduler_thread, NULL) != 0) {
        printf("Falha ao criar thread de escalonamento\n");
        memory_free(&mem);
        return 1;
    }

    printf("Comandos: new <prog>, rm <id>, ps, dump <id>, dumpM <ini> <fim>, exec <id>, execAll, traceOn, traceOff, exit\n");

    char linha[256];
    while (1) {
        printf("so> ");
        if (!fgets(linha, sizeof(linha), stdin)) {
            break;
        }

        linha[strcspn(linha, "\n")] = '\0';
        if (linha[0] == '\0') continue;

        if (strncmp(linha, "new ", 4) == 0) {
            char nomePrograma[128];
            if (sscanf(linha + 4, "%127s", nomePrograma) != 1) {
                printf("Uso: new <nomeDePrograma>\n");
                continue;
            }
            if (totalProcessos >= MAX_PROCESSOS) {
                printf("Limite de processos atingido\n");
                continue;
            }

            const Program *prog = retrieve_program(nomePrograma);
            if (!prog) {
                printf("Programa nao encontrado: %s\n", nomePrograma);
                continue;
            }

            PCB *pcb = criaProcesso((Program *)prog, &gm);
            if (!pcb) {
                printf("Falha ao criar processo\n");
                continue;
            }

            pthread_mutex_lock(&processMutex);
            if (totalProcessos >= MAX_PROCESSOS) {
                pthread_mutex_unlock(&processMutex);
                desalocaProcesso(pcb, &gm);
                printf("Limite de processos atingido\n");
                continue;
            }
            utils_load_program(&mem, &gm, pcb->pageTable, pcb->numPages, prog->size, prog->image);
            processos[totalProcessos++] = pcb;
            pthread_cond_signal(&processCond);
            pthread_mutex_unlock(&processMutex);
            printf("Processo criado com PID %d\n", pcb->pid);
            continue;
        }

        if (strcmp(linha, "ps") == 0) {
            pthread_mutex_lock(&processMutex);
            if (totalProcessos == 0) {
                pthread_mutex_unlock(&processMutex);
                printf("Nenhum processo\n");
                continue;
            }
            for (int i = 0; i < totalProcessos; ++i) {
                PCB *pcb = processos[i];
                printf("PID %d | %s | PC %d | prog %s\n",
                       pcb->pid,
                       pcb_state_name(pcb->state),
                       pcb->pc,
                       pcb->prog ? pcb->prog->name : "<sem programa>");
            }
            pthread_mutex_unlock(&processMutex);
            continue;
        }

        if (strncmp(linha, "dumpM", 5) == 0) {
            int ini, fim;
            if (sscanf(linha + 5, "%d %d", &ini, &fim) != 2) {
                if (sscanf(linha + 5, "%d,%d", &ini, &fim) != 2) {
                    printf("Uso: dumpM <inicio> <fim>\n");
                    continue;
                }
            }
            if (ini < 0) ini = 0;
            if (fim < ini) {
                printf("Intervalo invalido\n");
                continue;
            }
            pthread_mutex_lock(&processMutex);
            utils_dump_mem(&mem, ini, fim + 1);
            pthread_mutex_unlock(&processMutex);
            continue;
        }

        if (strncmp(linha, "dump ", 5) == 0) {
            int pid;
            if (sscanf(linha + 5, "%d", &pid) != 1) {
                printf("Uso: dump <id>\n");
                continue;
            }
            pthread_mutex_lock(&processMutex);
            PCB *pcb = find_process(processos, totalProcessos, pid);
            if (!pcb) {
                pthread_mutex_unlock(&processMutex);
                printf("Processo %d nao encontrado\n", pid);
                continue;
            }
            dump_pcb(pcb);
            dump_process_memory(pcb, &gm, &mem);
            pthread_mutex_unlock(&processMutex);
            continue;
        }

        if (strncmp(linha, "rm ", 3) == 0) {
            int pid;
            if (sscanf(linha + 3, "%d", &pid) != 1) {
                printf("Uso: rm <id>\n");
                continue;
            }
            pthread_mutex_lock(&processMutex);
            int index = find_process_index(processos, totalProcessos, pid);
            if (index < 0) {
                pthread_mutex_unlock(&processMutex);
                printf("Processo %d nao encontrado\n", pid);
                continue;
            }
            PCB *pcb = processos[index];
            if (pcb->state == RUNNING || currentRunningPid == pid) {
                pcb->killRequested = 1;
                pcb->state = TERMINATED;
                pthread_cond_signal(&processCond);
                pthread_mutex_unlock(&processMutex);
                printf("Processo %d marcado para remoção\n", pid);
                continue;
            }
            remove_process_at(processos, &totalProcessos, index, &gm);
            pthread_mutex_unlock(&processMutex);
            printf("Processo %d removido\n", pid);
            continue;
        }

        if (strncmp(linha, "exec ", 5) == 0) {
            int pid;
            if (sscanf(linha + 5, "%d", &pid) != 1) {
                printf("Uso: exec <id>\n");
                continue;
            }
            pthread_mutex_lock(&processMutex);
            PCB *pcb = find_process(processos, totalProcessos, pid);
            if (!pcb) {
                pthread_mutex_unlock(&processMutex);
                printf("Processo %d nao encontrado\n", pid);
                continue;
            }
            if (pcb->state == TERMINATED) {
                pthread_mutex_unlock(&processMutex);
                printf("Processo %d ja terminou\n", pid);
                continue;
            }
            pcb->state = READY;
            schedulerIndex = find_process_index_locked(processos, totalProcessos, pid);
            pthread_cond_signal(&processCond);
            pthread_mutex_unlock(&processMutex);
            printf("Processo %d agendado\n", pid);
            continue;
        }

        if (strcmp(linha, "execAll") == 0) {
            pthread_mutex_lock(&processMutex);
            execAllRequested = true;
            schedulerIndex = 0;
            pthread_cond_signal(&processCond);
            pthread_mutex_unlock(&processMutex);
            printf("ExecAll ativado\n");
            continue;
        }

        if (strcmp(linha, "traceOn") == 0) {
            pthread_mutex_lock(&processMutex);
            cpu.debug = 1;
            pthread_mutex_unlock(&processMutex);
            printf("Trace ligado\n");
            continue;
        }

        if (strcmp(linha, "traceOff") == 0) {
            pthread_mutex_lock(&processMutex);
            cpu.debug = 0;
            pthread_mutex_unlock(&processMutex);
            printf("Trace desligado\n");
            continue;
        }

        if (strcmp(linha, "exit") == 0) {
            pthread_mutex_lock(&processMutex);
            systemExit = true;
            pthread_cond_broadcast(&processCond);
            pthread_mutex_unlock(&processMutex);
            break;
        }

        printf("Comando desconhecido: %s\n", linha);
    }

    pthread_join(scheduler, NULL);

    pthread_mutex_lock(&processMutex);
    for (int i = 0; i < totalProcessos; ++i) {
        desalocaProcesso(processos[i], &gm);
        processos[i] = NULL;
    }
    totalProcessos = 0;
    pthread_mutex_unlock(&processMutex);

    memory_free(&mem);
    return 0;
}
