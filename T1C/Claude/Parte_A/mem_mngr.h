#include "hardware.h"

#include <stdbool.h>

// -----  G E R E N C I A M E N T O   D E   M E M Ó R I A   V I R T U A L  -----
// Funções para alocação e liberação de frames em memória virtual
// Implementa paginação simples com tabela de frames livres/ocupados

// Gerente de Memória
typedef struct {
    int tamMem;           // Tamanho total da memória
    int tamPg;            // Tamanho de página
    int nroFrames;        // Número de frames
    bool *frameLivre;     // Array indicando frames livres
    Word *memoria;        // Array de memória física
} Mem_Mngr;

bool gm_init(Mem_Mngr *gm, int tamMem, int tamPg);
bool gm_alloc(Mem_Mngr *gm, size_t nro_palavras);
void gm_free(Mem_Mngr *gm, size_t *frames, size_t nro_frames);
size_t gm_addr(Mem_Mngr *gm, size_t end_lógico);
