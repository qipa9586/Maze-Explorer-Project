#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <time.h>
#include "maze.h"

int main(void) {
    srand(time(NULL));

    // 初始化窗口
    InitWindow(LENGTH, WIDTH, "MAZE EXPLORER 迷宫寻路 v1.7 Made by ZHM");  // 也有 Claude Code 的功劳
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
            for (int i = 0; i < 12; i++) {  // 每帧跑 12 步 这个循环相当于改了 12 帧 然后再交给 drawMaze 显示出来
                if (stepGenerate(maze)) {
                    // 在状态改为生成完毕前再随机破坏几个墙 打破完美迷宫结构 更具迷惑性和挑战性
                    int extras = (maze->rows * maze->cols) / 50; // 约有 1% ~ 2% 的墙被随机拆除
                    for (int j = 0; j < extras; j++) {
                        // row 和 col 不碰边界行和列 范围是 1 到 rows-2
                        /* 
                         * 代码解析:
                         * maze->rows - 2 = 38（40 行去掉首尾） 
                         * rand() % 38 -> 0~37
                         * +1 -> 1~38   
                         */
                        int row = rand() % (maze->rows - 2) + 1;  
                        int col = rand() % (maze->cols - 2) + 1;
                        int dir = rand() % 4;  // 随机一个方向 随机取 0~3 上下左右
                        // 拆墙（对称, 本体拆了邻居也得拆）
                        int nextRow, nextCol;
                        switch(dir) {
                            case 0: {
                                nextRow = row - 1; 
                                nextCol = col;
                                if (maze->grid[row][col].top) {
                                    maze->grid[row][col].top = false;
                                    maze->grid[nextRow][nextCol].bottom = false;
                                }
                            } break;

                            case 1: {
                                nextRow = row + 1; 
                                nextCol = col;
                                if (maze->grid[row][col].bottom) {
                                    maze->grid[row][col].bottom = false;
                                    maze->grid[nextRow][nextCol].top = false;
                                }
                            } break;

                            case 2: {
                                nextRow = row; 
                                nextCol = col - 1;
                                if (maze->grid[row][col].left) {
                                    maze->grid[row][col].left = false;
                                    maze->grid[nextRow][nextCol].right = false;
                                }
                            } break;

                            case 3: {
                                nextRow = row; 
                                nextCol = col + 1;
                                if (maze->grid[row][col].right) {
                                    maze->grid[row][col].right = false;
                                    maze->grid[nextRow][nextCol].left = false;
                                }
                            } break;

                            default:
                                break;
                        }
                    }
                    maze->state = GENERATED;
                    break;
                }
            }
        }

        BeginDrawing();
        ClearBackground(BG_COLOR);
        drawMaze(maze, font);
        DrawTextEx(font, "R:Regenerate | Space:Solve | P:Player Mode |ESC:Quit", (Vector2){10, WIDTH - 25}, 20, 1, GREEN);
        if (maze->state == WON) {
            // 居中坐标
            int winX = LENGTH / 2 - 100;
            int winY = WIDTH / 2 - 100;
            DrawText("YOU WIN", winX, winY, 50, RED);
            
            char steps[64];
            sprintf(steps, "Your Steps: %d", maze->playerStep);
            DrawText(steps, winX, winY + 60, 30, WHITE);

            sprintf(steps, "Shortest Steps: %d", maze->pathLen - 1);
            DrawText(steps, winX, winY + 100, 30, WHITE);
        }
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

            maze->playerRow = 0;
            maze->playerCol = 0;
            maze->playerStep = 0;
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

        if (IsKeyPressed(KEY_P) && maze->state == GENERATED) {
            maze->playerRow = 0;
            maze->playerCol = 0;
            maze->playerStep = 0;
            maze->state = PLAYING;
        } 

        if (maze->state == PLAYING) {
            // 上下左右WASD移动逻辑 越界检查
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                if (!maze->grid[maze->playerRow][maze->playerCol].top && 
                    maze->playerRow - 1 >= 0) {
                    maze->playerRow--;
                    maze->playerStep++;
                }
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                if (!maze->grid[maze->playerRow][maze->playerCol].bottom && 
                    maze->playerRow + 1 < maze->rows) {
                    maze->playerRow++;
                    maze->playerStep++;
                }
            }
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                if (!maze->grid[maze->playerRow][maze->playerCol].left && 
                    maze->playerCol - 1 >= 0) {
                    maze->playerCol--;
                    maze->playerStep++;
                }
            }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                if (!maze->grid[maze->playerRow][maze->playerCol].right && 
                    maze->playerCol + 1 < maze->cols) {
                    maze->playerCol++;
                    maze->playerStep++;
                }
            }
            // 终点检查
            if (maze->playerRow == maze->rows - 1 && maze->playerCol == maze->cols - 1) {
                maze->pathLen = 0;
                mazeSolver(maze);
                maze->state = WON;
            }
        }
        // H 键提示
        if (IsKeyPressed(KEY_H) && maze->state == PLAYING) {
            if (maze->pathLen == 0) {
                maze->pathLen = 0;
                mazeSolver(maze);
            } else {
                maze->pathLen = 0;
            }
        }
    }

    destroyMaze(maze);
    UnloadFont(font);
    CloseWindow();

    return 0;
}