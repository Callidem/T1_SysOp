#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdbool.h>
#include "gp.h"
#include "hardware.h"

void scheduler_init(CPU *cpu, GM *gm);
// adiciona um PCB à lista do escalonador (retorna índice ou -1)
int scheduler_add(PCB *pcb);
PCB *scheduler_find_pcb(int pid);
int scheduler_find_idx(int pid);
void scheduler_remove_at_idx(int idx);
void scheduler_mark_ready_pid(int pid);
void scheduler_run_blocking(void);
int scheduler_count(void);
PCB *scheduler_get(int index);
void scheduler_dump_pcb(const PCB *pcb);
void scheduler_dump_process_memory(const PCB *pcb);

#endif // SCHEDULER_H
