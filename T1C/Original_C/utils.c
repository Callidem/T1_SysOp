#include "utils.h"

#include <stdio.h>

// -----  U T I L I T A R I O S   D O   S I S T E M A  -----
// Funções auxiliares para carga, execução e debug do sistema

// Carrega um programa na memória da máquina virtual
// Copia as instruções do programa para as primeiras posições de memória
void utils_load_program(Memory *mem, const Word *prog, int size) {
    for (int i = 0; i < size && (size_t)i < mem->size; ++i) {
        mem->pos[i] = prog[i];
    }
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
