// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
//
// -----  U T I L I T A R I O S   D O   S I S T E M A  -----
// Funções utilitárias para carga e debug do sistema
// - Carregamento de programas em memória
// - Dump (impressão) de memória para visualização
// - Função load_and_exec para carregar e executar programa
#include "hardware.h"
#include "programs.h"
#include "gm.h"
#ifndef UTILS_H
#define UTILS_H



// Carrega um programa na memória
// mem:  referência à memória do sistema
// prog: array de Words com as instruções
// size: número de instruções a carregar
void utils_load_program(Memory *mem, GM *gm, const int *tabelaPaginas, int tamTabela, int progSize, const Word *prog);

// Imprime uma Word (instrução ou dado) para debug
// Formato: [ OPCODE, ra, rb, p ]
void utils_dump_word(const Word *w);

// Imprime um range de memória (dump da memória)
// mem: referência à memória do sistema
// ini: endereço inicial (inclusive)
// fim: endereço final (exclusivo)
void utils_dump_mem(const Memory *mem, int ini, int fim);

// Carrega e executa um programa
// cpu:  CPU onde o programa será executado
// mem:  Memória do sistema
// prog: Array de Words com as instruções
// size: Número de instruções
// Esta função faz load do programa, executa o setup da CPU e chama run
void utils_load_and_exec(CPU *cpu, Memory *mem, GM *gm, const int *tabelaPaginas, int tamTabela, int progSize, const Word *prog);

#endif // UTILS_H
