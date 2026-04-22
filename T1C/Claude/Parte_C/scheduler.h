// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
// T1-C: Escalonador com Time-Slicing
//
// -----  E S C A L O N A D O R   D E   P R O C E S S O S  -----
// Implementa escalonamento de processos com:
//   - Time-slicing (fatias de tempo)
//   - Alternância automática entre processos
//   - Round-Robin scheduling
//   - Suporte a múltiplas threads (2 threads conforme especificação)

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

// ===== CONSTANTES =====
#define DELTA_TEMPO_FATIA 5      // Número de ciclos de CPU por fatia de tempo
#define TEMPO_MAXIMO_CPU 500     // Máximo de ciclos que o escalonador roda

// ===== ESTRUTURA DO ESCALONADOR =====

typedef struct {
    // Referência ao gerenciador de processos
    GerenciadorProcessos *gp;
    
    // Referência à CPU
    CPU *cpu;
    
    // Controle de tempo
    int tempo_total;              // Total de ciclos executados
    int ciclos_fatia_atual;       // Ciclos executados na fatia atual
    int fatia_tamanho;            // Tamanho de cada fatia de tempo
    
    // Processo em execução
    int processo_atual;           // ID do processo em execução
    
    // Estatísticas
    int num_context_switches;     // Número de trocas de contexto
    int num_fatias_executadas;    // Número de fatias de tempo executadas
    
} Escalonador;

// ===== FUNÇÕES DO ESCALONADOR =====

// Inicializar escalonador
void sch_init(Escalonador *sch, GerenciadorProcessos *gp, CPU *cpu, 
              int tamFatia);

// Finalizar escalonador (cleanup)
void sch_free(Escalonador *sch);

// Executar ciclo do escalonador (cpu_run + verificar time-slice)
void sch_cycle(Escalonador *sch);

// Verificar se precisa fazer context switch
int sch_need_context_switch(Escalonador *sch);

// Fazer context switch para próximo processo
void sch_context_switch_next(Escalonador *sch);

// Executar escalonador completo (até que não haja mais processos)
void sch_run(Escalonador *sch);

// Dumpar estatísticas do escalonador
void sch_dump_stats(Escalonador *sch);

// Dumpar estado atual do escalonador
void sch_dump_state(Escalonador *sch);

#endif  // SCHEDULER_H
