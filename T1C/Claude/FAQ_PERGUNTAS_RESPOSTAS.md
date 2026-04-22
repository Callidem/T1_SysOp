# FAQ - Perguntas Frequentes

## Perguntas Gerais

### P1: O que é T1-A, T1-B e T1-C?

**R:** São três partes de um projeto de Sistemas Operacionais:

- **T1-A:** Gerenciador de Memória com **Paginação** (virtualização de memória)
- **T1-B:** Gerenciador de Processos com **Multiprogramação** (múltiplos processos)
- **T1-C:** Escalonador com **Time-Slicing** (fairness automática)

Cada parte constrói sobre a anterior:
```
T1-A (memória isolada)
  ↓
T1-B (múltiplos processos isolados)
  ↓
T1-C (múltiplos processos com fairness)
  ↓
= Mini Sistema Operacional!
```

### P2: Por que dividir em 3 partes?

**R:** Cada parte ensina um conceito fundamental:

1. **T1-A:** Como gerenciar memória para múltiplos programas?
   → Resposta: Paginação com tradução de endereços

2. **T1-B:** Como rodar múltiplos programas simultaneamente?
   → Resposta: Process Control Blocks com Context Switching

3. **T1-C:** Como garantir que cada processo é justo?
   → Resposta: Escalonador com Round-Robin

### P3: Preciso implementar os 3 ou posso pular algum?

**R:** **Não pule!** Cada um depende do anterior:

- T1-B precisa de T1-A (para alocar memória de cada processo)
- T1-C precisa de T1-B (para ter processos para escalonar)

Pular uma parte quebra todo o sistema.

### P4: Qual é a relação com SO real?

**R:** Este projeto simula conceitos reais:

| Conceito | Projeto | SO Real |
|----------|---------|---------|
| Paginação | T1-A | Linux/Windows |
| Processos | T1-B | Linux/Windows |
| Escalonador | T1-C | Linux/Windows |

É uma **versão educacional simplificada**, mas os conceitos são os mesmos!

## Perguntas sobre T1-A

### P5: O que é Paginação?

**R:** Dividir memória em **pedaços fixos** para evitar fragmentação:

```
SEM PAGINAÇÃO:
Prog1 (20 palavras): [ocupado]
Prog2 (30 palavras): [livre]
Prog3 (25 palavras): [ocupado]
Problema: Espaço livre fragmentado!

COM PAGINAÇÃO:
Frame 0 [16 palavras] ← Prog1
Frame 1 [16 palavras] ← Prog1
Frame 2 [16 palavras] ← Livre
Frame 3 [16 palavras] ← Prog2
Benefício: Sem fragmentação!
```

### P6: O que é Tabela de Páginas?

**R:** Um **mapeamento** de endereços virtuais para físicos:

```
Processo 0 vê: 0, 1, 2, ... (endereços virtuais)
Tabela de Páginas 0:
├─ Página 0 → Frame 2
├─ Página 1 → Frame 3
└─ Tradução: Endereço 20 → Frame 3 → memoria[48+4=52]
```

### P7: Por que cada processo tem tabela própria?

**R:** **Segurança e isolamento!**

```
Processo 0 e Processo 1 podem ter o mesmo programa!

Processo 0 acessa endereço 20:
├─ Tabela: [2, 3]
├─ Traduz: Frame 3 → memoria[52]

Processo 1 acessa endereço 20:
├─ Tabela: [4, 5]
├─ Traduz: Frame 5 → memoria[84]

→ DIFERENTES! Dados protegidos!
```

### P8: Como funciona a tradução de endereço?

**R:** Fórmula simples:

```
Endereço Virtual: V
Tamanho de Página: P (16)

Página: V / P
Offset: V % P

Frame: tabela_paginas[página]

Endereço Físico: Frame * P + Offset

Exemplo: V=20, P=16, tabela=[2,3]
├─ Página = 20/16 = 1
├─ Offset = 20%16 = 4
├─ Frame = tabela[1] = 3
└─ Físico = 3*16 + 4 = 52 ✓
```

### P9: O que é o Bitmap?

**R:** Array que rastreia frames **livres vs ocupados**:

```
frameLivre[0] = true   (livre)
frameLivre[1] = true   (livre)
frameLivre[2] = false  (ocupado - Proc0)
frameLivre[3] = false  (ocupado - Proc0)
frameLivre[4] = false  (ocupado - Proc1)
...
```

**Benefício:** Alocação rápida (busca bit por bit)

### P10: First Fit allocation - o que é?

**R:** Estratégia simples de alocar:

```
Preciso de 2 frames livres:
├─ Procuro desde o início
├─ Encontro Frame 0: livre ✓
├─ Encontro Frame 1: livre ✓
└─ Aloco para este processo

Próxima vez:
├─ Procuro desde o início novamente
├─ Frames 0-1: ocupados (pulo)
├─ Frames 2-3: ocupados (pulo)
├─ Frames 4-5: livres ✓
└─ Aloco aqui
```

**Simples, não ótimo, mas funciona!**

## Perguntas sobre T1-B

### P11: O que é um Processo?

**R:** Programa + Contexto + Estado

```
Program (estático):
└─ fatorialV2.bin (20 instruções em disco)

Processo (dinâmico):
├─ Código: carregado em memoria
├─ Dados: armazenados em memoria
├─ Contexto: PC, registradores (em PCB)
├─ Estado: PRONTO, EXECUTANDO, etc
└─ Identidade: ID único
```

Múltiplos processos podem ser da **mesma instância** de programa!

### P12: O que é PCB?

**R:** **Process Control Block** - "Carteira de Identidade" do processo

```c
struct ProcessoControlBlock {
    int id;                    // Quem sou
    int pc_salvo;              // Onde estou executando
    int regs_salvos[10];       // Estado de CPU
    int estado;                // PRONTO, EXECUTANDO, etc
    int *tabela_paginas;       // Onde minha memória está
    int nro_paginas;           // Quanto de memória tenho
};
```

### P13: O que é Context Switch?

**R:** Trocar qual processo está em execução:

```
ANTES:
CPU: PC=5, R0=20, R1=3, tabela=[2,3]
└─ Está rodando Processo 0

CONTEXT SWITCH:

1. SAVE:
   PCB0.pc_salvo = 5
   PCB0.regs_salvos = [20, 3, ...]
   PCB0 = CONGELADO

2. RESTORE:
   CPU.pc = 0  (do PCB1)
   CPU.reg[] = [0, 0, ...]  (do PCB1)
   CPU.tabela_paginas = [4,5]  (do PCB1)

DEPOIS:
CPU: PC=0, R0=0, R1=0, tabela=[4,5]
└─ Agora está rodando Processo 1
```

### P14: Por que precisa mudar tabela_paginas?

**R:** Porque cada processo tem **memória diferente**!

```
Se não muda:
├─ Proc0 acessa 20 → Frame 2 (sua memória)
├─ Muda para Proc1
├─ Proc1 acessa 20 → Frame 2 (ainda de Proc0!)
└─ DADO ERRADO! ❌

Se muda:
├─ Proc0 acessa 20 → Frame 2 (sua memória)
├─ Muda para Proc1 E muda tabela para [4,5]
├─ Proc1 acessa 20 → Frame 4 (sua memória!)
└─ CORRETO! ✓
```

### P15: Como a fila de prontos funciona?

**R:** **Fila FIFO circular** para ordem dos processos:

```
Fila: [0, 1, 2]

Exec:
├─ Tira 0, executa
├─ Coloca 0 ao final → fila = [1, 2, 0]
├─ Tira 1, executa
├─ Coloca 1 ao final → fila = [2, 0, 1]
├─ Tira 2, executa
├─ Coloca 2 ao final → fila = [0, 1, 2]
└─ ... repete (circular!)
```

### P16: Qual a diferença entre guardar contexto em PCB vs em CPU?

**R:** Armazenamento vs Uso

```
CPU: Registro imediatamente acessível pela CPU
└─ Rápido, mas temporário
└─ Se muda de processo, sobrescreve!

PCB: Almacenamiento permanente do processo
└─ Lento para acessar (dereferência)
└─ Persiste mesmo congelado
└─ Permite restaurar depois

Fluxo:
CPU (ativo) ← → PCB (armazenamento)
```

## Perguntas sobre T1-C

### P17: O que é Escalonador?

**R:** **Programa que decide qual processo executa a cada momento**

```
Sem escalonador:
├─ Desenvolvedor decide manualmente
├─ Chamar context_switch(Proc0)
├─ Chamar context_switch(Proc1)
└─ Cansativo, propenso a erros

Com escalonador:
├─ Automático
├─ Detecta quando trocar
├─ Garante fairness
└─ Sem intervenção manual
```

### P18: O que é Time-Slicing?

**R:** Dar a cada processo uma **fatia de tempo** fixa

```
Quantum = 5 ciclos

Processo 0: [===== 5 ciclos ====>]
Processo 1: [===== 5 ciclos ====>]
Processo 2: [===== 5 ciclos ====>]
Processo 0: [===== 5 ciclos ====>]

→ Justo! Todos recebem 5 ciclos
```

### P19: O que é Round-Robin?

**R:** Algoritmo que alterna **circularmente** entre processos

```
Fila: [0, 1, 2]
Quantum: 5

Ciclos 0-4:   Processo 0 (sai de fila, volta ao fim)
Ciclos 5-9:   Processo 1 (sai de fila, volta ao fim)
Ciclos 10-14: Processo 2 (sai de fila, volta ao fim)
Ciclos 15-19: Processo 0 (novamente!)
...

Ordem cíclica: 0→1→2→0→1→2→...
```

### P20: Como garantir fairness?

**R:** Cada processo recebe **tempo igual**

```
Com 500 ciclos e 3 processos:
├─ Proc0: 500/3 ≈ 167 ciclos (33.4%)
├─ Proc1: 500/3 ≈ 167 ciclos (33.4%)
└─ Proc2: 500/3 ≈ 166 ciclos (33.2%)

Se usar Round-Robin com quantum:
├─ Distribui em "fatias" iguais
├─ ~167 fatias por processo
└─ Garantia matemática!
```

### P21: O que sch_cycle() faz?

**R:** Um passo do escalonador:

```
sch_cycle():
├─ if (precisa trocar?) → context_switch()
├─ cpu_run()           → executa 1 instrução
└─ atualiza_contadores()
```

### P22: O que sch_run() faz?

**R:** O loop principal do escalonador:

```
sch_run():
├─ while (tempo_total < MAXIMO)
│  └─ sch_cycle()
└─ Fim
```

Simples! É um loop que chama `sch_cycle()` até atingir 500 ciclos.

### P23: Como detectar fim de fatia?

**R:** Contador simples:

```c
if (ciclos_fatia_atual >= fatia_tamanho) {
    // Trocar processo!
    context_switch_next();
}
```

### P24: Por que precisa resetar ciclos_fatia_atual após switch?

**R:** Para contar a próxima fatia corretamente:

```
Fatia de Proc0 (ciclos 0-4):
├─ ciclos_fatia_atual = 0, 1, 2, 3, 4, 5
└─ 5 >= 5? SIM! Trocar

Context switch:
└─ ciclos_fatia_atual = 0  ← RESET!

Fatia de Proc1 (ciclos 5-9):
├─ ciclos_fatia_atual = 0, 1, 2, 3, 4, 5
└─ 5 >= 5? SIM! Trocar
```

## Perguntas sobre Integração

### P25: Como T1-A, T1-B, T1-C trabalham juntos?

**R:** Camadas de abstração:

```
T1-C: "Execute próximo processo por 5 ciclos"
  ↓
T1-B: "Carrega contexto do Processo X"
  ↓
T1-A: "Acessa memoria[endereco] via tabela[pagina]"
  ↓
CPU+Mem: "Executa instrução"
```

Cada camada usa a anterior!

### P26: O que acontece quando um processo termina?

**R:** É removido da fila (em T1-C):

```
Se CPU.cpuStop = true:
├─ save_context()
├─ NÃO enqueue de volta
├─ dequeue próximo
└─ Processo desaparece da fila

Fila antes: [0, 1, 2]
Proc0 termina:
Fila depois: [1, 2]
```

### P27: E se fila fica vazia?

**R:** Espera nova inserção (não aplicável neste projeto):

```
Fila: []
sch_cycle():
├─ dequeue() → retorna -1 (vazio)
├─ processo_atual = -1
└─ Pula cpu_run()

Na prática: Loop infinito inofensivo até TEMPO_MAXIMO
```

### P28: Posso mudar tamanho de quantum?

**R:** **Sim!** Edite scheduler.c:

```c
sch.fatia_tamanho = 10;  // Em vez de 5

Efeitos:
├─ Menos context switches (200 em vez de 100)
├─ Cada processo roda mais tempo seguido
└─ Fairness mantida (ainda ~33% cada)
```

### P29: Posso mudar para outro algoritmo?

**R:** **Sim!** Implemente em scheduler.c:

```c
// Atual: Round-Robin
int proximo = fila_pronto[(idx + 1) % tamanho];

// Alternativa: Priority (por prioridade)
int proximo = fila[prioridade_maxima];

// Alternativa: FCFS (First Come First Served)
int proximo = fila_pronto[0];
```

## Perguntas de Debugging

### P30: Programa trava na execução

**R:** Causas possíveis:

```
1. Loop infinito em sch_run()
   Solução: Diminua TEMPO_MAXIMO_CPU

2. Fila infinita
   Solução: Adicione contador de desperdícios

3. Memory leak
   Solução: Use valgrind para detectar
```

### P31: Fairness incorreta

**R:** Verifique:

```
1. Tamanho de quantum correto?
   printf("Fatia: %d\n", fatia_tamanho);

2. Context switch sendo feito?
   printf("Switch: Proc%d → Proc%d\n", antes, depois);

3. Enqueue/dequeue funcionando?
   for (int i = 0; i < tamanho_fila; i++)
       printf("%d ", fila[i]);
```

### P32: Contexto não se preserva

**R:** Verifique save/restore:

```
1. save_context está salve PC?
   printf("Save: PC=%d\n", pcb.pc_salvo);

2. restore_context restaura?
   printf("Restore: PC=%d\n", cpu.pc);

3. PCB sendo passado corretamente?
   Adicione assert ou print
```

### P33: Compilação falha

**R:** Comum:

```
1. #include errado
   Solução: Verificar todos os #include

2. Estrutura não definida
   Solução: Adicionar typedef ou struct

3. Múltiplas definições
   Solução: Adicionar #ifndef include guards
```

## Perguntas de Performance

### P34: Quanto overhead tem um context switch?

**R:** Neste projeto: **negligível**

```
Operações:
├─ save_context: 11 assignments (PC + 10 regs)
├─ restore_context: 11 assignments
├─ muda tabela_paginas: 1 assignment
└─ Total: ~33 operações

Em CPU real: ~microsegundos
Aqui: instant (simulado)

Overhead: 0 ciclos (fora dos 5 de fatia)
```

### P35: Quanto tempo gasta em escalonamento?

**R:** Incluído nos 500 ciclos:

```
500 ciclos total:
├─ ~490 ciclos executando programas
├─ ~10 ciclos gerenciando (negligível)
└─ ~99% tempo real, ~1% overhead
```

## Perguntas Conceituais

### P36: Por que SO existe?

**R:** Para **abstrair e compartilhar** recursos:

```
Sem SO:
├─ Cada programa controla TUDO (CPU, Mem, IO)
├─ Programas conflitam
└─ Impossível multitarefa

Com SO:
├─ SO arbitro recursos
├─ Programas compartilham CPU
├─ Multitarefa possível!
```

### P37: Qual é o papel de cada camada?

**R:** Progressão de abstração:

```
T1-A: Abstrai memória física
  └─ Programa vê: espaço contíguo
  └─ Realidade: frames espalhados

T1-B: Abstrai isolamento de processo
  └─ Programa vê: execução contínua
  └─ Realidade: alternância

T1-C: Abstrai distribuição de tempo
  └─ Programa vê: tempo justo
  └─ Realidade: concorrência compartilhada
```

### P38: Isso é igual a um SO real?

**R:** **Conceitos sim, implementação não:**

```
Igual:
├─ Paginação de memória ✓
├─ Processos com contexto ✓
├─ Context switching ✓
├─ Round-Robin scheduling ✓

Diferente:
├─ Sem IO (não lida com disco/rede)
├─ Sem sincronização (threads)
├─ Sem exceções complexas
├─ Sem gerenciamento dinâmico avançado
└─ Muito mais simples!
```

### P39: Qual SO real usa Round-Robin?

**R:** **Nenhum moderno** (mas foi usado no passado):

```
Linux:
├─ CFS (Completely Fair Scheduler)
└─ Complexo, baseado em fairness (não quantum fixo)

Windows:
├─ Priority-based scheduler
└─ Múltiplas filas por prioridade

Android:
├─ Linux scheduler (CFS)
└─ + real-time extensions

Este projeto: Round-Robin (educacional, não production)
```

### P40: Por que aprender SO simulado?

**R:** Várias razões:

```
1. Entender conceitos fundamentais
   ├─ Paginação, processos, escalonamento
   └─ Sem lidar com complexidade do OS real

2. Implementar na mão
   ├─ Entender "por baixo do capô"
   ├─ Não apenas usar APIs prontas
   └─ Conhecimento profundo

3. Experimentar modificações
   ├─ Mudar tamanho de quantum
   ├─ Implementar outro algoritmo
   ├─ Sem quebrar sistema real!
   └─ Sandbox seguro

4. Avaliar trade-offs
   ├─ fairness vs overhead
   ├─ fragmentação vs velocidade
   └─ Tomar decisões informadas
```

## Resumo FAQ

**Conceitual:**
- T1-A ensina paginação (mem virtual)
- T1-B ensina multiprogramação (processos)
- T1-C ensina escalonamento (fairness)

**Implementação:**
- Save/restore contexto em PCB
- Fila circular para Round-Robin
- Contador de ciclos para quantum

**Debugging:**
- Adicione prints de debug
- Use valgrind para memory
- Verifique fairness manualmente

Se não encontrou sua pergunta aqui, procure na **DOCUMENTACAO_COMPLETA.md** ou nos arquivos específicos! 📚
