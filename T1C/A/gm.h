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
bool aloca(int nroPalavras, GM *gm);
void desaloca(GM *gm, int numPaginas);
int end_fisico(GM *gm, int enderecoLogico);