#include <stdlib.h>
#include <raylib.h>
#include <time.h>
#include "maze.h"

int main(void) {
    srand(time(NULL));

    // 初始化窗口
    InitWindow(LENGTH, WIDTH, "MAZE EXPLORER 迷宫寻路 v1.0 Made by ZHM");  // 也有 Claude Code 的功劳
    // 设置目标帧率
    SetTargetFPS(60);
    // 定义字体
    Font font = LoadFontEx(FONT_PATH, 20, NULL, 0);
    
    // 创建/初始化迷宫 + 生成
    Maze *maze = createMaze(ROWS, COLS);
    generateRandomizedMaze(maze);
    bool solving = false; // 是否解答了

    // 主循环 GUI
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BG_COLOR);
        drawMaze(maze, font);
        DrawTextEx(font, "R:Regenerate | Space:Solve | ESC:Quit", (Vector2){10, WIDTH - 25}, 20, 2, GREEN);
        EndDrawing();
        // R键刷新
        if (IsKeyPressed(KEY_R)) {
            // 重置所有格子
            for (int row = 0; row < maze->rows; row++) {
                for (int col = 0; col < maze->cols; col++) {
                    maze->grid[row][col].top = true;
                    maze->grid[row][col].left = true;
                    maze->grid[row][col].right = true;
                    maze->grid[row][col].bottom = true;
                    maze->grid[row][col].visited = false;
                    maze->grid[row][col].isPath = false;
                    maze->pathLen = 0;
                    solving = false;
                }
            }
            generateRandomizedMaze(maze);
        }

        // 空格寻路
        if (IsKeyPressed(KEY_SPACE) && !solving) {
            solving = true;
            mazeSolver(maze);
        }
    }

    destroyMaze(maze);
    UnloadFont(font);
    CloseWindow();

    return 0;
}