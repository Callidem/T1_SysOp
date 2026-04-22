// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
// T1-B: Gerenciador de Processos - Implementação
//
// Implementa suporte a múltiplos processos simultâneos

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "programs.h"
#include "utils.h"

// ===== INICIALIZAÇÃO =====

void gp_init(GerenciadorProcessos *gp, int max_procs, Mem_Mngr *gm) {
    gp->processos = (ProcessoControlBlock *)malloc(max_procs * sizeof(ProcessoControlBlock));
    if (!gp->processos) {
        fprintf(stderr, "Erro ao alocar tabela de processos\n");
        return;
    }
    
    gp->fila_pronto = (int *)malloc(max_procs * sizeof(int));
    if (!gp->fila_pronto) {
        fprintf(stderr, "Erro ao alocar fila de prontos\n");
        free(gp->processos);
        return;
    }
    
    gp->max_processos = max_procs;
    gp->nro_processos = 0;
    gp->proximo_id = 0;
    gp->processo_executando = -1;
    gp->inicio_fila = 0;
    gp->fim_fila = 0;
    gp->tamanho_fila = 0;
    gp->gm = gm;
    
    printf("[GP] Gerenciador de Processos inicializado: max=%d\n", max_procs);
}

void gp_free(GerenciadorProcessos *gp) {
    // Liberar memória de todos os processos
    for (int i = 0; i < gp->nro_processos; ++i) {
        ProcessoControlBlock *pcb = &gp->processos[i];
        if (pcb->tabela_paginas) {
            gm_free_process(gp->gm, pcb->tabela_paginas, pcb->nro_paginas);
            free(pcb->tabela_paginas);
        }
    }
    
    free(gp->processos);
    free(gp->fila_pronto);
    printf("[GP] Gerenciador de Processos finalizado\n");
}

// ===== GERENCIAMENTO DE PROCESSOS =====

int gp_new(GerenciadorProcessos *gp, const char *nome_programa, int tamPg) {
    // Verificar se há espaço
    if (gp->nro_processos >= gp->max_processos) {
        printf("[GP] Erro: Tabela de processos cheia\n");
        return -1;
    }
    
    // Procurar programa
    const Program *p = retrieve_program(nome_programa);
    if (!p) {
        printf("[GP] Erro: Programa '%s' não encontrado\n", nome_programa);
        return -1;
    }
    
    // Calcular número de páginas necessárias
    int nro_paginas = (p->size + tamPg - 1) / tamPg;
    
    // Alocar frames via gerenciador de memória
    if (!gm_alloc(gp->gm, p->size)) {
        printf("[GP] Erro: Memória insuficiente para alocar processo\n");
        return -1;
    }
    
    // Criar PCB
    ProcessoControlBlock *pcb = &gp->processos[gp->nro_processos];
    pcb->id = gp->proximo_id++;
    strcpy(pcb->name, nome_programa);
    pcb->estado = PROC_NOVO;
    pcb->pc_salvo = 0;
    pcb->nro_paginas = nro_paginas;
    pcb->tamPg = tamPg;
    pcb->tempo_criacao = 0;  // Seria um timestamp real em produção
    pcb->fatias_usadas = 0;
    
    // Alocar tabela de páginas
    pcb->tabela_paginas = (int *)malloc(nro_paginas * sizeof(int));
    if (!pcb->tabela_paginas) {
        printf("[GP] Erro ao alocar tabela de páginas\n");
        return -1;
    }
    
    // Inicializar tabela de páginas com frames alocados (First Fit simples)
    int paginas_alocadas = 0;
    for (int i = 0; i < gp->gm->nroFrames && paginas_alocadas < nro_paginas; ++i) {
        if (gp->gm->frameLivre[i]) {
            // Frame alocado para este processo
            pcb->tabela_paginas[paginas_alocadas] = i;
            paginas_alocadas++;
        }
    }
    
    // Inicializar registradores salvos
    for (int i = 0; i < 10; ++i) {
        pcb->regs_salvos[i] = 0;
    }
    
    // Carregar programa na memória com paginação
    // Aqui usamos a tabela de páginas do processo
    printf("[GP] Carregando programa '%s' para processo %d\n", nome_programa, pcb->id);
    gm_load_program(gp->gm, pcb->tabela_paginas, pcb->nro_paginas, 
                   pcb->tamPg, p->image, p->size);
    
    // Adicionar à fila de prontos
    pcb->estado = PROC_PRONTO;
    enqueue_pronto(gp, pcb->id);
    
    gp->nro_processos++;
    
    printf("[GP] Processo %d criado: '%s' (páginas=%d, estado=PRONTO)\n", 
           pcb->id, nome_programa, nro_paginas);
    
    return pcb->id;
}

void gp_rm(GerenciadorProcessos *gp, int processo_id) {
    // Procurar processo
    ProcessoControlBlock *pcb = NULL;
    int idx = -1;
    
    for (int i = 0; i < gp->nro_processos; ++i) {
        if (gp->processos[i].id == processo_id) {
            pcb = &gp->processos[i];
            idx = i;
            break;
        }
    }
    
    if (!pcb) {
        printf("[GP] Erro: Processo %d não encontrado\n", processo_id);
        return;
    }
    
    // Se é o processo em execução, parar
    if (gp->processo_executando == processo_id) {
        gp->processo_executando = -1;
    }
    
    // Remover da fila se está lá
    for (int i = gp->inicio_fila; i < gp->tamanho_fila; ++i) {
        int pos = (gp->inicio_fila + i) % gp->max_processos;
        if (gp->fila_pronto[pos] == processo_id) {
            // Remove movimentando
            for (int j = i; j < gp->tamanho_fila - 1; ++j) {
                int p1 = (gp->inicio_fila + j) % gp->max_processos;
                int p2 = (gp->inicio_fila + j + 1) % gp->max_processos;
                gp->fila_pronto[p1] = gp->fila_pronto[p2];
            }
            gp->tamanho_fila--;
            break;
        }
    }
    
    // Liberar memória
    if (pcb->tabela_paginas) {
        gm_free_process(gp->gm, pcb->tabela_paginas, pcb->nro_paginas);
        free(pcb->tabela_paginas);
    }
    
    // Remover da tabela (move último para posição deletada)
    if (idx < gp->nro_processos - 1) {
        gp->processos[idx] = gp->processos[gp->nro_processos - 1];
    }
    gp->nro_processos--;
    
    printf("[GP] Processo %d removido\n", processo_id);
}

void gp_ps(GerenciadorProcessos *gp) {
    printf("\n");
    printf("===== TABELA DE PROCESSOS =====\n");
    printf("%-5s %-20s %-15s %-5s %-10s\n", 
           "PID", "Nome", "Estado", "PC", "Regs[0]");
    printf("%-5s %-20s %-15s %-5s %-10s\n", 
           "---", "----", "-----", "--", "-------");
    
    for (int i = 0; i < gp->nro_processos; ++i) {
        ProcessoControlBlock *pcb = &gp->processos[i];
        const char *estado_str = "";
        
        switch (pcb->estado) {
            case PROC_NOVO:        estado_str = "NOVO";       break;
            case PROC_PRONTO:      estado_str = "PRONTO";     break;
            case PROC_EXECUTANDO:  estado_str = "EXECUTANDO"; break;
            case PROC_BLOQUEADO:   estado_str = "BLOQUEADO";  break;
            case PROC_FINALIZADO:  estado_str = "FINALIZADO"; break;
            default:               estado_str = "DESCONHECIDO"; break;
        }
        
        printf("%-5d %-20s %-15s %-5d %-10d\n",
               pcb->id, pcb->name, estado_str, pcb->pc_salvo, pcb->regs_salvos[0]);
    }
    
    printf("\nProcesso em execução: ");
    if (gp->processo_executando == -1) {
        printf("NENHUM\n");
    } else {
        printf("PID %d\n", gp->processo_executando);
    }
    
    printf("\nFila de prontos: ");
    if (is_fila_pronto_vazia(gp)) {
        printf("VAZIA\n");
    } else {
        printf("[");
        for (int i = 0; i < gp->tamanho_fila; ++i) {
            int pos = (gp->inicio_fila + i) % gp->max_processos;
            if (i > 0) printf(", ");
            printf("%d", gp->fila_pronto[pos]);
        }
        printf("]\n");
    }
    printf("\n");
}

void gp_dump(GerenciadorProcessos *gp, int processo_id) {
    ProcessoControlBlock *pcb = NULL;
    
    for (int i = 0; i < gp->nro_processos; ++i) {
        if (gp->processos[i].id == processo_id) {
            pcb = &gp->processos[i];
            break;
        }
    }
    
    if (!pcb) {
        printf("[GP] Erro: Processo %d não encontrado\n", processo_id);
        return;
    }
    
    const char *estado_str = "";
    switch (pcb->estado) {
        case PROC_NOVO:        estado_str = "NOVO";       break;
        case PROC_PRONTO:      estado_str = "PRONTO";     break;
        case PROC_EXECUTANDO:  estado_str = "EXECUTANDO"; break;
        case PROC_BLOQUEADO:   estado_str = "BLOQUEADO";  break;
        case PROC_FINALIZADO:  estado_str = "FINALIZADO"; break;
        default:               estado_str = "DESCONHECIDO"; break;
    }
    
    printf("\n");
    printf("===== PCB DO PROCESSO %d =====\n", processo_id);
    printf("Nome:               %s\n", pcb->name);
    printf("Estado:             %s\n", estado_str);
    printf("PC salvo:           %d\n", pcb->pc_salvo);
    printf("Páginas:            %d (tamanho=%d)\n", pcb->nro_paginas, pcb->tamPg);
    printf("Fatias usadas:      %d\n", pcb->fatias_usadas);
    
    printf("Registradores salvos:\n");
    for (int i = 0; i < 10; ++i) {
        printf("  r[%d] = %d\n", i, pcb->regs_salvos[i]);
    }
    
    if (pcb->tabela_paginas) {
        printf("Tabela de páginas: [");
        for (int i = 0; i < pcb->nro_paginas; ++i) {
            if (i > 0) printf(", ");
            printf("frame=%d", pcb->tabela_paginas[i]);
        }
        printf("]\n");
    }
    printf("\n");
}

// ===== CONTEXT SWITCH =====

void save_context(CPU *cpu, ProcessoControlBlock *pcb) {
    if (!pcb) return;
    
    // Salvar PC
    pcb->pc_salvo = cpu->pc;
    
    // Salvar registradores
    for (int i = 0; i < 10; ++i) {
        pcb->regs_salvos[i] = cpu->reg[i];
    }
}

void restore_context(ProcessoControlBlock *pcb, CPU *cpu) {
    if (!pcb) return;
    
    // Restaurar PC
    cpu->pc = pcb->pc_salvo;
    
    // Restaurar registradores
    for (int i = 0; i < 10; ++i) {
        cpu->reg[i] = pcb->regs_salvos[i];
    }
}

void context_switch(GerenciadorProcessos *gp, CPU *cpu, int novo_processo_id) {
    // Salvar contexto do processo anterior
    if (gp->processo_executando != -1) {
        for (int i = 0; i < gp->nro_processos; ++i) {
            if (gp->processos[i].id == gp->processo_executando) {
                save_context(cpu, &gp->processos[i]);
                gp->processos[i].estado = PROC_PRONTO;
                break;
            }
        }
    }
    
    // Restaurar contexto do novo processo
    ProcessoControlBlock *novo_pcb = NULL;
    for (int i = 0; i < gp->nro_processos; ++i) {
        if (gp->processos[i].id == novo_processo_id) {
            novo_pcb = &gp->processos[i];
            restore_context(novo_pcb, cpu);
            novo_pcb->estado = PROC_EXECUTANDO;
            novo_pcb->fatias_usadas++;
            break;
        }
    }
    
    gp->processo_executando = novo_processo_id;
    
    // Atualizar tabela de páginas da CPU
    if (novo_pcb) {
        cpu->tabela_paginas = novo_pcb->tabela_paginas;
        cpu->nro_paginas = novo_pcb->nro_paginas;
        cpu->processo_id = novo_processo_id;
    }
}

// ===== FILA DE PRONTOS =====

void enqueue_pronto(GerenciadorProcessos *gp, int processo_id) {
    if (gp->tamanho_fila >= gp->max_processos) {
        printf("[GP] Erro: Fila de prontos cheia\n");
        return;
    }
    
    int pos = (gp->inicio_fila + gp->tamanho_fila) % gp->max_processos;
    gp->fila_pronto[pos] = processo_id;
    gp->tamanho_fila++;
}

int dequeue_pronto(GerenciadorProcessos *gp) {
    if (is_fila_pronto_vazia(gp)) {
        return -1;
    }
    
    int processo_id = gp->fila_pronto[gp->inicio_fila];
    gp->inicio_fila = (gp->inicio_fila + 1) % gp->max_processos;
    gp->tamanho_fila--;
    
    return processo_id;
}

ProcessoControlBlock* front_pronto(GerenciadorProcessos *gp) {
    if (is_fila_pronto_vazia(gp)) {
        return NULL;
    }
    
    int processo_id = gp->fila_pronto[gp->inicio_fila];
    
    for (int i = 0; i < gp->nro_processos; ++i) {
        if (gp->processos[i].id == processo_id) {
            return &gp->processos[i];
        }
    }
    
    return NULL;
}

int is_fila_pronto_vazia(GerenciadorProcessos *gp) {
    return gp->tamanho_fila == 0;
}
