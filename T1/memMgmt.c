#include "hardware.h"

#include <stdbool.h>

// -----  G E R E N C I A M E N T O   D E   M E M Ó R I A   V I R T U A L  -----
// Alocação e liberação de frames para paginação simples

// -----  A L O C A Ç Ã O   D E   P R O G R A M A  -----
// Reserva frames para um programa
// Parâmetros:
//    mem:      memória física
//    tabelaPaginas: tabela onde será gravado o frame de cada página
//    nro_pgs:  número de páginas do programa
// Retorno: true se conseguiu alocar, false caso contrário

bool alloc_program(Memory *mem, int *tabelaPaginas, size_t nro_pgs) {
    // Verifica se há frames livres suficientes
    if (nro_pgs > (size_t)mem->free_frames) {
        return false;
    }

    // Procura frames livres do início ao fim da memória
    int frame_atual = 0;

    // Preenche a tabela de páginas com os frames escolhidos
    for (size_t pagina = 0; pagina < nro_pgs; ++pagina) {
        while (frame_atual < mem->nro_frames && mem->allocated_frames[frame_atual]) {
            frame_atual++;
        }

        if (frame_atual >= mem->nro_frames) {
            return false;
        }

        tabelaPaginas[pagina] = frame_atual;
        mem->allocated_frames[frame_atual] = true;
        mem->free_frames--;
        frame_atual++;
    }

    return true;
}

// -----  L I B E R A Ç Ã O   D E   P R O G R A M A  -----
// Libera os frames usados por um programa
// Parâmetros:
//    mem:            memória física
//    tabelaPaginas:  frames usados por cada página
//    nro_pgs:        número de páginas do programa
void free_program(Memory *mem, const int *tabelaPaginas, size_t nro_pgs) {
    for (size_t pagina = 0; pagina < nro_pgs; ++pagina) {
        int frame = tabelaPaginas[pagina];

        if (frame >= 0 && frame < mem->nro_frames && mem->allocated_frames[frame]) {
            mem->allocated_frames[frame] = false;
            mem->free_frames++;
        }
    }
}