# Integração Completa: T1-A + T1-B + T1-C

## Visão Geral da Arquitetura

```
┌──────────────────────────────────────────────────┐
│       APLICAÇÃO (Programa do Usuário)            │
│          (fatorialV2.bin, soma.bin, ...)         │
└──────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────┐
│      CAMADA 3: ESCALONADOR (T1-C)                │
│  ┌────────────────────────────────────────────┐  │
│  │ Função: Automatizar context switches       │  │
│  │ Estratégia: Round-Robin (5 ciclos)         │  │
│  │ Resultado: Fairness entre processos        │  │
│  └────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────┐
│    CAMADA 2: GERENCIADOR DE PROCESSOS (T1-B)    │
│  ┌────────────────────────────────────────────┐  │
│  │ Função: Suportar multiprogramação          │  │
│  │ Estrutura: PCBs (Process Control Blocks)   │  │
│  │ Operação: Context Switch (save/restore)    │  │
│  │ Isolamento: Cada processo tem contexto     │  │
│  └────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────┐
│   CAMADA 1: GERENCIADOR DE MEMÓRIA (T1-A)       │
│  ┌────────────────────────────────────────────┐  │
│  │ Função: Prover memória virtual             │  │
│  │ Técnica: Paginação                         │  │
│  │ Proteção: Cada processo vê memória própria │  │
│  │ Benefício: Sem fragmentação, isolamento    │  │
│  └────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────┐
│    HARDWARE: CPU + Memória Física                │
│  ┌────────────────────────────────────────────┐  │
│  │ CPU: PC, Registradores, Controle          │  │
│  │ Memória: 1024 palavras = 64 frames         │  │
│  └────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────┘
```

## Como as 3 Camadas Funcionam Juntas

### Exemplo: 3 Processos Executando 30 Ciclos

```
TEMPO = 0 CICLOS:

INICIALIZAÇÃO:
├─ T1-A: Cria bitmap de frames, marca todos como livres
│  frameLivre = [T,T,T,T,...,T] (64 frames)
│
├─ T1-B: Cria gerenciador de processos
│  gp.nro_processos = 0
│  gp.processo_executando = -1
│
└─ T1-C: Cria escalonador
   sch.tempo_total = 0
   sch.num_context_switches = 0

CRIAÇÃO DE PROCESSO 0:
├─ T1-B: gp_new("fatorialV2")
│  │
│  ├─ T1-A: gm_alloc(20 palavras)
│  │  └─ Busca 2 frames livres → encontra frames 2 e 3
│  │  └─ frameLivre[2]=F, frameLivre[3]=F
│  │  └─ Retorna sucesso
│  │
│  ├─ T1-A: gm_load_program()
│  │  └─ Copia 20 palavras de fatorialV2.bin
│  │  └─ Para memoria[32..51] (frames 2-3)
│  │
│  ├─ T1-B: Cria ProcessoControlBlock
│  │  ├─ id = 0
│  │  ├─ tabela_paginas = [2, 3]
│  │  ├─ nro_paginas = 2
│  │  ├─ estado = PRONTO
│  │  └─ pc_salvo = 0, regs_salvos = [0,0,...]
│  │
│  └─ T1-B: Adiciona à fila de prontos
│     fila_pronto = [0]

CRIAÇÃO DE PROCESSO 1:
├─ T1-B: gp_new("fatorialV2")
│  ├─ T1-A: gm_alloc(20)
│  │  └─ Encontra frames 4 e 5
│  │  └─ frameLivre[4]=F, frameLivre[5]=F
│  │
│  ├─ T1-A: gm_load_program()
│  │  └─ Carrega em memoria[64..79] (frames 4-5)
│  │
│  ├─ T1-B: Cria PCB1
│  │  └─ tabela_paginas = [4, 5]
│  │
│  └─ T1-B: fila_pronto = [0, 1]

CRIAÇÃO DE PROCESSO 2:
└─ T1-B: gp_new("fatorialV2")
   ├─ T1-A: gm_alloc(20)
   │  └─ Encontra frames 6 e 7
   │  └─ frameLivre[6]=F, frameLivre[7]=F
   │
   ├─ T1-A: gm_load_program()
   │  └─ Carrega em memoria[96..111] (frames 6-7)
   │
   ├─ T1-B: Cria PCB2
   │  └─ tabela_paginas = [6, 7]
   │
   └─ T1-B: fila_pronto = [0, 1, 2]

ESTADO APÓS CRIAÇÃO:
┌─────────────────────────────────┐
│ T1-A: Memória                   │
│ ┌─────────────────────────────┐ │
│ │ Frames: [L][L][P0][P0][P1]  │ │
│ │         [P1][P2][P2][L]...  │ │
│ │ (P0=Proc0, P1=Proc1, etc)   │ │
│ └─────────────────────────────┘ │
│                                 │
│ T1-B: Processos                 │
│ ┌─────────────────────────────┐ │
│ │ PCB0: id=0, tab=[2,3]       │ │
│ │ PCB1: id=1, tab=[4,5]       │ │
│ │ PCB2: id=2, tab=[6,7]       │ │
│ │ Fila: [0, 1, 2]             │ │
│ └─────────────────────────────┘ │
│                                 │
│ T1-C: Escalonador               │
│ ┌─────────────────────────────┐ │
│ │ tempo_total = 0             │ │
│ │ ciclos_fatia_atual = 0      │ │
│ │ processo_atual = -1         │ │
│ │ fatia_tamanho = 5           │ │
│ └─────────────────────────────┘ │
└─────────────────────────────────┘


TEMPO = 0-4 CICLOS (Fatia 1):

sch_cycle():
├─ sch_need_context_switch()
│  └─ processo_atual == -1 → true (primeiro ciclo)
│
├─ sch_context_switch_next()
│  ├─ T1-B: dequeue_pronto()
│  │  └─ Tira processo 0 da fila
│  │  └─ fila_pronto = [1, 2]
│  │
│  ├─ T1-B: context_switch(Proc0)
│  │  ├─ restore_context(PCB0, CPU)
│  │  │  ├─ CPU.pc = 0 (começo)
│  │  │  └─ CPU.reg[0-9] = [0,0,...] (zerados)
│  │  │
│  │  ├─ T1-A: CPU.tabela_paginas = [2, 3]
│  │  │  └─ Agora CPU enxerga memória de Proc0!
│  │  │
│  │  └─ T1-B: gp.processo_executando = 0
│  │
│  ├─ T1-B: enqueue_pronto(Proc0)
│  │  └─ Volta para fila
│  │  └─ fila_pronto = [1, 2, 0]
│  │
│  ├─ T1-C: ciclos_fatia_atual = 0
│  └─ T1-C: processo_atual = 0
│
├─ CPU.run()
│  └─ Executa 1 instrução com Proc0
│     Obs: usa memoria[frame] onde frame vem de tabela_paginas!
│
└─ T1-C: ciclos_fatia_atual = 1, tempo_total = 1

... ciclos 1-3 similares ...

CICLO 4:
├─ ciclos_fatia_atual = 5 (atingiu tamanho de fatia)
└─ tempo_total = 5


TEMPO = 5-9 CICLOS (Fatia 2):

sch_cycle():
├─ sch_need_context_switch()
│  └─ ciclos_fatia_atual >= fatia_tamanho (5 >= 5) → true
│
├─ sch_context_switch_next()
│  ├─ T1-B: save_context(CPU, PCB0)
│  │  ├─ PCB0.pc_salvo = 5 (onde parou)
│  │  ├─ PCB0.regs_salvos[] = [...] (estado atual)
│  │  └─ Proc0 CONGELADO com snapshot
│  │
│  ├─ T1-B: dequeue_pronto()
│  │  └─ Tira processo 1 da fila
│  │  └─ fila_pronto = [2, 0]
│  │
│  ├─ T1-B: context_switch(Proc1)
│  │  ├─ restore_context(PCB1, CPU)
│  │  │  ├─ CPU.pc = 0 (resetado para Proc1)
│  │  │  └─ CPU.reg[0-9] = [0,0,...] (zerados para Proc1)
│  │  │
│  │  ├─ T1-A: CPU.tabela_paginas = [4, 5]
│  │  │  └─ Agora CPU enxerga memória de Proc1!
│  │  │
│  │  └─ T1-B: gp.processo_executando = 1
│  │
│  ├─ T1-B: enqueue_pronto(Proc1)
│  │  └─ fila_pronto = [2, 0, 1]
│  │
│  ├─ T1-C: ciclos_fatia_atual = 0
│  ├─ T1-C: processo_atual = 1
│  └─ T1-C: num_context_switches = 1
│
├─ CPU.run()
│  └─ Executa com Proc1
│     Acessa memoria[frame] onde frame vem de [4,5]
│
└─ T1-C: tempo_total = 6

... ciclos 6-9 similares com Proc1 ...


TEMPO = 10-14 CICLOS (Fatia 3):

sch_cycle():
├─ sch_need_context_switch() → true (ciclos_fatia_atual >= 5)
│
├─ sch_context_switch_next()
│  ├─ T1-B: save_context(CPU, PCB1)
│  │  ├─ PCB1.pc_salvo = 5
│  │  ├─ PCB1.regs_salvos[] = [...]
│  │  └─ Proc1 CONGELADO
│  │
│  ├─ T1-B: dequeue_pronto() → Proc2
│  │  └─ fila_pronto = [0, 1]
│  │
│  ├─ T1-B: context_switch(Proc2)
│  │  ├─ restore_context(PCB2, CPU)
│  │  │  ├─ CPU.pc = 0
│  │  │  └─ CPU.reg[] = [0,0,...]
│  │  │
│  │  ├─ T1-A: CPU.tabela_paginas = [6, 7]
│  │  │  └─ Agora acessa memoria de Proc2!
│  │  │
│  │  └─ T1-B: gp.processo_executando = 2
│  │
│  ├─ T1-B: enqueue_pronto(Proc2)
│  │  └─ fila_pronto = [0, 1, 2]
│  │
│  └─ T1-C: num_context_switches = 2
│
├─ CPU.run() × 5
└─ T1-C: tempo_total = 15


TEMPO = 15-19 CICLOS (Fatia 4 - Volta para Proc0!):

sch_cycle():
├─ sch_context_switch_next()
│  ├─ T1-B: save_context(CPU, PCB2)
│  │  ├─ PCB2.pc_salvo = 5
│  │  └─ PCB2 congelado
│  │
│  ├─ T1-B: dequeue_pronto() → Proc0 NOVAMENTE
│  │  └─ fila_pronto = [1, 2]
│  │
│  ├─ T1-B: restore_context(PCB0, CPU)
│  │  ├─ CPU.pc = 5 ← RETOMA DE ONDE PAROU!
│  │  ├─ CPU.reg[] = [...] ← ESTADO RESTAURADO!
│  │  └─ Proc0 REATIVADO!
│  │
│  ├─ T1-A: CPU.tabela_paginas = [2, 3]
│  │  └─ Volta para memoria de Proc0!
│  │
│  └─ T1-C: num_context_switches = 3
│
├─ CPU.run()
│  └─ CONTINUA DE PC=5 COM ESTADO PRESERVADO!
│     Proc0 não sabe que "dormiu" por 10 ciclos!
│
└─ T1-C: tempo_total = 20


TEMPO = 20-30 CICLOS:

... Repete: Proc1 → Proc2 → Proc0 → Proc1 → Proc2 ...

RESULTADO APÓS 30 CICLOS:
├─ Proc0: 10 ciclos (ciclos 0-4 e 15-19)
├─ Proc1: 10 ciclos (ciclos 5-9 e 20-24)
├─ Proc2: 10 ciclos (ciclos 10-14 e 25-29)
│
├─ Context Switches: 6 (0→1, 1→2, 2→0, 0→1, 1→2, 2→0)
│
├─ Cada processo recebe ~1/3 do tempo
│
└─ Fairness PERFEITA! 🎯
```

## Fluxo de Dados Entre Camadas

### Quando Programa Acessa Endereço 20

```
PROGRAMA: lê memória[20]

  ↓ (Processo 1 executando)

T1-B: Qual processo está rodando?
  └─ gp.processo_executando = 1

  ↓

T1-A: Traduzir endereço virtual 20 para físico
  ├─ CPU.tabela_paginas = [4, 5] (de Proc1)
  ├─ página = 20 / 16 = 1
  ├─ offset = 20 % 16 = 4
  ├─ frame = CPU.tabela_paginas[1] = 5
  └─ endereço_físico = 5 * 16 + 4 = 84

  ↓

HARDWARE: Acessa memória física
  └─ memoria[84]

  ↓

RESULTADO: Programa recebe valor de memoria[84]
  (Mesmo se outro processo pedisse 20, acessaria diferente!)
```

### Quando Fatia Acaba

```
T1-C: sch_cycle() detecta ciclos_fatia_atual >= 5
  │
  ├─ Chama sch_context_switch_next()
  │
  ├─ T1-B: save_context()
  │  └─ Salva CPU.pc e CPU.reg[] no PCB do processo anterior
  │
  ├─ T1-B: dequeue_pronto()
  │  └─ Tira próximo processo da fila
  │
  ├─ T1-B: restore_context()
  │  └─ Restaura CPU.pc e CPU.reg[] do novo PCB
  │
  ├─ T1-A: CPU.tabela_paginas = novo_tabela_paginas
  │  └─ Muda endereçamento de memória
  │
  ├─ T1-B: enqueue_pronto()
  │  └─ Coloca processo anterior de volta na fila
  │
  └─ T1-C: tempo_total++
     └─ Próximo ciclo executa com novo processo
```

## Exemplo Completo: Multiprogramação em Ação

```
CENÁRIO: 3 processos, cada um com seu programa

PROCESSO 0:
R0 = 5; R1 = 3
RESULTADO = R0 + R1

PROCESSO 1:
R0 = 10; R1 = 2
RESULTADO = R0 * R1

PROCESSO 2:
R0 = 20; R1 = 5
RESULTADO = R0 - R1

EXECUÇÃO EM PARALELO:

Ciclo  Proc  Instrução           R0  R1  Estado
0      P0    LDI R0, 5            5   ?   Executando
1      P0    LDI R1, 3            5   3   Executando
2      P0    ADD R0, R1          8   3   Executando
3      P0    STO R0, @0          8   3   Executando
4      P0    STOP                -   -   [Finalizou]
       ↓ CONTEXT SWITCH 1 ↓
5      P1    LDI R0, 10         10   ?   Executando
6      P1    LDI R1, 2          10   2   Executando
7      P1    MUL R0, R1         20   2   Executando
8      P1    STO R0, @0         20   2   Executando
9      P1    STOP                -   -   [Finalizou]
       ↓ CONTEXT SWITCH 2 ↓
10     P2    LDI R0, 20         20   ?   Executando
11     P2    LDI R1, 5          20   5   Executando
12     P2    SUB R0, R1         15   5   Executando
13     P2    STO R0, @0         15   5   Executando
14     P2    STOP                -   -   [Finalizou]

RESULTADO:
- Proc0 resultado = 8 (5+3), armazenado em memoria[2+2]
- Proc1 resultado = 20 (10*2), armazenado em memoria[4+2]
- Proc2 resultado = 15 (20-5), armazenado em memoria[6+2]

→ Todos rodaram "em paralelo"!
→ Cada um viu sua memória protegida!
→ Sistema Operacional orquestrou tudo!
```

## Isolamento: Por Que é Seguro?

```
Mesmo código (fatorialV2.bin):

Processo 0 vê endereço 20:
  ├─ tabela_paginas[0] = frame 2
  ├─ Traduz: endereço_físico = 2*16 + 20%16 = 36
  └─ Acessa memoria[36]

Processo 1 vê endereço 20:
  ├─ tabela_paginas[0] = frame 4
  ├─ Traduz: endereço_físico = 4*16 + 20%16 = 68
  └─ Acessa memoria[68]

→ Mesmo endereço virtual, posições físicas DIFERENTES!
→ Processos nunca interferem!
→ Segurança garantida por T1-A!
```

## Performance: Fairness Verificada

```
COM 500 CICLOS E 3 PROCESSOS:

Proc0: 167 * 5 ciclos (aprox)
Proc1: 167 * 5 ciclos
Proc2: 166 * 5 ciclos

Distribuição: 167/500 = 33.4%, 167/500 = 33.4%, 166/500 = 33.2%

→ BALANCEADO! Round-Robin funciona! ✓
```

## Estatísticas Completas

```
AO FINAL DA EXECUÇÃO:

Tempo total: 500 ciclos
Fatias de tempo: 100 (500/5)
Processos: 3
Context switches: 100 (um por fatia, menos o último)

Por processo:
- Proc0: ~167 fatias = ~834 ciclos (em time-slicing puro)
- Proc1: ~167 fatias = ~834 ciclos
- Proc2: ~166 fatias = ~830 ciclos

→ Fairness: 33.4%, 33.4%, 33.2% (praticamente igual!)
→ Overhead: Cada switch leva ~0 ciclos (salvam apenas registradores)
```

## Resumo da Integração

| Camada | T1-A | T1-B | T1-C |
|--------|------|------|------|
| **Função** | Memória Virtual | Processos | Escalonamento |
| **Provê** | Tradução endereços | Context Switch | Automação |
| **Usa** | Hardware | T1-A | T1-B + T1-A |
| **Resultado** | Proteção | Multiprogramação | Fairness |

```
SISTEMA = Automação(Isolamento(Hardware))
        = T1-C(T1-B(T1-A(CPU+Mem)))
```

## Diagrama de Interação

```
        PROGRAMA
            ↑↓
    ┌──────────────┐
    │   T1-C       │ ← Escalonador escolhe
    │ scheduler.c  │   qual processo
    └──────┬───────┘
           ↓
    ┌──────────────┐
    │   T1-B       │ ← Muda contexto
    │  process.c   │   (save/restore)
    └──────┬───────┘
           ↓
    ┌──────────────┐
    │   T1-A       │ ← Traduz endereço
    │ mem_mngr.c   │   virtual → físico
    └──────┬───────┘
           ↓
    ┌──────────────┐
    │  HARDWARE    │ ← Acessa memória
    │ CPU + Mem    │   de verdade
    └──────────────┘
```

**Conclusão:** O sistema é uma **mini-simulação de um Sistema Operacional real**! 🎓
