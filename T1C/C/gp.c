#include "gp.h"


PCB *criaProcesso(Program *p, GM *gm){
    PCB *pcb = (PCB *)malloc(sizeof(PCB));
    if (!pcb) return NULL;
    pcb->prog = p;
    static int next_pid = 1;
    pcb->pid = next_pid++; // PID unico
    pcb->pc = 0;
    pcb->pageTable = NULL;
    pcb->numPages = 0;
    for (int i = 0; i < 10; ++i) pcb->regs[i] = 0;
    pcb->state = NEW;
    pcb->killRequested = 0;

    if (!aloca(p->size, &pcb->pageTable, &pcb->numPages, gm)) {
        printf("criaProcesso: falha alocar paginas para o processo\n");
        free(pcb);
        return NULL;
    }

    pcb->state = READY;

    // adicionar PCB à fila de processos prontos (ready queue)
    // ...

    return pcb;
}

bool desalocaProcesso(PCB *pcb, GM *gm){
    if (!pcb) return false;
    desaloca(pcb->pageTable, pcb->numPages, gm);
    // remover PCB da fila de processos prontos
    // ...
    free(pcb->pageTable);
    free(pcb);
    return true;
}

void salva_contexto(CPU *cpu, PCB *pcb) {
    if (!cpu || !pcb) return;
    pcb->pc = cpu->pc;
    for (int i = 0; i < 10; ++i) pcb->regs[i] = cpu->reg[i];
    pcb->state = READY;
}

void recup_contexto(CPU *cpu, PCB *pcb) {
    if (!cpu || !pcb) return;
    cpu->pc = pcb->pc;
    for (int i = 0; i < 10; ++i) cpu->reg[i] = pcb->regs[i];
    cpu->irpt = noInterrupt;
    pcb->state = RUNNING;
}