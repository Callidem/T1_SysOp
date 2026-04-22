# Análise de V1 e Recomendações de Implementação

## 🔍 Estado Atual da Implementação (V1)

### ✅ O que já existe

| Componente | Status | Descrição |
|------------|--------|-----------|
| **Hardware** | ✅ Completo | CPU, Memory, Word, Opcode, Interrupts |
| **CPU** | ✅ Funcional | Fetch-Decode-Execute, Handlers de interrupção |
| **Memory** | ✅ Funcional | 1024 palavras lineares, inicialização, limpeza |
| **SO Básico** | ✅ Funcional | Tratamento de interrupções, IO (syscall), STOP |
| **Programas** | ✅ Funcional | Carregamento de programas (fatorialV2, etc) |
| **Utilitários** | ✅ Funcional | Load, exec, dump para debug |

### ⚠️ O que está Incompleto

| Componente | Status | O que falta |
|------------|--------|------------|
| **Mem_Mngr** | 🟡 70% | Implementação apenas de `gm_init()` |
| `gm_alloc()` | ❌ Não implementado | Alocação de frames |
| `gm_free()` | ❌ Não implementado | Liberação de frames |
| `gm_addr()` | ❌ Não implementado | Tradução de endereços |
| **Paginação** | ❌ Não funcional | CPU não usa tabela de páginas |
| **Múltiplos Processos** | ❌ Não existe | T1-B não iniciado |
| **Escalonamento** | ❌ Não existe | T1-C não iniciado |

---

## 📋 Problemas Identificados

### Problema 1: Dupla Alocação de Memória

**Localização:** `mem_mngr.c` linha 20

```c
gm_init(&gm, 1024, 16);
cpu_init(&cpu, gm.mem, 1);  // ← Passa gm.mem, OK
```

```c
// Em mem_mngr.c
memory_init(&gm->mem, 1024);  // ← Aloca DENTRO do Mem_Mngr
```

**Problema:** 
- `memory_init()` aloca memória DENTRO de `Mem_Mngr`
- `main.c` passa `gm.mem` para CPU
- Funciona, mas arquitetura confusa

**Solução:** Deixar como está (funciona) ou refatorar para melhor clareza

---

### Problema 2: Funções `gm_alloc()` e `gm_free()` Não Implementadas

**Localização:** `mem_mngr.c` linhas 21-23

```c
bool gm_alloc(Mem_Mngr *gm, size_t nro_palavras);  // ← Apenas declaração!
void gm_free(Mem_Mngr *gm, size_t *frames, size_t nro_frames);
size_t gm_addr(Mem_Mngr *gm, size_t end_lógico);
```

**Problema:**
- Funções declaradas mas **não implementadas**
- Não compila (undefined reference)
- Impede uso de paginação

**Solução:** Implementar as 3 funções

---

### Problema 3: CPU Não Usa Tabela de Páginas

**Localização:** `hardware.h` (CPU structure)

```c
struct CPU {
    // ... nenhuma referência a tabela_paginas
    // ... nenhuma referência a Mem_Mngr
}
```

**Problema:**
- CPU acessa memória diretamente: `mem->pos[pc]`
- Não há tradução de endereços
- Paginação não funciona

**Solução:** Adicionar tabela de páginas ao contexto de CPU

---

### Problema 4: Bitmap Sem Lógica de Alocação

**Localização:** `mem_mngr.h` e `mem_mngr.c`

```c
bool *free_frames;  // ← Existe bitmap
```

```c
for (int i = 0; i < gm->nroFrames; ++i) {
    gm->free_frames[i] = true;  // ← Inicializa, mas nunca usa
}
```

**Problema:**
- Bitmap criado mas **nunca consultado**
- Não há lógica de First Fit
- `gm_alloc()` não implementada

**Solução:** Implementar `gm_alloc()` com First Fit

---

## 🎯 Roadmap de Implementação

### Fase 1: Completar T1-A (Paginação)

**Ordem de prioridade:**

1. **Implementar `gm_alloc()`**
   - Input: número de palavras
   - Output: array de frames alocados
   - Lógica: First Fit no bitmap

2. **Implementar `gm_free()`**
   - Input: array de frames
   - Output: nada
   - Lógica: marcar frames como livres

3. **Implementar `gm_addr()`**
   - Input: endereço lógico
   - Output: endereço físico
   - Lógica: tradução página → frame

4. **Adicionar tabela de páginas ao CPU**
   - Campo novo em `CPU struct`
   - Inicializado em `cpu_init()`
   - Usado em `cpu_run()` para tradução

5. **Integrar CPU com tabela de páginas**
   - Modificar fetch de instrução
   - Usar `gm_addr()` para traduzir
   - Modificar acesso à memória

6. **Testar T1-A completo**
   - Alocar memória para processo
   - Executar programa com paginação
   - Verificar isolamento

---

### Fase 2: Implementar T1-B (Multiprogramação)

**Após T1-A funcionar:**

1. **Criar estrutura ProcessoControlBlock (PCB)**
   - Guardar contexto completo
   - Guardar tabela de páginas
   - Guardar estado do processo

2. **Criar GerenciadorProcessos**
   - Array de PCBs
   - Fila de processos prontos
   - Processo em execução

3. **Implementar context_switch()**
   - Save contexto de CPU em PCB anterior
   - Restore contexto de CPU do novo PCB
   - Muda tabela de páginas

4. **Testar T1-B**
   - Criar 2-3 processos
   - Context switch manual
   - Verificar isolamento

---

### Fase 3: Implementar T1-C (Escalonamento)

**Após T1-B funcionar:**

1. **Criar estrutura Escalonador**
   - Tempo total
   - Ciclos da fatia atual
   - Tamanho de fatia

2. **Implementar Round-Robin**
   - Detectar fim de fatia
   - Fazer context switch automático
   - Rotacionar processos

3. **Implementar sch_run()**
   - Loop principal
   - Chamar sch_cycle()
   - Coletar estatísticas

4. **Testar T1-C**
   - 3 processos automáticos
   - Fairness (33% cada)
   - Estatísticas

---

## 📝 Especificação vs Implementação

### O que a Especificação Pede (T1-A)

```
✓ Paginação de memória
✓ Bitmap de frames (livre/ocupado)
✓ Tabela de páginas (lógico → físico)
✓ Tradução de endereços transparente
✓ Proteção entre processos
✓ First Fit allocation (ou similar)
✓ Suportar múltiplos processos (cada um com sua tabela)
✓ Interrupção intEnderecoInvalido em acesso inválido
```

### O que V1 Tem

```
✓ Bitmap criado (free_frames[])
✓ Estrutura Mem_Mngr pronta
~ CPU + Memory separados
✗ Alocação de frames (gm_alloc não implementada)
✗ Liberação de frames (gm_free não implementada)
✗ Tradução de endereços (gm_addr não implementada)
✗ Múltiplos processos (não existe gerenciador)
✗ Context switching (não existe)
✗ Escalonamento (não existe)
```

### Gap (O que Falta)

1. **~150 linhas** em `mem_mngr.c` (gm_alloc, gm_free, gm_addr)
2. **~100 linhas** em `hardware.h` (adicionar tabela_paginas ao CPU)
3. **~200 linhas** em `hardware.c` (usar gm_addr em cpu_run)
4. **~50 linhas** em `so.c` (tratar intEnderecoInvalido)
5. **~100 linhas** em `main.c` (testar paginação)

**Total: ~600 linhas para T1-A ficar completo**

---

## 🚀 Recomendação de Próximos Passos

### Prioridade 1: COMPLETAR T1-A

**Razão:** Sem T1-A, T1-B e T1-C não funcionam

**Tarefas:**

- [ ] Implementar `gm_alloc(size_t nro_palavras)`
  - Calcular frames necessários
  - First Fit no bitmap
  - Retornar sucesso/falha

- [ ] Implementar `gm_free(size_t *frames, size_t nro_frames)`
  - Marcar frames como livres

- [ ] Implementar `gm_addr(size_t end_lógico)`
  - Tradução: página = end / 16, offset = end % 16
  - Buscar frame na tabela
  - Retornar: frame * 16 + offset

- [ ] Modificar `CPU` para suportar tabela de páginas
  - Adicionar `int *tabela_paginas` em CPU struct
  - Adicionar `int nro_paginas`

- [ ] Modificar `cpu_run()` para usar `gm_addr()`
  - Ao buscar instrução: traduzir PC
  - Ao acessar memória: traduzir endereço

- [ ] Testar T1-A
  - Alocar memória para processo
  - Carregar programa com paginação
  - Executar com tradução

---

### Prioridade 2: COMEÇAR T1-B

**Após T1-A funcionar:**

- [ ] Criar `process.h` com PCB struct
- [ ] Criar `process.c` com gp_new(), context_switch(), etc
- [ ] Criar `GerenciadorProcessos`
- [ ] Testar context switching manual

---

### Prioridade 3: COMPLETAR T1-C

**Após T1-B funcionar:**

- [ ] Criar `scheduler.h/c`
- [ ] Implementar Round-Robin
- [ ] Testar automação e fairness

---

## 📊 Resumo das Mudanças Necessárias

| Arquivo | Linhas | Mudanças |
|---------|--------|----------|
| `mem_mngr.c` | 150 | Implementar 3 funções |
| `mem_mngr.h` | 10 | Adicionar função gm_addr proto |
| `hardware.h` | 20 | Adicionar tabela_paginas ao CPU |
| `hardware.c` | 100 | Usar gm_addr em cpu_run |
| `so.c` | 50 | Tratar intEnderecoInvalido |
| `main.c` | 100 | Testar paginação |
| `process.h` | 100 | Nova estrutura PCB (T1-B) |
| `process.c` | 300 | Implementações T1-B |
| `scheduler.h` | 50 | Nova estrutura Escalonador (T1-C) |
| `scheduler.c` | 200 | Implementações T1-C |

**Total: ~1000 linhas de novo código**

---

## ✅ Checklist de Validação

### Para T1-A Estar Completo:

- [ ] `gm_alloc()` implementada
- [ ] `gm_free()` implementada
- [ ] `gm_addr()` implementada
- [ ] CPU usa tabela de páginas
- [ ] Programa executa com tradução de endereços
- [ ] Múltiplos processos simultaneamente na memória
- [ ] Cada processo acessa apenas seus frames
- [ ] intEnderecoInvalido disparado em acesso inválido
- [ ] Teste com 2 processos em simultaneidade
- [ ] Fairness verificada manualmente

---

## 🎓 Conclusão

**V1 é um bom ponto de partida!** Tem:
- Hardware funcionando
- Estrutura básica de SO
- Bitmap e Mem_Mngr criados

**Falta:**
- Implementação completa do gerenciador (3 funções)
- Integração CPU com paginação
- Multiprogramação (T1-B)
- Escalonamento (T1-C)

**Próximo passo:** Implementar `gm_alloc()`, `gm_free()`, `gm_addr()` para completar T1-A. Depois expandir para T1-B e T1-C.

**Tempo estimado:** 4-6 horas para T1-A+B+C completos.
