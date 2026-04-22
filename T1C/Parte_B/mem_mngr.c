#include "hardware.h"
#include "mem_mngr.h"
#include <stdlib.h>

#include <stdio.h>

bool gm_init(Mem_Mngr *gm, int tamMem, int tamPg){

    gm->tamMem = tamMem;
    gm->tamPg = tamPg;
    gm->nroFrames = tamMem / tamPg;
    gm->free_frames = (bool *)malloc(gm->nroFrames * sizeof(bool));
    if (!gm->free_frames) {
        fprintf(stderr, "Erro ao alocar tabela de frames\n");
        return false;
    }
    for (int i = 0; i < gm->nroFrames; ++i) {
        gm->free_frames[i] = true; // começa com todos os frames livres
    }
    memory_init(&gm->mem, 1024);      // Cria memória com 1024 palavras
    return true;
}
bool gm_alloc(Mem_Mngr *gm, size_t nro_palavras){
if (nro_palavras > (size_t)gm->nroFrames * gm->tamPg) {
        return false; // Não há memória suficiente para alocar o programa
    }
    size_t frames_necessarios = (nro_palavras + gm->tamPg - 1) / gm->tamPg; // número de frames necessários para alocar o programa
    size_t frames_alocados = 0;
    for (size_t i = 0; i < (size_t)gm->nroFrames && frames_alocados < frames_necessarios; ++i) {
        if (gm->free_frames[i]) {
            gm->free_frames[i] = false; // Marca frame como ocupado
            frames_alocados++;
        }
    }
    return true;
}
void gm_free_all(Mem_Mngr *gm, size_t *frames, size_t nro_frames){
    for (size_t i = 0; i < nro_frames; ++i) {
        if (frames[i] < (size_t)gm->nroFrames) {
            gm->free_frames[frames[i]] = true; // Marca o frame como livre
        }
    }
}