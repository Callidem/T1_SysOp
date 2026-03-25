#include "hardware.h"

#include <stdbool.h>

// -----  G E R E N C I A M E N T O   D E   M E M Ó R I A   V I R T U A L  -----
// Funções para alocação e liberação de frames em memória virtual
// Implementa paginação simples com tabela de frames livres/ocupados

// -----  A L O C A Ç Ã O   D E   P R O G R A M A  -----
// Aloca frames na memória para armazenar um programa
// Parâmetros:
//    mem:      referência à memória (contém informação de frames)
//    pgTable:  tabela com as páginas (Words) do programa
//    nro_pgs:  número de páginas/frames do programa
// Retorno: true se alocação bem-sucedida, false se não há frames suficientes
bool alloc_program(Memory *mem, Word *pgTable, size_t nro_pgs) {
    //Se não houver frames suficientes para alocar o programa, retorna false
    if (nro_pgs > mem->free_frames) { 
        return false;
    }
    int frame_atual = 0;
    //Verifica os frames disponíveis e aloca o programa, atualizando a tabela de páginas
    for (size_t i = 0; i < nro_pgs; ++i) {
        for(; frame_atual < mem->nro_frames; ++frame_atual) {
            if (!mem->allocated_frames[frame_atual]) {
                mem->allocated_frames[frame_atual] = true;
                mem->free_frames--;
                size_t addr = frame_atual * (mem->size / mem->nro_frames);
                mem->pos[addr] = pgTable[i];
                break;
            }
        }
    }
    return true;
}

// -----  L I B E R A Ç Ã O   D E   P R O G R A M A  -----
// Libera os frames que foram alocados para um programa
// Marca os frames como livres na tabela de alocação (allocated_frames)
// Incrementa o contador de frames livres
// Parâmetros:
//    mem:      referência à memória
//    pgTable:  tabela com as páginas (Words) do programa a liberar
//    nro_pgs:  número de páginas/frames do programa
void free_program(Memory *mem, Word *pgTable, size_t nro_pgs) {
    //Libera os frames alocados para o programa, atualizando a tabela de páginas
    for (size_t i = 0; i < nro_pgs; ++i) {
        for (size_t j = 0; j < mem->nro_frames; ++j) {
            size_t addr = j * (mem->size / mem->nro_frames);
            if (mem->pos[addr].opc == pgTable[i].opc &&
                mem->pos[addr].ra == pgTable[i].ra &&
                mem->pos[addr].rb == pgTable[i].rb &&
                mem->pos[addr].p == pgTable[i].p) {
                mem->allocated_frames[j] = false;
                mem->free_frames++;
                break;
            }
        }
    }
}