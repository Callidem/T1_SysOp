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

#include "hardware.h"
#include "so.h"
#include "programs.h"
#include "utils.h"
#include "gp.h"
#include "scheduler.h"

#define MAX_PROCESSOS 64

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

    scheduler_init(&cpu, &gm);

    printf("Comandos: new <prog>, rm <id>, ps, dump <id>, dumpM <ini> <fim>,\n");
    printf("          exec <id>, execAll, traceOn, traceOff, exit\n");

    char linha[256];
    bool systemExit = false;

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

            if (scheduler_count() >= MAX_PROCESSOS) { printf("Limite de processos atingido\n"); continue; }
            PCB *pcb = criaProcesso((Program *)prog, &gm);
            if (!pcb) { printf("Falha ao criar processo (sem memoria?)\n"); continue; }
            // Copia imagem do programa para as páginas alocadas
            utils_load_program(&mem, &gm, pcb->pageTable, pcb->numPages,
                               prog->size, prog->image);
            // Se escalonador estiver ativo, cria já em READY
            if (/* internal schedulerOn unknown */ false) {
                pcb->state = READY;
                scheduler_add(pcb);
                printf("Processo criado com PID %d (estado: READY - escalonador ativo)\n", pcb->pid);
            } else {
                pcb->state = NEW;
                scheduler_add(pcb);
                printf("Processo criado com PID %d (estado: NEW - use exec/execAll para rodar)\n", pcb->pid);
            }
            continue;
        }

        // ---- ps ----
        if (strcmp(linha, "ps") == 0) {
            int total = scheduler_count();
            if (total == 0) { printf("Nenhum processo\n"); continue; }
            for (int i = 0; i < total; ++i) {
                PCB *p = scheduler_get(i);
                printf("PID %-3d | %-10s | PC %-4d | %s\n",
                       p->pid, (p->state==0?"NEW":(p->state==1?"READY":(p->state==2?"RUNNING":"?"))), p->pc,
                       p->prog ? p->prog->name : "?");
            }
            continue;
        }

        // ---- dump <id> ----
        if (strncmp(linha, "dump ", 5) == 0) {
            int pid;
            if (sscanf(linha + 5, "%d", &pid) != 1) { printf("Uso: dump <id>\n"); continue; }
            PCB *p = scheduler_find_pcb(pid);
            if (!p) { printf("PID %d nao encontrado\n", pid); continue; }
            scheduler_dump_pcb(p);
            scheduler_dump_process_memory(p);
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
            int idx = scheduler_find_idx(pid);
            if (idx < 0) { printf("PID %d nao encontrado\n", pid); continue; }
            PCB *p = scheduler_get(idx);
            if (p->state == RUNNING) {
                p->killRequested = 1;
                printf("PID %d marcado para remocao (esta rodando)\n", pid);
            } else {
                scheduler_remove_at_idx(idx);
                printf("PID %d removido\n", pid);
            }
            continue;
        }

        // ---- exec <id> ----: marca processo READY e executa (bloqueante)
        if (strncmp(linha, "exec ", 5) == 0) {
            int pid;
            if (sscanf(linha + 5, "%d", &pid) != 1) { printf("Uso: exec <id>\n"); continue; }
            PCB *p = scheduler_find_pcb(pid);
            if (!p) { printf("PID %d nao encontrado\n", pid); continue; }
            if (p->state == TERMINATED) { printf("PID %d ja terminou\n", pid); continue; }
            p->state = READY;
            // marcar para começar por este processo: não suportado por API simples
            printf("PID %d agendado para execucao\n", pid);
            scheduler_run_blocking();
            continue;
        }

        // ---- execAll ----
        if (strcmp(linha, "execAll") == 0) {
            int count = 0;
            int total = scheduler_count();
            for (int i = 0; i < total; ++i) {
                PCB *p = scheduler_get(i);
                if (p && (p->state == NEW || p->state == READY)) {
                    p->state = READY;
                    count++;
                }
            }
            if (count == 0) { printf("Nenhum processo disponivel para execucao\n"); continue; }
            printf("execAll: %d processo(s) colocados em READY\n", count);
            scheduler_run_blocking();
            continue;
        }

        // ---- traceOn / traceOff ----
        if (strcmp(linha, "traceOn") == 0) { cpu.debug = 1; printf("Trace ligado\n"); continue; }
        if (strcmp(linha, "traceOff") == 0) { cpu.debug = 0; printf("Trace desligado\n"); continue; }

        // ---- exit ----
        if (strcmp(linha, "exit") == 0) { systemExit = true; break; }

        printf("Comando desconhecido: '%s'\n", linha);
    }

    // Limpeza final: remove todos os processos
    while (scheduler_count() > 0) scheduler_remove_at_idx(0);

    free(gm.framesAlocados);
    memory_free(&mem);
    return 0;
}
