#include "generation_fusion_aleatoire.h"
#include "global.h"



// Initialisation du labyrinthe (murs)
void initializeMaze(int maze[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            maze[i][j] = 1; // Tout est mur
        }
    }
}

// Vérifie si une cellule voisine est valide
int isValidNeighbor(int x, int y, int maze[HEIGHT][WIDTH]) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && maze[y][x] == 1;
}

// Génération du labyrinthe (fusion aléatoire de chemins)
void generateMaze(int maze[HEIGHT][WIDTH]) {
    Cell stack[WIDTH * HEIGHT];
    int stackSize = 0;

    // Point de départ
    int startX = rand() % WIDTH;
    int startY = rand() % HEIGHT;

    maze[startY][startX] = 0;
    stack[stackSize++] = (Cell){startX, startY};

    while (stackSize > 0) {
        int idx = rand() % stackSize;
        Cell current = stack[idx];
        stack[idx] = stack[--stackSize];

        Cell neighbors[4];
        int neighborCount = 0;

        for (int i = 0; i < 4; i++) {
            int nx = current.x + DX[i] * 2;
            int ny = current.y + DY[i] * 2;

            if (isValidNeighbor(nx, ny, maze)) {
                neighbors[neighborCount++] = (Cell){nx, ny};
            }
        }

        if (neighborCount > 0) {
            Cell chosen = neighbors[rand() % neighborCount];
            int midX = (current.x + chosen.x) / 2;
            int midY = (current.y + chosen.y) / 2;

            maze[midY][midX] = 0;
            maze[chosen.y][chosen.x] = 0;
            stack[stackSize++] = current; // Push current cell back to stack
            stack[stackSize++] = chosen;  // Push chosen neighbor to stack
        }
    }
}

void labSansIlots(int maze[HEIGHT][WIDTH]) {
    initializeMaze(maze);
    generateMaze(maze);
}