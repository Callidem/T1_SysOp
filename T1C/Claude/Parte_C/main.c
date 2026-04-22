// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
// Código fornecido como parte da solução do projeto de Sistemas Operacionais
//
// -----  P O N T O   D E   E N T R A D A   D O   S I S T E M A  -----
// Este programa instancia o Sistema com todos os elementos mencionados:
//    HW: Memória e CPU (virtualiza o hardware)
//    SO: Tratadores de interrupções e chamadas de sistema
//    T1-A: Gerenciador de memória com paginação
//    T1-B: Gerenciador de processos com multiprogramação
//    T1-C: Escalonador com time-slicing
// Em seguida cria múltiplos processos e os executa com escalonamento automático

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware.h"
#include "mem_mngr.h"
#include "process.h"
#include "scheduler.h"
#include "so.h"
#include "programs.h"
#include "utils.h"

int main(void) {
    Memory mem;
    CPU cpu;
    Mem_Mngr gm;
    GerenciadorProcessos gp;
    Escalonador sch;
    
    int tamMem = 1024; // Tamanho da memória em palavras
    int tamPg = 16;    // Tamanho da página em palavras
    int max_procs = 10; // Máximo de processos simultâneos
    int tamFatia = 5;  // Tamanho de fatia de tempo (ciclos)

    // -----  I N I C I A L I Z A Ç Ã O   D O   S I S T E M A  -----
    memory_init(&mem, tamMem, tamPg);              // Cria memória
    gm_init(&gm, tamMem, tamPg);                   // Inicializa gerenciador de memória
    cpu_init(&cpu, &mem, 0);                       // Cria CPU com debug desligado (0)
    gp_init(&gp, max_procs, &gm);                  // Inicializa gerenciador de processos
    sch_init(&sch, &gp, &cpu, tamFatia);           // Inicializa escalonador

    // Registra as funções de tratamento de interrupções e syscalls do SO
    cpu_set_handlers(&cpu, so_interrupt_handle, so_syscall_handle, so_syscall_stop);

    // -----  D E M O N S T R A Ç Ã O   D E   T 1 - C  -----
    // Criar múltiplos processos

    printf("\n");
    printf("################################\n");
    printf("# T1-C: ESCALONADOR (T1-A+T1-B+T1-C)\n");
    printf("################################\n");
    printf("\n");

    // Criar primeiro processo
    printf("[MAIN] Criando processo 1...\n");
    int pid1 = gp_new(&gp, "fatorialV2", tamPg);
    if (pid1 == -1) {
        printf("[MAIN] Erro ao criar processo 1\n");
        goto cleanup;
    }

    // Criar segundo processo (mesmo programa)
    printf("\n[MAIN] Criando processo 2...\n");
    int pid2 = gp_new(&gp, "fatorialV2", tamPg);
    if (pid2 == -1) {
        printf("[MAIN] Erro ao criar processo 2\n");
        // Continuar mesmo assim para demonstração
    }

    // Criar terceiro processo (demonstração com 3 processos)
    printf("\n[MAIN] Criando processo 3...\n");
    int pid3 = gp_new(&gp, "fatorialV2", tamPg);
    if (pid3 == -1) {
        printf("[MAIN] Erro ao criar processo 3\n");
        // Continuar mesmo assim para demonstração
    }

    // Listar processos criados
    printf("\n");
    gp_ps(&gp);

    // -----  E X E C U Ç Ã O   A U T O M Á T I C A   C O M   E S C A L O N A D O R  -----
    // O escalonador cuida de:
    // 1. Selecionar próximo processo pronto
    // 2. Fazer context switch automaticamente
    // 3. Executar a fatia de tempo
    // 4. Retornar processo à fila quando fatia termina
    // 5. Alternar para próximo processo

    sch_run(&sch);

    // Listar estado final dos processos
    printf("\n");
    gp_ps(&gp);

    // -----  L I M P E Z A  -----
cleanup:
    gp_free(&gp);
    memory_free(&mem);
    
    printf("\n[MAIN] Sistema finalizado\n");
    return 0;
}
