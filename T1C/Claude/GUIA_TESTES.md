# Guia Completo de Testes e Validação

## Teste T1-A: Paginação

### 1. Compilação

```bash
cd /home/bruno/Documentos/1_Ensino/Engenharia_de_computacao/Semestre_VIII/Sistemas_Operacionais/T1_SysOp/T1C
make clean
make

# Esperado:
# - Sem erros de compilação
# - Warnings apenas (unused variables, etc)
# - Gera executável: vm_so
```

### 2. Execução

```bash
./vm_so

# Esperado:
# - Carrega memória
# - Programa executa
# - Estado: Finalizado (STOP)
# - Sem erros de acesso à memória
```

### 3. Validação de T1-A

Procure na saída por:

```
✓ Memória inicializada (1024 palavras)
✓ Frames criados (64 frames de 16 palavras)
✓ Programa carregado com paginação
✓ Tabela de páginas criada
✓ Endereços virtuais traduzidos corretamente
```

### 4. Teste Específico: Tradução de Endereço

Se adicionar debug em mem_mngr.c:

```c
// Ao acessar endereço 20 com Proc0:
printf("Acesso: Virtual=20 → Page=%d, Offset=%d → Frame=%d → Physical=%d\n",
       20/16, 20%16, tabela_paginas[20/16], 
       tabela_paginas[20/16]*16 + 20%16);

// Esperado:
// Acesso: Virtual=20 → Page=1, Offset=4 → Frame=2 → Physical=36
```

## Teste T1-B: Multiprogramação

### 1. Verificar Criação de Processos

Na saída, procure por:

```
===== TABELA DE PROCESSOS =====
PID  Nome           Estado      PC   R[0]
0    fatorialV2     PRONTO      0    0
1    fatorialV2     EXECUTANDO  ?    ?
2    fatorialV2     PRONTO      0    0
```

✓ Deve mostrar 3 processos
✓ Cada um com ID único
✓ Estados corretos (PRONTO ou EXECUTANDO)

### 2. Verificar Context Switches

Procure por padrão na saída:

```
... Processo 0 executando ...
... Processo 1 executando ...
... Processo 2 executando ...
... Processo 0 executando ... (volta!)
```

✓ Mostra alternância entre processos
✓ Cada um retoma de onde parou

### 3. Verificar Isolamento de Memória

Se adicionar debug, verificar que:

```
Proc0 com tabela_paginas = [2, 3]
Proc1 com tabela_paginas = [4, 5]
Proc2 com tabela_paginas = [6, 7]

Quando muda de processo:
├─ CPU.tabela_paginas muda
├─ Endereçamento de memória muda
└─ Proc0 nunca acessa frames 4-7
```

### 4. Teste: Preservação de Contexto

Adicione em main.c:

```c
printf("Ciclo %d: Proc=%d, PC=%d, R0=%d\n", 
       ciclo, proc_atual, cpu.pc, cpu.reg[0]);
```

Esperado:
- Quando volta para Proc0, PC volta ao valor anterior
- Registradores retomam estados anteriores
- Contexto completamente preservado

## Teste T1-C: Escalonamento

### 1. Verificar Ciclos Totais

Na saída, procure:

```
===== ESTATÍSTICAS DO ESCALONADOR =====
Tempo total:        500 ciclos
Context switches:   ~100
Fatias executadas:  ~100
Tamanho de fatia:   5 ciclos
```

✓ Tempo total deve ser 500 (ou próximo)
✓ Context switches ≈ Tempo total / fatia_tamanho
✓ Fatias ≈ tempo total / 5

### 2. Verificar Fairness

Procure por:

```
Estatísticas por processo:
PID   Nome       Fatias Utilizadas
0     fatorialV2 167
1     fatorialV2 167
2     fatorialV2 166
```

✓ Distribuição aproximadamente igual
✓ Diferença máxima: 1 fatia
✓ Percentual: ~33% cada
✓ Round-Robin funcionando!

### 3. Verificar Round-Robin

Na saída de execução detalhada:

```
Ciclo 0-4:   Proc0
Ciclo 5-9:   Proc1
Ciclo 10-14: Proc2
Ciclo 15-19: Proc0 (volta!)
...
```

✓ Padrão cíclico
✓ Cada processo recebe 5 ciclos
✓ Ordem: 0→1→2→0→1→2...

### 4. Cálculo Manual de Fairness

```
Total: 500 ciclos
Processos: 3
Fatia: 5 ciclos

Fatias por processo ideal: 500 / 3 / 5 = 33,33 ciclos
                          ≈ 167 fatias por processo

Esperado:
Proc0: 167 fatias = 835 ciclos (mas limitado a 500 total, logo ~167)
Proc1: 167 fatias = 835 ciclos
Proc2: 166 fatias = 830 ciclos

Verificação: 167 + 167 + 166 = 500 ✓
```

## Teste Completo: Integração T1-A + T1-B + T1-C

### Teste de Cenário Real

```bash
# 1. Compilar
cd T1C
make clean && make

# 2. Executar
./vm_so > output.txt 2>&1

# 3. Verificar resultado
head -50 output.txt   # Inicialização
tail -50 output.txt   # Estatísticas
wc -l output.txt      # Deve ter ~800+ linhas
```

### Checklist de Validação Completa

```
├─ [ ] COMPILAÇÃO
│  ├─ [ ] Sem erros
│  └─ [ ] Executável criado
│
├─ [ ] T1-A: PAGINAÇÃO
│  ├─ [ ] Memória inicializada (1024 palavras)
│  ├─ [ ] Frames criados (64 total)
│  ├─ [ ] Bitmap funcionando
│  └─ [ ] Tradução de endereços correta
│
├─ [ ] T1-B: MULTIPROGRAMAÇÃO
│  ├─ [ ] 3 processos criados
│  ├─ [ ] Cada processo tem PCB
│  ├─ [ ] Tabelas de páginas diferentes
│  ├─ [ ] Context switches funcionando
│  ├─ [ ] Contexto preservado
│  └─ [ ] Isolamento de memória
│
├─ [ ] T1-C: ESCALONAMENTO
│  ├─ [ ] 500 ciclos executados
│  ├─ [ ] ~100 context switches
│  ├─ [ ] Round-Robin ativo (0→1→2→0...)
│  ├─ [ ] Fairness: ~167, 167, 166 fatias
│  ├─ [ ] Estatísticas corretas
│  └─ [ ] Sem travamentos ou erros
│
└─ [ ] INTEGRAÇÃO
   ├─ [ ] T1-A funciona com T1-B
   ├─ [ ] T1-B funciona com T1-C
   ├─ [ ] T1-A+B+C funcionam juntos
   └─ [ ] Sem contradições entre camadas
```

## Testes de Stress

### Teste 1: Muitos Processos

Modifique main.c:

```c
// Em vez de criar 3, crie 5
gp_new("fatorialV2");
gp_new("fatorialV2");
gp_new("fatorialV2");
gp_new("fatorialV2");
gp_new("fatorialV2");

// Compile e execute
// Esperado:
// - Todos recebem tempo justo
// - ~20% tempo cada
// - Sem erros de memória
```

### Teste 2: Muitos Ciclos

Modifique scheduler.c:

```c
#define TEMPO_MAXIMO_CPU 1000  // Em vez de 500

// Compile e execute
// Esperado:
// - Mesmo padrão, 2× mais longo
// - Fairness mantida
// - Nenhum vazamento de memória
```

### Teste 3: Diferentes Tamanhos de Fatia

Modifique scheduler.c:

```c
sch.fatia_tamanho = 10;  // Em vez de 5

// Compile e execute
// Esperado:
// - Context switches reduzidos
// - Fatias maiores
// - Fairness ainda mantida
```

## Debugging

### Se Travou

```bash
# 1. Limite de ciclos
# Modifique TEMPO_MAXIMO_CPU para valor menor

# 2. Adicione prints de debug
printf("Ciclo %d\n", tempo_total);

# 3. Rode com valgrind (se disponível)
valgrind ./vm_so
```

### Se Fairness Está Errada

```bash
# 1. Verifique tamanho de fatia
printf("Fatia: %d ciclos\n", sch.fatia_tamanho);

# 2. Verifique counter de fatia
printf("Ciclo na fatia: %d\n", sch.ciclos_fatia_atual);

# 3. Verifique fila de prontos
gp_ps(gp);  // Mostra fila
```

### Se Contexto Não Preserva

```bash
# 1. Verifique save_context
printf("Save: PC=%d, R0=%d\n", cpu.pc, cpu.reg[0]);

# 2. Verifique restore_context
printf("Restore: PC=%d, R0=%d\n", cpu.pc, cpu.reg[0]);

# 3. Verifique enqueue/dequeue
printf("Fila antes: ");
for (int i = 0; i < gp.tamanho_fila; i++)
    printf("%d ", gp.fila_pronto[i]);
printf("\n");
```

## Testes de Validação Específica

### Teste: Bitmap de Frames

```c
// Verificar que frames estão sendo usados corretamente
for (int i = 0; i < gm.nroFrames; i++) {
    if (!gm.frameLivre[i]) {
        printf("Frame %d ocupado\n", i);
    }
}
// Esperado: Frames 2,3,4,5,6,7 ocupados
// (resto livre)
```

### Teste: Proteção de Memória

```c
// Simular tentativa de acesso indevido
int tabela_errada[2] = {2, 3};  // De Proc0
int endereco_indevido = 100;    // Além dos 20 de Proc0

// Com T1-A, isto nunca deveria acessar Proc1
int pagina = endereco_indevido / 16;  // = 6
if (pagina < 2) {  // Só tem 2 páginas
    // Acessaria fora! Error!
}
```

### Teste: Round-Robin

```c
// Verificar que fila funciona como circular
printf("Enqueue 0: ");
enqueue_pronto(gp, 0);
printf("Enqueue 1: ");
enqueue_pronto(gp, 1);
printf("Enqueue 2: ");
enqueue_pronto(gp, 2);

printf("Dequeue: %d\n", dequeue_pronto(gp));  // 0
printf("Enqueue 0: ");
enqueue_pronto(gp, 0);
printf("Dequeue: %d\n", dequeue_pronto(gp));  // 1
printf("Dequeue: %d\n", dequeue_pronto(gp));  // 2
printf("Dequeue: %d\n", dequeue_pronto(gp));  // 0 (volta!)
```

## Validação de Saída

### Formato Esperado

```
===== MEMÓRIA INICIALIZADA =====
Tamanho: 1024 palavras
Frames: 64
Tamanho de página: 16 palavras

===== GERENCIADOR DE PROCESSOS INICIALIZADO =====

===== CRIANDO PROCESSO 0 =====
Nome: fatorialV2
Páginas: 2 (32 palavras)
Frames alocados: 2 3
Estado: PRONTO

===== CRIANDO PROCESSO 1 =====
...

===== CRIANDO PROCESSO 2 =====
...

===== TABELA DE PROCESSOS =====
PID  Nome           Estado       PC   R[0]
0    fatorialV2     PRONTO       0    0
1    fatorialV2     EXECUTANDO   ?    ?
2    fatorialV2     PRONTO       0    0

===== INICIANDO ESCALONADOR =====
Tempo máximo: 500 ciclos
Tamanho de fatia: 5

... (simulação rodando) ...

===== ESTATÍSTICAS DO ESCALONADOR =====
Tempo total: 500 ciclos
Context switches: 100
Fatias executadas: 100

Estatísticas por processo:
PID   Nome           Estado    Fatias
0     fatorialV2     PRONTO    167
1     fatorialV2     EXECUTANDO 167
2     fatorialV2     PRONTO    166
```

## Resumo de Validação

```
VALIDAÇÃO MÍNIMA:
├─ [ ] Compila sem erros
├─ [ ] Executa sem travamento
├─ [ ] Mostra 3 processos
├─ [ ] Conta 500 ciclos
└─ [ ] Fairness ~33% cada

VALIDAÇÃO COMPLETA:
├─ [ ] T1-A: Paginação funciona
├─ [ ] T1-B: Multiprogramação funciona
├─ [ ] T1-C: Escalonamento funciona
├─ [ ] Integração: Tudo junto funciona
├─ [ ] Isolamento: Processos protegidos
├─ [ ] Contexto: Estado preservado
├─ [ ] Fairness: Distribuição justa
├─ [ ] Estatísticas: Números corretos
└─ [ ] Stress tests: Sem problemas
```

Se todos os testes passarem, **seu sistema está funcionando corretamente!** 🎉
