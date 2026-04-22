// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
// T1-C: Escalonador - Implementação
//
// Implementa escalonamento round-robin com time-slicing

#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"

// ===== INICIALIZAÇÃO =====

void sch_init(Escalonador *sch, GerenciadorProcessos *gp, CPU *cpu,
              int tamFatia) {
    sch->gp = gp;
    sch->cpu = cpu;
    sch->tempo_total = 0;
    sch->ciclos_fatia_atual = 0;
    sch->fatia_tamanho = tamFatia > 0 ? tamFatia : DELTA_TEMPO_FATIA;
    sch->processo_atual = -1;
    sch->num_context_switches = 0;
    sch->num_fatias_executadas = 0;
    
    printf("[SCH] Escalonador inicializado: tamFatia=%d ciclos\n", sch->fatia_tamanho);
}

void sch_free(Escalonador *sch) {
    printf("[SCH] Escalonador finalizado\n");
}

// ===== VERIFICAÇÃO DE TIME-SLICE =====

int sch_need_context_switch(Escalonador *sch) {
    // Precisa trocar processo se:
    // 1. Nenhum processo está em execução
    // 2. Fatia de tempo acabou
    // 3. Processo terminou (STOP)
    // 4. Processo está bloqueado (por erro)
    
    if (sch->processo_atual == -1) {
        return 1;  // Nenhum processo, precisa selecionar um
    }
    
    // Verificar se fatia de tempo terminou
    if (sch->ciclos_fatia_atual >= sch->fatia_tamanho) {
        return 1;
    }
    
    // Verificar se CPU parou (STOP ou erro)
    if (sch->cpu->cpuStop || sch->cpu->irpt != noInterrupt) {
        return 1;
    }
    
    return 0;
}

// ===== CONTEXT SWITCH =====

void sch_context_switch_next(Escalonador *sch) {
    // Se há um processo em execução, salvar seu contexto
    if (sch->processo_atual != -1) {
        for (int i = 0; i < sch->gp->nro_processos; ++i) {
            if (sch->gp->processos[i].id == sch->processo_atual) {
                save_context(sch->cpu, &sch->gp->processos[i]);
                break;
            }
        }
    }
    
    // Buscar próximo processo pronto
    if (is_fila_pronto_vazia(sch->gp)) {
        printf("[SCH] Nenhum processo pronto para execução\n");
        sch->processo_atual = -1;
        return;
    }
    
    // Pegar próximo processo da fila
    int proximo_pid = dequeue_pronto(sch->gp);
    
    if (proximo_pid == -1) {
        printf("[SCH] Erro ao pegar processo da fila\n");
        sch->processo_atual = -1;
        return;
    }
    
    // Restaurar contexto do novo processo
    context_switch(sch->gp, sch->cpu, proximo_pid);
    
    // Se há outro processo, re-adicionar à fila (Round-Robin)
    if (!is_fila_pronto_vazia(sch->gp) || sch->gp->nro_processos > 1) {
        enqueue_pronto(sch->gp, proximo_pid);
    }
    
    sch->processo_atual = proximo_pid;
    sch->ciclos_fatia_atual = 0;  // Resetar contador de fatia
    sch->num_context_switches++;
    sch->num_fatias_executadas++;
    
    printf("[SCH] Context Switch para processo %d (fatia %d)\n", 
           proximo_pid, sch->num_fatias_executadas);
}

// ===== CICLO DE EXECUÇÃO =====

void sch_cycle(Escalonador *sch) {
    // Verificar se precisa fazer context switch
    if (sch_need_context_switch(sch)) {
        // Se CPU parou, remover processo
        if (sch->cpu->cpuStop || sch->cpu->irpt != noInterrupt) {
            if (sch->processo_atual != -1) {
                printf("[SCH] Processo %d terminado (STOP ou erro)\n", sch->processo_atual);
                
                // Encontrar e remover processo
                for (int i = 0; i < sch->gp->nro_processos; ++i) {
                    if (sch->gp->processos[i].id == sch->processo_atual) {
                        sch->gp->processos[i].estado = PROC_FINALIZADO;
                        break;
                    }
                }
                
                sch->processo_atual = -1;
                sch->cpu->cpuStop = 0;  // Resetar flag
                sch->cpu->irpt = noInterrupt;  // Resetar interrupção
            }
        }
        
        // Fazer context switch
        sch_context_switch_next(sch);
    }
    
    // Se há processo para executar, executar um ciclo
    if (sch->processo_atual != -1) {
        cpu_run(sch->cpu);
        sch->ciclos_fatia_atual++;
        sch->tempo_total++;
    }
}

// ===== EXECUÇÃO COMPLETA =====

void sch_run(Escalonador *sch) {
    printf("\n");
    printf("################################\n");
    printf("# T1-C: ESCALONADOR COM TIME-SLICING\n");
    printf("################################\n");
    printf("\n");
    printf("[SCH] Iniciando execução de %d processo(s)\n", sch->gp->nro_processos);
    printf("[SCH] Tamanho de fatia: %d ciclos\n", sch->fatia_tamanho);
    printf("\n");
    
    // Executar até que não haja mais processos prontos
    int ciclos_parado = 0;  // Contador de ciclos sem progresso
    
    while (sch->tempo_total < TEMPO_MAXIMO_CPU && ciclos_parado < 10) {
        sch_cycle(sch);
        
        // Se nenhum processo executou, incrementar contador
        if (sch->processo_atual == -1) {
            ciclos_parado++;
        } else {
            ciclos_parado = 0;
        }
    }
    
    printf("\n");
    printf("[SCH] Escalonamento finalizado\n");
    sch_dump_stats(sch);
    printf("\n");
}

// ===== DUMP E ESTATÍSTICAS =====

void sch_dump_stats(Escalonador *sch) {
    printf("\n");
    printf("===== ESTATÍSTICAS DO ESCALONADOR =====\n");
    printf("Tempo total:              %d ciclos\n", sch->tempo_total);
    printf("Context switches:         %d\n", sch->num_context_switches);
    printf("Fatias executadas:        %d\n", sch->num_fatias_executadas);
    printf("Tamanho de fatia:         %d ciclos\n", sch->fatia_tamanho);
    printf("\n");
    
    // Mostrar estatísticas por processo
    printf("Estatísticas por processo:\n");
    printf("%-5s %-20s %-15s %-10s\n", "PID", "Nome", "Estado", "Fatias");
    printf("%-5s %-20s %-15s %-10s\n", "---", "----", "-----", "------");
    
    for (int i = 0; i < sch->gp->nro_processos; ++i) {
        ProcessoControlBlock *pcb = &sch->gp->processos[i];
        const char *estado_str = "";
        
        switch (pcb->estado) {
            case PROC_NOVO:        estado_str = "NOVO";       break;
            case PROC_PRONTO:      estado_str = "PRONTO";     break;
            case PROC_EXECUTANDO:  estado_str = "EXECUTANDO"; break;
            case PROC_BLOQUEADO:   estado_str = "BLOQUEADO";  break;
            case PROC_FINALIZADO:  estado_str = "FINALIZADO"; break;
            default:               estado_str = "DESCONHECIDO"; break;
        }
        
        printf("%-5d %-20s %-15s %-10d\n",
               pcb->id, pcb->name, estado_str, pcb->fatias_usadas);
    }
    printf("\n");
}

void sch_dump_state(Escalonador *sch) {
    printf("\n");
    printf("===== ESTADO DO ESCALONADOR =====\n");
    printf("Tempo total:              %d ciclos\n", sch->tempo_total);
    printf("Ciclos nesta fatia:       %d/%d\n", sch->ciclos_fatia_atual, sch->fatia_tamanho);
    printf("Processo atual:           ");
    
    if (sch->processo_atual == -1) {
        printf("NENHUM\n");
    } else {
        printf("PID %d\n", sch->processo_atual);
    }
    
    printf("Context switches:         %d\n", sch->num_context_switches);
    printf("Fatias executadas:        %d\n", sch->num_fatias_executadas);
    printf("\n");
    
    // Mostrar fila de prontos
    printf("Fila de prontos: ");
    if (is_fila_pronto_vazia(sch->gp)) {
        printf("VAZIA\n");
    } else {
        printf("[");
        for (int i = 0; i < sch->gp->tamanho_fila; ++i) {
            int pos = (sch->gp->inicio_fila + i) % sch->gp->max_processos;
            if (i > 0) printf(", ");
            printf("%d", sch->gp->fila_pronto[pos]);
        }
        printf("]\n");
    }
    printf("\n");
}
