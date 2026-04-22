#include "utils.h"

#include <stdio.h>

// -----  U T I L I T A R I O S   D O   S I S T E M A  -----
// Funções auxiliares para carga, execução e debug do sistema

// Carrega um programa na memória da máquina virtual com paginação
// Copia as instruções do programa para os frames alocados do processo
void utils_load_program(CPU *cpu, const Word *prog, int size) {
    if (cpu->tabela_paginas == NULL || cpu->nro_paginas == 0) {
        printf("Erro: Programa não tem pages alocadas\n");
        return;
    }
    
    for (int i = 0; i < size && i < (int)(cpu->nro_paginas * cpu->mem->pg_size); ++i) {
        // Traduz endereço lógico para físico
        int page = i / cpu->mem->pg_size;
        int offset = i % cpu->mem->pg_size;
        
        if (page >= cpu->nro_paginas) {
            printf("Erro: Índice de página %d fora de limites (%d)\n", page, cpu->nro_paginas);
            break;
        }
        
        int frame = cpu->tabela_paginas[page];
        if (frame < 0 || frame >= cpu->mem->nro_frames) {
            printf("Erro: Frame %d inválido\n", frame);
            break;
        }
        
        cpu->mem->frames[frame].pos[offset] = prog[i];
    }
}

// Imprime uma Word (instrução ou dado) para debug
// Formato: [ OPCODE, ra, rb, p ]
void utils_dump_word(const Word *w) {
    printf("[ %s, %d, %d, %d ]\n", opcode_name(w->opc), w->ra, w->rb, w->p);
}

// Imprime um range de memória (dump da memória) com paginação
// Útil para visualizar as instruções/dados carregados
void utils_dump_mem(CPU *cpu, int ini, int fim) {
    if (cpu->tabela_paginas == NULL || cpu->nro_paginas == 0) {
        printf("Erro: Sem pages alocadas para dump\n");
        return;
    }
    
    if (ini < 0) ini = 0;
    if (fim > (int)(cpu->nro_paginas * cpu->mem->pg_size)) {
        fim = (int)(cpu->nro_paginas * cpu->mem->pg_size);
    }
    
    for (int i = ini; i < fim; ++i) {
        int page = i / cpu->mem->pg_size;
        int offset = i % cpu->mem->pg_size;
        
        if (page >= cpu->nro_paginas) break;
        
        int frame = cpu->tabela_paginas[page];
        if (frame >= 0 && frame < cpu->mem->nro_frames) {
            printf("%d:  ", i);
            utils_dump_word(&cpu->mem->frames[frame].pos[offset]);
        }
    }
}

// Carrega e executa um programa completo com paginação
// Passos:
//   1. Carrega o programa na memória (usando tabela_paginas)
//   2. Imprime o programa carregado (dump)
//   3. Seta o contexto da CPU (PC = 0)
//   4. Executa o programa
//   5. Imprime a memória após a execução (resultado)
void utils_load_and_exec(CPU *cpu, Memory *mem, const Word *prog, int size) {
    utils_load_program(cpu, prog, size);
    printf("---------------------------------- programa carregado na memoria\n");
    utils_dump_mem(cpu, 0, size);
    cpu_set_context(cpu, 0);
    printf("---------------------------------- inicia execucao \n");
    cpu_run(cpu);
    printf("---------------------------------- memoria apos execucao \n");
    utils_dump_mem(cpu, 0, size);
}
