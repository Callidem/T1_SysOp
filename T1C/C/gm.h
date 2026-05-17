#ifndef GM_H
#define GM_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "hardware.h"

typedef struct GM {
    int tamPg;
    int numFrames;
    bool *framesAlocados;
    int framesLivres;
    int *tabelaPaginas;
    int tamTabela; // número de páginas alocadas na tabela corrente
} GM;


bool gm_init(GM *gm, Memory *mem, int tamPg);
// aloca: devolve via parametro a tabela de paginas e o numero de paginas
bool aloca(int nroPalavras, int **tabelaPaginasOut, int *numPaginasOut, GM *gm);
// desaloca: recebe a tabela do processo e o numero de paginas e libera frames
void desaloca(const int *tabelaPaginas, int numPaginas, GM *gm);
// Tradução usando tabela fornecida (por processo)
int end_fisico(GM *gm, const int *tabelaPaginas, int tamTabela, int enderecoLogico);

#endif // GM_H