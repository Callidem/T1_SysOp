# Sistema Operacional Simulado - Documentação Completa

## 📋 Índice
1. [Visão Geral](#visão-geral)
2. [T1-A: Gerenciador de Memória com Paginação](#t1-a-gerenciador-de-memória)
3. [T1-B: Gerenciador de Processos](#t1-b-gerenciador-de-processos)
4. [T1-C: Escalonador com Time-Slicing](#t1-c-escalonador)
5. [Arquitetura Integrada](#arquitetura-integrada)
6. [Como Compilar e Executar](#como-compilar-e-executar)

---

## Visão Geral

Este projeto implementa um **Sistema Operacional Simulado** completo que executa múltiplos programas simultaneamente em uma máquina virtual. O sistema é dividido em três componentes principais:

### Componentes Principais

```
┌──────────────────────────────────────────────────────┐
│  APLICAÇÃO: Múltiplos Processos (e.g., Fatorial)   │
└────────────────────┬─────────────────────────────────┘
                     │
┌────────────────────▼─────────────────────────────────┐
│ T1-C: ESCALONADOR (Scheduler)                        │
│ - Time-Slicing (fatias de tempo)                     │
│ - Round-Robin scheduling                             │
│ - Context switches automáticos                        │
└────────────────────┬─────────────────────────────────┘
                     │
┌────────────────────▼─────────────────────────────────┐
│ T1-B: GERENCIADOR DE PROCESSOS                       │
│ - Process Control Block (PCB)                         │
│ - Múltiplos processos simultâneos                     │
│ - Fila de processos prontos                           │
│ - Context save/restore                               │
└────────────────────┬─────────────────────────────────┘
                     │
┌────────────────────▼─────────────────────────────────┐
│ T1-A: GERENCIADOR DE MEMÓRIA (Memory Manager)        │
│ - Paginação virtual                                   │
│ - Alocação/desalocação de frames                      │
│ - Tradução de endereços lógicos → físicos             │
│ - Proteção de memória entre processos                 │
└────────────────────┬─────────────────────────────────┘
                     │
┌────────────────────▼─────────────────────────────────┐
│ HARDWARE VIRTUAL                                      │
│ - CPU: 10 registradores, fetch-decode-execute        │
│ - Memória: 1024 palavras organizada em páginas        │
│ - Interrupções e tratamento de erros                  │
└──────────────────────────────────────────────────────┘
```

---

## T1-A: Gerenciador de Memória

### Conceito

**Paginação** é uma técnica de gerenciamento de memória que divide a memória física em blocos fixos chamados **frames** e a memória virtual dos processos em blocos chamados **páginas**.

### Objetivo

- Permitir que múltiplos processos compartilhem a memória física
- Cada processo vê uma memória virtual contígua (0 até N-1)
- Mapeamento automático de endereços virtuais → físicos
- Proteção: um processo não pode acessar memória de outro

### Estruturas de Dados Principais

#### 1. **Mem_Mngr** (Gerenciador de Memória)

```c
typedef struct {
    int tamMem;           // Tamanho total da memória (1024 palavras)
    int tamPg;            // Tamanho de página (16 palavras)
    int nroFrames;        // Número de frames (tamMem / tamPg = 64)
    bool *frameLivre;     // Bitmap: true = livre, false = ocupado
    Word *memoria;        // Array com toda a memória física
} Mem_Mngr;
```

#### 2. **Tabela de Páginas** (Por Processo)

Cada processo tem uma tabela que mapeia suas páginas virtuais para frames físicos.

```c
// Processo com 2 páginas alocadas nos frames 2 e 3
tabela_paginas[0] = 2;   // Página 0 (endereços 0-15) → Frame 2
tabela_paginas[1] = 3;   // Página 1 (endereços 16-31) → Frame 3
```

### Funcionamento

#### Alocação de Memória

```
Programa precisa de 32 palavras
    ↓
Calcula: nro_paginas = (32 + 15) / 16 = 2 páginas
    ↓
Busca 2 frames livres: encontra frames 2 e 3
    ↓
Marca como ocupados: frameLivre[2] = false, frameLivre[3] = false
    ↓
Cria tabela de páginas: [2, 3]
```

#### Tradução de Endereço

```
Processo acessa endereço lógico 25
    ↓
Calcula: página = 25 / 16 = 1
         offset = 25 % 16 = 9
    ↓
Consulta: frame = tabela_paginas[1] = 3
    ↓
Endereço físico = 3 * 16 + 9 = 57
    ↓
Acessa: memoria[57]
```

### Exemplo Prático

```
MEMÓRIA FÍSICA: 64 frames × 16 palavras cada

Frame 0:  [________] (livre)
Frame 1:  [________] (livre)
Frame 2:  [PROC0    ] ← Página 0 de Proc0 (tabela_paginas[0] = 2)
Frame 3:  [PROC0    ] ← Página 1 de Proc0 (tabela_paginas[1] = 3)
Frame 4:  [PROC1    ] ← Página 0 de Proc1 (tabela_paginas[0] = 4)
Frame 5:  [PROC1    ] ← Página 1 de Proc1 (tabela_paginas[1] = 5)
...

PROCESSO 0 vê endereços: 0 a 31
  Endereço 0  → frame 2, offset 0 → endereço físico 32
  Endereço 16 → frame 3, offset 0 → endereço físico 48

PROCESSO 1 vê endereços: 0 a 31
  Endereço 0  → frame 4, offset 0 → endereço físico 64
  Endereço 16 → frame 5, offset 0 → endereço físico 80

→ Mesmos endereços lógicos, diferentes endereços físicos!
→ Proteção: Processo 0 não pode acessar frames 4-5
```

### Funções Principais (mem_mngr.c)

| Função | Descrição |
|--------|-----------|
| `gm_init()` | Inicializa gerenciador, aloca estruturas |
| `gm_alloc()` | Aloca frames para um novo programa |
| `gm_free()` | Libera frames (marca como livres) |
| `gm_free_process()` | Libera memória de um processo específico |
| `gm_load_program()` | Carrega programa na memória com tradução |

### Arquivo: mem_mngr.h

```c
typedef struct {
    int tamMem;           // Tamanho total da memória
    int tamPg;            // Tamanho de página
    int nroFrames;        // Número total de frames
    bool *frameLivre;     // Bitmap de alocação
    Word *memoria;        // Memória física
} Mem_Mngr;
```

---

## T1-B: Gerenciador de Processos

### Conceito

Um **processo** é um programa em execução com seu próprio contexto isolado. **Multiprogramação** permite múltiplos processos na memória simultaneamente.

### Objetivo

- Suportar múltiplos processos simultâneos
- Manter contexto isolado de cada processo
- Permitir save/restore de estado (context switch)
- Gerenciar fila de processos prontos

### Estruturas de Dados Principais

#### 1. **ProcessoControlBlock (PCB)**

Cada processo tem um PCB que contém todo seu contexto:

```c
typedef struct {
    int id;                    // ID único (0, 1, 2, ...)
    char name[64];             // Nome do programa
    
    // Contexto da CPU
    int pc_salvo;              // Program Counter
    int regs_salvos[10];       // Registradores (r[0-3]: dados, r[8-9]: IO)
    
    // Estado
    int estado;                // NOVO, PRONTO, EXECUTANDO, BLOQUEADO, FINALIZADO
    
    // Memória (T1-A)
    int *tabela_paginas;       // Tabela de páginas deste processo
    int nro_paginas;           // Quantas páginas alocadas
    
    // Estatísticas
    int fatias_usadas;         // Quantas fatias de tempo executou
} ProcessoControlBlock;
```

#### 2. **GerenciadorProcessos**

Mantém todos os processos e coordena execução:

```c
typedef struct {
    ProcessoControlBlock *processos;  // Array de PCBs
    int nro_processos;               // Quantidade atual
    
    int processo_executando;         // ID do processo ativo
    
    int *fila_pronto;                // IDs de processos prontos
    int tamanho_fila;                // Tamanho da fila
    
    Mem_Mngr *gm;                    // Referência ao gerenciador de memória
} GerenciadorProcessos;
```

#### 3. **Estados de um Processo**

```
        ┌─────────────────┐
        │  NOVO           │ ← Acabou de ser criado
        └────────┬────────┘
                 │ (pronto)
                 ↓
        ┌─────────────────┐
   ┌───→│  PRONTO         │◄──────────┐ (Na fila)
   │    └────────┬────────┘           │
   │             │ (escalonador       │
   │             │  seleciona)        │
   │             ↓                    │
   │    ┌─────────────────┐           │
   │    │ EXECUTANDO      │           │
   │    └────────┬────────┘           │
   │             │                    │
   │    ┌────────┴──────────┐         │
   │    │                   │         │
   │    ↓ (IO)              ↓ (time slice
   │ ┌────────┐        acabou)
   └─┤BLOQUEADO        ↓
     └───────┬──────────┘
             │ (IO end)
             ↓
      ┌─────────────────┐
      │ FINALIZADO      │ ← Programa terminou
      └─────────────────┘
```

### Funcionamento

#### Criação de Processo

```
gp_new("fatorialV2")
    ↓
1. Lookup do programa → encontra imagem
    ↓
2. Aloca memória via T1-A → obtém tabela_paginas
    ↓
3. Carrega programa na memória (com paginação)
    ↓
4. Cria PCB:
   - id = 0
   - pc_salvo = 0
   - estado = PRONTO
   - tabela_paginas = [2, 3]  (exemplar)
    ↓
5. Adiciona à fila de prontos
    ↓
Retorna: PID = 0
```

#### Context Switch

```
Processo 0 em execução → tempo slice acabou

1. SAVE CONTEXT (Processo 0):
   - PCB0.pc_salvo = CPU.pc
   - PCB0.regs_salvos[i] = CPU.reg[i]
   - PCB0.estado = PRONTO

2. Seleciona próximo processo (Processo 1)

3. RESTORE CONTEXT (Processo 1):
   - CPU.pc = PCB1.pc_salvo
   - CPU.reg[i] = PCB1.regs_salvos[i]
   - CPU.tabela_paginas = PCB1.tabela_paginas
   - PCB1.estado = EXECUTANDO

4. Executa CPU com tabela_paginas de Proc1
```

### Funções Principais (process.c)

| Função | Descrição |
|--------|-----------|
| `gp_init()` | Inicializa gerenciador de processos |
| `gp_new()` | Cria novo processo (alocação + carregamento) |
| `gp_rm()` | Remove processo |
| `gp_ps()` | Lista todos os processos |
| `gp_dump()` | Exibe detalhes de um processo |
| `save_context()` | Salva contexto de CPU em PCB |
| `restore_context()` | Restaura contexto de PCB em CPU |
| `context_switch()` | Save + Restore automático |
| `enqueue_pronto()` | Adiciona à fila de prontos |
| `dequeue_pronto()` | Remove da fila de prontos |

### Exemplo: Dois Processos na Memória

```
TEMPO 0: Criação
├─ Processo 0: id=0, pc=0, regs=[0,0,...], tabela=[2,3], estado=PRONTO
└─ Processo 1: id=1, pc=0, regs=[0,0,...], tabela=[4,5], estado=PRONTO

TEMPO 1: Context Switch para Proc0
├─ Restore context: CPU.pc=0, CPU.regs=[0,0,...], CPU.tabela=[2,3]
├─ Executa alguns ciclos
└─ Proc0 modifica registrador: CPU.reg[0] = 5

TEMPO 2: Save & Switch para Proc1
├─ Save context: PCB0.pc_salvo=2, PCB0.regs_salvos[0]=5
├─ Restore context: CPU.pc=0, CPU.regs=[0,0,...], CPU.tabela=[4,5]
├─ Executa alguns ciclos
└─ Proc1 modifica registrador: CPU.reg[0] = 3

TEMPO 3: Save & Switch back para Proc0
├─ Save context: PCB1.pc_salvo=2, PCB1.regs_salvos[0]=3
├─ Restore context: CPU.pc=2, CPU.regs=[5,0,...], CPU.tabela=[2,3]
└─ Continua de onde parou!

→ Cada processo vê seu próprio estado preservado
```

---

## T1-C: Escalonador

### Conceito

Um **escalonador** (scheduler) decide qual processo executar a cada momento. **Time-slicing** dá a cada processo uma fatia de tempo fixa antes de alternar.

### Objetivo

- Automatizar a alternância entre processos
- Implementar fairness (igualdade de tempo)
- Usar round-robin scheduling
- Manter estatísticas

### Estrutura de Dados Principal

#### **Escalonador**

```c
typedef struct {
    GerenciadorProcessos *gp;      // Referência ao gerenciador de processos
    CPU *cpu;                      // Referência à CPU
    
    int tempo_total;               // Total de ciclos executados
    int ciclos_fatia_atual;        // Ciclos na fatia atual
    int fatia_tamanho;             // Ciclos por fatia (default: 5)
    
    int processo_atual;            // ID do processo em execução
    
    // Estatísticas
    int num_context_switches;      // Total de trocas
    int num_fatias_executadas;     // Total de fatias
} Escalonador;
```

### Algoritmo: Round-Robin com Time-Slicing

```
LOOP (até não haver mais processos):
    1. Verificar se fatia de tempo acabou
       - Se não, continuar executando ciclo atual
       - Se sim, preparar context switch
    
    2. Fazer context switch se necessário
       - Salvar contexto do processo atual
       - Buscar próximo processo pronto
       - Restaurar contexto do novo processo
       - Recoloc ar processo na fila
    
    3. Executar 1 ciclo da CPU
       - Fetch, decode, execute
       - Atualizar PC e registradores
    
    4. Repetir

RESULTADO: Todos os processos executam equitativamente
```

### Funcionamento Passo a Passo

```
INICIALIZAÇÃO:
├─ Processo 0: PRONTO (fila: [0])
├─ Processo 1: PRONTO (fila: [0, 1])
├─ Processo 2: PRONTO (fila: [0, 1, 2])
└─ Tamanho de fatia: 5 ciclos

EXECUÇÃO:

Fatia 1 (ciclos 0-4): Processo 0
├─ Context Switch → Proc0
├─ Ciclo 0: exec [LDI, 0, -1, 5]  → r[0]=5
├─ Ciclo 1: exec [STD, 0, -1, 19] → salva em memória
├─ Ciclo 2: exec [LDD, 0, -1, 19] → carrega de memória
├─ Ciclo 3: exec [LDI, 1, -1, -1] → r[1]=-1
├─ Ciclo 4: exec [LDI, 2, -1, 13] → r[2]=13
└─ Fatia acabou → Save context de Proc0

Fatia 2 (ciclos 5-9): Processo 1
├─ Proc0 volta à fila → fila: [1, 2, 0]
├─ Context Switch → Proc1
├─ Ciclos 5-9: Executa instruções de Proc1 com seu contexto
└─ Fatia acabou → Save context de Proc1

Fatia 3 (ciclos 10-14): Processo 2
├─ Proc1 volta à fila → fila: [2, 0, 1]
├─ Context Switch → Proc2
├─ Ciclos 10-14: Executa com contexto de Proc2
└─ Fatia acabou

Fatia 4 (ciclos 15-19): Processo 0 (NOVAMENTE)
├─ Proc2 volta à fila → fila: [0, 1, 2]
├─ Context Switch → Proc0
├─ Restaura contexto salvo: r[0]=5, r[1]=-1, r[2]=13
├─ Continua de onde parou!
└─ Ciclos 15-19: Executa mais 5 ciclos

... (repete até terminar ou atingir limite)
```

### Funções Principais (scheduler.c)

| Função | Descrição |
|--------|-----------|
| `sch_init()` | Inicializa escalonador |
| `sch_cycle()` | Executa 1 ciclo completo (check + switch + run) |
| `sch_need_context_switch()` | Verifica se fatia acabou |
| `sch_context_switch_next()` | Alterna para próximo processo |
| `sch_run()` | Loop principal até TEMPO_MAXIMO |
| `sch_dump_stats()` | Exibe estatísticas |

### Estatísticas Esperadas

Com 3 processos e 500 ciclos máximos:

```
Tempo total:        500 ciclos
Context switches:   ~500
Fatias executadas:  ~500 (em fatias de 5 ciclos)

Distribuição (Round-Robin):
- Processo 0: ~167 fatias
- Processo 1: ~167 fatias
- Processo 2: ~166 fatias

→ Praticamente igual para todos!
```

---

## Arquitetura Integrada

### Fluxo Completo de Execução

```
1. INICIALIZAÇÃO
   ├─ memory_init() → cria memória com 64 frames
   ├─ gm_init() → inicializa gerenciador de memória
   ├─ gp_init() → inicializa gerenciador de processos
   └─ sch_init() → inicializa escalonador

2. CRIAÇÃO DE PROCESSOS
   ├─ gp_new("fatorialV2")
   │  ├─ retrieve_program() → carrega imagem do programa
   │  ├─ gm_alloc() → aloca frames via T1-A
   │  ├─ gm_load_program() → carrega na memória
   │  └─ Cria PCB e adiciona à fila
   │
   ├─ gp_new("fatorialV2") → Processo 1
   └─ gp_new("fatorialV2") → Processo 2

3. ESCALONAMENTO E EXECUÇÃO (T1-C)
   └─ sch_run() → loop principal
      └─ Enquanto tempo_total < 500:
         ├─ sch_cycle()
         │  ├─ Verifica fatia de tempo
         │  ├─ Se acabou: context_switch()
         │  │  ├─ save_context() → salva PC e regs em PCB
         │  │  ├─ dequeue_pronto() → pega próximo
         │  │  ├─ restore_context() → restaura contexto
         │  │  ├─ enqueue_pronto() → recoloca na fila
         │  │  └─ Atualiza CPU.tabela_paginas (T1-A)
         │  │
         │  └─ cpu_run() → executa 1 ciclo
         │     ├─ fetch: memoria[PC] com tradução (T1-A)
         │     ├─ decode: interpreta opcode
         │     └─ execute: altera registradores/PC/memória

4. FINALIZAÇÃO
   ├─ sch_dump_stats() → mostra estatísticas
   ├─ gp_ps() → lista processos finais
   └─ Libera toda memória
```

### Interação T1-A ↔ T1-B ↔ T1-C

```
T1-C (Scheduler)
    │
    ├─→ Seleciona processo (context_switch)
    │   └─→ T1-B (gp)
    │       └─→ Altera CPU.tabela_paginas para processo
    │
    ├─→ Executa cpu_run() → acessa memória
    │   └─→ Traduz endereços automáticamente (T1-A)
    │       └─→ End. Lógico → T1-B (tabela_paginas) → T1-A (frames)
    │           └─→ End. Físico
    │
    └─→ Context switch quando fatia acaba
        └─→ T1-B salva contexto em PCB
            └─→ T1-A continua protegendo memória
```

### Exemplo de Acesso à Memória

```
Processo 1 executa: LDI reg[0], valor 5
CPU precisa carregar na memória...

1. CPU.pc = 0 (endereço lógico)
2. T1-B: tabela_paginas (do Proc1) = [4, 5]
3. T1-A: página = 0 / 16 = 0
         frame = tabela_paginas[0] = 4
         offset = 0 % 16 = 0
         endereço_físico = 4 * 16 + 0 = 64
4. Acessa: memoria[64] → instrução carregada
5. CPU acessa memoria[64] diretamente (já traduzido)

Se Processo 0 tentasse acessar mesmo endereço:
1. CPU.pc = 0
2. T1-B: tabela_paginas (do Proc0) = [2, 3]
3. T1-A: frame = tabela_paginas[0] = 2
         endereço_físico = 2 * 16 + 0 = 32
4. Acessa: memoria[32] ← DIFERENTE!
   → Proteção: Cada processo acessa sua própria memória
```

---

## Como Compilar e Executar

### Compilação

#### T1-A (Gerenciador de Memória)

```bash
cd /path/to/T1C/Claude/Parte_A
make clean
make
```

Arquivo compilado: `vm_so`

#### T1-B (Gerenciador de Processos)

```bash
cd /path/to/T1C/Claude/Parte_B
make clean
make
```

Arquivo compilado: `vm_so`

#### T1-C (Escalonador Completo)

```bash
cd /path/to/T1C/Claude/Parte_C
make clean
make
```

Arquivo compilado: `vm_so`

### Execução

```bash
./vm_so
```

### Saída Esperada

```
[GP] Gerenciador de Processos inicializado: max=10
[SCH] Escalonador inicializado: tamFatia=5 ciclos

################################
# T1-C: ESCALONADOR (T1-A+T1-B+T1-C)
################################

[MAIN] Criando processo 1...
[GP] Carregando programa 'fatorialV2' para processo 0
[GP] Processo 0 criado: 'fatorialV2' (páginas=2, estado=PRONTO)

... (mais processos) ...

################################
# T1-C: ESCALONADOR COM TIME-SLICING
################################

[SCH] Iniciando execução de 3 processo(s)
[SCH] Tamanho de fatia: 5 ciclos

[SCH] Context Switch para processo 0 (fatia 1)
[SCH] Context Switch para processo 1 (fatia 2)
[SCH] Context Switch para processo 2 (fatia 3)
... (alternância contínua) ...

===== ESTATÍSTICAS DO ESCALONADOR =====
Tempo total:              500 ciclos
Context switches:         500
Fatias executadas:        500
Tamanho de fatia:         5 ciclos

Estatísticas por processo:
PID   Nome       Estado       Fatias
0     fatorialV2 PRONTO       167
1     fatorialV2 EXECUTANDO   167
2     fatorialV2 PRONTO       166
```

---

## Conceitos Chave

### Paginação (T1-A)

**O que é:** Divisão de memória em blocos pequenos para melhor gerenciamento

**Benefício:** Permite multiprogramação sem fragmentação externa

**Trade-off:** Overhead de tradução (reduzido com cache/TLB em sistemas reais)

### Multiprogramação (T1-B)

**O que é:** Múltiplos processos na memória simultaneamente

**Benefício:** Melhor utilização de CPU (enquanto um espera IO, outro executa)

**Contexto:** Tudo que identifica um processo (registradores, PC, memória)

### Escalonamento (T1-C)

**O que é:** Decisão de qual processo executar a cada momento

**Round-Robin:** Alterna entre processos de forma cíclica

**Time-Slice:** Cada processo recebe uma fatia de tempo igual

**Fairness:** Nenhum processo monopoliza a CPU

---

## Testes e Validação

### Teste T1-A: Paginação

```bash
# Verificar alocação de memória
./vm_so
# Esperado: Processo carregado com 2 páginas
```

### Teste T1-B: Multiprogramação

```bash
# Verificar múltiplos processos
./vm_so
# Esperado: 3 processos criados, contextos salvos
```

### Teste T1-C: Escalonamento

```bash
# Verificar alternância automática
./vm_so
# Esperado: ~500 ciclos, distribuição equilibrada (167, 167, 166)
```

---

## Arquivos Principais

### T1-A
- `hardware.h/.c` - CPU e Memória
- `mem_mngr.h/.c` - Gerenciador de Memória com Paginação
- `main.c` - Demonstração T1-A

### T1-B
- `process.h/.c` - Gerenciador de Processos
- `mem_mngr.h/.c` - (Estendido com funções T1-B)
- `process.h/.c` - Adições de context switch
- `main.c` - Demonstração T1-B

### T1-C
- `scheduler.h/.c` - Escalonador com Time-Slicing
- `main.c` - Demonstração T1-C completa

### Suporte
- `programs.h/.c` - Biblioteca de programas disponíveis
- `so.h/.c` - Tratadores de interrupção e syscall
- `utils.h/.c` - Funções utilitárias
- `Makefile` - Compilação

---

## Resumo

| Aspecto | T1-A | T1-B | T1-C |
|--------|------|------|------|
| **Foco** | Memória | Processos | Scheduling |
| **Abstração** | Paginação Virtual | PCB + Context | Time-Slicing |
| **Estrutura** | Mem_Mngr | GerenciadorProcessos | Escalonador |
| **Problema Resolvido** | Isolamento de memória | Multiprogramação | Justiça na CPU |
| **Resultado** | Proteção | Simultaneidade | Alternância Automática |

Um sistema operacional completo! 🎉
