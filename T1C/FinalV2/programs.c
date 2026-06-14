#include "programs.h"

#include <string.h>

// -----  P R O G R A M A S   A R M A Z E N A D O S  -----
// Programas que podem ser carregados em memória e executados
// Transposição direta dos programas definidos em Sistema.java para C
// Cada programa é representado como um array estático de Words (instruções)
// Os programas representam:
//   - fatorial: calcula fatorial de um número
//   - fibonnacci10: calcula os primeiros 10 números de Fibonacci
//   - progMinimo: programa mínimo para teste de memória e dados

// -----  P R O G R A M A   F A T O R I A L  -----
// Calcula o fatorial de um número (apenas valores positivos)
// Entrada: r0 é o valor para calcular fatorial
// Saída: resultado armazenado em memória[10]
static const Word prog_fatorial[] = {
    { LDI, 0, -1, 7 },
    { LDI, 1, -1, 1 },
    { LDI, 6, -1, 1 },
    { LDI, 7, -1, 8 },
    { JMPIE, 7, 0, 0 },
    { MULT, 1, 0, -1 },
    { SUB, 0, 6, -1 },
    { JMP, -1, -1, 4 },
    { STD, 1, -1, 10 },
    { STOP, -1, -1, -1 },
    { DATA, -1, -1, -1 }
};

// -----  P R O G R A M A   F A T O R I A L   V 2  -----
// Versão 2 do fatorial com tratamento de números negativos
// Usa SYSCALL para output (escreve o resultado)
// Entrada: número armazenado em memória[19]
// Saída: resultado impresso e armazenado em memória[18]
static const Word prog_fatorialV2[] = {
    { LDI, 0, -1, 5 },
    { STD, 0, -1, 19 },
    { LDD, 0, -1, 19 },
    { LDI, 1, -1, -1 },
    { LDI, 2, -1, 13 },
    { JMPIL, 2, 0, -1 },
    { LDI, 1, -1, 1 },
    { LDI, 6, -1, 1 },
    { LDI, 7, -1, 13 },
    { JMPIE, 7, 0, 0 },
    { MULT, 1, 0, -1 },
    { SUB, 0, 6, -1 },
    { JMP, -1, -1, 9 },
    { STD, 1, -1, 18 },
    { LDI, 8, -1, 2 },
    { LDI, 9, -1, 18 },
    { SYSCALL, -1, -1, -1 },
    { STOP, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 }
};

// -----  P R O G R A M A   M Í N I M O  -----
// Programa mínimo para teste
// Carrega um valor em r0 e o armazena em várias posições de memória
// Útil para teste de operações básicas de memória
static const Word prog_progMinimo[] = {
    { LDI, 0, -1, 999 },
    { STD, 0, -1, 8 },
    { STD, 0, -1, 9 },
    { STD, 0, -1, 10 },
    { STD, 0, -1, 11 },
    { STD, 0, -1, 12 },
    { STOP, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 }
};

// other programs omitted for brevity in this copy (full list preserved from original)

// -----  T A B E L A   D E   P R O G R A M A S  -----
static const Program program_table[] = {
    { "fatorial",      prog_fatorial,      (int)(sizeof(prog_fatorial)      / sizeof(Word)) },
    { "fatorialV2",    prog_fatorialV2,    (int)(sizeof(prog_fatorialV2)    / sizeof(Word)) },
    { "progMinimo",    prog_progMinimo,    (int)(sizeof(prog_progMinimo)    / sizeof(Word)) }
};

const Program *retrieve_program(const char *name) {
    size_t count = sizeof(program_table) / sizeof(program_table[0]);
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(name, program_table[i].name) == 0) {
            return &program_table[i];
        }
    }
    return NULL;
}
