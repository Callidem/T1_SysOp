# Sistema Operacional - Máquina Virtual Simulada (Java)

PUCRS - Escola Politécnica - Sistemas Operacionais  
Prof. Fernando Dotti

## Visão Geral

Este projeto implementa uma **máquina virtual simulada** que executa um conjunto de instruções em um ambiente controlado. O sistema é composto por dois componentes principais:

- **Hardware Virtual (HW)**: CPU e Memória que simulam um processador real
- **Software (SO)**: Sistema Operacional que trata interrupções e chamadas de sistema

Esta é a **versão em Java** que contém anotações e comentários explicativos detalhados.

## Estrutura do Código (Sistema.java)

### Organização Geral

```
Dentro da classe Sistema:
  ├─ Hardware HW
  │   ├─ Memory (memória física)
  │   ├─ Word (estrutura de uma palavra de memória)
  │   ├─ CPU (processador)
  │   │   ├─ Opcodes (códigos de operação)
  │   │   └─ Interrupts (interrupções possíveis)
  │   └─ HW (composição de CPU + Memory)
  │
  ├─ Software SO
  │   ├─ InterruptHandling (tratamento de interrupções)
  │   ├─ SysCallHandling (tratamento de chamadas de sistema)
  │   ├─ Utilities (funções auxiliares)
  │   └─ SO (composição dos elementos do SO)
  │
  ├─ Programs (conjunto de programas disponíveis)
  │   ├─ Program (estrutura de um programa)
  │   └─ Programs (coleção de programas)
  │
  └─ main() (ponto de entrada)
```

### Camada de Hardware

#### Definições Básicas

**Memory - Estrutura de Memória**
- `pos[]`: Array de posições de memória
- Cada posição é uma `Word` (instrução ou dado)
- Inicializada com palavras vazias (opcode = ___)

**Word - Estrutura de uma Palavra de Memória**
- `opc`: Código de operação (instrução a executar)
- `ra`: Índice do primeiro registrador da operação
- `rb`: Índice do segundo registrador da operação
- `p`: Parâmetro para a instrução (constante k ou endereço A)
- Se opcode = DATA, `p` contém um dado

**Opcode - Códigos de Operação Suportados**
```
Dados:        DATA, ___
Desvios:      JMP, JMPI, JMPIG, JMPIL, JMPIE, 
              JMPIM, JMPIGM, JMPILM, JMPIEM,
              JMPIGK, JMPILK, JMPIEK, JMPIGT
Aritméticos:  ADDI, SUBI, ADD, SUB, MULT
Movimentação: LDI, LDD, STD, LDX, STX, MOVE
Sistema:      SYSCALL, STOP
```

**Interrupts - Interrupções Possíveis**
- `noInterrupt`: Sem interrupção
- `intEnderecoInvalido`: Acesso a endereço fora da memória
- `intInstrucaoInvalida`: Instrução inválida ou não suportada
- `intOverflow`: Resultado de operação ultrapassou limites

#### Processador (CPU)

**Contexto da CPU** - Tudo que é necessário para executar um processo:
- `pc`: Program Counter (próxima instrução a executar)
- `ir`: Instruction Register (instrução atual)
- `reg[]`: 10 registradores da CPU (reg[8] e reg[9] para IO)
- `irpt`: Interrupção sinalizada durante execução

**Verificação de Endereçamento**
- Função `legal()`: Valida todo acesso a memória
- Verifica se endereço está dentro dos limites válidos
- Se inválido, liga interrupção `intEnderecoInvalido`

**Teste de Overflow**
- Função `testOverflow()`: Valida operações matemáticas
- Se valor < minInt ou > maxInt, liga interrupção `intOverflow`
- minInt = -32767, maxInt = 32767

#### Ciclo de Execução da CPU

A função `run()` implementa o **ciclo de fetch-decode-execute**:

1. **FASE DE FETCH**
   - Verifica se PC é válido
   - Busca palavra de memória no endereço PC
   - Carrega em IR (Instruction Register)
   - Se debug ativo, imprime registradores e instrução

2. **FASE DE EXECUÇÃO DA INSTRUÇÃO**
   - Conforme opcode, executa a ação correspondente
   - Cada instrução segue sua semântica definida na tabela
   - Exemplos:
     - `LDI`: Carrega constante em registrador
     - `ADD`: Soma dois registradores
     - `JMP`: Salta para novo endereço
     - `SYSCALL`: Chama sistema (IO)
     - `STOP`: Para execução

3. **VERIFICAÇÃO DE INTERRUPÇÃO**
   - TERCEIRA FASE DO CICLO
   - Se interrupção foi sinalizada durante execução:
     - Desvia para rotina de tratamento (handler do SO)
     - Para a CPU (nesta versão)

### Camada de Software (Sistema Operacional)

#### Tratamento de Interrupções

Classe `InterruptHandling`:
- Recebe referência ao hardware
- Função `handle()`: Trata qualquer interrupção
- Nesta versão apenas avisa e finaliza o programa
- Imprime tipo de interrupção e posição do PC

#### Tratamento de Chamadas de Sistema

Classe `SysCallHandling`:
- Suporta operações de IO
- Parâmetros passados via registradores:
  - `reg[8]`: Tipo de operação (1=entrada, 2=saída)
  - `reg[9]`: Endereço da memória
- Operações:
  - 1 (entrada): Lê inteiro da entrada padrão e armazena em memória
  - 2 (saída): Escreve valor da memória na saída padrão
- Função `stop()`: Finaliza o programa

#### Utilitários do Sistema

Classe `Utilities`:
- `loadProgram()`: Carrega programa em memória
  - Copia instruções para as primeiras posições
- `dump()`: Funções para impressão/debug
  - `dump(Word)`: Imprime uma instrução
  - `dump(int ini, int fim)`: Imprime range [ini, fim)
- `loadAndExec()`: Sequência completa
  1. Carrega programa
  2. Imprime programa carregado (dump)
  3. Seta contexto da CPU (PC = 0)
  4. Executa programa
  5. Imprime memória após execução

#### Composição do SO

Classe `SO`:
- Agrupa os elementos do sistema operacional
- `InterruptHandling ih`: Tratadora de interrupções
- `SysCallHandling sc`: Tratadora de syscalls
- `Utilities utils`: Funções utilitárias
- Registra os handlers na CPU

### Programas Armazenados

Classe `Programs` - Representa programas em "disco" (armazenados):

**Programa: fatorial**
- Apenas valores positivos
- r0: valor para calcular
- Resultado: mem[10]
- Usa loop com multiplicação sucessiva

**Programa: fatorialV2**
- Trata valores negativos
- Entrada: mem[19]
- Saída: mem[18] + impressão (SYSCALL)
- Mais robusto que versão 1

**Programa: progMinimo**
- Teste mínimo
- Carrega valor e armazena em múltiplas posições
- Valida operações básicas

**Programa: fibonacci10**
- Calcula 10 números de Fibonacci
- Usa arrays em memória
- Iterativo com índices

**Programa: fibonacci10v2**
- Versão alternativa

**Programa: fibonacciREAD**
- Fibonacci com entrada na memória
- Lê valor de entrada padrão

**Programa: PB e PC**
- Programas de teste
- PC implementa Bubble Sort

### Classe Sistema (Principal)

A classe externo que compõe todo o sistema:
- `HW hw`: Instância do hardware
- `SO so`: Instância do SO
- `Programs progs`: Conjunto de programas

Função `run()`:
- Seleciona um programa
- Chama `loadAndExec()` para executá-lo

Função `main()`:
- Instancia o Sistema com 1024 palavras de memória
- Chama `run()`

## Fluxo de Execução

```
main()
  └─ new Sistema(1024)
     ├─ HW hw = new HW(1024)
     │   ├─ Memory mem = new Memory(1024)
     │   └─ CPU cpu = new CPU(mem, true) // debug=true
     ├─ SO so = new SO(hw)
     │   ├─ InterruptHandling ih = new InterruptHandling(hw)
     │   ├─ SysCallHandling sc = new SysCallHandling(hw)
     │   └─ Utilities utils = new Utilities(hw)
     ├─ cpu.setAddressOfHandlers(ih, sc)
     ├─ cpu.setUtilities(utils)
     └─ run()
        └─ utils.loadAndExec(program)
           ├─ loadProgram(program)
           ├─ dump(0, program.length) // Mostra carregado
           ├─ cpu.setContext(0) // PC = 0
           ├─ cpu.run() // Executa ciclo
           └─ dump(0, program.length) // Mostra resultado
```

## Estrutura de Memória

- **Tamanho**: 1024 palavras
- **Endereçamento**: 0 a 1023
- **Programas**: Carregados começando em endereço 0
- **Registradores**: 10 registradores na CPU
  - reg[0..7]: Uso geral
  - reg[8]: Parâmetro para IO
  - reg[9]: Endereço de memória para IO

## Conceitos Principais

### Máquina Virtual
Simula um processador real com seu próprio conjunto de instruções, permitindo executar programas em um ambiente controlado e isolado.

### Contexto de Processo
O estado completo de um processo (PC, IR, Registradores, Interrupção) que permite retomar sua execução posteriormente.

### Tratamento de Interrupções
Mecanismo para desviar de um fluxo normal de execução quando eventos excepcionais ocorrem (erros, exceções).

### Chamadas de Sistema
Interface entre programas em execução e o SO para requisitar serviços (IO, etc).

## Exemplo de Execução: Fatorial v2

1. **Carga**: Programa é carregado em memória
2. **Setup**: PC = 0, registradores zerados
3. **Execução**:
   - LDI r0, 5: Carrega 5 em r0
   - STD r0, 19: Armazena 5 em mem[19]
   - LDD r0, 19: Recarrega de mem[19]
   - ... loop de multiplicação ...
   - STD r1, 18: Armazena resultado em mem[18]
   - SYSCALL: Escreve resultado
   - STOP: Finaliza
4. **Output**: Valor calculado impresso

## Notas Importantes

- Esta é uma simulação educacional de máquina virtual
- Apenas um processo execute por vez
- Não há proteção de memória entre processos
- Todas as interrupções finalizam o programa (nesta versão)
- Versão completa com comentários explicativos detalhados

## Comparação Java vs C

- **Java**: Contém as anotações e comentários explicativos originais
- **C**: Transposição direta com mesmo comportamento
- Ambas as versões implementam a mesma máquina virtual
- Compatibilidade total de programas
