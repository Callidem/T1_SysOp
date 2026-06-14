#include <stdio.h>

#include "utils.h"


// -----  U T I L I T A R I O S   D O   S I S T E M A  -----
// Funções auxiliares para carga, execução e debug do sistema

// Carrega um programa na memória da máquina virtual
// Copia as instruções do programa para as primeiras posições de memória
void utils_load_program(Memory *mem, GM *gm, const int *tabelaPaginas, int tamTabela, int progSize, const Word *image) {
    for (int i = 0; i < progSize; ++i) {
        int fisico = end_fisico(gm, tabelaPaginas, tamTabela, i);
        if (fisico < 0) {
            printf("Falha ao traduzir endereco logico %d, abortando carga\n", i);
            return;
        }
        mem->pos[fisico] = image[i];
        /*printf("Carregando instrucao %d: [ %s, %d, %d, %d ] no endereco logico %d (fisico %d)\n",
               i,
               opcode_name(image[i].opc),
               image[i].ra,
               image[i].rb,
               image[i].p,
               i,
               fisico);*/
    }
    //printf("Programa carregado na memoria: %d palavras\n", progSize);
}




// Imprime uma Word (instrução ou dado) para debug
// Formato: [ OPCODE, ra, rb, p ]
void utils_dump_word(const Word *w) {
    printf("[ %s, %d, %d, %d ]\n", opcode_name(w->opc), w->ra, w->rb, w->p);
}

// Imprime um range de memória (dump da memória)
// Útil para visualizar as instruções/dados carregados
void utils_dump_mem(const Memory *mem, int ini, int fim) {
    if (ini < 0) ini = 0;
    if ((size_t)fim > mem->size) fim = (int)mem->size;
    for (int i = ini; i < fim; ++i) {
        printf("%d:  ", i);
        utils_dump_word(&mem->pos[i]);
    }
}

// Carrega e executa um programa completo
// Passos:
//   1. Carrega o programa na memória
//   2. Imprime o programa carregado (dump)
//   3. Seta o contexto da CPU (PC = 0)
//   4. Executa o programa
//   5. Imprime a memória após a execução (resultado)
void utils_load_and_exec(CPU *cpu, Memory *mem, GM *gm, const int *tabelaPaginas, int tamTabela, int progSize, const Word *prog) {
    // Associa a tabela de páginas ao CPU (contexto do processo)
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
