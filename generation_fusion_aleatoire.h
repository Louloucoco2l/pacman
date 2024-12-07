#ifndef PACMAN_GENERATION_FUSION_ALEATOIRE_H
#define PACMAN_GENERATION_FUSION_ALEATOIRE_H

#include "global.h"

// Structure pour les cellules
typedef struct {
    int x, y;
} Cell;



void labSansIlots(int maze[HEIGHT][WIDTH]);


#endif
