#include "utils.h"

#include <stdio.h>

void utils_load_program(Memory *mem, const Word *prog, int size) {
    for (int i = 0; i < size && (size_t)i < mem->size; ++i) {
        mem->pos[i] = prog[i];
    }
}

void utils_dump_word(const Word *w) {
    printf("[ %s, %d, %d, %d ]\n", opcode_name(w->opc), w->ra, w->rb, w->p);
}

void utils_dump_mem(const Memory *mem, int ini, int fim) {
    if (ini < 0) ini = 0;
    if ((size_t)fim > mem->size) fim = (int)mem->size;
    for (int i = ini; i < fim; ++i) {
        printf("%d:  ", i);
        utils_dump_word(&mem->pos[i]);
    }
}

void utils_load_and_exec(CPU *cpu, Memory *mem, const Word *prog, int size) {
    utils_load_program(mem, prog, size);
    printf("---------------------------------- programa carregado na memoria\n");
    utils_dump_mem(mem, 0, size);
    cpu_set_context(cpu, 0);
    printf("---------------------------------- inicia execucao \n");
    cpu_run(cpu);
    printf("---------------------------------- memoria apos execucao \n");
    utils_dump_mem(mem, 0, size);
}
