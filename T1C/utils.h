#ifndef UTILS_H
#define UTILS_H

#include "hardware.h"

void utils_load_program(Memory *mem, const Word *prog, int size);
void utils_dump_word(const Word *w);
void utils_dump_mem(const Memory *mem, int ini, int fim);
void utils_load_and_exec(CPU *cpu, Memory *mem, const Word *prog, int size);

#endif // UTILS_H
