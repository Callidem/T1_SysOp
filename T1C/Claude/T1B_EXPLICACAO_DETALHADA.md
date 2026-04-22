# T1-B: Gerenciador de Processos com Multiprogramação

## Objetivo

Implementar um gerenciador que suporta **múltiplos processos simultâneos** na memória, cada um com seu próprio contexto isolado.

## Problema Resolvido

**Sem Multiprogramação (T1-A):**
```
Sistema consegue executar 1 processo de cada vez
CPU fica ociosa enquanto processo espera IO
Não aproveita o tempo de espera
```

**Com Multiprogramação (T1-B):**
```
Múltiplos processos na memória
Enquanto Proc1 espera IO, Proc2 executa
Melhor utilização de CPU
Ilusão de paralelismo
```

## Conceitos Fundamentais

### 1. O que é um Processo?

Um **processo** é um programa em execução com seu próprio:

- **Código:** Instruções (carregadas na memória)
- **Dados:** Variáveis (armazenadas na memória)
- **Contexto:** Estado interno (registradores, PC)
- **Memória Virtual:** Ilusão de memória própria (via T1-A)
- **Identificação:** ID único

```
Programa (estático) vs Processo (dinâmico)
│
Program "fatorialV2.bin" → 20 palavras no disco
│
Processo 0 (instância 1) → 20 palavras em RAM, PC=0, r[]=[0,0,...]
Processo 1 (instância 2) → 20 palavras em RAM, PC=0, r[]=[0,0,...]
Processo 2 (instância 3) → 20 palavras em RAM, PC=0, r[]=[0,0,...]

Mesmo programa, 3 execuções independentes!
```

### 2. PCB - Process Control Block

O **PCB** é a estrutura que armazena tudo o que é necessário para conhecer o estado de um processo:

```c
typedef struct {
    int id;                    // Identificação
    char name[64];             // Nome do programa
    
    // CONTEXTO (CPU state)
    int pc_salvo;              // Program Counter
    int regs_salvos[10];       // Registradores
    
    // ESTADO DO PROCESSO
    int estado;                // NOVO, PRONTO, EXECUTANDO, BLOQUEADO, FINALIZADO
    
    // MEMÓRIA (integração com T1-A)
    int *tabela_paginas;       // Onde está alocado em RAM
    int nro_paginas;           // Quantas páginas ocupa
    
    // ESTATÍSTICAS
    int fatias_usadas;         // Quantas vezes executou
} ProcessoControlBlock;
```

### 3. Estados de um Processo

```
         ┌──────────┐
         │   NOVO   │  ← Recém-criado, não iniciou
         └────┬─────┘
              │ (pronto para executar)
              ↓
         ┌──────────┐
    ┌───→│  PRONTO  │◄────────┐ (Aguardando seleção)
    │    └────┬─────┘         │
    │         │ (escalonador  │
    │         │  seleciona)   │ (time slice
    │         ↓               │  acabou)
    │    ┌──────────┐         │
    │    │EXECUTANDO│─────────┘
    │    └────┬─────┘
    │         │
    │    ┌────┴──────────┐
    │    │               │
    │    ↓ (IO)          ↓ (STOP/erro)
    │ ┌────────┐      ┌────────────┐
    └─┤BLOQUEADO      │ FINALIZADO │
      └────────┘      └────────────┘
         │
         │ (IO end)
         └─→ volta para PRONTO
```

### 4. Execução de um Programa

```
SEM T1-B (direto):
PC=0, instruções carregadas, executa sequencialmente

COM T1-B (com multiprogramação):
└─ Cria Processo 0
   ├─ Aloca memória (T1-A) → tabela_paginas = [2, 3]
   ├─ Carrega programa nos frames 2-3
   ├─ Cria PCB: id=0, pc=0, regs=[0,0,...]
   └─ Adiciona à fila de processos prontos

└─ Cria Processo 1
   ├─ Aloca memória (T1-A) → tabela_paginas = [4, 5]
   ├─ Carrega programa nos frames 4-5
   ├─ Cria PCB: id=1, pc=0, regs=[0,0,...]
   └─ Adiciona à fila

Agora há 2 PROCESSOS simultâneos:
├─ Proc0: pc=0, contexto salvo
└─ Proc1: pc=0, contexto salvo
```

## Context Switch (Troca de Contexto)

É a **operação mais importante** em multiprogramação. Permite trocar qual processo está executando.

### Estrutura

```
ANTES:     CPU ← Processo 0 rodando
DURANTE:   Salvamos estado de Proc0, restauramos Proc1
DEPOIS:    CPU ← Processo 1 rodando, Proc0 congelado
```

### Passo a Passo

```
1. SAVE CONTEXT (Processo 0)
   └─ Cria snapshot do estado atual:
      - CPU.pc → PCB0.pc_salvo
      - CPU.reg[0] → PCB0.regs_salvos[0]
      - CPU.reg[1] → PCB0.regs_salvos[1]
      - ... todos os 10 registradores
      
   Resultado: PCB0 contém estado completo de CPU

2. SELECT NEXT (Processo 1)
   └─ Busca próximo processo pronto
      - Pode ser First Come First Served
      - Ou Round-Robin (visto em T1-C)

3. RESTORE CONTEXT (Processo 1)
   └─ Recupera snapshot do Processo 1:
      - PCB1.pc_salvo → CPU.pc
      - PCB1.regs_salvos[0] → CPU.reg[0]
      - ... todos os 10 registradores

4. UPDATE MEMORY MAPPING (T1-A)
   └─ IMPORTANTE! Muda tabela de páginas:
      - CPU.tabela_paginas = PCB1.tabela_paginas = [4, 5]
      → Agora CPU acessa memória de Proc1!

5. CONTINUE EXECUTION
   └─ CPU retoma execução do PC de Proc1
      Proc1 continua como se nada tivesse acontecido!
```

### Exemplo Visual

```
CICLO 1: Processo 0 executa
┌──────────────────────┐
│ CPU                  │
│ PC = 5               │
│ r[0] = 10            │
│ r[1] = 3             │
│ tabela = [2,3]       │
└──────────────────────┘
      │
      ├─→ executa LDI r[0], 20
      │   r[0] = 20
      │
      ├─→ CONTEXT SWITCH!

CONTEXTO SALVO:
PCB0: pc_salvo=6, regs_salvos=[20,3,0,...]

CONTEXTO RESTAURADO:
PCB1: pc_salvo=3, regs_salvos=[5,7,0,...]

CICLO 2: Processo 1 executa
┌──────────────────────┐
│ CPU                  │
│ PC = 3  (restaurado) │
│ r[0] = 5 (restaurado)│
│ r[1] = 7 (restaurado)│
│ tabela = [4,5]       │
└──────────────────────┘
      │
      ├─→ executa SUB r[0], r[1]
      │   r[0] = 5 - 7 = -2
      │
      ├─→ CONTEXT SWITCH!

CICLO 3: Processo 0 executa NOVAMENTE
┌──────────────────────┐
│ CPU                  │
│ PC = 6  (restaurado) │
│ r[0] = 20 (restaurado)
│ r[1] = 3 (restaurado)│
│ tabela = [2,3]       │
└──────────────────────┘
      │
      └─→ Continua com estado antigo preservado!
```

## Estruturas de Dados

### ProcessoControlBlock (process.h)

```c
typedef struct {
    // Identificação
    int id;                    // 0, 1, 2, ...
    char name[64];             // "fatorialV2"
    
    // Estado da CPU
    int pc_salvo;              // PC quando foi congelado
    int regs_salvos[10];       // Snapshot dos registradores
    
    // Estado
    int estado;                // NOVO, PRONTO, EXECUTANDO, ...
    
    // Memória
    int *tabela_paginas;       // [frame0, frame1, ...]
    int nro_paginas;           // Número de páginas
    int tamPg;                 // Tamanho de página (16)
    
    // Estatísticas
    int tempo_criacao;         // Timestamp (não usado neste projeto)
    int fatias_usadas;         // Quantas vezes executou (para T1-C)
} ProcessoControlBlock;
```

### GerenciadorProcessos (process.h)

```c
typedef struct {
    // Tabela de todos os PCBs
    ProcessoControlBlock *processos;  // [PCB0, PCB1, PCB2, ...]
    int nro_processos;               // Quantidade atual
    int max_processos;               // Capacidade máxima
    
    // Controle
    int processo_executando;         // ID do que está rodando (-1 se nenhum)
    
    // Fila de prontos
    int *fila_pronto;                // [id0, id1, id2, ...]
    int tamanho_fila;                // Quantos estão na fila
    
    // Referência
    Mem_Mngr *gm;                    // Aponta para gerenciador de memória
} GerenciadorProcessos;
```

## Funções Principais

### gp_new() - Criar Processo

```c
int gp_new(GerenciadorProcessos *gp, const char *nome_programa, int tamPg);

// O que faz:
// 1. Busca programa no repositório
// 2. Aloca memória via T1-A (gm_alloc)
// 3. Carrega programa na memória (gm_load_program)
// 4. Cria PCB:
//    - id = gp->proximo_id++
//    - estado = PRONTO
//    - tabela_paginas = [frame1, frame2, ...]
// 5. Adiciona à fila de prontos
// 6. Retorna ID do processo criado
```

### context_switch() - Trocar Processo

```c
void context_switch(GerenciadorProcessos *gp, CPU *cpu, int novo_processo_id);

// O que faz:
// 1. save_context(cpu, &gp->processos[processo_anterior])
//    - Salva PC e registradores no PCB
// 2. restore_context(&gp->processos[novo_processo_id], cpu)
//    - Restaura PC e registradores de novo PCB
// 3. cpu->tabela_paginas = gp->processos[novo_id]->tabela_paginas
//    - CRUCIAL! Muda mapeamento de memória
// 4. gp->processo_executando = novo_processo_id
```

### save_context() e restore_context()

```c
void save_context(CPU *cpu, ProcessoControlBlock *pcb) {
    // Cria snapshot do estado atual
    pcb->pc_salvo = cpu->pc;
    for (int i = 0; i < 10; i++) {
        pcb->regs_salvos[i] = cpu->reg[i];
    }
}

void restore_context(ProcessoControlBlock *pcb, CPU *cpu) {
    // Recupera snapshot
    cpu->pc = pcb->pc_salvo;
    for (int i = 0; i < 10; i++) {
        cpu->reg[i] = pcb->regs_salvos[i];
    }
}
```

### gp_ps() - Listar Processos

```c
void gp_ps(GerenciadorProcessos *gp);

// Mostra tabela com todos os processos:
// PID  Nome         Estado      PC    Regs[0]
// 0    fatorialV2   PRONTO      0     0
// 1    fatorialV2   EXECUTANDO  5     20
// 2    fatorialV2   PRONTO      0     0
```

## Fluxo de Execução

```
INICIALIZAÇÃO:
├─ gm_init() → memória pronta
└─ gp_init() → gerenciador pronto

CRIAÇÃO DE PROCESSOS:
├─ gp_new("fatorialV2")
│  ├─ gm_alloc(20) → aloca frames 2-3
│  ├─ gm_load_program() → carrega em frames 2-3
│  ├─ cria PCB0: tabela=[2,3], estado=PRONTO
│  └─ enqueue_pronto(0) → adiciona à fila
│
├─ gp_new("fatorialV2")
│  ├─ gm_alloc(20) → aloca frames 4-5
│  ├─ gm_load_program() → carrega em frames 4-5
│  ├─ cria PCB1: tabela=[4,5], estado=PRONTO
│  └─ enqueue_pronto(1)
│
└─ gp_new("fatorialV2")
   ├─ gm_alloc(20) → aloca frames 6-7
   ├─ gm_load_program() → carrega em frames 6-7
   ├─ cria PCB2: tabela=[6,7], estado=PRONTO
   └─ enqueue_pronto(2)

ESTADO:
┌─ PCB0: id=0, estado=PRONTO, tabela=[2,3]
├─ PCB1: id=1, estado=PRONTO, tabela=[4,5]
└─ PCB2: id=2, estado=PRONTO, tabela=[6,7]
Fila: [0, 1, 2]

EXECUÇÃO MANUAL (em T1-B):
├─ context_switch(0)
│  └─ CPU.tabela_paginas = [2,3]
│  └─ CPU executa algumas instruções
│  └─ save_context() → PCB0.pc_salvo = 2, PCB0.regs[0] = 5
│
├─ context_switch(1)
│  └─ CPU.tabela_paginas = [4,5]
│  └─ CPU executa algumas instruções
│  └─ save_context() → PCB1.pc_salvo = 3, PCB1.regs[0] = 7
│
└─ context_switch(2)
   └─ CPU.tabela_paginas = [6,7]
   └─ CPU executa algumas instruções
```

## Proteção Entre Processos

T1-B + T1-A fornece **isolamento de processos**:

```
Processo 0 tem tabela_paginas = [2, 3]
Processo 1 tem tabela_paginas = [4, 5]

Se Proc0 tenta acessar endereço 30:
  - Sua tabela: [2, 3]
  - Página = 30 / 16 = 1
  - Frame = tabela_paginas[1] = 3
  - Endereço físico = 3 * 16 + (30 % 16) = 48 + 14 = 62
  → Acessa apenas frames 2-3

Se Proc1 tenta acessar endereço 30:
  - Sua tabela: [4, 5]
  - Página = 30 / 16 = 1
  - Frame = tabela_paginas[1] = 5
  - Endereço físico = 5 * 16 + 14 = 94
  → Acessa apenas frames 4-5

→ Processos são isolados! Segurança garantida!
```

## Teste

```bash
cd Parte_B
make clean && make
./vm_so

Esperado:
- 2 processos criados (ou 3 em Parte_C)
- Context switches manuais
- Cada processo vê seu próprio estado preservado
```

## Resumo

| Conceito | Descrição |
|----------|-----------|
| **Processo** | Instância de programa em execução |
| **PCB** | Snapshot completo do estado de um processo |
| **Context Switch** | Save (congelamento) + Restore (reativação) |
| **Isolamento** | Cada processo tem memória própria via tabela |
| **Multiprogramação** | Múltiplos processos simultâneos |

T1-B permite **execução simultânea** de múltiplos programas! 🚀

Em T1-C, o escalonador automatiza o context switch! ⚡
