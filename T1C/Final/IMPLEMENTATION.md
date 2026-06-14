# Documentação da Implementação — Pasta C Gabriel

## Resumo rápido

- Implementação mínima que cumpre os requisitos dos PDFs A/B/C: shell de comandos, criação/remoção de processos, escalonador Round‑Robin com quantum fixo, suporte a paginação básica via `GM`, e execução de programas de usuário definidos em `programs.c`.
- Optámos por uma implementação simples e direta: o escalonador é cooperativo/bloqueante (após `exec`/`execAll` roda quantums até a fila esvaziar). Não usamos `pthread` — assim o código fica mais simples e corresponde ao comportamento exigido pela especificação.
- Principais ficheiros: [main.c](T1C/Final/main.c#L208), [programs.c](T1C/Final/programs.c#L1), [gm.c/gp.c] (criação e gestão de PCBs), [Makefile](T1C/Final/Makefile).

## Análise detalhada (item por item da especificação)

Aqui listamos os requisitos solicitados nos PDFs A/B/C e explicamos como cada item foi implementado, com referências diretas ao código.

- Comandos do shell (new, exec, execAll, ps, dump, dumpM, rm, traceOn/Off, exit): implementados no laço principal de `main.c` — ver implementação dos comandos em [main.c](T1C/Final/main.c#L220-L240) e tratamento de `exec`/`execAll` em [main.c](T1C/Final/main.c#L304-L327).

- Estados de processo (NEW, READY, RUNNING, BLOCKED, TERMINATED): representados na `struct PCB` (ver definição em `so.h`/`so.c`/gp files) e usados por `main.c` e funções de gestão. O fluxo de estados (NEW→READY→RUNNING→(READY|BLOCKED|TERMINATED)) segue a lógica pedida na especificação; criação marca NEW (veja mensagens em [main.c](T1C/Final/main.c#L236-L240)).

- Escalonamento Round‑Robin com quantum: implementado com o define `QUANTUM` em [main.c](T1C/Final/main.c#L33) e com o dispatch em `run_one_quantum()` ([main.c](T1C/Final/main.c#L138-L158)). A seleção do próximo processo READY é feita por varredura circular em `pick_next_ready()` (ver [main.c](T1C/Final/main.c#L99-L109)).

- Preempção/quantum: o comportamento pedido (quantum fixo e retorno a READY caso não termine) é realizado em `run_one_quantum()` — a função chama `cpu_run_steps(sharedCpu, QUANTUM)` e depois decide se o processo terminou ou volta a READY; o salvamento/restauração de contexto usa `salva_contexto()` e `recup_contexto()` (chamadas em [main.c](T1C/Final/main.c#L151-L156)).

- Carregamento de programas e memória/paginação: os programas estão definidos em [programs.c](T1C/Final/programs.c). A alocação de páginas e carregamento para memória física é feita por `utils_load_program()` durante `new` (veja [main.c](T1C/Final/main.c#L228-L235)). A gestão de frames é feita pela camada `GM` (ver `gm.c`/`gm.h`).

- Remoção de processos (`rm <id>`): implementada em [main.c](T1C/Final/main.c#L282-L294). Se o processo está `RUNNING`, marca-se `killRequested` para remoção no próximo ciclo; caso contrário remove-se imediatamente com `remove_at()`.

- Dump de PCB e memória (`dump`, `dumpM`): `dump_pcb()` e `dump_process_memory()` em [main.c](T1C/Final/main.c#L45-L70) exibem o PCB, tabela de páginas e regiões físicas associadas.

- Trace da CPU (`traceOn`/`traceOff`): controla o campo `cpu.debug` da CPU em [main.c](T1C/Final/main.c#L331-L332); a VM/CPU respeita essa flag durante `cpu_run_steps()` para imprimir detalhes de execução.

- Conformidade com os PDFs A/B/C sobre threads/escalonador: os PDFs mencionam a presença conceitual de threads (Main/Shell/Escalonador). Para manter o projeto simples e sem dependências extras, escolhemos uma implementação equivalente funcionalmente (escalonador cooperativo/bloqueante), que satisfaz os requisitos observáveis da especificação (RR, comandos, troca de contexto). A decisão de não usar `pthread` é documentada e a motivação é minimalismo e previsibilidade do comportamento para avaliação.

- Arquivo `Makefile`: atualizado para remover `-pthread` (não sendo mais necessário). Veja [Makefile](T1C/Final/Makefile).

## Onde procurar cada funcionalidade no código

- Shell / parsing de comandos: [main.c](T1C/Final/main.c#L208-L240) e blocos de comando subsequentes.
- Scheduler e política RR: `pick_next_ready()` e `run_one_quantum()` em [main.c](T1C/Final/main.c#L99-L109) e [main.c](T1C/Final/main.c#L138-L158).
- Criação/carregamento de programas: [main.c](T1C/Final/main.c#L221-L239) e [programs.c](T1C/Final/programs.c#L1).
- Memória / GM: `gm.c`/`gm.h` (veja chamadas a `gm_init()` e `utils_load_program()` em [main.c](T1C/Final/main.c#L214-L235)).
- Salvamento/recuperação de contexto: `salva_contexto()` / `recup_contexto()` chamadas em [main.c](T1C/Final/main.c#L151-L156).

## Notas sobre decisões de design

- Optámos por implementar o escalonador de forma bloqueante após `exec`/`execAll` (função `run_scheduler_blocking()`), porque isto simplifica o comportamento e atende aos requisitos do trabalho sem introduzir concorrência real. Esta abordagem evita problemas com `stdin` piped e garante que os processos sejam efetivamente executados quando solicitados.
- Alternativa: implementar com `pthread` (shell + scheduler em threads separadas) ou usar I/O não‑bloqueante (`select`) para intercalar input e escalonamento. Não foi necessário para cumprir os PDFs A/B/C, por isso foi omitido.

## Como compilar e executar (resumido)

```bash
cd "T1C/C Gabriel"
make
./vm_so
```

## Testes rápidos realizados

- Testes básicos realizados via script: `new progMinimo`, `execAll`, `ps`, `exit` — o escalonador despachou e os processos completaram.
- Teste de `fatorial` com `traceOn` para verificar salvamento/recuperação de contexto e saída esperada.

---

Se quiser, posso adicionar exemplos de saída reais (logs de execução), extrair trechos de código comentados para referência direta, ou adicionar um pequeno conjunto de testes automatizados. Qual prefere a seguir?
