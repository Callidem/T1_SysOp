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
// Em seguida permite criar múltiplos processos e executá-los

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware.h"
#include "mem_mngr.h"
#include "process.h"
#include "so.h"
#include "programs.h"
#include "utils.h"

int main(void) {
    Memory mem;
    CPU cpu;
    Mem_Mngr gm;
    GerenciadorProcessos gp;
    
    int tamMem = 1024; // Tamanho da memória em palavras
    int tamPg = 16;    // Tamanho da página em palavras
    int max_procs = 10; // Máximo de processos simultâneos

    // -----  I N I C I A L I Z A Ç Ã O   D O   S I S T E M A  -----
    memory_init(&mem, tamMem, tamPg);              // Cria memória
    gm_init(&gm, tamMem, tamPg);                   // Inicializa gerenciador de memória
    cpu_init(&cpu, &mem, 1);                       // Cria CPU com debug ligado (1)
    gp_init(&gp, max_procs, &gm);                  // Inicializa gerenciador de processos

    // Registra as funções de tratamento de interrupções e syscalls do SO
    cpu_set_handlers(&cpu, so_interrupt_handle, so_syscall_handle, so_syscall_stop);

    // -----  D E M O N S T R A Ç Ã O   D E   T 1 - B  -----
    // Criar múltiplos processos

    printf("\n");
    printf("################################\n");
    printf("# T1-B: GERENCIADOR DE PROCESSOS\n");
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

    // Listar processos criados
    printf("\n");
    gp_ps(&gp);

    // Dumpar estado de um processo
    printf("[MAIN] Detalhes do processo %d:\n", pid1);
    gp_dump(&gp, pid1);

    // -----  E X E C U Ç Ã O   C O M   A L T E R N Ã N C I A  -----
    printf("[MAIN] Iniciando execução com alternância de processos...\n\n");
    
    // Executar primeira fatia do processo 1
    printf("[MAIN] Executando fatia de tempo do processo %d\n", pid1);
    context_switch(&gp, &cpu, pid1);
    
    // Executar apenas 10 ciclos
    int ciclos_max = 10;
    for (int ciclos = 0; ciclos < ciclos_max && !cpu.cpuStop; ++ciclos) {
        cpu_run(&cpu);  // Executa um ciclo completo
        if (cpu.irpt != noInterrupt) {
            printf("  [CICLO %d] Interrupção: %d\n", ciclos, cpu.irpt);
            break;  // Para se houver interrupção
        }
    }

    // Salvar contexto (antes de trocar processo)
    ProcessoControlBlock *pcb1 = NULL;
    for (int i = 0; i < gp.nro_processos; ++i) {
        if (gp.processos[i].id == pid1) {
            pcb1 = &gp.processos[i];
            break;
        }
    }
    if (pcb1) {
        save_context(&cpu, pcb1);
    }

    // Se há segundo processo, alternar
    if (pid2 != -1) {
        printf("\n[MAIN] Alternando para processo %d\n", pid2);
        context_switch(&gp, &cpu, pid2);
        
        // Executar fatia do processo 2
        for (int ciclos = 0; ciclos < ciclos_max && !cpu.cpuStop; ++ciclos) {
            cpu_run(&cpu);
            if (cpu.irpt != noInterrupt) {
                printf("  [CICLO %d] Interrupção: %d\n", ciclos, cpu.irpt);
                break;
            }
        }
        
        ProcessoControlBlock *pcb2 = NULL;
        for (int i = 0; i < gp.nro_processos; ++i) {
            if (gp.processos[i].id == pid2) {
                pcb2 = &gp.processos[i];
                break;
            }
        }
        if (pcb2) {
            save_context(&cpu, pcb2);
        }
    }

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
