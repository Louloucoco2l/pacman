#include "generation_division_recursive.h"
#include "global.h"

void shuffle(int *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void dfs(int maze[HEIGHT][WIDTH], int x, int y) {
    int directions[4] = {0, 1, 2, 3};
    shuffle(directions, 4);

    for (int i = 0; i < 4; i++) {
        int nx = x + DX[directions[i]];
        int ny = y + DY[directions[i]];

        if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT && maze[ny][nx] == 1) {
            maze[ny][nx] = 0; // Carve path
            maze[(y + ny) / 2][(x + nx) / 2] = 0; // Carve path between cells
            dfs(maze, nx, ny);
        }
    }
}

void initializeMazeWithDFS(int maze[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            maze[i][j] = 1; // Initialize all cells as walls
        }
    }

    int startX = rand() % WIDTH;
    int startY = rand() % HEIGHT;
    maze[startY][startX] = 0; // Start point
    dfs(maze, startX, startY);
}