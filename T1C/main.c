#include <stdio.h>

#include "hardware.h"
#include "so.h"
#include "programs.h"
#include "utils.h"

int main(void) {
    Memory mem;
    CPU cpu;

    memory_init(&mem, 1024);
    cpu_init(&cpu, &mem, 1); // debug = 1

    cpu_set_handlers(&cpu, so_interrupt_handle, so_syscall_handle, so_syscall_stop);

    const char *progName = "fatorialV2"; // escolha do programa
    const Program *p = retrieve_program(progName);

    if (!p) {
        printf("Programa nao encontrado: %s\n", progName);
        memory_free(&mem);
        return 1;
    }

    printf("Executando programa: %s\n", p->name);
    utils_load_and_exec(&cpu, &mem, p->image, p->size);

    memory_free(&mem);
    return 0;
}
