#include <stdlib.h>
#include <raylib.h>
#include <time.h>
#include "maze.h"

int main(void) {
    srand(time(NULL));

    // 初始化窗口
    InitWindow(LENGTH, WIDTH, "MAZE EXPLORER 迷宫寻路 Version 0.5");
    // 设置目标帧率
    SetTargetFPS(60);  
    
    // 创建/初始化迷宫 + 生成
    Maze *maze = createMaze(ROWS, COLS);
    generateRandomizedMaze(maze);

    // 主循环 GUI
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BG_COLOR);
        drawMaze(maze);
        EndDrawing();
    }

    destroyMaze(maze);
    CloseWindow();

    return 0;
}