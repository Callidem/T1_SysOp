#include "gp.h"

// Criação, desalocação e manipulação de contexto de processos (PCB)
PCB *criaProcesso(Program *p, GM *gm){
    PCB *pcb = (PCB *)malloc(sizeof(PCB));
    if (!pcb) return NULL;
    pcb->prog = p;
    // `next_pid` é estático para garantir PIDs únicos entre chamadas
    static int next_pid = 1;
    pcb->pid = next_pid++; // PID unico
    pcb->pc = 0;
    pcb->pageTable = NULL;
    pcb->numPages = 0;
    for (int i = 0; i < 10; ++i) pcb->regs[i] = 0;
    pcb->state = NEW;
    pcb->killRequested = 0;

    // Aloca páginas físicas para armazenar o programa
    if (!aloca(p->size, &pcb->pageTable, &pcb->numPages, gm)) {
        printf("criaProcesso: falha alocar paginas para o processo\n");
        free(pcb);
        return NULL;
    }

    // Após carregar páginas, o processo fica pronto para execução
    pcb->state = READY;

    return pcb;
}

// Remove um processo e libera suas páginas
bool desalocaProcesso(PCB *pcb, GM *gm){
    if (!pcb) return false;
    desaloca(pcb->pageTable, pcb->numPages, gm);
    free(pcb->pageTable);
    free(pcb);
    return true;
}

// Salva o contexto da CPU no PCB
void salva_contexto(CPU *cpu, PCB *pcb) {
    if (!cpu || !pcb) return;
    // Salva o contador de programa e os registradores gerais (r0..r9)
    pcb->pc = cpu->pc;
    for (int i = 0; i < 10; ++i) pcb->regs[i] = cpu->reg[i];
    pcb->state = READY;
}

// Restaura o contexto do PCB para a CPU
void recup_contexto(CPU *cpu, PCB *pcb) {
    if (!cpu || !pcb) return;
    // Restaura PC e registradores antes de executar o processo
    cpu->pc = pcb->pc;
    for (int i = 0; i < 10; ++i) cpu->reg[i] = pcb->regs[i];
    cpu->irpt = noInterrupt; // limpa interrupções pendentes
    pcb->state = RUNNING;
}
