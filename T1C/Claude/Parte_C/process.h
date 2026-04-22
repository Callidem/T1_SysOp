// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
// T1-B: Gerenciador de Processos
//
// -----  G E R E N C I A D O R   D E   P R O C E S S O S  -----
// Implementa suporte a múltiplos processos com:
//   - PCB (Process Control Block) para cada processo
//   - Criação/destruição de processos
//   - Save/Restore de contexto (context switch)
//   - Fila de processos prontos para execução

#ifndef PROCESS_H
#define PROCESS_H

#include "hardware.h"
#include "mem_mngr.h"

// ===== CONSTANTES DE ESTADO =====
#define PROC_NOVO        0
#define PROC_PRONTO      1
#define PROC_EXECUTANDO  2
#define PROC_BLOQUEADO   3
#define PROC_FINALIZADO  4

// ===== PCB (PROCESS CONTROL BLOCK) =====
// Estrutura que contém todo o contexto de um processo

typedef struct {
    // Identificação
    int id;                      // ID único do processo (0, 1, 2, ...)
    char name[64];               // Nome do programa/processo
    
    // Estado da CPU (contexto salvo)
    int pc_salvo;                // Program Counter salvo
    int regs_salvos[10];         // Registradores salvos
    
    // Estado do processo
    int estado;                  // Estado atual (NOVO, PRONTO, EXECUTANDO, BLOQUEADO, FINALIZADO)
    
    // Memória (integração com T1-A)
    int *tabela_paginas;         // Tabela de páginas deste processo
    int nro_paginas;             // Número de páginas alocadas
    int tamPg;                   // Tamanho da página em palavras
    
    // Timestamps (para T1-C)
    int tempo_criacao;           // Quando foi criado
    int tempo_inicio;            // Quando começou a executar
    int fatias_usadas;           // Quantas fatias de tempo foram usadas
    
} ProcessoControlBlock;

// ===== GERENCIADOR DE PROCESSOS =====
// Gerencia criação, remoção e execução de múltiplos processos

typedef struct {
    // Tabela de todos os processos
    ProcessoControlBlock *processos;   // Array dinâmico de PCBs
    int max_processos;                 // Capacidade máxima
    int nro_processos;                 // Quantidade de processos atual
    int proximo_id;                    // ID para próximo processo
    
    // Processo em execução
    int processo_executando;           // ID do processo atual (-1 se nenhum)
    
    // Fila de prontos
    int *fila_pronto;                  // Array com IDs de processos prontos
    int inicio_fila;                   // Início da fila circular
    int fim_fila;                      // Fim da fila circular
    int tamanho_fila;                  // Quantidade de processos na fila
    
    // Integração com T1-A
    Mem_Mngr *gm;                      // Pointer para gerenciador de memória
    
} GerenciadorProcessos;

// ===== FUNÇÕES DE GERENCIAMENTO =====

// Inicializar gerenciador de processos
void gp_init(GerenciadorProcessos *gp, int max_procs, Mem_Mngr *gm);

// Liberar recursos do gerenciador
void gp_free(GerenciadorProcessos *gp);

// Criar novo processo com um programa
// Retorna ID do processo, ou -1 se falhar
int gp_new(GerenciadorProcessos *gp, const char *nome_programa, int tamPg);

// Remover processo por ID
void gp_rm(GerenciadorProcessos *gp, int processo_id);

// Listar todos os processos
void gp_ps(GerenciadorProcessos *gp);

// Dumpar estado completo de um processo
void gp_dump(GerenciadorProcessos *gp, int processo_id);

// ===== FUNÇÕES DE CONTEXTO =====

// Salvar contexto da CPU no PCB
void save_context(CPU *cpu, ProcessoControlBlock *pcb);

// Restaurar contexto do PCB na CPU
void restore_context(ProcessoControlBlock *pcb, CPU *cpu);

// Trocar processo em execução (context switch)
void context_switch(GerenciadorProcessos *gp, CPU *cpu, 
                   int novo_processo_id);

// ===== FUNÇÕES DE FILA =====

// Adicionar processo à fila de prontos
void enqueue_pronto(GerenciadorProcessos *gp, int processo_id);

// Remover primeiro processo da fila de prontos
int dequeue_pronto(GerenciadorProcessos *gp);

// Ver primeiro processo da fila (sem remover)
ProcessoControlBlock* front_pronto(GerenciadorProcessos *gp);

// Verificar se fila está vazia
int is_fila_pronto_vazia(GerenciadorProcessos *gp);

#endif  // PROCESS_H
