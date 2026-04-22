# Sistema Operacional - Máquina Virtual Simulada

PUCRS - Escola Politécnica - Sistemas Operacionais  
Prof. Fernando Dotti

## Visão Geral

Este projeto implementa uma **máquina virtual simulada** que executa um conjunto de instruções em um ambiente controlado. O sistema é composto por dois componentes principais:

- **Hardware Virtual (HW)**: CPU e Memória que simulam um processador real
- **Software (SO)**: Sistema Operacional que trata interrupções e chamadas de sistema

## Estrutura do Código

### Camada de Hardware

#### `hardware.h` e `hardware.c`
Define a abstração de hardware com:

- **Memory**: Estrutura que representa a memória física
  - Array de `Word` (posições de memória)
  - Cada posição armazena uma instrução ou um dado
  - Possui informação sobre frames (para gerenciamento de memória virtual)

- **Word**: Estrutura de uma palavra de memória
  - `opc`: Código de operação (instrução a executar)
  - `ra`: Índice do primeiro registrador
  - `rb`: Índice do segundo registrador
  - `p`: Parâmetro da instrução (constante, endereço ou dado)

- **Opcode**: Enum com todos os códigos de operação suportados
  - Instruções de dados: `DATA`, `___`
  - Desvios: `JMP`, `JMPI`, `JMPIG`, `JMPIL`, `JMPIE`, etc.
  - Aritmética: `ADDI`, `SUBI`, `ADD`, `SUB`, `MULT`
  - Movimentação: `LDI`, `LDD`, `STD`, `LDX`, `STX`, `MOVE`
  - Sistema: `SYSCALL`, `STOP`

- **Interrupts**: Enum com interrupções possíveis
  - `intEnderecoInvalido`: Acesso a endereço fora da memória
  - `intInstrucaoInvalida`: Instrução inválida ou não suportada
  - `intOverflow`: Resultado de operação ultrapassou limites

- **CPU**: Estrutura do processador
  - **Contexto da CPU**: Tudo que é necessário para executar um processo
    - `pc`: Program Counter (próxima instrução)
    - `ir`: Instruction Register (instrução atual)
    - `reg[10]`: 10 registradores (reg[8] e reg[9] usados para IO)
    - `irpt`: Interrupção sinalizada
  - Referência à memória física
  - Handlers para interrupções e syscalls
  - Flag `debug` para visualizar execução

#### Ciclo de Execução (Fetch-Decode-Execute)

A função `cpu_run()` implementa o ciclo de execução:

1. **FETCH**: Busca a instrução no endereço apontado por PC
   - Verifica se PC é um endereço válido
   - Carrega a instrução em IR

2. **DECODE/EXECUTE**: Executa a instrução conforme seu opcode
   - Cada instrução segue a semântica da tabela de instruções
   - Pode modificar registradores, memória ou PC

3. **VERIFICAÇÃO DE INTERRUPÇÃO**: Trata exceções
   - Se uma interrupção foi sinalizada, desvia para handler do SO
   - Para a execução da CPU

### Camada de Software (Sistema Operacional)

#### `so.h` e `so.c`

Define as funções de tratamento do SO:

- **`so_interrupt_handle()`**: Trata interrupções
  - Chamada quando ocorre exceção durante execução
  - Tipos: endereço inválido, instrução inválida, overflow
  - Nesta versão apenas avisa e finaliza o programa

- **`so_syscall_handle()`**: Trata chamadas de sistema (IO)
  - Suporta operações de leitura (reg[8] = 1) e escrita (reg[8] = 2)
  - Endereço em memória passado via reg[9]
  - Leitura: input padrão → memória
  - Escrita: memória → output padrão

- **`so_syscall_stop()`**: Trata término de programa
  - Chamada quando CPU encontra instrução STOP
  - Indica fim de execução do programa

### Utilitários

#### `utils.h` e `utils.c`

Funções auxiliares para carga, execução e debug:

- **`utils_load_program()`**: Carrega programa na memória
  - Copia instruções para as primeiras posições de memória

- **`utils_dump_word()`**: Imprime uma palavra (debug)
  - Formato: `[ OPCODE, ra, rb, p ]`

- **`utils_dump_mem()`**: Imprime un range de memória
  - Útil para visualizar instruções/dados carregados

- **`utils_load_and_exec()`**: Sequência completa
  1. Carrega programa em memória
  2. Imprime programa carregado (dump)
  3. Seta contexto da CPU (PC = 0)
  4. Executa programa
  5. Imprime memória com resultado

### Programas Armazenados

#### `programs.h` e `programs.c`

Define programas que podem ser escolhidos e executados:

- **`fatorial`**: Calcula fatorial de um número
  - Entrada: r0 contém o valor
  - Saída: resultado em memória[10]
  - Apenas valores positivos

- **`fatorialV2`**: Fatorial versão 2
  - Trata números negativos
  - Usa SYSCALL para output
  - Resultado impresso

- **`progMinimo`**: Programa mínimo de teste
  - Carrega valor em r0
  - Armazena em múltiplas posições de memória
  - Teste de operações básicas

- **`fibonacci10`**: Sequência de Fibonacci (10 termos)
  - Calcula iterativamente
  - Resultados em memória[20..29]

- **`fibonacciREAD`**: Fibonacci com entrada
  - Lê valor da entrada padrão

- **`PB`**: Teste B

- **`PC`**: Algoritmo de Bubble Sort
  - Ordena um array

#### Função `retrieve_program()`
Busca programa pela identificação (nome)

### Gerenciamento de Memória Virtual

#### `memMgmt.c`

Implementa paginação simples (não usado atualmente):

- **`alloc_program()`**: Aloca frames para programa
  - Verifica disponibilidade
  - Marca frames como ocupados

- **`free_program()`**: Libera frames de programa
  - Marca frames como livres

## Fluxo de Execução

```
main()
  ├─ memory_init()     // Cria memória de 1024 palavras
  ├─ cpu_init()        // Cria CPU com debug ligado
  ├─ cpu_set_handlers() // Registra handlers do SO
  ├─ retrieve_program() // Carrega programa pelo nome
  ├─ utils_load_and_exec()
  │   ├─ utils_load_program()  // Carrega em memória
  │   ├─ utils_dump_mem()      // Mostra program carregado
  │   ├─ cpu_set_context()     // PC = 0
  │   ├─ cpu_run()
  │   │   └─ Ciclo fetch-decode-execute
  │   └─ utils_dump_mem()      // Mostra resultado
  └─ memory_free()     // Libera memória
```

## Compilação e Execução

```bash
make              # Compila o projeto
./vm_so           # Executa a máquina virtual
make clean        # Remove arquivos compilados
```

## Estrutura de Memória

- **Tamanho**: 1024 palavras (endereços 0-1023)
- **Programas**: Carregados a partir do endereço 0
- **Registradores**: 10 registradores na CPU
  - reg[0..7]: Uso geral
  - reg[8]: Parâmetro para IO (1=leitura, 2=escrita)
  - reg[9]: Endereço de memória para IO

## Exemplo de Uso

Para executar o programa de Fatorial v2:

1. O código em `main.c` seleciona `"fatorialV2"`
2. Programa é carregado em memória começando em endereço 0
3. CPU começa execução com PC=0
4. Programa lê número de entrada (SYSCALL com reg[8]=1)
5. Calcula fatorial
6. Escreve resultado (SYSCALL com reg[8]=2)
7. Execução termina com STOP

## Notas Importantes

- Esta é uma simulação educacional de máquina virtual e SO
- O debug pode ser ligado/desligado via parâmetro em `cpu_init()`
- As interrupções nesta versão sempre finalizam o programa
- Não há escalonamento de múltiplos processos (um programa por vez)
- Não há proteção de memória entre processos (não há múltiplos processos)

## Referências

- Baseado em código fornecido e implementado em Java
- Transposição direta para C dos programas e estruturas
- Mantém compatibilidade com versão em Java
