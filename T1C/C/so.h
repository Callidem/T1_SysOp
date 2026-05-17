// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
// 
// -----  S I S T E M A   O P E R A C I O N A L  -----
// Interface de software do sistema operacional
//
// O SO fornece:
//    Tratamento de Interrupções: rotinas que tratam exceções durante execução
//    Tratamento de Syscalls: rotinas que executam operações de sistema (IO, STOP)

#ifndef SO_H
#define SO_H

#include "hardware.h"

// -----  T R A T A M E N T O   D E   I N T E R R U P C O E S  -----
// Função chamada quando uma interrupção ocorre durante a execução
// Recebe o tipo de interrupção e trata de acordo com o tipo
void so_interrupt_handle(CPU *cpu, Interrupts irpt);

// -----  T R A T A M E N T O   D E   C H A M A D A S   D E   S I S T E M A  -----
// Função chamada quando a CPU encontra instrução SYSCALL
// Suporta IO com parâmetros em reg[8] (tipo: 1=leitura, 2=escrita) 
//                                e reg[9] (endereço da memória)
void so_syscall_handle(CPU *cpu);

// -----  P A R A D A   D O   P R O G R A M A  -----
// Função chamada quando a CPU encontra instrução STOP
// Indica fim de execução do programa
void so_syscall_stop(CPU *cpu);

#endif // SO_H
