// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti

#ifndef SO_H
#define SO_H

#include "hardware.h"

void so_interrupt_handle(CPU *cpu, Interrupts irpt);
void so_syscall_handle(CPU *cpu);
void so_syscall_stop(CPU *cpu);

#endif // SO_H
