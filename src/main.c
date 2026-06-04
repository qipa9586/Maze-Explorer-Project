#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <time.h>
#include "maze.h"

int main(void) {
    srand(time(NULL));

    // 初始化窗口
    InitWindow(LENGTH, WIDTH, "Maze Explorer 迷宫寻路");  // 也有 Claude Code 的功劳
    // 禁止 raylib 默认 ESC 退出 防止误触
    SetExitKey(0);
    // 设置目标帧率
    SetTargetFPS(60);
    // 定义字体
    // Font font = LoadFontEx(FONT_PATH, 20, NULL, 0);
    // 生成帧率控制
    // static int frameCount = 0;
    
    /* 创建/初始化默认迷宫 + 初始化菜单按钮 + 初始化动画帧栈 */
    Maze *maze = createMaze(DEFAULT_ROWS, DEFAULT_COLS);    // 创建/初始化迷宫
    
    // 主循环 GUI
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (maze->state == IDLE) {
                break;
            } else {
                maze->confirmQuit = true;
            }
        }

        if (maze->state == IDLE && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            for (int i = 0; i < 3; i++) {
                if (CheckCollisionPointRec(mouse, maze->menuButton[i].bounds)) {
                    // MazeState target = maze->button[i].target;
                    if (i == 0) {
                        maze->instantMode = false;
                        maze->genStack = initStack();           
                        maze->grid[0][0].visited = true;        
                        Position start = {0, 0};                
                        push(maze->genStack, start);            
                        maze->state = GENERATING;               
                    } else if (i == 1) {
                        maze->instantMode = true;
                        generateRandomizedMaze(maze);
                        breakCycles(maze);
                        maze->state = GENERATED;
                    } else if (i == 2) {
                        maze->playerRow = 0;
                        maze->playerCol = 0;
                        maze->playerStep = 0;
                        maze->state = SELECTING_SIZE;
                    }
                    // maze->state = target;
                }
            }
        } else if (maze->state == SELECTING_SIZE && 
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mouse = GetMousePosition();
                for (int i = 0; i < 3; i++) {
                    if (CheckCollisionPointRec(mouse, maze->sizeButton[i].bounds)) {
                        if (i == 0) {
                            destroyMaze(maze);
                            maze = createMaze(20, 26);
                            generateRandomizedMaze(maze);
                            breakCycles(maze);
                            maze->state = PLAYING;
                        } else if (i == 1) {
                            generateRandomizedMaze(maze);
                            breakCycles(maze);
                            maze->state = PLAYING;
                        } else if (i == 2) {
                            destroyMaze(maze);
                            maze = createMaze(60, 78);
                            generateRandomizedMaze(maze);
                            breakCycles(maze);
                            maze->state = PLAYING;
                        }
                    }
                }
        } else if (maze->state != IDLE && maze->state != SELECTING_SIZE && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            if (maze->state == GENERATING || maze->state == GENERATED || 
                maze->state == SOLVING || maze->state == SOLVED) {
                    for (int i = 0; i < 4; i++) {
                        if (CheckCollisionPointRec(mouse, maze->speedButton[i].bounds)) {
                            maze->animSpeed = maze->speedButton[i].speed;
                        }
                    }
                }

            if (maze->state == PLAYING) {
                if (CheckCollisionPointRec(mouse, maze->saveButton.bounds)) {
                    // 等下写
                }
            }
        }

        if (maze->state == GENERATING) {
            for (int i = 0; i < maze->animSpeed; i++) {  // 每帧跑 12(默认) 步 这个循环相当于改了 12 帧 然后再交给 drawMaze 显示出来
                if (stepGenerate(maze)) {
                    breakCycles(maze);
                    maze->state = GENERATED;
                    break;
                }
            }
        }

        BeginDrawing();
        ClearBackground(BG_COLOR);
        drawMaze(maze);
        drawToolbar(maze);
        
        // 获胜提示信息
        if (maze->state == WON) {
            // 居中
            int boxW = 400, boxH = 280;
            int winX = LENGTH / 2 - boxW / 2;
            int winY = WIDTH / 2 - boxH / 2;
            DrawRectangle(winX, winY, boxW, boxH, (Color){0, 0, 0, 200});
            // 文字在框内居中：框 x + 半宽 - 文字半宽
            DrawText("YOU WIN !", winX + boxW / 2 - MeasureText("YOU WIN !", 60) / 2, winY + 30, 60, GOLD);
            
            char steps[64];
            sprintf(steps, "Your Steps: %d", maze->playerStep);
            DrawText(steps, winX + boxW / 2 - MeasureText(steps, 30) / 2, winY + 100, 30, WHITE);

            sprintf(steps, "Shortest Steps: %d", maze->pathLen - 1);
            DrawText(steps, winX + boxW / 2 - MeasureText(steps, 30) / 2, winY + 150, 30, WHITE);

            // sprintf(steps, "Shortest Steps: %d", maze->timeUsage);
        }

        if (maze->confirmQuit) {
            DrawRectangle(LENGTH / 2 - 340, WIDTH / 2 - 40, 710, 80, (Color){0, 0, 0, 200});
            DrawText("Do you want to QUIT ? (Y/N)", LENGTH / 2 - 290, WIDTH / 2 - 20, 40, YELLOW);
            if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_ENTER)) {
                break;
            }
            if (IsKeyPressed(KEY_N)) {
                maze->confirmQuit = false;
            }
        }

        EndDrawing();

        // R 键刷新/重置
        if (IsKeyPressed(KEY_R) && maze->state != IDLE && maze->state != PLAYING) {
            // 重置所有格子
            for (int row = 0; row < maze->rows; row++) {
                for (int col = 0; col < maze->cols; col++) {
                    maze->grid[row][col].top = true;
                    maze->grid[row][col].left = true;
                    maze->grid[row][col].right = true;
                    maze->grid[row][col].bottom = true;
                    maze->grid[row][col].visited = false;
                    maze->grid[row][col].isPath = false;
                    maze->grid[row][col].explored = false;
                    maze->grid[row][col].playerPath = false;
                    maze->pathLen = 0;
                }
            }

            if (maze->state == WON) {
                maze->playerRow = 0;
                maze->playerCol = 0;
                maze->playerStep = 0;
                generateRandomizedMaze(maze);
                breakCycles(maze);
                maze->state = PLAYING;
            } else if (maze->instantMode) {
                // 一键版 -> 重新一键生成
                generateRandomizedMaze(maze);
                breakCycles(maze);
                maze->state = GENERATED;
            } else if (maze->state == GENERATED || maze->state == SOLVED) {
                // 动画版 -> 重新初始化栈
                if (maze->genStack) { 
                    // 清理旧栈
                    Position tmp;
                    pop(maze->genStack, &tmp); 
                    free(maze->genStack);
                }
                maze->genStack = initStack();
                maze->grid[0][0].visited = true;
                Position start = {0, 0};
                push(maze->genStack, start);
                maze->state = GENERATING;
            }
        }

        // M 键回主菜单
        if (IsKeyPressed(KEY_M)) {
            if (maze->state == PLAYING) {
                maze->confirmReturn = true;
            } else if (maze->state != IDLE) {
                // 重置 + state = IDLE
                for (int row = 0; row < maze->rows; row++) {
                    for (int col = 0; col < maze->cols; col++) {
                        maze->grid[row][col].top = true;
                        maze->grid[row][col].left = true;
                        maze->grid[row][col].right = true;
                        maze->grid[row][col].bottom = true;
                        maze->grid[row][col].visited = false;
                        maze->grid[row][col].isPath = false;
                        maze->grid[row][col].explored = false;
                        maze->grid[row][col].playerPath = false;
                        maze->pathLen = 0;
                    }
                }
                maze->state = IDLE;
            }
        }

        // 空格寻路
        if (IsKeyPressed(KEY_SPACE) && maze->state == GENERATED) {
            if (maze->instantMode) {
                maze->pathLen = 0;
                mazeSolver(maze);
                maze->state = SOLVED;
            } else {
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
        }

        if (maze->state == SOLVING) {
            for (int i = 0; i < maze->animSpeed / 2; i++) {
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

        if (maze->state == PLAYING) {
            static int cooldown = 0;
            cooldown++;
            maze->grid[0][0].playerPath = true;
            if (cooldown % 5 == 0) { // 每 5 帧动一步 = 12步/s 不会出现滑步乱飘情况
                // 上下左右WASD移动逻辑 越界检查
                if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
                    if (!maze->grid[maze->playerRow][maze->playerCol].top && 
                        maze->playerRow - 1 >= 0) {
                        maze->playerRow--;
                        maze->playerStep++;
                        maze->grid[maze->playerRow][maze->playerCol].playerPath = true;
                    }
                }
                if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
                    if (!maze->grid[maze->playerRow][maze->playerCol].bottom && 
                        maze->playerRow + 1 < maze->rows) {
                        maze->playerRow++;
                        maze->playerStep++;
                        maze->grid[maze->playerRow][maze->playerCol].playerPath = true;
                    }
                }
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                    if (!maze->grid[maze->playerRow][maze->playerCol].left && 
                        maze->playerCol - 1 >= 0) {
                        maze->playerCol--;
                        maze->playerStep++;
                        maze->grid[maze->playerRow][maze->playerCol].playerPath = true;
                    }
                }
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                    if (!maze->grid[maze->playerRow][maze->playerCol].right && 
                        maze->playerCol + 1 < maze->cols) {
                        maze->playerCol++;
                        maze->playerStep++;
                        maze->grid[maze->playerRow][maze->playerCol].playerPath = true;
                    }
                }
            }
            // 终点检查
            if (maze->playerRow == maze->rows - 1 && maze->playerCol == maze->cols - 1) {
                maze->pathLen = 0;
                mazeSolver(maze);
                maze->state = WON;
            }
        }
        // H 键提示 (我搞的后门😄 如何打开只给玩家少量提示哈哈)
        if (IsKeyPressed(KEY_H) && maze->state == PLAYING) {
            if (maze->pathLen == 0) {
                maze->pathLen = 0;
                mazeSolver(maze);
            } else {
                maze->pathLen = 0;
            }
        }

        if (IsKeyPressed(KEY_F12)) {
            TakeScreenshot(TextFormat("maze_%lld.png", (long long)time(NULL)));
        }
    }

    destroyMaze(maze);
    // UnloadFont(font);
    CloseWindow();

    return 0;
}