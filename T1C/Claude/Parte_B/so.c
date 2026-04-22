#include "so.h"

#include <stdio.h>

// -----  T R A T A M E N T O   D E   I N T E R R U P C O E S  -----
// Função chamada quando uma interrupção ocorre durante a execução
// Tipos de interrupção possíveis:
//    intEnderecoInvalido:   acesso a endereço fora da memória
//    intInstrucaoInvalida:  instrução inválida ou não suportada
//    intOverflow:           resultado de operação ultrapassou limites
// Nesta versão apenas avisa - todas as interrupções finalizam o programa
void so_interrupt_handle(CPU *cpu, Interrupts irpt) {
    (void)cpu; // cpu pode ser usado para mais informação no futuro
    printf("                                               Interrupcao %d\n", irpt);
}

// -----  P A R A D A   D O   P R O G R A M A  -----
// Função chamada quando a CPU encontra instrução STOP
// Indica fim de execução do programa
// Nesta versão CPU simplesmente pára
void so_syscall_stop(CPU *cpu) {
    (void)cpu;
    printf("                                               SYSCALL STOP\n");
}

// -----  C H A M A D A S   D E   S I S T E M A  -----
// Chamada de sistema com suporte a operações de IO
// Parâmetros passados pelos registradores:
//    reg[8]: tipo de operação (1=leitura, 2=escrita)
//    reg[9]: endereço da memória onde está o valor (endereço lógico do processo)
// Operações:
//    1 (leitura):  lê valor inteiro da entrada padrão e armazena em mem[reg[9]]
//    2 (escrita):  escreve valor armazenado em mem[reg[9]]
void so_syscall_handle(CPU *cpu) {
    int op = cpu->reg[8];
    int addr = cpu->reg[9];

    printf("SYSCALL pars:  %d / %d\n", op, addr);

    if (op == 1) {
        // leitura simples de inteiro da entrada padrao com paginação
        int val;
        if (scanf("%d", &val) == 1) {
            // Traduz endereço lógico para físico
            int endereco_fisico = addr;
            if (cpu->tabela_paginas != NULL && cpu->nro_paginas > 0) {
                int page = addr / cpu->mem->pg_size;
                int offset = addr % cpu->mem->pg_size;
                
                if (page >= 0 && page < cpu->nro_paginas) {
                    int frame = cpu->tabela_paginas[page];
                    if (frame >= 0 && frame < cpu->mem->nro_frames) {
                        cpu->mem->frames[frame].pos[offset].opc = DATA;
                        cpu->mem->frames[frame].pos[offset].p = val;
                    } else {
                        printf("  ENDERECO INVALIDO NA LEITURA (frame inválido)\n");
                    }
                } else {
                    printf("  ENDERECO INVALIDO NA LEITURA (page inválida)\n");
                }
            } else {
                printf("  ENDERECO INVALIDO NA LEITURA (sem paginação)\n");
            }
        } else {
            printf("  FALHA NA LEITURA\n");
        }
    } else if (op == 2) {
        // escrita com paginação
        if (cpu->tabela_paginas != NULL && cpu->nro_paginas > 0) {
            int page = addr / cpu->mem->pg_size;
            int offset = addr % cpu->mem->pg_size;
            
            if (page >= 0 && page < cpu->nro_paginas) {
                int frame = cpu->tabela_paginas[page];
                if (frame >= 0 && frame < cpu->mem->nro_frames) {
                    printf("OUT:   %d\n", cpu->mem->frames[frame].pos[offset].p);
                } else {
                    printf("  ENDERECO INVALIDO NA ESCRITA (frame inválido)\n");
                }
            } else {
                printf("  ENDERECO INVALIDO NA ESCRITA (page inválida)\n");
            }
        } else {
            printf("  ENDERECO INVALIDO NA ESCRITA (sem paginação)\n");
        }
    } else {
        printf("  PARAMETRO INVALIDO\n");
    }
}
