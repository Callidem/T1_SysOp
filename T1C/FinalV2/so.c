#include "so.h"

#include <stdio.h>

void so_interrupt_handle(CPU *cpu, Interrupts irpt) {
    (void)cpu;
    printf("                                               Interrupcao %d\n", irpt);
}

void so_syscall_stop(CPU *cpu) {
    (void)cpu;
    printf("                                               SYSCALL STOP\n");
}

void so_syscall_handle(CPU *cpu) {
    int op = cpu->reg[8];
    int addr = cpu->reg[9];

    printf("SYSCALL pars:  %d / %d\n", op, addr);

    if (op == 1) {
        int val;
        if (scanf("%d", &val) == 1) {
            if (addr >= 0 && (size_t)addr < cpu->mem->size) {
                cpu->mem->pos[addr].opc = DATA;
                cpu->mem->pos[addr].p = val;
            } else {
                printf("  ENDERECO INVALIDO NA LEITURA\n");
            }
        } else {
            printf("  FALHA NA LEITURA\n");
        }
    } else if (op == 2) {
        if (addr >= 0 && (size_t)addr < cpu->mem->size) {
            printf("OUT:   %d\n", cpu->mem->pos[addr].p);
        } else {
            printf("  ENDERECO INVALIDO NA ESCRITA\n");
        }
    } else {
        printf("  PARAMETRO INVALIDO\n");
    }
}
