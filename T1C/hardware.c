#include "hardware.h"

#include <stdio.h>
#include <stdlib.h>

const char *opcode_name(Opcode op) {
    switch (op) {
        case DATA:   return "DATA";
        case ___:    return "___";
        case JMP:    return "JMP";
        case JMPI:   return "JMPI";
        case JMPIG:  return "JMPIG";
        case JMPIL:  return "JMPIL";
        case JMPIE:  return "JMPIE";
        case JMPIM:  return "JMPIM";
        case JMPIGM: return "JMPIGM";
        case JMPILM: return "JMPILM";
        case JMPIEM: return "JMPIEM";
        case JMPIGK: return "JMPIGK";
        case JMPILK: return "JMPILK";
        case JMPIEK: return "JMPIEK";
        case JMPIGT: return "JMPIGT";
        case ADDI:   return "ADDI";
        case SUBI:   return "SUBI";
        case ADD:    return "ADD";
        case SUB:    return "SUB";
        case MULT:   return "MULT";
        case LDI:    return "LDI";
        case LDD:    return "LDD";
        case STD:    return "STD";
        case LDX:    return "LDX";
        case STX:    return "STX";
        case MOVE:   return "MOVE";
        case SYSCALL:return "SYSCALL";
        case STOP:   return "STOP";
        default:     return "?";
    }
}

static int legal(CPU *cpu, int addr) {
    if (addr >= 0 && (size_t)addr < cpu->mem->size) {
        return 1;
    }
    cpu->irpt = intEnderecoInvalido;
    return 0;
}

static int testOverflow(CPU *cpu, int v) {
    if (v < cpu->minInt || v > cpu->maxInt) {
        cpu->irpt = intOverflow;
        return 0;
    }
    return 1;
}

void memory_init(Memory *mem, size_t size) {
    mem->size = size;
    mem->pos = (Word *)malloc(size * sizeof(Word));
    if (!mem->pos) {
        fprintf(stderr, "Erro ao alocar memoria virtual\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < size; ++i) {
        mem->pos[i].opc = ___;
        mem->pos[i].ra = -1;
        mem->pos[i].rb = -1;
        mem->pos[i].p = -1;
    }
}

void memory_free(Memory *mem) {
    free(mem->pos);
    mem->pos = NULL;
    mem->size = 0;
}

void cpu_init(CPU *cpu, Memory *mem, int debug) {
    cpu->maxInt = 32767;
    cpu->minInt = -32767;
    cpu->mem = mem;
    cpu->debug = debug;
    cpu->ih = NULL;
    cpu->sysCallIO = NULL;
    cpu->sysCallStop = NULL;
    cpu->pc = 0;
    cpu->irpt = noInterrupt;
    cpu->cpuStop = 0;
    for (int i = 0; i < 10; ++i) {
        cpu->reg[i] = 0;
    }
}

void cpu_set_handlers(CPU *cpu,
                      InterruptHandler ih,
                      SysCallIOHandler sysIO,
                      SysCallStopHandler sysStop) {
    cpu->ih = ih;
    cpu->sysCallIO = sysIO;
    cpu->sysCallStop = sysStop;
}

void cpu_set_context(CPU *cpu, int pc) {
    cpu->pc = pc;
    cpu->irpt = noInterrupt;
}

void cpu_run(CPU *cpu) {
    cpu->cpuStop = 0;

    while (!cpu->cpuStop) {
        // FETCH
        if (!legal(cpu, cpu->pc)) {
            break;
        }
        cpu->ir = cpu->mem->pos[cpu->pc];

        if (cpu->debug) {
            printf("                                              regs: ");
            for (int i = 0; i < 10; ++i) {
                printf(" r[%d]:%d", i, cpu->reg[i]);
            }
            printf("\n");
                 printf("                      pc: %d       exec: [ %s, %d, %d, %d ]\n",
                     cpu->pc,
                     opcode_name(cpu->ir.opc),
                     cpu->ir.ra,
                     cpu->ir.rb,
                     cpu->ir.p);
        }

        // EXECUTA INSTRUCAO
        switch (cpu->ir.opc) {
            // Memoria / dados
            case LDI:
                cpu->reg[cpu->ir.ra] = cpu->ir.p;
                cpu->pc++;
                break;
            case LDD:
                if (legal(cpu, cpu->ir.p)) {
                    cpu->reg[cpu->ir.ra] = cpu->mem->pos[cpu->ir.p].p;
                    cpu->pc++;
                }
                break;
            case LDX:
                if (legal(cpu, cpu->reg[cpu->ir.rb])) {
                    cpu->reg[cpu->ir.ra] = cpu->mem->pos[cpu->reg[cpu->ir.rb]].p;
                    cpu->pc++;
                }
                break;
            case STD:
                if (legal(cpu, cpu->ir.p)) {
                    cpu->mem->pos[cpu->ir.p].opc = DATA;
                    cpu->mem->pos[cpu->ir.p].p = cpu->reg[cpu->ir.ra];
                    cpu->pc++;
                }
                break;
            case STX:
                if (legal(cpu, cpu->reg[cpu->ir.ra])) {
                    cpu->mem->pos[cpu->reg[cpu->ir.ra]].opc = DATA;
                    cpu->mem->pos[cpu->reg[cpu->ir.ra]].p = cpu->reg[cpu->ir.rb];
                    cpu->pc++;
                }
                break;
            case MOVE:
                cpu->reg[cpu->ir.ra] = cpu->reg[cpu->ir.rb];
                cpu->pc++;
                break;

            // Aritmeticas
            case ADD:
                cpu->reg[cpu->ir.ra] = cpu->reg[cpu->ir.ra] + cpu->reg[cpu->ir.rb];
                testOverflow(cpu, cpu->reg[cpu->ir.ra]);
                cpu->pc++;
                break;
            case ADDI:
                cpu->reg[cpu->ir.ra] = cpu->reg[cpu->ir.ra] + cpu->ir.p;
                testOverflow(cpu, cpu->reg[cpu->ir.ra]);
                cpu->pc++;
                break;
            case SUB:
                cpu->reg[cpu->ir.ra] = cpu->reg[cpu->ir.ra] - cpu->reg[cpu->ir.rb];
                testOverflow(cpu, cpu->reg[cpu->ir.ra]);
                cpu->pc++;
                break;
            case SUBI:
                cpu->reg[cpu->ir.ra] = cpu->reg[cpu->ir.ra] - cpu->ir.p;
                testOverflow(cpu, cpu->reg[cpu->ir.ra]);
                cpu->pc++;
                break;
            case MULT:
                cpu->reg[cpu->ir.ra] = cpu->reg[cpu->ir.ra] * cpu->reg[cpu->ir.rb];
                testOverflow(cpu, cpu->reg[cpu->ir.ra]);
                cpu->pc++;
                break;

            // Desvios
            case JMP:
                cpu->pc = cpu->ir.p;
                break;
            case JMPI:
                // nao existe na versao Java atual, deixado aqui para compatibilidade
                cpu->pc = cpu->reg[cpu->ir.ra];
                break;
            case JMPIM:
                cpu->pc = cpu->mem->pos[cpu->ir.p].p;
                break;
            case JMPIG:
                if (cpu->reg[cpu->ir.rb] > 0) {
                    cpu->pc = cpu->reg[cpu->ir.ra];
                } else {
                    cpu->pc++;
                }
                break;
            case JMPIGK:
                if (cpu->reg[cpu->ir.rb] > 0) {
                    cpu->pc = cpu->ir.p;
                } else {
                    cpu->pc++;
                }
                break;
            case JMPILK:
                if (cpu->reg[cpu->ir.rb] < 0) {
                    cpu->pc = cpu->ir.p;
                } else {
                    cpu->pc++;
                }
                break;
            case JMPIEK:
                if (cpu->reg[cpu->ir.rb] == 0) {
                    cpu->pc = cpu->ir.p;
                } else {
                    cpu->pc++;
                }
                break;
            case JMPIL:
                if (cpu->reg[cpu->ir.rb] < 0) {
                    cpu->pc = cpu->reg[cpu->ir.ra];
                } else {
                    cpu->pc++;
                }
                break;
            case JMPIE:
                if (cpu->reg[cpu->ir.rb] == 0) {
                    cpu->pc = cpu->reg[cpu->ir.ra];
                } else {
                    cpu->pc++;
                }
                break;
            case JMPIGM:
                if (legal(cpu, cpu->ir.p)) {
                    if (cpu->reg[cpu->ir.rb] > 0) {
                        cpu->pc = cpu->mem->pos[cpu->ir.p].p;
                    } else {
                        cpu->pc++;
                    }
                }
                break;
            case JMPILM:
                if (cpu->reg[cpu->ir.rb] < 0) {
                    cpu->pc = cpu->mem->pos[cpu->ir.p].p;
                } else {
                    cpu->pc++;
                }
                break;
            case JMPIEM:
                if (cpu->reg[cpu->ir.rb] == 0) {
                    cpu->pc = cpu->mem->pos[cpu->ir.p].p;
                } else {
                    cpu->pc++;
                }
                break;
            case JMPIGT:
                if (cpu->reg[cpu->ir.ra] > cpu->reg[cpu->ir.rb]) {
                    cpu->pc = cpu->ir.p;
                } else {
                    cpu->pc++;
                }
                break;

            case DATA:
                cpu->irpt = intInstrucaoInvalida;
                break;

            case SYSCALL:
                if (cpu->sysCallIO) {
                    cpu->sysCallIO(cpu);
                }
                cpu->pc++;
                break;

            case STOP:
                if (cpu->sysCallStop) {
                    cpu->sysCallStop(cpu);
                }
                cpu->cpuStop = 1;
                break;

            case ___:
            default:
                cpu->irpt = intInstrucaoInvalida;
                break;
        }

        if (cpu->irpt != noInterrupt) {
            if (cpu->ih) {
                cpu->ih(cpu, cpu->irpt);
            }
            cpu->cpuStop = 1;
        }
    }
}
