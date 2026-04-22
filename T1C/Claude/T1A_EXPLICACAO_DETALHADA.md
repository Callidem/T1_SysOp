# T1-A: Gerenciador de Memória com Paginação

## Objetivo

Implementar um gerenciador de memória que permite múltiplos programas compartilharem memória física através de **paginação virtual**.

## Problema Resolvido

**Sem Paginação:**
```
Processo 1: 20 palavras → ocupa posições 0-19
Processo 2: 20 palavras → ocupa posições 20-39
Problema: Se Processo 1 só precisa de 18 palavras, desperdiça 2

+ Quando um processo termina, deixa "buracos" (fragmentação)
+ Difícil encontrar espaço para novo processo
```

**Com Paginação (T1-A):**
```
Memória dividida em FRAMES (blocos fixos de 16 palavras)
Processo 1 vê endereços: 0 a 19 (sua visão virtual)
Realmente ocupa: frames 2 e 3 (tradução automática)

Processo 2 vê endereços: 0 a 19 (SUA visão virtual)
Realmente ocupa: frames 4 e 5

→ Sem desperdício, sem fragmentação, proteção!
```

## Conceitos Fundamentais

### 1. Memória Virtual vs Física

```
MEMÓRIA VIRTUAL (o que o programa vê):
┌─────────────────────────────┐
│ Endereços: 0, 1, 2, ..., N  │  Contíguo
└─────────────────────────────┘

MEMÓRIA FÍSICA (o que realmente existe):
┌─────────────────────────────┐
│ Frame 0 (16 palavras)        │  Pode estar
│ Frame 1 (16 palavras)        │  em qualquer
│ Frame 2 (16 palavras) ← Proc │  lugar
│ Frame 3 (16 palavras) ← Proc │
│ Frame 4 (16 palavras) ← Proc │
│ ...                          │
└─────────────────────────────┘

TRADUÇÃO AUTOMÁTICA:
Endereço Virtual 25 → (Proc ativa)
  → Página 1, Offset 9
  → Frame 3 (da tabela do Proc)
  → Endereço Físico 57
```

### 2. Tabela de Páginas

Cada processo tem uma **tabela de páginas** que mapeia suas páginas virtuais para frames físicos.

```c
// Processo com 2 páginas (32 palavras)
tabela_paginas[0] = 2;  // Página 0 → Frame 2
tabela_paginas[1] = 3;  // Página 1 → Frame 3

// Quando programa acessa endereço 20:
página = 20 / 16 = 1
offset = 20 % 16 = 4
frame = tabela_paginas[1] = 3
endereço_físico = 3 * 16 + 4 = 52
```

### 3. Bitmap de Frames

Controla quais frames estão livres/ocupados:

```c
bool frameLivre[64];  // 64 frames total

frameLivre[0] = true;   // Livre
frameLivre[1] = true;   // Livre
frameLivre[2] = false;  // Ocupado (Processo 0)
frameLivre[3] = false;  // Ocupado (Processo 0)
frameLivre[4] = false;  // Ocupado (Processo 1)
...
```

## Exemplo Passo a Passo

### Cenário: Dois Processos, Cada Um com 32 Palavras

```
ESTADO INICIAL:
Frames: [ Livre ] [ Livre ] [ Livre ] ... (64 frames)
                                
PROCESSO 0 PEDE 32 PALAVRAS:
  1. Precisa de 2 frames (32 / 16)
  2. Busca frames livres → encontra 2 e 3   
  3. Marca como ocupados: frameLivre[2]=false, frameLivre[3]=false
  4. Cria tabela: tabela_paginas[0]=2, tabela_paginas[1]=3
  5. Carrega programa nos frames 2-3
  
ESTADO APÓS PROC0:
Frames: [ L ] [ L ] [ P0] [ P0] [ L ] [ L ] ... (L=Livre, P0=Proc0)
                ↑    ↑    ↑    ↑
              Frame Frame Frame Frame
                0    1    2    3

PROCESSO 1 PEDE 32 PALAVRAS:
  1. Precisa de 2 frames
  2. Busca frames livres → encontra 4 e 5
  3. Marca como ocupados: frameLivre[4]=false, frameLivre[5]=false
  4. Cria tabela: tabela_paginas[0]=4, tabela_paginas[1]=5
  5. Carrega programa nos frames 4-5

ESTADO APÓS PROC1:
Frames: [ L ] [ L ] [ P0] [ P0] [ P1] [ P1] [ L ] ...
                     ↑    ↑    ↑    ↑
                   Proc0      Proc1

ACESSO À MEMÓRIA:

Processo 0 lê endereço 20:
  - Vê: 20 (seu endereço virtual)
  - Traduz: página=1, offset=4
  - Busca: frame=tabela_paginas[1]=3
  - Acessa: memoria[3*16+4]=memoria[52]

Processo 1 lê endereço 20:
  - Vê: 20 (seu endereço virtual)
  - Traduz: página=1, offset=4
  - Busca: frame=tabela_paginas[1]=5
  - Acessa: memoria[5*16+4]=memoria[84]

→ PROTEÇÃO! Mesmo endereço virtual, diferentes locais físicos!
```

## Estruturas de Dados

### Mem_Mngr (mem_mngr.h)

```c
typedef struct {
    int tamMem;           // Tamanho total (1024 palavras)
    int tamPg;            // Tamanho de página (16 palavras)
    int nroFrames;        // Número de frames (1024/16 = 64)
    bool *frameLivre;     // Bitmap: true=livre, false=ocupado
    Word *memoria;        // Array contendo toda a memória
} Mem_Mngr;
```

### Tabela de Páginas (em process.h)

```c
int *tabela_paginas;      // Array: [frame1, frame2, ...]
int nro_paginas;          // Número de páginas (linhas da tabela)
```

## Funções Principais

### gm_init() - Inicialização

```c
bool gm_init(Mem_Mngr *gm, int tamMem, int tamPg);

// O que faz:
// 1. Aloca array frameLivre com tamanho nroFrames
// 2. Marca todos como livres (true)
// 3. Aloca array memoria com tamanho tamMem
// 4. Inicializa todas as palavras com DATA
```

### gm_alloc() - Alocação

```c
bool gm_alloc(Mem_Mngr *gm, size_t nro_palavras);

// O que faz:
// 1. Calcula frames necessários: nro_frames = (nro_palavras + tamPg - 1) / tamPg
// 2. Busca nro_frames livres sequencialmente (First Fit)
// 3. Marca como ocupados
// 4. Retorna true se sucesso, false se insuficiente
```

### gm_free_process() - Liberação

```c
void gm_free_process(Mem_Mngr *gm, int *tabela_paginas, int nro_paginas);

// O que faz:
// 1. Itera sobre cada página na tabela
// 2. Obtém o frame correspondente
// 3. Marca como livre: frameLivre[frame] = true
```

### gm_load_program() - Carregamento

```c
void gm_load_program(Mem_Mngr *gm, int *tabela_paginas, int nro_paginas,
                     int tamPg, const Word *programa, int tamanho);

// O que faz:
// 1. Para cada página da tabela
// 2. Obtém o frame correspondente
// 3. Calcula endereço base: frame * tamPg
// 4. Copia instruções do programa para memoria[endereco]
```

## Fluxo de Execução

```
SISTEMA OPERACIONAL INICIA:
├─ memory_init(&mem, 1024, 16)
│  └─ Cria memória com 64 frames de 16 palavras
│
├─ gm_init(&gm, 1024, 16)
│  └─ Inicializa gerenciador de memória
│     └─ frameLivre = [true, true, ..., true]
│
├─ Cria PROCESSO 0
│  ├─ gm_alloc(&gm, 20)
│  │  └─ Aloca 2 frames (2 e 3), marca como false
│  │
│  ├─ gm_load_program(&gm, [2,3], 2, 16, programa, 20)
│  │  └─ Copia 20 palavras de programa para memoria[32..51]
│  │
│  └─ Cria PCB com tabela_paginas=[2,3]
│
├─ Cria PROCESSO 1
│  ├─ gm_alloc(&gm, 20)
│  │  └─ Aloca 2 frames (4 e 5)
│  │
│  └─ gm_load_program(&gm, [4,5], 2, 16, programa, 20)
│
├─ CPU EXECUTA COM PROCESSO 0
│  ├─ cpu.tabela_paginas = [2, 3]
│  ├─ Fetch: memoria[0]
│  │  └─ Traduz: página=0, frame=2, endereço_físico=32
│  │  └─ memoria[32]
│  │
│  └─ Execute
│
└─ CPU EXECUTA COM PROCESSO 1
   └─ cpu.tabela_paginas = [4, 5]
   └─ Mesmo programa, mesmos endereços, DIFERENTES frames!
```

## Proteção de Memória

T1-A fornece **proteção de memória** porque cada processo tem sua própria tabela de páginas:

```
Processo 0 tenta acessar endereço 30:
  - Sua tabela: [2, 3]
  - Página: 30 / 16 = 1
  - Frame: tabela_paginas[1] = 3
  - Acessa: memoria[48..63]
  → Só pode acessar frames 2-3 (seu próprio código)

Processo 1 tenta acessar endereço 30:
  - Sua tabela: [4, 5]
  - Página: 30 / 16 = 1
  - Frame: tabela_paginas[1] = 5
  - Acessa: memoria[80..95]
  → Só pode acessar frames 4-5 (seu próprio código)

→ Proc0 NUNCA consegue acessar frames 4-5 de Proc1!
```

## Teste

```bash
cd Parte_A
make clean && make
./vm_so

Esperado:
- Memória inicializada
- Programa carregado com paginação
- Execução simples de 1 processo
```

## Resumo

| Conceito | Descrição |
|----------|-----------|
| **Frame** | Bloco fixo de 16 palavras em memória física |
| **Página** | Bloco fixo de 16 palavras em memória virtual |
| **Tradução** | Conversão de endereço virtual → frame → endereço físico |
| **Proteção** | Cada processo tem sua tabela, acessa apenas seus frames |
| **Bitmap** | Controla alocação/liberação de frames |
| **First Fit** | Estratégia simples de buscar primeiro frame livre |

T1-A prepara o sistema para **multiprogramação** em T1-B! 🚀
