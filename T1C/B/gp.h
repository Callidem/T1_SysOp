#include "programs.h"
#include "gm.h"
#include "hardware.h"

typedef struct {
    Program *prog;
    int *pageTable; // tabela de paginas para o programa (preenchida por aloca)    
    int pid;
    int pc;
    int numPages;
    int regs[10];
    enum { NEW, READY, RUNNING, BLOCKED, TERMINATED } state;
    //outros registradores
    
} PCB;

PCB *criaProcesso(Program *p, GM *gm);
bool desalocaProcesso(PCB *pcb, GM *gm);


void salva_contexto(CPU *cpu, PCB *pcb);
void recup_contexto(CPU *cpu, PCB *pcb);
