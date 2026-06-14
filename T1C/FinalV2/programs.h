// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti

#ifndef PROGRAMS_H
#define PROGRAMS_H

#include "hardware.h"

typedef struct {
    const char *name;
    const Word *image;
    int size;
} Program;

const Program *retrieve_program(const char *name);

#endif // PROGRAMS_H
