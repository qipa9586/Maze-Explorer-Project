#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

/* 创建&初始化迷宫 */
Maze *createMaze(int rows, int cols) {
    // 先分配内存
    Maze *maze = (Maze *)malloc(sizeof(Maze));
    maze->rows = rows;
    maze->cols = cols;
    maze->cellSize = (LENGTH / cols < WIDTH / rows) ? LENGTH / cols : WIDTH / rows;  // 取两个方向中较小的那个做格子大小 保证迷宫不超出窗口
    maze->offsetX = (LENGTH - maze->cols * maze->cellSize) / 2;
    maze->offsetY = (WIDTH - maze->rows * maze->cellSize) / 2;
    maze->grid = (Cell **)malloc(sizeof(Cell *) * maze->rows);
    for (int i = 0; i < maze->rows; i++) {
        maze->grid[i] = (Cell *)malloc(sizeof(Cell) * maze->cols);
    }
    maze->path = (Position *)malloc(sizeof(Position) * maze->rows * maze->cols);
    maze->pathLen = 0;

    // 再初始化每个格子里面的值
    for (int i = 0; i < maze->rows; i++) {
        for (int j = 0; j < maze->cols; j++) {
            maze->grid[i][j].top = true;
            maze->grid[i][j].left = true;
            maze->grid[i][j].right = true;
            maze->grid[i][j].bottom = true;
            maze->grid[i][j].visited = false;
            maze->grid[i][j].isPath = false;
            maze->grid[i][j].explored = false;
            maze->grid[i][j].playerPath = false;
        }
    }

    // 初始化状态
    maze->state = IDLE;
    maze->genStack = NULL;
    maze->bfsQueue = NULL;
    maze->playerRow = 0;
    maze->playerCol = 0;
    maze->playerStep = 0;

    maze->prevRow = NULL;
    maze->prevCol = NULL;
    maze->instantMode = false;
    maze->confirmReturn = false;

    // 初始化菜单按钮
    int btnWidth = 600, btnHeight = 100, gap = 40;
    int startY = WIDTH / 2 - (btnHeight * 3 + gap * 2) / 2;
    // btnX = 水平居中：窗口宽减按钮宽，左右平分空白
    int btnX = LENGTH / 2 - btnWidth / 2;
    maze->menuButton[0] = (MenuButton) {{btnX, startY, btnWidth, btnHeight}, "Animated Generation", false};
    maze->menuButton[1] = (MenuButton) {{btnX, startY + btnHeight + gap, btnWidth, btnHeight}, "Instant Generation", false};
    maze->menuButton[2] = (MenuButton) {{btnX, startY + (btnHeight + gap) * 2, btnWidth, btnHeight}, "Player Exploration", false};
    btnWidth = 450, btnHeight = 75, gap = 30;
    startY = WIDTH / 2 - (btnHeight * 3 + gap * 2) / 2;
    btnX = LENGTH / 2 - btnWidth / 2;
    maze->sizeButton[0] = (MenuButton) {{btnX, startY, btnWidth, btnHeight}, "SMALL", false};
    maze->sizeButton[1] = (MenuButton) {{btnX, startY + btnHeight + gap, btnWidth, btnHeight}, "MEDIUM", false};
    maze->sizeButton[2] = (MenuButton) {{btnX, startY + (btnHeight + gap) * 2, btnWidth, btnHeight}, "LARGE", false};

    return maze;
}

/* 销毁迷宫 */
void destroyMaze(Maze *maze) {
    for (int i = 0; i < maze->rows; i++) {
        free(maze->grid[i]);
    }
    free(maze->grid);
    free(maze->path);
    maze->pathLen = 0;
    free(maze);
}