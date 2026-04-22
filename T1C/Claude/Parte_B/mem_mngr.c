#include "hardware.h"
#include "mem_mngr.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// -----  G E R E N C I A M E N T O   D E   M E M Ó R I A   V I R T U A L  -----
// Funções para alocação e liberação de frames em memória virtual
// Implementa paginação simples com tabela de frames livres/ocupados
bool gm_init(Mem_Mngr *gm, int tamMem, int tamPg){
    gm->tamMem = tamMem;
    gm->tamPg = tamPg;
    gm->nroFrames = tamMem / tamPg;
    gm->frameLivre = (bool *)malloc(gm->nroFrames * sizeof(bool));
    if (!gm->frameLivre) {
        fprintf(stderr, "Erro ao alocar tabela de alocação de frames\n");
        return false;
    }
    for (int i = 0; i < gm->nroFrames; ++i) {
        gm->frameLivre[i] = true; // começa com todos os frames livres
    }
    gm->memoria = (Word *)malloc(gm->tamMem * sizeof(Word));
    if (!gm->memoria) {
        fprintf(stderr, "Erro ao alocar memoria física\n");
        free(gm->frameLivre);
        return false;
    }
    for (size_t i = 0; i < gm->tamMem; ++i) {
        gm->memoria[i].opc = ___;
        gm->memoria[i].ra = -1;
        gm->memoria[i].rb = -1;
        gm->memoria[i].p = -1;
    }
    return true;
}

// -----  A L O C A Ç Ã O   D E   P R O G R A M A  -----
size_t gm_addr(Mem_Mngr *gm, size_t end_lógico){
    size_t frame = end_lógico / gm->tamPg;
    size_t offset = end_lógico % gm->tamPg;
    return frame * gm->tamPg + offset;
}

// -----  A L O C A Ç Ã O   D E   P R O G R A M A  -----
// Aloca frames na memória para armazenar um programa
// Parâmetros:
//    frameLivre: array indicando quais frames estão livres
//    nroFrames: número total de frames na memória
//    frames_alocados: array para armazenar os índices dos frames alocados
//    frames_necessarios: número de frames necessários para alocar o programa
// Retorno: true se alocação bem-sucedida, false se não há frames suficientes

bool gm_alloc(Mem_Mngr *gm, size_t nro_palavras){
    if (nro_palavras > (size_t)gm->nroFrames * gm->tamPg) {
        return false; // Não há memória suficiente para alocar o programa
    }
    size_t frames_necessarios = (nro_palavras + gm->tamPg - 1) / gm->tamPg; // Calcula o número de frames necessários
    size_t frames_alocados = 0;
    for (size_t i = 0; i < (size_t)gm->nroFrames && frames_alocados < frames_necessarios; ++i) {
        if (gm->frameLivre[i]) {
            gm->frameLivre[i] = false; // Marca o frame como ocupado
            frames_alocados++;
        }
    }
    return frames_alocados == frames_necessarios; // Retorna true se conseguiu alocar todos os frames necessários
}

// -----  L I B E R A Ç Ã O   D E   P R O G R A M A  -----
// Libera os frames que foram alocados para um programa
// Marca os frames como livres na tabela de alocação (allocated_frames)
// Incrementa o contador de frames livres
// Parâmetros:
//    mem:      referência à memória
//    pgTable:  tabela com as páginas (Words) do programa a liberar
//    nro_pgs:  número de páginas/frames do programa
void gm_free(Mem_Mngr *gm, size_t *frames, size_t nro_frames){
    for (size_t i = 0; i < nro_frames; ++i) {
        if (frames[i] < (size_t)gm->nroFrames) {
            gm->frameLivre[frames[i]] = true; // Marca o frame como livre
        }
    }
}

// ===== FUNÇÕES ADICIONAIS PARA T1-B =====

// Libera memória de um processo específico
void gm_free_process(Mem_Mngr *gm, int *tabela_paginas, int nro_paginas) {
    if (!tabela_paginas || nro_paginas <= 0) {
        return;
    }
    
    for (int i = 0; i < nro_paginas; ++i) {
        int frame = tabela_paginas[i];
        if (frame >= 0 && frame < gm->nroFrames) {
            gm->frameLivre[frame] = true;  // Marca frame como livre
        }
    }
}

// Carrega programa na memória com paginação
void gm_load_program(Mem_Mngr *gm, int *tabela_paginas, int nro_paginas,
                     int tamPg, const Word *programa, int tamanho) {
    if (!tabela_paginas || !programa || nro_paginas <= 0) {
        fprintf(stderr, "Erro ao carregar programa: parametros inválidos\n");
        return;
    }
    
    int palavras_carregadas = 0;
    
    for (int i = 0; i < nro_paginas && palavras_carregadas < tamanho; ++i) {
        int frame = tabela_paginas[i];
        
        if (frame < 0 || frame >= gm->nroFrames) {
            fprintf(stderr, "Erro: Frame inválido %d para página %d\n", frame, i);
            continue;
        }
        
        int endereco_base = frame * tamPg;
        int palavras_nesta_pagina = (tamanho - palavras_carregadas > tamPg) ? 
                                     tamPg : (tamanho - palavras_carregadas);
        
        for (int j = 0; j < palavras_nesta_pagina; ++j) {
            int endereco_fisico = endereco_base + j;
            
            if (endereco_fisico >= 0 && endereco_fisico < gm->tamMem) {
                gm->memoria[endereco_fisico] = programa[palavras_carregadas];
                palavras_carregadas++;
            }
        }
    }
    
    if (palavras_carregadas < tamanho) {
        fprintf(stderr, "Aviso: Nem todas as palavras foram carregadas "
                "(%d de %d)\n", palavras_carregadas, tamanho);
    }
}

