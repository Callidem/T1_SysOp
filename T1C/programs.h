// PUCRS - Escola Politécnica - Sistemas Operacionais
// Prof. Fernando Dotti
//
// -----  P R O G R A M A S  -----
// Programas armazenados (como se estivessem em disco)
// que podem ser carregados para a memória (load faz isto)
// Esta estrutura representa a imagem de um programa em memória

#ifndef PROGRAMS_H
#define PROGRAMS_H

#include "hardware.h"

// Estrutura de um programa
// name:  nome identificador do programa
// image: array de Words contendo as instruções do programa
// size:  número de instruções do programa
typedef struct {
    const char *name;
    const Word *image;
    int size;
} Program;

// Recupera um programa pelo nome
// Retorna pointer para a estrutura Program ou NULL se não encontrado
const Program *retrieve_program(const char *name);

#endif // PROGRAMS_H
