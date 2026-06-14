// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
// Código fornecido como parte da solução do projeto de Sistemas Operacionais

#ifndef HARDWARE_H
#define HARDWARE_H

#include <stddef.h>
#include <stdbool.h>

typedef enum {
    DATA, ___,
    JMP, JMPI, JMPIG, JMPIL, JMPIE,
    JMPIM, JMPIGM, JMPILM, JMPIEM,
    JMPIGK, JMPILK, JMPIEK, JMPIGT,
    ADDI, SUBI, ADD, SUB, MULT,
    LDI, LDD, STD, LDX, STX, MOVE,
    SYSCALL, STOP
} Opcode;

typedef enum {
    noInterrupt,
    intEnderecoInvalido,
    intInstrucaoInvalida,
    intOverflow
} Interrupts;

typedef struct {
    Opcode opc;
    int ra;
    int rb;
    int p;
} Word;

typedef struct {
    Word *pos;
    size_t size;
} Memory;

struct CPU;
typedef struct CPU CPU;

typedef void (*InterruptHandler)(CPU *cpu, Interrupts irpt);
typedef void (*SysCallIOHandler)(CPU *cpu);
typedef void (*SysCallStopHandler)(CPU *cpu);

struct CPU {
    int maxInt;
    int minInt;

    int pc;
    Word ir;
    int reg[10];
    Interrupts irpt;

    Memory *mem;
    struct GM *gm;
    int *pageTable;
    int pageTableSize;

    InterruptHandler ih;
    SysCallIOHandler sysCallIO;
    SysCallStopHandler sysCallStop;

    int debug;
    int cpuStop;
};

void memory_init(Memory *mem, size_t size);
void memory_free(Memory *mem);

void cpu_init(CPU *cpu, Memory *mem, int debug);
void cpu_set_handlers(CPU *cpu, InterruptHandler ih, SysCallIOHandler sysIO, SysCallStopHandler sysStop);
void cpu_set_context(CPU *cpu, int pc);
void cpu_run(CPU *cpu);
void cpu_set_gm(CPU *cpu, struct GM *gm);
void cpu_set_page_table(CPU *cpu, int *pageTable, int pageTableSize);
int cpu_run_steps(CPU *cpu, int steps);

const char *opcode_name(Opcode op);

#endif // HARDWARE_H
