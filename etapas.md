## T1-A - Gerente de Memória com Paginação

### Objetivo
Implementar paginação para alocar programas na memória física e executar usando endereços lógicos.

### Escopo da etapa
- Implementar alocação de frames para um programa.
- Implementar desalocação de frames do programa.
- Carregar programa em memória de forma paginada (não contígua na memória física).
- Traduzir endereço lógico para endereço físico em todos os acessos relevantes.
- Garantir proteção de memória (endereços/páginas inválidas geram interrupção).

### Checklist de implementação
- Definir tamanho de página e número de frames com base em tamMem e tamPg.
- Criar/ajustar estrutura de controle de frames livres/ocupados.
- Ajustar interface do GM para retornar tabela de páginas.
- Adaptar rotina de carga para copiar por página em frames da tabela.
- Inserir tradução lógico->físico no ciclo da CPU (fetch e acessos de leitura/escrita).
- Tratar acesso fora do espaço lógico do processo como erro.
- Garantir liberação correta dos frames ao fim da execução.

### Definição de pronto (DoD)
- Um programa executa corretamente mesmo espalhado em diferentes frames.
- A CPU não depende de endereçamento físico contíguo.
- Alocação e desalocação não deixam estado inconsistente.
- Acesso inválido interrompe execução com sinalização adequada.

### Testes mínimos sugeridos
- Programa pequeno que cabe em poucas páginas.
- Programa maior que ocupa várias páginas.
- Tentativa de alocação sem memória suficiente.
- Acesso inválido (lógico fora do mapeamento).

---

## T1-B - Gerente de Processos

### Objetivo
Permitir múltiplos processos no sistema, com criação, remoção e controle via comandos.

### Escopo da etapa
- Criar PCB (Process Control Block) para cada processo.
- Manter estrutura de processo rodando e fila de prontos.
- Criar processo a partir de um programa.
- Remover processo e liberar seus recursos.
- Implementar interface de comandos iterativa.

### Checklist de implementação
- Definir estrutura de PCB com pelo menos:
	- id do processo
	- estado
	- PC inicial/contexto necessário
	- tabela de páginas
	- tamanho do programa
- Implementar criaProcesso:
	- validar programa
	- pedir memória ao GM
	- criar PCB
	- carregar programa
	- inserir em prontos
- Implementar removeProcesso:
	- localizar PCB
	- remover de fila/execução
	- desalocar memória no GM
	- liberar PCB
- Implementar comandos mínimos:
	- new <programa>
	- rm <id>
	- ps
	- dump <id>
	- dumpM <inicio,fim>
	- exec <id>
	- traceOn
	- traceOff
	- exit

### Definição de pronto (DoD)
- É possível criar vários processos e listá-los.
- É possível executar processo por id.
- Remoção libera memória e estrutura de controle corretamente.
- Dumps mostram informações coerentes de PCB e memória.

### Testes mínimos sugeridos
- Criar 3 processos diferentes e listar com ps.
- Executar um processo específico com exec.
- Remover processo não executado e processo já executado.
- Tentar criar processo com memória insuficiente.

---

## T1-C - Escalonamento

### Objetivo
Executar múltiplos processos com política de escalonamento por fatia de tempo (delta).

### Escopo da etapa
- Introduzir interrupção por tempo (contador de instruções ou relógio).
- Salvar contexto do processo interrompido no PCB.
- Restaurar contexto do próximo processo selecionado.
- Implementar execução escalonada de todos os processos.
- Tratar término (STOP) removendo processo finalizado e escalonando próximo.

### Checklist de implementação
- Definir política (ex.: round-robin) e tamanho da fatia (delta).
- Implementar interrupção de tempo na CPU.
- No handler de tempo:
	- salvar contexto do running
	- mover running para ready
	- escolher próximo da ready
	- restaurar contexto na CPU
- Tratar STOP como finalização de processo:
	- desalocar memória
	- remover PCB
	- escalar próximo
- Adicionar comando execAll para rodar tudo de forma escalonada.
- Evoluir para operação contínua com thread(s), se exigido na entrega final.

### Definição de pronto (DoD)
- Múltiplos processos compartilham CPU em ciclos (sem starvation no cenário básico).
- Contexto é preservado entre preempções.
- Processos finalizados são removidos e desalocados corretamente.
- execAll executa até não restar processos ativos.

### Testes mínimos sugeridos
- Carregar 3+ processos e executar com execAll.
- Validar alternância entre processos por delta.
- Confirmar resultado final correto de cada processo.
- Confirmar que todos os recursos foram liberados ao final.

