#include <stdlib.h>
#include <raylib.h>
#include <time.h>
#include "maze.h"

int main(void) {
    srand(time(NULL));

    // 初始化窗口
    InitWindow(LENGTH, WIDTH, "MAZE EXPLORER 迷宫寻路 v1.5 Made by ZHM");  // 也有 Claude Code 的功劳
    // 设置目标帧率
    SetTargetFPS(60);
    // 定义字体
    Font font = LoadFontEx(FONT_PATH, 20, NULL, 0);
    // 生成帧率控制
    // static int frameCount = 0;
    
    // 创建/初始化迷宫 + 生成 + 逐帧动画
    Maze *maze = createMaze(ROWS, COLS);
    maze->genStack = initStack();
    maze->grid[0][0].visited = true; 
    Position start = {0, 0};
    push(maze->genStack, start);  
    maze->state = GENERATING;  // 切换状态为生成中

    // 主循环 GUI
    while (!WindowShouldClose()) {
        if (maze->state == GENERATING) {
            for (int i = 0; i < 16; i++) {  // 每帧跑 16 步 这个循环相当于改了 16 帧, 然后再交给 drawMaze 显示出来
                if (stepGenerate(maze)) {
                    maze->state = GENERATED;
                    break;
                }
            }
        }

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
                    maze->grid[row][col].explored = false;
                    // maze->state = IDLE;
                }
            }

            if (maze->genStack) {
                while (!isStackEmpty(maze->genStack)) {
                    Position tmp;
                    pop(maze->genStack, &tmp);
                }
                free(maze->genStack);
            }

            maze->genStack = initStack();
            maze->grid[0][0].visited = true; 
            Position start = {0, 0};
            push(maze->genStack, start);  
            maze->state = GENERATING;  // 切换状态为生成中
        }

        // 空格寻路
        if (IsKeyPressed(KEY_SPACE) && maze->state == GENERATED) {
            maze->bfsQueue = initQueue();
            QueueNode node = {0, 0, -1, -1, NULL};
            enqueue(maze->bfsQueue, node);

            // 回溯数组初始化
            maze->prevRow = (int **)malloc(sizeof(int *) * maze->rows);
            maze->prevCol = (int **)malloc(sizeof(int *) * maze->rows);
            for (int i = 0; i < maze->rows; i++) {
                maze->prevRow[i] = (int *)malloc(sizeof(int) * maze->cols);
                maze->prevCol[i] = (int *)malloc(sizeof(int) * maze->cols);
            }
            // 全部初始化为-1 表示没有前驱
            for (int i = 0; i < maze->rows; i++) {
                for (int j = 0; j < maze->cols; j++) {
                    maze->prevRow[i][j] = -1;
                    maze->prevCol[i][j] = -1;
                }
            }
            // 重置 visited
            for (int row = 0; row < maze->rows; row++) {
                for (int col = 0; col < maze->cols; col++) {
                    maze->grid[row][col].visited = false;
                }
            }
            maze->grid[0][0].visited = true;    
            maze->grid[0][0].explored = true; 
            maze->state = SOLVING;  // 切换状态为解答中
        }

        if (maze->state == SOLVING) {
            for (int i = 0; i < 1; i++) {
                if (stepSolve(maze)) {
                    int row = maze->rows - 1;
                    int col = maze->cols - 1;
                    while (row != -1 && col != -1) {
                        maze->grid[row][col].isPath = true;
                        maze->path[maze->pathLen].row = row;
                        maze->path[maze->pathLen].col = col;
                        maze->pathLen++;
        
                        int prevR = maze->prevRow[row][col];
                        int prevC = maze->prevCol[row][col];
                        row = prevR; 
                        col = prevC;
                    }   
                    maze->state = SOLVED;
                    break;
                }
            }
        }
    }

    destroyMaze(maze);
    UnloadFont(font);
    CloseWindow();

    return 0;
}