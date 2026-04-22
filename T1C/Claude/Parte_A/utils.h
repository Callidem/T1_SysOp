// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
//
// -----  U T I L I T A R I O S   D O   S I S T E M A  -----
// Funções utilitárias para carga e debug do sistema
// - Carregamento de programas em memória
// - Dump (impressão) de memória para visualização
// - Função load_and_exec para carregar e executar programa

#ifndef UTILS_H
#define UTILS_H

#include "hardware.h"

// Carrega um programa na memória com paginação
// cpu:  referência à CPU com tabela_paginas alocada
// prog: array de Words com as instruções
// size: número de instruções a carregar
void utils_load_program(CPU *cpu, const Word *prog, int size);

// Imprime uma Word (instrução ou dado) para debug
// Formato: [ OPCODE, ra, rb, p ]
void utils_dump_word(const Word *w);

// Imprime um range de memória (dump da memória) com paginação
// cpu: referência à CPU com pages alocadas
// ini: endereço lógico inicial (inclusive)
// fim: endereço lógico final (exclusivo)
void utils_dump_mem(CPU *cpu, int ini, int fim);

// Carrega e executa um programa
// cpu:  CPU onde o programa será executado
// mem:  Memória do sistema
// prog: Array de Words com as instruções
// size: Número de instruções
// Esta função faz load do programa, executa o setup da CPU e chama run
void utils_load_and_exec(CPU *cpu, Memory *mem, const Word *prog, int size);

#endif // UTILS_H
