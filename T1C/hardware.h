#ifndef HARDWARE_H
#define HARDWARE_H

#include <stddef.h>

// Mesmos opcodes da VM em Java

typedef enum {
    DATA, ___,              // dado ou posicao nao usada
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

// Declaracao adiantada
struct CPU;
typedef struct CPU CPU;

// Ponteiros de funcao para tratadores
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

    InterruptHandler ih;
    SysCallIOHandler sysCallIO;
    SysCallStopHandler sysCallStop;

    int debug;
    int cpuStop;
};

// Interface de hardware
void memory_init(Memory *mem, size_t size);
void memory_free(Memory *mem);

void cpu_init(CPU *cpu, Memory *mem, int debug);
void cpu_set_handlers(CPU *cpu,
                      InterruptHandler ih,
                      SysCallIOHandler sysIO,
                      SysCallStopHandler sysStop);
void cpu_set_context(CPU *cpu, int pc);
void cpu_run(CPU *cpu);

// Nome textual de um opcode (para debug/dump)
const char *opcode_name(Opcode op);

#endif // HARDWARE_H
