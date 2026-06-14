#include <stdio.h>

#include "utils.h"

void utils_load_program(Memory *mem, GM *gm, const int *tabelaPaginas, int tamTabela, int progSize, const Word *image) {
    for (int i = 0; i < progSize; ++i) {
        int fisico = end_fisico(gm, tabelaPaginas, tamTabela, i);
        if (fisico < 0) {
            printf("Falha ao traduzir endereco logico %d, abortando carga\n", i);
            return;
        }
        mem->pos[fisico] = image[i];
        printf("Carregando instrucao %d: [ %s, %d, %d, %d ] no endereco logico %d (fisico %d)\n",
               i,
               opcode_name(image[i].opc),
               image[i].ra,
               image[i].rb,
               image[i].p,
               i,
               fisico);
    }
    printf("Programa carregado na memoria: %d palavras\n", progSize);
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

void utils_load_and_exec(CPU *cpu, Memory *mem, GM *gm, const int *tabelaPaginas, int tamTabela, int progSize, const Word *prog) {
    cpu_set_page_table(cpu, (int *)tabelaPaginas, tamTabela);
    utils_load_program(mem, gm, tabelaPaginas, tamTabela, progSize, prog);
    printf("---------------------------------- programa carregado na memoria\n");
    utils_dump_mem(mem, 0, progSize);
    cpu_set_context(cpu, 0);
    printf("---------------------------------- inicia execucao \n");
    cpu_run(cpu);
    printf("---------------------------------- memoria apos execucao \n");
    utils_dump_mem(mem, 0, progSize);
}
