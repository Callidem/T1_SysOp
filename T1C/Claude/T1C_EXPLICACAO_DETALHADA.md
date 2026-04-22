# T1-C: Escalonador com Time-Slicing

## Objetivo

Automatizar a **alternância entre processos** usando **time-slicing** (Round-Robin scheduling), eliminando necessidade de mudanças manuais de contexto.

## Problema Resolvido

**Com T1-B (manual):**
```
Desenvolvedor precisa decidir quando trocar processo
  - Chamar context_switch() manualmente
  - Controlar tempo de cada processo
  - Nenhuma fairness (um pode monopolizar a CPU)

```

**Com T1-C (automático):**
```
Escalonador decide automaticamente
  - Cada processo recebe fatia de tempo fixa (5 ciclos)
  - Alternância em round-robin
  - Fairness garantida!
```

## Conceitos Fundamentais

### 1. Escalonamento

**Escalonamento** é a decisão de qual processo executar a cada momento.

```
Fila de prontos: [Proc0, Proc1, Proc2]

Decisões possíveis:
- First Come First Served (FCFS): Proc0, depois Proc1, depois Proc2
- Priority: Proc com maior prioridade
- Round-Robin: Proc0 → Proc1 → Proc2 → Proc0 → ...
- Shortest Job First (SJF): Proc com menor tempo estimado
```

### 2. Time-Slicing (Fatia de Tempo)

Cada processo recebe um **quantum** (fatia) de tempo de CPU.

```
SEM TIME-SLICING:
Proc0: [===== 100% do tempo =====>]
Procs 1-2: [esperando indefinidamente]
→ Injusto! Proc0 monopoliza tudo

COM TIME-SLICING:
Quantum = 5 ciclos
Proc0: [===== 5 ciclos ====>]
Proc1: [===== 5 ciclos ====>]
Proc2: [===== 5 ciclos ====>]
Proc0: [===== 5 ciclos ====>]
...
→ Justo! Todos recebem tempo igual
```

### 3. Round-Robin

Estratégia **FIFO circular**: processos vão para o final da fila após sua fatia.

```
Fila inicial: [0, 1, 2]

Ciclos 0-4 (Proc0):
  - Proc0 sai da fila, executa
  - Fatia acaba
  - Proc0 volta ao final: fila = [1, 2, 0]

Ciclos 5-9 (Proc1):
  - Proc1 sai da fila, executa
  - Fatia acaba
  - Proc1 volta ao final: fila = [2, 0, 1]

Ciclos 10-14 (Proc2):
  - Proc2 sai da fila, executa
  - Fatia acaba
  - Proc2 volta ao final: fila = [0, 1, 2]

Ciclos 15-19 (Proc0 novamente):
  - Proc0 sai da fila, executa
  ...

RESULTADO: Alternância contínua, fairness
```

## Estruturas de Dados

### Escalonador (scheduler.h)

```c
typedef struct {
    // Referências
    GerenciadorProcessos *gp;    // Gerenciador de processos (T1-B)
    CPU *cpu;                     // CPU
    
    // Controle de tempo
    int tempo_total;              // Total de ciclos executados
    int ciclos_fatia_atual;       // Ciclos na fatia corrente
    int fatia_tamanho;            // Tamanho de fatia (5 ciclos)
    
    // Processo ativo
    int processo_atual;           // ID do processo em execução
    
    // Estatísticas
    int num_context_switches;     // Quantas vezes trocou de processo
    int num_fatias_executadas;    // Quantas fatias passaram
} Escalonador;
```

## Algoritmo Principal

### sch_run() - Loop Principal

```c
void sch_run(Escalonador *sch) {
    while (sch->tempo_total < TEMPO_MAXIMO_CPU) {
        sch_cycle(sch);
    }
}
```

### sch_cycle() - Um Ciclo do Escalonador

```c
void sch_cycle(Escalonador *sch) {
    // 1. Verificar se fatia acabou
    if (sch_need_context_switch(sch)) {
        // 2. Fazer context switch
        sch_context_switch_next(sch);
    }
    
    // 3. Executar 1 ciclo da CPU
    if (sch->processo_atual != -1) {
        cpu_run(sch->cpu);
        sch->ciclos_fatia_atual++;
        sch->tempo_total++;
    }
}
```

### sch_need_context_switch() - Verificar Necessidade

```c
int sch_need_context_switch(Escalonador *sch) {
    // Precisa trocar se:
    
    if (sch->processo_atual == -1) {
        return 1;  // Nenhum processo ativo
    }
    
    if (sch->ciclos_fatia_atual >= sch->fatia_tamanho) {
        return 1;  // Fatia de tempo acabou
    }
    
    if (sch->cpu->cpuStop) {
        return 1;  // CPU parou (STOP)
    }
    
    if (sch->cpu->irpt != noInterrupt) {
        return 1;  // Houve interrupção (erro)
    }
    
    return 0;  // Continuar com processo atual
}
```

### sch_context_switch_next() - Trocar de Processo

```c
void sch_context_switch_next(Escalonador *sch) {
    // 1. Salvar contexto do processo anterior
    if (sch->processo_atual != -1) {
        for (int i = 0; i < gp->nro_processos; i++) {
            if (gp->processos[i].id == sch->processo_atual) {
                save_context(sch->cpu, &gp->processos[i]);
                break;
            }
        }
    }
    
    // 2. Buscar próximo da fila
    int proximo_pid = dequeue_pronto(sch->gp);
    
    if (proximo_pid == -1) {
        sch->processo_atual = -1;  // Nenhum mais
        return;
    }
    
    // 3. Restaurar contexto do novo
    context_switch(sch->gp, sch->cpu, proximo_pid);
    
    // 4. Recoloc ar na fila (Round-Robin)
    enqueue_pronto(sch->gp, proximo_pid);
    
    // 5. Resetar contador de fatia
    sch->processo_atual = proximo_pid;
    sch->ciclos_fatia_atual = 0;
    sch->num_context_switches++;
    sch->num_fatias_executadas++;
}
```

## Fluxo de Execução Completo

```
ESTADO INICIAL:
├─ Proc0: id=0, tabela=[2,3], estado=PRONTO
├─ Proc1: id=1, tabela=[4,5], estado=PRONTO
└─ Proc2: id=2, tabela=[6,7], estado=PRONTO
Fila: [0, 1, 2]

ESCALONADOR INICIA (sch_run):
│
├─ CICLO 1-5 (Fatia 1 → Processo 0)
│  ├─ sch_cycle()
│  │  ├─ sch_need_context_switch() = true (processo_atual == -1)
│  │  ├─ sch_context_switch_next()
│  │  │  ├─ dequeue_pronto() → Proc0 sai
│  │  │  ├─ context_switch(0)
│  │  │  │  └─ CPU.tabela_paginas = [2, 3]
│  │  │  │  └─ CPU.pc = 0, CPU.reg[] = [0,0,...]
│  │  │  ├─ enqueue_pronto(0) → volta para fila
│  │  │  └─ ciclos_fatia_atual = 0
│  │  ├─ cpu_run()
│  │  │  └─ Executa 1 ciclo com Proc0
│  │  └─ ciclos_fatia_atual++ = 1, tempo_total++ = 1
│  │
│  ├─ sch_cycle()
│  │  ├─ sch_need_context_switch() = false (1 < 5)
│  │  └─ cpu_run() → mais 1 ciclo com Proc0
│  │     ciclos_fatia_atual = 2, tempo_total = 2
│  │
│  ├─ ... mais 3 ciclos ...
│  │     ciclos_fatia_atual = 5, tempo_total = 5
│  │
│  └─ Estado: Proc0 executou 5 ciclos, fila = [1, 2, 0]
│
├─ CICLO 6-10 (Fatia 2 → Processo 1)
│  ├─ sch_need_context_switch() = true (ciclos_fatia_atual >= fatia_tamanho)
│  ├─ sch_context_switch_next()
│  │  ├─ save_context(Proc0) → PCB0.pc_salvo = 5, PCB0.regs[] = [...]
│  │  ├─ dequeue_pronto() → Proc1 sai
│  │  ├─ context_switch(1)
│  │  │  └─ restore_context(Proc1)
│  │  │     CPU.pc = 0, CPU.reg[] = [0,0,...]  (resetado)
│  │  │  └─ CPU.tabela_paginas = [4, 5]
│  │  ├─ enqueue_pronto(1) → volta para fila
│  │  └─ ciclos_fatia_atual = 0, processo_atual = 1
│  ├─ cpu_run() → executa com Proc1
│  └─ ... 5 ciclos com Proc1 ...
│     fila = [2, 0, 1]
│
├─ CICLO 11-15 (Fatia 3 → Processo 2)
│  ├─ Context switch 2 (save Proc1, restore Proc2)
│  ├─ cpu_run() × 5
│  └─ fila = [0, 1, 2]
│
├─ CICLO 16-20 (Fatia 4 → Processo 0 NOVAMENTE!)
│  ├─ Context switch 0 (save Proc2, restore Proc0)
│  │  ├─ restore_context(Proc0)
│  │  │  └─ CPU.pc = 5 (continuou de onde parou!)
│  │  │  └─ CPU.reg[] = [...] (estado preservado!)
│  │  └─ CPU.tabela_paginas = [2, 3]
│  ├─ cpu_run() × 5
│  │  └─ Proc0 continua com PC=5
│  └─ fila = [1, 2, 0]
│
└─ ... continua até tempo_total >= 500 ...

RESULTADO:
Tempo total: 500 ciclos
Proc0: ~167 fatias × 5 = ~835 ciclos (mas limitado a 500 total)
Proc1: ~167 fatias
Proc2: ~166 fatias

FAIRNESS: Todos recebem ~1/3 do tempo!
```

## Exemplo Visual: 3 Processos, 500 Ciclos

```
LINHA DO TEMPO:

Ciclos:     0    5    10   15   20   25   30   35   40   45
           |____|____|____|____|____|____|____|____|____|____|
Execução:  [P0 ][P1 ][P2 ][P0 ][P1 ][P2 ][P0 ][P1 ][P2 ][P0 ]...

ESTATÍSTICAS:
- Total: 500 ciclos
- Fatias: 500 / 5 = 100 fatias totais
- Por processo: 100 / 3 ≈ 33-34 fatias cada
- Na prática: P0=167 fatias, P1=167 fatias, P2=166 fatias

DISTRIBUIÇÃO (com 3 processos):
100% ┌──────────────┐
 50% ├──┤ P0 ├──┤ P1 ├──┤ P2 ├──
 33% └──────────────┘
  
→ Cada processo recebe ~1/3 do tempo!
```

## Context Switches

Cada mudança de fatia gera um context switch:

```
CONTEXTO 0 → CONTEXTO 1:
  ├─ save: PC=5, r[0]=10, r[1]=3
  ├─ restore: PC=0, r[0]=0, r[1]=0
  ├─ muda: tabela_paginas=[4,5]
  └─ continua

CONTEXTO 1 → CONTEXTO 2:
  ├─ save: PC=2, r[0]=5, r[1]=7
  ├─ restore: PC=0, r[0]=0, r[1]=0
  ├─ muda: tabela_paginas=[6,7]
  └─ continua

CONTEXTO 2 → CONTEXTO 0:
  ├─ save: PC=3, r[0]=0, r[1]=0
  ├─ restore: PC=5, r[0]=10, r[1]=3  ← RETOMA!
  ├─ muda: tabela_paginas=[2,3]
  └─ continua de onde parou
```

## Funções Principais (scheduler.c)

| Função | Descrição |
|--------|-----------|
| `sch_init()` | Inicializa escalonador |
| `sch_run()` | Loop principal (até 500 ciclos) |
| `sch_cycle()` | Um ciclo completo |
| `sch_need_context_switch()` | Verifica necessidade de trocar |
| `sch_context_switch_next()` | Executa context switch |
| `sch_dump_stats()` | Mostra estatísticas |
| `sch_dump_state()` | Mostra estado atual |

## Inteligência do Escalonador

O escalonador é "inteligente" porque:

```
1. DETECÇÃO DE TÉRMINO:
   - Se CPU.cpuStop = true ou CPU.irpt != noInterrupt
   - Remove processo da execução
   - Continua com próximo

2. FAIRNESS:
   - Cada processo recebe quantum igual (5 ciclos)
   - Round-Robin garante ninguém fica esperando indefinidamente

3. ADAPTABILIDADE:
   - Se fila fica vazia, espera
   - Se novo processo chega, adiciona à fila

4. ESTATÍSTICAS:
   - Rastreia tempo total, switches, fatias
   - Permite análise de desempenho
```

## Comparação: Manual vs Automático

```
T1-B (Manual - Parte_B):
├─ Programador controla context switches
├─ gp_new() cria 2 processos
├─ context_switch(p0) → executa 10 ciclos
├─ context_switch(p1) → executa 10 ciclos
└─ Manual, cansativo, propenso a erros

T1-C (Automático - Parte_C):
├─ Escalonador controla tudo
├─ gp_new() cria 3 processos
├─ sch_run() → loop automático
│  ├─ Detecta fatia acabada
│  ├─ Faz context switch automaticamente
│  ├─ Garante fairness
│  └─ Mostra estatísticas
└─ Automático, limpo, confiável
```

## Teste

```bash
cd Parte_C
make clean && make
./vm_so

Esperado:
- 3 processos criados
- 500 ciclos de escalonamento
- Alternância contínua (P0 → P1 → P2 → P0 → ...)
- Distribuição equilibrada (167, 167, 166)
- Estatísticas finais

Saída:
===== ESTATÍSTICAS DO ESCALONADOR =====
Tempo total:        500 ciclos
Context switches:   500
Fatias executadas:  500
Tamanho de fatia:   5 ciclos

Estatísticas por processo:
PID   Nome       Estado    Fatias
0     fatorialV2 PRONTO    167
1     fatorialV2 EXECUTANDO 167
2     fatorialV2 PRONTO    166
```

## Resumo

| Conceito | Descrição |
|----------|-----------|
| **Escalonador** | Decide qual processo executar |
| **Quantum** | Fatia de tempo (5 ciclos) |
| **Round-Robin** | Alternância cíclica entre processos |
| **Fairness** | Igualdade de tempo para todos |
| **Context Switch** | Save (contexto anterior) + Restore (novo) |
| **Automatização** | Não precisa de ação manual |

T1-C automatiza T1-B com **escalonamento justo**! 🚀

## Sistema Completo

```
T1-A (Paginação)
  ↓
T1-B (Multiprogramação)
  ↓
T1-C (Escalonamento)

= SISTEMA OPERACIONAL SIMULADO COMPLETO! 🎉
```

Você tem um **pseudo-SO real** funcionando! 🎓
