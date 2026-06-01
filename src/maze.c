#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

/* 创建\初始化迷宫 */
Maze *createMaze(int rows, int cols) {
    // 先分配内存
    Maze *maze = (Maze *)malloc(sizeof(Maze));
    maze->rows = rows;
    maze->cols = cols;
    maze->grid = (Cell **)malloc(sizeof(Cell *) * maze->rows);
    for (int i = 0; i < maze->rows; i++) {
        maze->grid[i] = (Cell *)malloc(sizeof(Cell) * maze->cols);
    }

    // 再初始化每个格子里面的值
    for (int i = 0; i < maze->rows; i++) {
        for (int j = 0; j < maze->cols; j++) {
            maze->grid[i][j].top = true;
            maze->grid[i][j].left = true;
            maze->grid[i][j].right = true;
            maze->grid[i][j].bottom = true;
            maze->grid[i][j].visited = false;
        }
    }

    return maze;
}

/* 销毁迷宫 */
void destroyMaze(Maze *maze) {
    for (int i = 0; i < maze->rows; i++) {
        free(maze->grid[i]);
    }
    free(maze->grid);
    free(maze);
}