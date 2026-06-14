// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
//
// T1C: SO com escalonamento Round-Robin
//
// Comportamento correto:
//   new <prog>   -> cria processo em estado NEW (não roda automaticamente)
//   exec <id>    -> coloca processo em READY, escalonador executa só ele
//   execAll      -> coloca TODOS os NEW/READY em READY, escalonador roda todos em round-robin
//   ps           -> lista processos
//   dump <id>    -> mostra PCB + memória do processo
//   dumpM <i> <f>-> mostra memória física entre i e f
//   rm <id>      -> remove processo
//   traceOn/Off  -> liga/desliga trace da CPU
//   exit         -> encerra

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// ---- Estado global compartilhado entre shell e escalonador cooperativo ----


// Flags de controle do shell/escalonador
static bool systemExit    = false;           // true quando shell deve terminar
static bool schedulerOn   = false;           // true enquanto o escalonador deve rodar
static int  schedulerIndex = 0;              // índice circular usado pelo RR

// Ponteiros compartilhados para CPU e gerenciador de memória
// usados pelo escalonador para configurar o contexto de execução
static CPU *sharedCpu = NULL;
static GM  *sharedGm  = NULL;

static PCB *processos[MAX_PROCESSOS] = {0};
static int  totalProcessos = 0;

// --------------------------------------------------------------------------
// Utilitários internos
// --------------------------------------------------------------------------

static const char *state_name(int s) {
    switch (s) {
        case NEW:        return "NEW";
        case READY:      return "READY";
        case RUNNING:    return "RUNNING";
        case BLOCKED:    return "BLOCKED";
        case TERMINATED: return "TERMINATED";
        default:         return "UNKNOWN";
    }
}

static PCB *find_pcb(int pid) {
    for (int i = 0; i < totalProcessos; ++i)
        if (processos[i] && processos[i]->pid == pid) return processos[i];
    return NULL;
}

static int find_idx(int pid) {
    for (int i = 0; i < totalProcessos; ++i)
        if (processos[i] && processos[i]->pid == pid) return i;
    return -1;
}

static void remove_at(int index) {
    // Remove processo no índice, desaloca recursos e compacta o vetor
    if (index < 0 || index >= totalProcessos) return;
    desalocaProcesso(processos[index], sharedGm);
    for (int i = index; i < totalProcessos - 1; ++i)
        processos[i] = processos[i + 1];
    processos[--totalProcessos] = NULL;
    if (schedulerIndex >= totalProcessos && totalProcessos > 0)
        schedulerIndex = 0;
}

static void dump_pcb(const PCB *pcb) {
    printf("PID: %d\n", pcb->pid);
    printf("Programa: %s\n", pcb->prog ? pcb->prog->name : "<sem programa>");
    printf("Estado: %s\n", state_name(pcb->state));
    printf("PC: %d\n", pcb->pc);
    printf("Paginas: %d\n", pcb->numPages);
    printf("Tabela de paginas:\n");
    for (int i = 0; i < pcb->numPages; ++i)
        printf("  [%d] = frame %d\n", i, pcb->pageTable[i]);
    printf("Registradores:\n");
    for (int i = 0; i < 10; ++i)
        printf("  r[%d] = %d\n", i, pcb->regs[i]);
}

static void dump_process_memory(const PCB *pcb) {
    for (int i = 0; i < pcb->numPages; ++i) {
        int frame = pcb->pageTable[i];
        if (frame < 0) continue;
        int ini = frame * sharedGm->tamPg;
        int fim = ini + sharedGm->tamPg;
        printf("  pagina %d -> frame %d  (end. fisicos %d..%d)\n", i, frame, ini, fim-1);
        utils_dump_mem(sharedCpu->mem, ini, fim);
    }
}

// --------------------------------------------------------------------------
// Seleciona o próximo processo em READY (Round‑Robin)
// --------------------------------------------------------------------------
static PCB *pick_next_ready(void) {
    // Varre circularmente a partir de `schedulerIndex` buscando um READY
    for (int a = 0; a < totalProcessos; ++a) {
        int idx = (schedulerIndex + a) % totalProcessos;
        PCB *p = processos[idx];
        if (p && p->state == READY) {
            // Atualiza `schedulerIndex` para começar da próxima posição
            schedulerIndex = (idx + 1) % totalProcessos;
            return p;
        }
    }
    return NULL;
}

// Verifica se ainda há algum processo READY ou RUNNING
static bool has_active_processes(void) {
    for (int i = 0; i < totalProcessos; ++i) {
        if (processos[i] && (processos[i]->state == READY || processos[i]->state == RUNNING))
            return true;
    }
    return false;
}

// --------------------------------------------------------------------------
// Executa um quantum para o próximo processo READY.
// Salva/recupera contexto e decide se o processo terminou ou volta a READY.
// --------------------------------------------------------------------------
static void run_one_quantum(void) {
    PCB *pcb = pick_next_ready();
    if (!pcb) {
        if (!has_active_processes()) {
            schedulerOn = false;
            printf("\n[sch] todos os processos terminaram\n");
        }
        return;
    }

    pcb->state = RUNNING;
    int pid = pcb->pid;

    // Configura CPU e executa quantum de instruções
    // Ajusta tabela de páginas para este processo (tradução de endereços)
    cpu_set_page_table(sharedCpu, pcb->pageTable, pcb->numPages);
    // Restaura PC/regs do PCB para a CPU antes de executar
    recup_contexto(sharedCpu, pcb);
    // Garante que a CPU não pare prematuramente
    sharedCpu->cpuStop = 0;

    //printf("\n[sch] PID %d (%s) executando (quantum %d)\n",
           pid, pcb->prog ? pcb->prog->name : "?", QUANTUM);
    // Executa até `QUANTUM` instruções (ou até STOP/IRQ)
    cpu_run_steps(sharedCpu, QUANTUM);

    // Processo pode ter sido removido enquanto executava
    PCB *cur = find_pcb(pid);
    if (!cur) return;

    // Decide se o processo terminou por: kill, parada voluntária, IRQ, ou STOP
    bool terminou = cur->killRequested
                 || sharedCpu->cpuStop
                 || sharedCpu->irpt != noInterrupt
                 || sharedCpu->ir.opc == STOP;

    if (terminou) {
        printf("[sch] PID %d terminou\n", pid);
        int idx = find_idx(pid);
        if (idx >= 0) remove_at(idx);
        if (!has_active_processes()) schedulerOn = false;
    } else {
        // Não terminou: salva contexto (PC + regs) e coloca em READY
        salva_contexto(sharedCpu, cur);
        cur->state = READY;
    }
}

// --------------------------------------------------------------------------
// Executa quantums repetidos enquanto houver processos READY.
// --------------------------------------------------------------------------
static void run_scheduler_blocking(void) {
    // Loop bloqueante: chama `run_one_quantum` repetidamente até
    // que não hajam mais processos ativos ou `systemExit` seja true.
    while (schedulerOn && !systemExit) run_one_quantum();
}

// --------------------------------------------------------------------------
// main — loop do shell; processa comandos e aciona o escalonador
// --------------------------------------------------------------------------
int main(void) {
    CPU    cpu;
    Memory mem;
    GM     gm;


    int tamPg, tamMem;
    printf("Insira o tamanho da pagina (em palavras): ");
    scanf("%d", &tamPg);
    printf("Insira o tamanho da memoria (em palavras): ");
    scanf("%d", &tamMem);

    memory_init(&mem, tamMem);
    cpu_init(&cpu, &mem, 0);
    gm_init(&gm, &mem, tamPg);
    cpu_set_handlers(&cpu, so_interrupt_handle, so_syscall_handle, so_syscall_stop);
    cpu_set_gm(&cpu, &gm);

    sharedCpu = &cpu;
    sharedGm  = &gm;

    printf("Comandos: new <prog>, rm <id>, ps, dump <id>, dumpM <ini> <fim>,\n");
    printf("          exec <id>, execAll, traceOn, traceOff, exit\n");

    char linha[256];

    while (!systemExit) {
        printf("so> ");
        fflush(stdout);
        if (!fgets(linha, sizeof(linha), stdin)) { systemExit = true; break; }
        linha[strcspn(linha, "\n")] = '\0';
        if (linha[0] == '\0') continue;
            // ---- new <prog> ----: cria PCB, aloca páginas e carrega o programa
            if (strncmp(linha, "new ", 4) == 0) {
                char nome[128];
                if (sscanf(linha + 4, "%127s", nome) != 1) { printf("Uso: new <nomeDePrograma>\n"); continue; }
                const Program *prog = retrieve_program(nome);
                if (!prog) { printf("Programa nao encontrado: %s\n", nome); continue; }

                if (totalProcessos >= MAX_PROCESSOS) { printf("Limite de processos atingido\n"); continue; }
                PCB *pcb = criaProcesso((Program *)prog, &gm);
                if (!pcb) { printf("Falha ao criar processo (sem memoria?)\n"); continue; }
                // Copia imagem do programa para as páginas alocadas
                utils_load_program(&mem, &gm, pcb->pageTable, pcb->numPages,
                                   prog->size, prog->image);
                // Se escalonador estiver ativo, cria já em READY
                if (schedulerOn) {
                    pcb->state = READY;
                    processos[totalProcessos++] = pcb;
                    printf("Processo criado com PID %d (estado: READY - escalonador ativo)\n", pcb->pid);
                } else {
                    pcb->state = NEW;
                    processos[totalProcessos++] = pcb;
                    printf("Processo criado com PID %d (estado: NEW - use exec/execAll para rodar)\n", pcb->pid);
                }
                continue;
            }

            // ---- ps ----
            if (strcmp(linha, "ps") == 0) {
                if (totalProcessos == 0) { printf("Nenhum processo\n"); continue; }
                for (int i = 0; i < totalProcessos; ++i) {
                    PCB *p = processos[i];
                    printf("PID %-3d | %-10s | PC %-4d | %s\n",
                           p->pid, state_name(p->state), p->pc,
                           p->prog ? p->prog->name : "?");
                }
                continue;
            }

            // ---- dump <id> ----
            if (strncmp(linha, "dump ", 5) == 0) {
                int pid;
                if (sscanf(linha + 5, "%d", &pid) != 1) { printf("Uso: dump <id>\n"); continue; }
                PCB *p = find_pcb(pid);
                if (!p) { printf("PID %d nao encontrado\n", pid); continue; }
                dump_pcb(p);
                dump_process_memory(p);
                continue;
            }

            // ---- dumpM <ini> <fim> ----
            if (strncmp(linha, "dumpM", 5) == 0) {
                int ini, fim;
                if (sscanf(linha + 5, "%d %d", &ini, &fim) != 2 &&
                    sscanf(linha + 5, "%d,%d", &ini, &fim) != 2) {
                    printf("Uso: dumpM <inicio> <fim>\n"); continue;
                }
                if (ini < 0) ini = 0;
                if (fim < ini) { printf("Intervalo invalido\n"); continue; }
                utils_dump_mem(&mem, ini, fim + 1);
                continue;
            }

            // ---- rm <id> ----
            if (strncmp(linha, "rm ", 3) == 0) {
                int pid;
                if (sscanf(linha + 3, "%d", &pid) != 1) { printf("Uso: rm <id>\n"); continue; }
                int idx = find_idx(pid);
                if (idx < 0) { printf("PID %d nao encontrado\n", pid); continue; }
                PCB *p = processos[idx];
                if (p->state == RUNNING) {
                    p->killRequested = 1;
                    printf("PID %d marcado para remocao (esta rodando)\n", pid);
                } else {
                    remove_at(idx);
                    printf("PID %d removido\n", pid);
                }
                continue;
            }
            // ---- exec <id> ----: marca processo READY e executa (bloqueante)
            if (strncmp(linha, "exec ", 5) == 0) {
                int pid;
                if (sscanf(linha + 5, "%d", &pid) != 1) { printf("Uso: exec <id>\n"); continue; }
                PCB *p = find_pcb(pid);
                if (!p) { printf("PID %d nao encontrado\n", pid); continue; }
                if (p->state == TERMINATED) { printf("PID %d ja terminou\n", pid); continue; }
                // Coloca em READY e indica ao escalonador para iniciar
                p->state = READY;
                schedulerOn = true;
                int idx = find_idx(pid);
                if (idx >= 0) schedulerIndex = idx; // começar por este processo
                printf("PID %d agendado para execucao\n", pid);
                // Bloqueia aqui até que o escalonador termine (sem threads)
                run_scheduler_blocking();
                continue;
            }

            // ---- execAll ----
            if (strcmp(linha, "execAll") == 0) {
                int count = 0;
                for (int i = 0; i < totalProcessos; ++i) {
                    PCB *p = processos[i];
                    if (p && (p->state == NEW || p->state == READY)) {
                        p->state = READY;
                        count++;
                    }
                }
                if (count == 0) { printf("Nenhum processo disponivel para execucao\n"); continue; }
                schedulerOn = true;
                schedulerIndex = 0; // começa do início da lista
                printf("execAll: %d processo(s) colocados em READY\n", count);
                run_scheduler_blocking();
                continue;
            }

            // ---- traceOn / traceOff ----
            if (strcmp(linha, "traceOn") == 0) { cpu.debug = 1; printf("Trace ligado\n"); continue; }
            if (strcmp(linha, "traceOff") == 0) { cpu.debug = 0; printf("Trace desligado\n"); continue; }

            // ---- exit ----
            if (strcmp(linha, "exit") == 0) { systemExit = true; break; }

            printf("Comando desconhecido: '%s'\n", linha);
    }

    // Limpeza final
    for (int i = 0; i < totalProcessos; ++i) {
        desalocaProcesso(processos[i], &gm);
        processos[i] = NULL;
    }
    totalProcessos = 0;

    free(gm.framesAlocados);
    memory_free(&mem);
    return 0;
}
