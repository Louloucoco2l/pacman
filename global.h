#ifndef PACMAN_GLOBAL_H
#define PACMAN_GLOBAL_H


#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH (SCREEN_WIDTH / CELL_SIZE)
#define HEIGHT (SCREEN_HEIGHT / CELL_SIZE)
#define CELL_SIZE 30 // Pour agrandir les cellules sur l'écran
#define PLAYER_SIZE 20//est carré
#define MOVE_SIZE 2
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600


extern int DX[4];
extern int DY[4];


#endif
