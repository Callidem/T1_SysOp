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

// -----  P R O G R A M A   F I B O N A C C I  1 0  -----
// Calcula os primeiros 10 números da sequência de Fibonacci
// Utiliza índices de memória para armazenar a sequência
// Saída: sequência armazenada em memória[20] até memória[29]
static const Word prog_fibonacci10[] = {
    { LDI, 1, -1, 0 },
    { STD, 1, -1, 20 },
    { LDI, 2, -1, 1 },
    { STD, 2, -1, 21 },
    { LDI, 0, -1, 22 },
    { LDI, 6, -1, 6 },
    { LDI, 7, -1, 31 },
    { LDI, 3, -1, 0 },
    { ADD, 3, 1, -1 },
    { LDI, 1, -1, 0 },
    { ADD, 1, 2, -1 },
    { ADD, 2, 3, -1 },
    { STX, 0, 2, -1 },
    { ADDI, 0, -1, 1 },
    { SUB, 7, 0, -1 },
    { JMPIG, 6, 7, -1 },
    { STOP, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 }
};

// fibonacci10v2
static const Word prog_fibonacci10v2[] = {
    { LDI, 1, -1, 0 },
    { STD, 1, -1, 20 },
    { LDI, 2, -1, 1 },
    { STD, 2, -1, 21 },
    { LDI, 0, -1, 22 },
    { LDI, 6, -1, 6 },
    { LDI, 7, -1, 31 },
    { MOVE, 3, 1, -1 },
    { MOVE, 1, 2, -1 },
    { ADD, 2, 3, -1 },
    { STX, 0, 2, -1 },
    { ADDI, 0, -1, 1 },
    { SUB, 7, 0, -1 },
    { JMPIG, 6, 7, -1 },
    { STOP, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 }
};

// fibonacciREAD
static const Word prog_fibonacciREAD[] = {
    { LDI, 8, -1, 1 },
    { LDI, 9, -1, 55 },
    { SYSCALL, -1, -1, -1 },
    { LDD, 7, -1, 55 },
    { LDI, 3, -1, 0 },
    { ADD, 3, 7, -1 },
    { LDI, 4, -1, 36 },
    { LDI, 1, -1, -1 },
    { STD, 1, -1, 41 },
    { JMPIL, 4, 7, -1 },
    { JMPIE, 4, 7, -1 },
    { ADDI, 7, -1, 41 },
    { LDI, 1, -1, 0 },
    { STD, 1, -1, 41 },
    { SUBI, 3, -1, 1 },
    { JMPIE, 4, 3, -1 },
    { ADDI, 3, -1, 1 },
    { LDI, 2, -1, 1 },
    { STD, 2, -1, 42 },
    { SUBI, 3, -1, 2 },
    { JMPIE, 4, 3, -1 },
    { LDI, 0, -1, 43 },
    { LDI, 6, -1, 25 },
    { LDI, 5, -1, 0 },
    { ADD, 5, 7, -1 },
    { LDI, 7, -1, 0 },
    { ADD, 7, 5, -1 },
    { LDI, 3, -1, 0 },
    { ADD, 3, 1, -1 },
    { LDI, 1, -1, 0 },
    { ADD, 1, 2, -1 },
    { ADD, 2, 3, -1 },
    { STX, 0, 2, -1 },
    { ADDI, 0, -1, 1 },
    { SUB, 7, 0, -1 },
    { JMPIG, 6, 7, -1 },
    { STOP, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 }
};

// PB - calcula fatorial de 7, resultado fica em endereco logico 15
// Dados temporarios em enderecos 15 e 16 (apos o STOP em indice 14)
static const Word prog_PB[] = {
    { LDI, 0, -1, 7 },      // 0: r0 = 7
    { STD, 0, -1, 15 },     // 1: mem[15] = 7  (dado temporario, apos STOP)
    { LDD, 0, -1, 15 },     // 2: r0 = mem[15]
    { LDI, 1, -1, -1 },     // 3: r1 = -1 (flag negativo)
    { LDI, 2, -1, 13 },     // 4: r2 = 13 (endereco do STOP logico = 13)
    { JMPIL, 2, 0, -1 },    // 5: se r0 < 0, pula para r2 (end 13 = STD resultado)
    { LDI, 1, -1, 1 },      // 6: r1 = 1 (acumulador fatorial)
    { LDI, 6, -1, 1 },      // 7: r6 = 1 (constante para decremento)
    { LDI, 7, -1, 13 },     // 8: r7 = 13 (endereco do STD resultado)
    { JMPIE, 7, 0, 0 },     // 9: se r0 == 0, pula para r7 (end 13)
    { MULT, 1, 0, -1 },     // 10: r1 = r1 * r0
    { SUB, 0, 6, -1 },      // 11: r0 = r0 - r6 (r0--)
    { JMP, -1, -1, 9 },     // 12: volta para 9
    { STD, 1, -1, 16 },     // 13: mem[16] = resultado fatorial
    { STOP, -1, -1, -1 },   // 14: fim
    { DATA, -1, -1, -1 },   // 15: espaco para dado temporario
    { DATA, -1, -1, -1 }    // 16: espaco para resultado
};

// PC (bubble sort)
static const Word prog_PC[] = {
    { LDI, 7, -1, 5 },
    { LDI, 6, -1, 5 },
    { LDI, 5, -1, 46 },
    { LDI, 4, -1, 47 },
    { LDI, 0, -1, 4 },
    { STD, 0, -1, 46 },
    { LDI, 0, -1, 3 },
    { STD, 0, -1, 47 },
    { LDI, 0, -1, 5 },
    { STD, 0, -1, 48 },
    { LDI, 0, -1, 1 },
    { STD, 0, -1, 49 },
    { LDI, 0, -1, 2 },
    { STD, 0, -1, 50 },
    { LDI, 3, -1, 25 },
    { STD, 3, -1, 99 },
    { LDI, 3, -1, 22 },
    { STD, 3, -1, 98 },
    { LDI, 3, -1, 38 },
    { STD, 3, -1, 97 },
    { LDI, 3, -1, 25 },
    { STD, 3, -1, 96 },
    { LDI, 6, -1, 0 },
    { ADD, 6, 7, -1 },
    { SUBI, 6, -1, 1 },
    { JMPIEM, -1, 6, 97 },
    { LDX, 0, 5, -1 },
    { LDX, 1, 4, -1 },
    { LDI, 2, -1, 0 },
    { ADD, 2, 0, -1 },
    { SUB, 2, 1, -1 },
    { ADDI, 4, -1, 1 },
    { SUBI, 6, -1, 1 },
    { JMPILM, -1, 2, 99 },
    { STX, 5, 1, -1 },
    { SUBI, 4, -1, 1 },
    { STX, 4, 0, -1 },
    { ADDI, 4, -1, 1 },
    { JMPIGM, -1, 6, 99 },
    { ADDI, 5, -1, 1 },
    { SUBI, 7, -1, 1 },
    { LDI, 4, -1, 0 },
    { ADD, 4, 5, -1 },
    { ADDI, 4, -1, 1 },
    { JMPIGM, -1, 7, 98 },
    { STOP, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 },
    { DATA, -1, -1, -1 }
};

// -----  T A B E L A   D E   P R O G R A M A S  -----
// Tabela centralizada com todos os programas disponíveis
// Cada programa é armazenado com seu nome e tamanho
static const Program program_table[] = {
    { "fatorial",      prog_fatorial,      (int)(sizeof(prog_fatorial)      / sizeof(Word)) },
    { "fatorialV2",    prog_fatorialV2,    (int)(sizeof(prog_fatorialV2)    / sizeof(Word)) },
    { "progMinimo",    prog_progMinimo,    (int)(sizeof(prog_progMinimo)    / sizeof(Word)) },
    { "fibonacci10",   prog_fibonacci10,   (int)(sizeof(prog_fibonacci10)   / sizeof(Word)) },
    { "fibonacci10v2", prog_fibonacci10v2, (int)(sizeof(prog_fibonacci10v2) / sizeof(Word)) },
    { "fibonacciREAD", prog_fibonacciREAD, (int)(sizeof(prog_fibonacciREAD) / sizeof(Word)) },
    { "PB",            prog_PB,            (int)(sizeof(prog_PB)            / sizeof(Word)) },
    { "PC",            prog_PC,            (int)(sizeof(prog_PC)            / sizeof(Word)) }
};

// -----  R E C U P E R A Ç Ã O   D E   P R O G R A M A  -----
// Busca um programa pela sua identificação (nome)
// Percorre a tabela de programas procurando por um nome que corresponda
// Retorna: pointer para a estrutura Program se encontrado, NULL caso contrário
const Program *retrieve_program(const char *name) {
    size_t count = sizeof(program_table) / sizeof(program_table[0]);
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(name, program_table[i].name) == 0) {
            return &program_table[i];
        }
    }
    return NULL;
}