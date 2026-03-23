#include "programs.h"

#include <string.h>

// Transposicao direta dos programas de Sistema.java para C

// fatorial
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

// fatorialV2
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

// progMinimo
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

// fibonacci10
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

// PB
static const Word prog_PB[] = {
    { LDI, 0, -1, 7 },
    { STD, 0, -1, 50 },
    { LDD, 0, -1, 50 },
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
    { STD, 1, -1, 15 },
    { STOP, -1, -1, -1 },
    { DATA, -1, -1, -1 }
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

const Program *retrieve_program(const char *name) {
    size_t count = sizeof(program_table) / sizeof(program_table[0]);
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(name, program_table[i].name) == 0) {
            return &program_table[i];
        }
    }
    return NULL;
}
