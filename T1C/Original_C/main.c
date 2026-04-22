// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
// Código fornecido como parte da solução do projeto de Sistemas Operacionais
//
// -----  P O N T O   D E   E N T R A D A   D O   S I S T E M A  -----
// Este programa instancia o Sistema com todos os elementos mencionados:
//    HW: Memória e CPU (virtualiza o hardware)
//    SO: Tratadores de interrupções e chamadas de sistema
// Em seguida solicita a execução de algum programa com load_and_exec

#include <stdio.h>

#include "hardware.h"
#include "so.h"
#include "programs.h"
#include "utils.h"

int main(void) {
    Memory mem;
    CPU cpu;

    // -----  I N I C I A L I Z A Ç Ã O   D O   S I S T E M A  -----
    memory_init(&mem, 1024);      // Cria memória com 1024 palavras
    cpu_init(&cpu, &mem, 1);      // Cria CPU com debug ligado (1)

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

    printf("Executando programa: %s\n", p->name);
    utils_load_and_exec(&cpu, &mem, p->image, p->size);  // Carrega programa e executa

    memory_free(&mem);
    return 0;
}
