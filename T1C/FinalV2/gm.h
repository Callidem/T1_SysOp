#ifndef GM_H
#define GM_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "hardware.h"

typedef struct GM {
    int tamPg; // tamanho da página em palavras (definido na inicialização)
    int numFrames; // número total de frames disponíveis na memória
    bool *framesAlocados; // vetor de flags indicando se cada frame está alocado ou livre
    int framesLivres; // contador de frames livres para alocação rápida
    int *tabelaPaginas; //vetor com as páginas alocadas para o processo atual (usado para tradução)
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
