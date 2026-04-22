# Sistema Operacional Simulado - Documentação Completa

## 🎓 Bem-vindo!

Este projeto implementa um **mini-sistema operacional** em C, demonstrando conceitos fundamentais de SO como paginação, multiprogramação e escalonamento justo.

```
┌──────────────────────────────────────────────────┐
│  T1-C: Escalonador (Time-Slicing)               │
│        Automação + Fairness                      │
└──────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────┐
│  T1-B: Gerenciador de Processos                 │
│        Multiprogramação + Context Switch        │
└──────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────┐
│  T1-A: Gerenciador de Memória                   │
│        Paginação + Proteção                     │
└──────────────────────────────────────────────────┘
                        ↓
┌──────────────────────────────────────────────────┐
│  Hardware: CPU + Memória                        │
│           (Simulado em C)                       │
└──────────────────────────────────────────────────┘
```

## 📚 Índice Completo de Documentação

### 1. **Este Arquivo (README)**
   - Visão geral do projeto
   - Estrutura de arquivos
   - Quick start

### 2. **[DOCUMENTACAO_COMPLETA.md](DOCUMENTACAO_COMPLETA.md)**
   - Documentação técnica detalhada
   - Explicações de cada componente
   - Exemplos de uso
   - **Comece por aqui se quer conhecer tudo!**

### 3. **[T1A_EXPLICACAO_DETALHADA.md](T1A_EXPLICACAO_DETALHADA.md)**
   - Paginação de memória
   - Tradução de endereços
   - Bitmap de frames
   - Exemplos passo a passo
   - **Leia se quer entender paginação!**

### 4. **[T1B_EXPLICACAO_DETALHADA.md](T1B_EXPLICACAO_DETALHADA.md)**
   - Process Control Blocks (PCB)
   - Context switching
   - Multiprogramação
   - Estados de processos
   - **Leia se quer entender processos!**

### 5. **[T1C_EXPLICACAO_DETALHADA.md](T1C_EXPLICACAO_DETALHADA.md)**
   - Escalonamento Round-Robin
   - Time-slicing e quantum
   - Fairness
   - Algoritmo de scheduler
   - **Leia se quer entender escalonamento!**

### 6. **[INTEGRACAO_COMPLETA.md](INTEGRACAO_COMPLETA.md)**
   - Como T1-A, T1-B, T1-C trabalham juntos
   - Fluxo de dados entre camadas
   - Exemplos de execução completa
   - Diagramas de interação
   - **Leia se quer entender o sistema como um todo!**

### 7. **[GUIA_APRENDIZADO.md](GUIA_APRENDIZADO.md)**
   - Progressão educacional
   - O que cada parte ensina
   - Exemplos progressivos
   - Comparação com SO real
   - **Leia se quer aprender os conceitos em ordem!**

### 8. **[GUIA_TESTES.md](GUIA_TESTES.md)**
   - Como compilar e executar
   - Testes de validação
   - Checklist de funcionalidade
   - Debugging
   - Testes de stress
   - **Leia se quer verificar se tudo está funcionando!**

### 9. **[FAQ_PERGUNTAS_RESPOSTAS.md](FAQ_PERGUNTAS_RESPOSTAS.md)**
   - 40+ perguntas frequentes respondidas
   - Esclarecimentos de conceitos
   - Tips de debugging
   - Análise de performance
   - **Leia se tem dúvidas específicas!**

## 🚀 Quick Start

### Compilar

```bash
cd T1C
make clean
make
```

### Executar

```bash
./vm_so
```

### Esperado

```
[Muita saída mostrando:]
- Inicialização de memória
- Criação de 3 processos
- 500 ciclos de escalonamento
- Alternância Round-Robin (Proc0 → Proc1 → Proc2)
- Estatísticas finais com fairness ~33% cada
```

### Verificar Sucesso

```bash
./vm_so | tail -50
# Procure por:
# - "500 ciclos"
# - Distribuição: ~167, 167, 166
# - Sem erros de memória
```

## 📁 Estrutura de Arquivos

```
T1_SysOp/
├── T1C/                          # Parte C (Escalonador)
│   ├── hardware.c/h              # Simulação de CPU
│   ├── mem_mngr.c/h              # T1-A: Gerenciador de Memória
│   ├── process.c/h               # T1-B: Gerenciador de Processos
│   ├── scheduler.c/h             # T1-C: Escalonador
│   ├── utils.c/h                 # Utilitários
│   ├── so.c/h                    # SO (integração)
│   ├── programs.c/h              # Programas de teste
│   ├── main.c                    # Ponto de entrada
│   ├── Makefile                  # Build
│   └── vm_so                     # Executável compilado
│
├── Especificação/                # Documentos da especificação
│
└── DOCUMENTAÇÃO (este projeto):
    ├── README.md                 # Este arquivo
    ├── DOCUMENTACAO_COMPLETA.md  # Tudo de uma vez
    ├── T1A_EXPLICACAO_DETALHADA.md
    ├── T1B_EXPLICACAO_DETALHADA.md
    ├── T1C_EXPLICACAO_DETALHADA.md
    ├── INTEGRACAO_COMPLETA.md
    ├── GUIA_APRENDIZADO.md
    ├── GUIA_TESTES.md
    └── FAQ_PERGUNTAS_RESPOSTAS.md
```

## 🎯 O que Você Vai Aprender

### Parte 1: T1-A (Paginação)
✅ Como computadores gerenciam memória virtual
✅ Tradução de endereços (virtual → física)
✅ Proteção de memória entre processos
✅ Bitmap para alocação de frames

### Parte 2: T1-B (Multiprogramação)
✅ O que é um processo
✅ Process Control Block (PCB)
✅ Context switching (save/restore)
✅ Como rodar múltiplos programas simultaneamente

### Parte 3: T1-C (Escalonamento)
✅ Algoritmos de escalonamento (Round-Robin)
✅ Time-slicing e quantum
✅ Fairness e distribuição justa
✅ Automação de decisões de SO

## 💡 Guia de Leitura Recomendado

### Se você é iniciante em SO:

1. **Comece com:** [GUIA_APRENDIZADO.md](GUIA_APRENDIZADO.md)
   - Entenda a progressão educacional
   - Veja como cada parte constrói sobre a anterior

2. **Depois leia:** [T1A_EXPLICACAO_DETALHADA.md](T1A_EXPLICACAO_DETALHADA.md)
   - Aprenda paginação com exemplos simples

3. **Depois:** [T1B_EXPLICACAO_DETALHADA.md](T1B_EXPLICACAO_DETALHADA.md)
   - Aprenda processos e context switching

4. **Depois:** [T1C_EXPLICACAO_DETALHADA.md](T1C_EXPLICACAO_DETALHADA.md)
   - Aprenda escalonamento automático

5. **Finalmente:** [INTEGRACAO_COMPLETA.md](INTEGRACAO_COMPLETA.md)
   - Entenda como tudo funciona junto

### Se você quer entender a implementação:

1. **Comece com:** [DOCUMENTACAO_COMPLETA.md](DOCUMENTACAO_COMPLETA.md)
   - Visão geral de tudo
   - Como cada arquivo funciona

2. **Vá para:** Código-fonte em `T1C/`
   - Leia `main.c` para entender fluxo
   - Leia `mem_mngr.c` para paginação
   - Leia `process.c` para multiprogramação
   - Leia `scheduler.c` para escalonamento

### Se você tem dúvidas específicas:

1. **Vá para:** [FAQ_PERGUNTAS_RESPOSTAS.md](FAQ_PERGUNTAS_RESPOSTAS.md)
   - 40+ perguntas com respostas
   - Esclarecimentos de conceitos
   - Tips de debugging

### Se quer validar o funcionamento:

1. **Vá para:** [GUIA_TESTES.md](GUIA_TESTES.md)
   - Como compilar
   - Como executar
   - Checklist de validação
   - Testes de stress

## 🔍 Diagrama de Conceitos

```
MEMÓRIA VIRTUAL (T1-A)
├─ Paginação
├─ Tradução de endereços
├─ Bitmap de frames
└─ Proteção entre processos

PROCESSOS (T1-B)
├─ Process Control Block
├─ Context switching
├─ Multiprogramação
└─ Isolamento de contexto

ESCALONAMENTO (T1-C)
├─ Round-Robin
├─ Time-slicing
├─ Fairness
└─ Automação

SISTEMA COMPLETO
├─ T1-A + T1-B + T1-C integrados
├─ Múltiplos processos em paralelo
├─ Memória isolada e protegida
├─ Tempo justo para todos
└─ = Mini Sistema Operacional!
```

## 📊 Estatísticas do Projeto

### Código Fonte
- **Linhas de código:** ~1500
- **Arquivos:** 8 (hardware, mem_mngr, process, scheduler, utils, so, programs, main)
- **Linguagem:** C (std=c11)
- **Compiler:** gcc

### Documentação
- **Arquivos MD:** 9 documentos
- **Linhas de documentação:** ~3500+
- **Cobertura:** Conceitos, implementação, testes, FAQ

### Funcionalidade
- **Processos suportados:** 3 (configurável)
- **Ciclos:** 500 (configurável)
- **Quantum:** 5 ciclos (configurável)
- **Context switches:** ~100 por execução
- **Fairness:** ±33% para 3 processos

## ✅ Checklist: O Que Funciona

- [x] Compilação sem erros
- [x] Execução completa (500 ciclos)
- [x] Paginação (T1-A)
  - [x] Bitmap de frames
  - [x] Alocação de memória
  - [x] Tradução de endereços
  - [x] Proteção de memória
- [x] Multiprogramação (T1-B)
  - [x] Criação de processos
  - [x] Context switching
  - [x] Preservação de contexto
  - [x] Isolamento entre processos
- [x] Escalonamento (T1-C)
  - [x] Round-Robin
  - [x] Time-slicing
  - [x] Fairness (33% cada para 3 processos)
  - [x] Estatísticas

## 🎓 Conceitos Fundamentais Ensinados

| Conceito | Onde | Por Quê |
|----------|------|--------|
| Paginação | T1-A | Gerenciar memória eficientemente |
| Tradução de Endereços | T1-A | Abstrair memória física |
| Process Control Block | T1-B | Armazenar estado do processo |
| Context Switching | T1-B | Trocar processo em execução |
| Multiprogramação | T1-B | Executar múltiplos processos |
| Round-Robin | T1-C | Algoritmo de escalonamento justo |
| Time-Slicing | T1-C | Distribuir tempo igualmente |
| Fairness | T1-C | Garantir igualdade de oportunidade |

## 🔧 Troubleshooting Rápido

### Problema: Compilação falha
**Solução:** `make clean && make` no diretório T1C

### Problema: Execução trava
**Solução:** Reduzir TEMPO_MAXIMO_CPU em scheduler.c

### Problema: Fairness errada
**Solução:** Ver [GUIA_TESTES.md](GUIA_TESTES.md) seção "Debugging"

### Problema: Memória fragmentada
**Solução:** Isso é esperado! Não há liberação de memória neste projeto

## 📖 Referências Rápidas

### Estruturas Principais

```c
// T1-A: Memória
typedef struct {
    int tamMem, tamPg, nroFrames;
    bool *frameLivre;
    Word *memoria;
} Mem_Mngr;

// T1-B: Processo
typedef struct {
    int id, pc_salvo, regs_salvos[10];
    int estado, *tabela_paginas, nro_paginas;
} ProcessoControlBlock;

// T1-C: Escalonador
typedef struct {
    int tempo_total, ciclos_fatia_atual, fatia_tamanho;
    int num_context_switches;
} Escalonador;
```

### Funções Principais

```c
// T1-A
gm_init()           // Inicializa memória
gm_alloc()          // Aloca frames
gm_load_program()   // Carrega programa

// T1-B
gp_new()            // Cria processo
context_switch()    // Troca de contexto
save_context()      // Salva estado
restore_context()   // Restaura estado

// T1-C
sch_init()          // Inicializa escalonador
sch_run()           // Loop principal
sch_cycle()         // Um ciclo
sch_dump_stats()    // Mostra estatísticas
```

## 🚀 Próximos Passos

1. **Explore o código:** Leia os arquivos em `T1C/`
2. **Execute:** `./vm_so` e observe a saída
3. **Experimente:** Modifique tamanho de quantum, número de processos, etc
4. **Implemente:** Tente adicionar novo algoritmo de escalonamento
5. **Aprenda:** Use [FAQ_PERGUNTAS_RESPOSTAS.md](FAQ_PERGUNTAS_RESPOSTAS.md) para dúvidas

## 📞 Suporte

Todas as dúvidas podem ser respondidas em:

1. **Documentação técnica:** [DOCUMENTACAO_COMPLETA.md](DOCUMENTACAO_COMPLETA.md)
2. **Explicações específicas:** [T1A|T1B|T1C]_EXPLICACAO_DETALHADA.md
3. **Perguntas frequentes:** [FAQ_PERGUNTAS_RESPOSTAS.md](FAQ_PERGUNTAS_RESPOSTAS.md)
4. **Troubleshooting:** [GUIA_TESTES.md](GUIA_TESTES.md)

## 🎉 Conclusão

Você tem em mãos um **sistema operacional educacional completo** que simula:

✅ Memória virtual com paginação (T1-A)
✅ Multiprogramação com processos isolados (T1-B)
✅ Escalonamento justo com Round-Robin (T1-C)

Este é um excelente projeto para **entender SO em profundidade** e ter uma base sólida para aprender SOs reais como Linux e Windows!

**Bom aprendizado!** 📚🚀

---

**Última atualização:** 2024
**Versão:** 1.0 Completa
**Status:** ✅ Funcional e Documentado
