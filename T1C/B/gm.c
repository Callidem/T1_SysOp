#include "gm.h"


bool gm_init(GM *gm, Memory *mem, int tamPg) {
    gm->tamPg = tamPg;
    gm->numFrames = mem->size / gm->tamPg;
    gm->framesAlocados = (bool *)calloc(gm->numFrames, sizeof(bool));
    if (!gm->framesAlocados) {
        printf("Erro ao alocar vetor de frames\n");
        return false;
    }
    gm->framesLivres = gm->numFrames;
    gm->tabelaPaginas = NULL;
    return true;
}

bool aloca(int nroPalavras, int **tabelaPaginasOut, int *numPaginasOut, GM *gm){
    if(nroPalavras <= 0 || !tabelaPaginasOut || !numPaginasOut) {
        printf("Alocacao falhou: parametros invalidos\n");
        return false;
    }
    int numPaginas = (nroPalavras + gm->tamPg - 1) / gm->tamPg;
    if(numPaginas > gm->framesLivres){
        printf("Falha na alocação: não há frames livres suficientes\n");
        return false;
    }

    int *tabela = (int *)malloc(numPaginas * sizeof(int));
    if (!tabela) {
        printf("Alocacao falhou: erro ao alocar tabela de paginas\n");
        return false;
    }
    for (int i = 0; i < numPaginas; ++i) tabela[i] = -1;

    int alocadas = 0;
    for(int i = 0; i < gm->numFrames && alocadas < numPaginas; i++){
        if(!gm->framesAlocados[i]){
            gm->framesAlocados[i] = true;
            tabela[alocadas++] = i;
            //printf("Alocando pagina %d no frame %d\n", alocadas-1, i);
            gm->framesLivres--;
        }
    }

   /* for(int i = 0; i < numPaginas; i++){
        printf("Tabela de páginas[%d]= %d\n", i, tabela[i]);
    }*/
    *tabelaPaginasOut = tabela;
    *numPaginasOut = numPaginas;
    return true;
}

void desaloca(const int *tabelaPaginas, int numPaginas, GM *gm){
    if(!tabelaPaginas || numPaginas <= 0) return;
    for(int i = 0; i < numPaginas; i++){
        int frame = tabelaPaginas[i];
        if(frame >= 0 && frame < gm->numFrames){
            gm->framesAlocados[frame] = false;
            gm->framesLivres++;
        }
    }
}

int end_fisico(GM *gm, const int *tabelaPaginas, int tamTabela, int enderecoLogico){
    if(tabelaPaginas == NULL || enderecoLogico < 0) {
        printf("Tradução falhou: tabela de paginas nula ou endereço lógico inválido = %d\n", enderecoLogico);
        return -1;
    }
    int pagina = enderecoLogico / gm->tamPg;
    int offset = enderecoLogico % gm->tamPg;
    if (pagina < 0 || pagina >= tamTabela) {
        printf("Tradução falhou: página fora do intervalo = %d (tamTabela=%d)\n", pagina, tamTabela);
        return -1;
    }
    int frame = tabelaPaginas[pagina];
    if(frame < 0) {
        printf("Tradução falhou: página não alocada = %d\n", pagina);
        return -1;
    }
    return frame * gm->tamPg + offset; // endereço físico
}