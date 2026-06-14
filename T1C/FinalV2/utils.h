// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti

#ifndef UTILS_H
#define UTILS_H

#include "hardware.h"
#include "programs.h"
#include "gm.h"

void utils_load_program(Memory *mem, GM *gm, const int *tabelaPaginas, int tamTabela, int progSize, const Word *prog);
void utils_dump_word(const Word *w);
void utils_dump_mem(const Memory *mem, int ini, int fim);
void utils_load_and_exec(CPU *cpu, Memory *mem, GM *gm, const int *tabelaPaginas, int tamTabela, int progSize, const Word *prog);

#endif // UTILS_H
