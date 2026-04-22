// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
// Código fornecido como parte da solução do projeto de Sistemas Operacionais
//
// -----  P O N T O   D E   E N T R A D A   D O   S I S T E M A  -----
// Este programa instancia o Sistema com todos os elementos mencionados:
//    HW: Memória e CPU (virtualiza o hardware)
//    SO: Tratadores de interrupções e chamadas de sistema
//    T1-A: Gerenciador de memória com paginação
// Em seguida solicita a execução de algum programa com load_and_exec

#include <stdio.h>
#include <stdlib.h>

#include "hardware.h"
#include "mem_mngr.h"
#include "so.h"
#include "programs.h"
#include "utils.h"

int main(void) {
    Memory mem;
    CPU cpu;
    Mem_Mngr gm;
    
    int tamMem = 1024; // Tamanho da memória em palavras
    int tamPg = 16;   // Tamanho da página em palavras

    // -----  I N I C I A L I Z A Ç Ã O   D O   S I S T E M A  -----
    memory_init(&mem, tamMem, tamPg);      // Cria memória com tamMem palavras e tamPg palavras por página
    gm_init(&gm, tamMem, tamPg);            // Inicializa gerenciador de memória
    cpu_init(&cpu, &mem, 1);                // Cria CPU com debug ligado (1)

    // Registra as funções de tratamento de interrupções e syscalls do SO
    cpu_set_handlers(&cpu, so_interrupt_handle, so_syscall_handle, so_syscall_stop);

    // -----  C A R R E G A M E N T O   E   E X E C U Ç Ã O   D O   P R O G R A M A  -----
    const char *progName = "fatorialV2"; // escolha do programa a executar
    const Program *p = retrieve_program(progName);

    if (!p) {
        printf("Programa nao encontrado: %s\n", progName);
        memory_free(&mem);
        return 1;
    }

    // -----  A L O C A Ç Ã O   C O M   P A G I N A Ç Ã O  -----
    // Aloca frames para o programa
    if (!gm_alloc(&gm, p->size)) {
        printf("Erro: Memoria insuficiente para alocar programa\n");
        memory_free(&mem);
        return 1;
    }

    // Configura tabela de páginas da CPU
    // First Fit simples: aloca sequencialmente do frame 0
    int nro_paginas = (p->size + tamPg - 1) / tamPg;
    cpu.tabela_paginas = (int *)malloc(nro_paginas * sizeof(int));
    if (!cpu.tabela_paginas) {
        printf("Erro ao alocar tabela de páginas\n");
        memory_free(&mem);
        return 1;
    }

    // Preenche tabela de páginas (mapeamento simples sequencial)
    // Em um sistema real com múltiplos processos, isso viria do gerenciador
    int paginas_alocadas = 0;
    for (int i = 0; i < gm.nroFrames && paginas_alocadas < nro_paginas; ++i) {
        if (!gm.frameLivre[i]) {
            cpu.tabela_paginas[paginas_alocadas] = i;
            paginas_alocadas++;
        }
    }

    cpu.nro_paginas = nro_paginas;
    cpu.processo_id = 1;

    printf("Executando programa: %s\n", p->name);
    printf("Alocado %d frames para %d páginas\n", nro_paginas, nro_paginas);
    utils_load_and_exec(&cpu, &mem, p->image, p->size);  // Carrega programa e executa

    free(cpu.tabela_paginas);
    memory_free(&mem);
    return 0;
}
