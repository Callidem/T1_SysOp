#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define MAX_PROCESSOS 64
#define QUANTUM 5

static bool systemExit    = false;
static bool schedulerOn   = false;
static int  schedulerIndex = 0;

static CPU *sharedCpu = NULL;
static GM  *sharedGm  = NULL;

static PCB *processos[MAX_PROCESSOS] = {0};
static int  totalProcessos = 0;

static const char *state_name(int s);
static PCB *pick_next_ready(void);
static bool has_active_processes(void);
static void run_one_quantum(void);

void scheduler_init(CPU *cpu, GM *gm) {
    sharedCpu = cpu;
    sharedGm = gm;
}

int scheduler_add(PCB *pcb) {
    if (totalProcessos >= MAX_PROCESSOS) return -1;
    processos[totalProcessos++] = pcb;
    return totalProcessos - 1;
}

PCB *scheduler_find_pcb(int pid) {
    for (int i = 0; i < totalProcessos; ++i)
        if (processos[i] && processos[i]->pid == pid) return processos[i];
    return NULL;
}

int scheduler_find_idx(int pid) {
    for (int i = 0; i < totalProcessos; ++i)
        if (processos[i] && processos[i]->pid == pid) return i;
    return -1;
}

void scheduler_remove_at_idx(int index) {
    if (index < 0 || index >= totalProcessos) return;
    desalocaProcesso(processos[index], sharedGm);
    for (int i = index; i < totalProcessos - 1; ++i)
        processos[i] = processos[i + 1];
    processos[--totalProcessos] = NULL;
    if (schedulerIndex >= totalProcessos && totalProcessos > 0)
        schedulerIndex = 0;
}

void scheduler_mark_ready_pid(int pid) {
    PCB *p = scheduler_find_pcb(pid);
    if (!p) return;
    p->state = READY;
}

int scheduler_count(void) { return totalProcessos; }
PCB *scheduler_get(int index) { if (index < 0 || index >= totalProcessos) return NULL; return processos[index]; }

void scheduler_dump_pcb(const PCB *pcb) {
    if (!pcb) return;
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

void scheduler_dump_process_memory(const PCB *pcb) {
    if (!pcb) return;
    for (int i = 0; i < pcb->numPages; ++i) {
        int frame = pcb->pageTable[i];
        if (frame < 0) continue;
        int ini = frame * sharedGm->tamPg;
        int fim = ini + sharedGm->tamPg;
        printf("  pagina %d -> frame %d  (end. fisicos %d..%d)\n", i, frame, ini, fim-1);
        utils_dump_mem(sharedCpu->mem, ini, fim);
    }
}

// internal helpers
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

static PCB *pick_next_ready(void) {
    for (int a = 0; a < totalProcessos; ++a) {
        int idx = (schedulerIndex + a) % totalProcessos;
        PCB *p = processos[idx];
        if (p && p->state == READY) {
            schedulerIndex = (idx + 1) % totalProcessos;
            return p;
        }
    }
    return NULL;
}

static bool has_active_processes(void) {
    for (int i = 0; i < totalProcessos; ++i) {
        if (processos[i] && (processos[i]->state == READY || processos[i]->state == RUNNING))
            return true;
    }
    return false;
}

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

    cpu_set_page_table(sharedCpu, pcb->pageTable, pcb->numPages);
    recup_contexto(sharedCpu, pcb);
    sharedCpu->cpuStop = 0;

    printf("\n[sch] PID %d (%s) executando (quantum %d)\n",
           pid, pcb->prog ? pcb->prog->name : "?", QUANTUM);
    cpu_run_steps(sharedCpu, QUANTUM);

    PCB *cur = scheduler_find_pcb(pid);
    if (!cur) return;

    bool terminou = cur->killRequested
                 || sharedCpu->cpuStop
                 || sharedCpu->irpt != noInterrupt
                 || sharedCpu->ir.opc == STOP;

    if (terminou) {
        printf("[sch] PID %d terminou\n", pid);
        int idx = scheduler_find_idx(pid);
        if (idx >= 0) scheduler_remove_at_idx(idx);
        if (!has_active_processes()) schedulerOn = false;
    } else {
        salva_contexto(sharedCpu, cur);
        cur->state = READY;
    }
}

void scheduler_run_blocking(void) {
    schedulerOn = true;
    while (schedulerOn && !systemExit) run_one_quantum();
}
