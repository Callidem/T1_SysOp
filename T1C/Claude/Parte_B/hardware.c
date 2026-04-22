#include "hardware.h"

#include <stdio.h>
#include <stdlib.h>

// -----  N O M E   T E X T U A L   D E   O P C O D E  -----
// Função auxiliar para debug - retorna nome legível do opcode
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

// -----  V E R I F I C A Ç Ã O   D E   E N D E R E Ç A M E N T O  -----
// Todo acesso a memória tem que ser verificado se é válido
// Aqui no caso, verifica se o endereço é um endereço válido em toda memória
// Se não for, liga interrupção intEnderecoInvalido
static int legal(CPU *cpu, int addr) {
    if (addr >= 0 && (size_t)addr < cpu->mem->size) {
        return 1;
    }
    cpu->irpt = intEnderecoInvalido;
    return 0;
}

// -----  T E S T E   D E   O V E R F L O W  -----
// Toda operação matemática deve avaliar se ocorre overflow
// Se houver, liga interrupção intOverflow durante execução da instrução
static int testOverflow(CPU *cpu, int v) {
    if (v < cpu->minInt || v > cpu->maxInt) {
        cpu->irpt = intOverflow;
        return 0;
    }
    return 1;
}

// -----  T R A D U Ç Ã O   D E   E N D E R E Ç O S  -----
// Traduz endereço lógico (visto pelo processo) para endereço físico (na memória)
// Endereço lógico: 0 a (nro_paginas * pg_size - 1)
// Endereço físico: localização real na memória com frames
// Se endereço lógico é inválido, dispara interrupção intEnderecoInvalido
static int traducao_endereco(CPU *cpu, int endereco_logico) {
    // Valida se endereço está dentro do processo
    if (cpu->tabela_paginas == NULL || cpu->nro_paginas <= 0) {
        cpu->irpt = intEnderecoInvalido;
        return -1;
    }
    
    if (endereco_logico < 0 || endereco_logico >= (int)(cpu->nro_paginas * cpu->mem->pg_size)) {
        cpu->irpt = intEnderecoInvalido;
        return -1;
    }
    
    // Calcula página e offset dentro da página
    int page = endereco_logico / cpu->mem->pg_size;
    int offset = endereco_logico % cpu->mem->pg_size;
    
    // Consulta tabela de páginas para obter frame físico
    int frame = cpu->tabela_paginas[page];
    
    // Calcula endereço físico
    int endereco_fisico = frame * cpu->mem->pg_size + offset;
    
    return endereco_fisico;
}

// -----  M E M O R I A  -----

// Inicializa a memória com o tamanho especificado
// Aloca espaço para 'size' palavras
// Cada posição é inicializada com opcode ___ (não usada)
void memory_init(Memory *mem, int tamMem, int tamPg) {
    mem->size = tamMem; // a memória é composta por 'size' palavras
    mem->pg_size = tamPg;
    mem->nro_frames = tamMem / tamPg; // cada frame tem o tamanho pode ser uma página, então n frames = tamanho da memória / tamanho da página
    mem->free_frames = mem->nro_frames; // inicialmente, todos os frames estão livres
    mem->allocated_frames = (bool *)malloc(mem->nro_frames * sizeof(bool)); // aloca a tabela de alocação de frames
    if (!mem->allocated_frames) {
        fprintf(stderr, "Erro ao alocar tabela de alocação de frames\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < mem->nro_frames; ++i) {
        mem->allocated_frames[i] = false; // começa com todos os frames livres
    }
    mem->frames = (Page *)malloc(mem->nro_frames * sizeof(Page)); //aloca tamanho da memória baseado na quantidade de páginas

    if (!mem->frames) {
        fprintf(stderr, "Erro ao alocar memoria virtual\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < mem->nro_frames; ++i) {
        mem->frames[i].pos = (Word *)malloc(mem->pg_size * sizeof(Word));
        if (!mem->frames[i].pos) {
            fprintf(stderr, "Erro ao alocar memoria virtual\n");
            exit(EXIT_FAILURE);
        }
        for (size_t j = 0; j < mem->pg_size; ++j) {
            mem->frames[i].pos[j].opc = ___;
            mem->frames[i].pos[j].ra = -1;
            mem->frames[i].pos[j].rb = -1;
            mem->frames[i].pos[j].p = -1;
        }
    }
}


// Libera a memória previamente alocada
void memory_free(Memory *mem) {
    for (int i = 0; i < mem->nro_frames; ++i) {
        free(mem->frames[i].pos);
    }
    free(mem->frames);
    mem->frames = NULL;
    mem->size = 0;
}

// -----  C P U  -----

// Inicializa a CPU com referência à memória
// Define limites de inteiros (maxInt e minInt) para detecção de overflow
// Aloca espaço para os 10 registradores
// debug = 1 para mostrar cada instrução em execução
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
    // -----  I N I C I A L I Z A Ç Ã O   D E   P A G I N A Ç Ã O  -----
    cpu->tabela_paginas = NULL;
    cpu->nro_paginas = 0;
    cpu->processo_id = -1;
}

// Registra as funções de tratamento de interrupções e syscalls
// Estas funções são do SO (Sistema Operacional)
// ih: tratador de interrupções
// sysIO: tratador de chamadas de sistema (IO)
// sysStop: tratador de fim de programa (STOP)
void cpu_set_handlers(CPU *cpu,
                      InterruptHandler ih,
                      SysCallIOHandler sysIO,
                      SysCallStopHandler sysStop) {
    cpu->ih = ih;
    cpu->sysCallIO = sysIO;
    cpu->sysCallStop = sysStop;
}

// Seta o contexto da CPU para rodar um processo
// Nesta versão é somente colocar o PC na posição inicial (endereço lógico)
// Reset da interrupção registrada
void cpu_set_context(CPU *cpu, int pc) {
    cpu->pc = pc;
    cpu->irpt = noInterrupt;
}

// -----  E X E C U Ç Ã O   D A   C P U  -----
// Executa o ciclo de instruções (fetch-decode-execute)
// Supõe que o contexto da CPU estava devidamente setado via cpu_set_context()
// Ciclo: 
//    1. FETCH: Busca instrução da posição apontada por PC, guarda em IR
//    2. DECODE/EXECUTE: Conforme o opcode (código de operação) executa
//    3. INTERRUPÇÃO: Verifica se ocorreu interrupção durante execução
// Continua enquanto não houver uma instrução STOP ou interrupção fatal
void cpu_run(CPU *cpu) {
    cpu->cpuStop = 0;

    while (!cpu->cpuStop) {
        // -----  F A S E   D E   F E T C H  -----
        // Verifica se PC é endereço válido
        // Se válido, busca a palavra na posição de memória apontada por PC
        // Guarda essa palavra no Instruction Register (IR)
        // Com paginação: traduz endereço lógico (PC) para endereço físico
        int endereco_fisico = traducao_endereco(cpu, cpu->pc);
        if (endereco_fisico < 0) {
            break;  // Endereço inválido
        }
        
        int frame = endereco_fisico / cpu->mem->pg_size;
        int offset = endereco_fisico % cpu->mem->pg_size;
        cpu->ir = cpu->mem->frames[frame].pos[offset];

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

        // -----  F A S E   D E   E X E C U Ç Ã O   D A   I N S T R U Ç Ã O  -----
        // Conforme o opcode (código de operação) executa a ação correspondente
        // Cada instrução segue a semântica definida na tabela de instruções
        switch (cpu->ir.opc) {
            // Memoria / dados
            case LDI:
                cpu->reg[cpu->ir.ra] = cpu->ir.p;
                cpu->pc++;
                break;
            case LDD:
                {
                    int endereco_fisico = traducao_endereco(cpu, cpu->ir.p);
                    if (endereco_fisico >= 0) {
                        int frame = endereco_fisico / cpu->mem->pg_size;
                        int offset = endereco_fisico % cpu->mem->pg_size;
                        cpu->reg[cpu->ir.ra] = cpu->mem->frames[frame].pos[offset].p;
                        cpu->pc++;
                    }
                }
                break;
            case LDX:
                {
                    int endereco_logico = cpu->reg[cpu->ir.rb];
                    int endereco_fisico = traducao_endereco(cpu, endereco_logico);
                    if (endereco_fisico >= 0) {
                        int frame = endereco_fisico / cpu->mem->pg_size;
                        int offset = endereco_fisico % cpu->mem->pg_size;
                        cpu->reg[cpu->ir.ra] = cpu->mem->frames[frame].pos[offset].p;
                        cpu->pc++;
                    }
                }
                break;
            case STD:
                {
                    int endereco_fisico = traducao_endereco(cpu, cpu->ir.p);
                    if (endereco_fisico >= 0) {
                        int frame = endereco_fisico / cpu->mem->pg_size;
                        int offset = endereco_fisico % cpu->mem->pg_size;
                        cpu->mem->frames[frame].pos[offset].opc = DATA;
                        cpu->mem->frames[frame].pos[offset].p = cpu->reg[cpu->ir.ra];
                        cpu->pc++;
                    }
                }
                break;
            case STX:
                {
                    int endereco_logico = cpu->reg[cpu->ir.ra];
                    int endereco_fisico = traducao_endereco(cpu, endereco_logico);
                    if (endereco_fisico >= 0) {
                        int frame = endereco_fisico / cpu->mem->pg_size;
                        int offset = endereco_fisico % cpu->mem->pg_size;
                        cpu->mem->frames[frame].pos[offset].opc = DATA;
                        cpu->mem->frames[frame].pos[offset].p = cpu->reg[cpu->ir.rb];
                        cpu->pc++;
                    }
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
                {
                    int endereco_fisico = traducao_endereco(cpu, cpu->ir.p);
                    if (endereco_fisico >= 0) {
                        int frame = endereco_fisico / cpu->mem->pg_size;
                        int offset = endereco_fisico % cpu->mem->pg_size;
                        cpu->pc = cpu->mem->frames[frame].pos[offset].p;
                    }
                }
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
                {
                    int endereco_fisico = traducao_endereco(cpu, cpu->ir.p);
                    if (endereco_fisico >= 0) {
                        int frame = endereco_fisico / cpu->mem->pg_size;
                        int offset = endereco_fisico % cpu->mem->pg_size;
                        if (cpu->reg[cpu->ir.rb] > 0) {
                            cpu->pc = cpu->mem->frames[frame].pos[offset].p;
                        } else {
                            cpu->pc++;
                        }
                    }
                }
                break;
            case JMPILM:
                {
                    int endereco_fisico = traducao_endereco(cpu, cpu->ir.p);
                    if (endereco_fisico >= 0) {
                        int frame = endereco_fisico / cpu->mem->pg_size;
                        int offset = endereco_fisico % cpu->mem->pg_size;
                        if (cpu->reg[cpu->ir.rb] < 0) {
                            cpu->pc = cpu->mem->frames[frame].pos[offset].p;
                        } else {
                            cpu->pc++;
                        }
                    }
                }
                break;
            case JMPIEM:
                {
                    int endereco_fisico = traducao_endereco(cpu, cpu->ir.p);
                    if (endereco_fisico >= 0) {
                        int frame = endereco_fisico / cpu->mem->pg_size;
                        int offset = endereco_fisico % cpu->mem->pg_size;
                        if (cpu->reg[cpu->ir.rb] == 0) {
                            cpu->pc = cpu->mem->frames[frame].pos[offset].p;
                        } else {
                            cpu->pc++;
                        }
                    }
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

        // -----  V E R I F I C A Ç Ã O   D E   I N T E R R U P Ç Ã O  -----
        // TERCEIRA FASE DO CICLO DE INSTRUÇÕES
        // Se uma interrupção foi sinalizada durante a execução da instrução:
        //    - Desvia para a rotina de tratamento (função ih do SO)
        //    - Para a execução da CPU (nesta versão)
        if (cpu->irpt != noInterrupt) {
            if (cpu->ih) {
                cpu->ih(cpu, cpu->irpt);
            }
            cpu->cpuStop = 1;
        }
    }
}
