// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
// Código fornecido como parte da solução do projeto de Sistemas Operacionais
//
// Estrutura deste código:
//    Todo código está organizado em camadas:
//           Hardware: CPU, Memory, Word e Opcodes (definições de HW)
//           Software: SO com tratamento de Interrupcoes e SysCall Handling
//    CPU executa instruções carregadas em memória.
//    Memória armazena instruções e dados em Palavras (Word).
//    Interrupcoes podem ser geradas durante execução.
//    Chamadas de Sistema (SYSCALL) desviam para rotinas do SO.

#ifndef HARDWARE_H
#define HARDWARE_H

#include <stddef.h>
#include <stdbool.h>

// -----  M E M O R I A  -----
// Estrutura de Memória:
//    pos[i] é a posição i da memória. Cada posição armazena uma Word (instrução ou dado).

// -----  P A L A V R A   D E   M E M O R I A  -----
// Cada posição da memória contém uma Word com:
//    opc: código de operação (Opcode)
//    ra:  índice do primeiro registrador (Rs ou Rd)
//    rb:  índice do segundo registrador (Rs ou Rc)
//    p:   parâmetro para a instrução (k ou A), ou dado se opc = DATA

// -----  O P C O D E S   D A   C P U  -----
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

// -----  C O N T E X T O   D A   C P U  -----
// O contexto de um processo é composto de:
//    pc:    Program Counter - próxima instrução a executar
//    ir:    Instruction Register - instrução atualmente sendo executada
//    reg[]: Registradores da CPU (armazenam valores)
//    irpt:  Interrupção sinalizada durante a execução
// Tudo que é necessário para conhecer o estado de um processo para executá-lo
// Nas próximas versões isto pode modificar

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
    int maxInt;      // valores máximo e mínimo para inteiros nesta CPU
    int minInt;      // se exceder deve gerar interrupção de overflow

    int pc;          // Program Counter - próxima instrução a executar
    Word ir;         // Instruction Register - instrução atual
    int reg[10];     // Registradores da CPU (reg 8 e 9 usados para IO)
    Interrupts irpt; // Interrupção que possa ter sido sinalizada durante execução

    Memory *mem;     // Referência à memória física para acesso às instruções/dados
    struct GM *gm;   // Referência ao gerenciador de memória (para tradução lógica->fisica)
    int *pageTable;   // tabela de páginas do processo em execução
    int pageTableSize; // número de entradas na tabela

    // Ponteiros para rotinas de tratamento de interrupções e syscalls
    InterruptHandler ih;        // Desvia para tratamento de interrupções do SO
    SysCallIOHandler sysCallIO; // Trata chamadas de sistema (IO)
    SysCallStopHandler sysCallStop; // Trata parada do programa

    int debug;      // Se 1, mostra cada instrução em execução
    int cpuStop;    // Flag para parar a CPU
};

// -----  I N T E R F A C E   D E   H A R D W A R E  -----
// Funções de criação, inicialização e execução

// Operações de memória
void memory_init(Memory *mem, size_t size);  // Inicializa memória com tamanho
void memory_free(Memory *mem);                // Libera memória alocada

// Operações de CPU
void cpu_init(CPU *cpu, Memory *mem, int debug);  // Inicializa CPU com referência à memória
void cpu_set_handlers(CPU *cpu,                   // Registra as funções de tratamento
                      InterruptHandler ih,       // Tratador de interrupções
                      SysCallIOHandler sysIO,    // Tratador de IO
                      SysCallStopHandler sysStop); // Tratador de STOP
void cpu_set_context(CPU *cpu, int pc);      // Seta contexto da CPU (PC no endereço inicial)
void cpu_run(CPU *cpu);                      // Executa o ciclo de fetch-decode-execute
void cpu_set_gm(CPU *cpu, struct GM *gm);    // Associa gerenciador de memória à CPU
void cpu_set_page_table(CPU *cpu, int *pageTable, int pageTableSize); // Associa tabela de páginas ao CPU
int cpu_run_steps(CPU *cpu, int steps); // Run for limited number of instructions; returns 0 if process stopped, 1 otherwise

// Utilitário para debug
const char *opcode_name(Opcode op);          // Retorna nome textual do opcode para debug

#endif // HARDWARE_H
