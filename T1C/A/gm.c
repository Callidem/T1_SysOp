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

bool aloca(int nroPalavras, GM *gm){
// se consegue alocar, retorna true
    if(nroPalavras <= 0) {
        printf("Alocacao falhou: numero de palavras invalido ou tabela de paginas nula\n");
        return false;
    }
    int numPaginas = (nroPalavras + gm->tamPg - 1) / gm->tamPg;
    if(numPaginas > gm->framesLivres){
        printf("Falha na alocação: não há frames livres suficientes\n");
        return false; // não tem frames livres suficientes
    } 
    gm->tamTabela = numPaginas; // guarda numero de paginas
    gm->tabelaPaginas = (int *)malloc(numPaginas * sizeof(int));
    if (!gm->tabelaPaginas) {
        printf("Alocacao falhou: erro ao alocar tabela de paginas\n");
        return false;
    }

    // inicializa entradas como não alocadas
    for (int i = 0; i < numPaginas; ++i) {
        gm->tabelaPaginas[i] = -1;
    }
    for(int i = 0; i < numPaginas; i++){
        gm->tabelaPaginas[i] = -1; // inicializa tabela de páginas com -1 (não alocada)
    }

    //acha quais frames estão livres e aloca
    int alocadas = 0;
    for(int i = 0; i < gm->numFrames && alocadas < numPaginas; i++){
        if(!gm->framesAlocados[i]){
            gm->framesAlocados[i] = true; // marca como alocado
            gm->tabelaPaginas[alocadas++] = i; // armazena o número do frame na tabela de páginas
            printf("Alocando pagina %d no frame %d\n", alocadas-1, i);
            gm->framesLivres--;
        }
    }   
    for(int i = alocadas; i < numPaginas; i++){
        printf("Tabela de páginas[%d]= %d\n", i, gm->tabelaPaginas[i]);
    }
    return true;
}

void desaloca(GM *gm, int numPaginas){
    if(!gm->tabelaPaginas || numPaginas <= 0) return;
    for(int i = 0; i < numPaginas; i++){
        int frame = gm->tabelaPaginas[i];
        if(frame >= 0 && frame < gm->numFrames){
            gm->framesAlocados[frame] = false; // marca como livre
            gm->framesLivres++;
        }
    }
}

int end_fisico(GM *gm, int enderecoLogico){
    if(gm->tabelaPaginas == NULL || enderecoLogico < 0) {
        printf("Tradução falhou: tabela de paginas nula ou endereço lógico inválido = %d\n", enderecoLogico);
        return -1;
    }
    int pagina = enderecoLogico / gm->tamPg;
    int offset = enderecoLogico % gm->tamPg;
    if (pagina < 0 || pagina >= gm->tamTabela) {
        printf("Tradução falhou: página fora do intervalo = %d (tamTabela=%d)\n", pagina, gm->tamTabela);
        return -1;
    }
    int frame = gm->tabelaPaginas[pagina];
    if(frame < 0) {
        printf("Tradução falhou: página não alocada = %d\n", pagina);
        return -1;
    }
    return frame * gm->tamPg + offset; // endereço físico
}