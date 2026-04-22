#include "hardware.h"


typedef struct {
    int tamMem;
    int tamPg;
    int nroFrames;
    bool *free_frames;
    Memory mem;
} Mem_Mngr;

bool gm_init(Mem_Mngr *gm, int tamMem, int tamPg);
bool gm_alloc(Mem_Mngr *gm, size_t nro_palavras);
void gm_free_all(Mem_Mngr *gm, size_t *frames, size_t nro_frames);