#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>
#include <time.h>
#include "maze.h"
#include "about_logo.h"

int main(void) {
    srand(time(NULL));

    // 初始化窗口
    InitWindow(LENGTH, WIDTH, "Maze Explorer 迷宫寻路");  // 也有 Claude Code 的功劳
    // 禁止 raylib 默认 ESC 退出 防止误触
    SetExitKey(0);
    // 设置目标帧率
    SetTargetFPS(60);
    // 初始化音频
    InitAudioDevice();
    SetMasterVolume(0.1f);
    // 定义字体
    // Font font = LoadFontEx(FONT_PATH, 20, NULL, 0);
    // 生成帧率控制
    // static int frameCount = 0;
    static double lastShot = 0;
    
    /* 创建/初始化默认迷宫 + 加载玩家档案 + 初始化菜单按钮 + 初始化动画帧栈 */
    Maze *maze = createMaze(DEFAULT_ROWS, DEFAULT_COLS);
    if (!loadProfile(maze)) {
        maze->historyCount = 0;
        maze->gamesPlayed = 0;
        maze->gamesWon = 0;
    }
    Image aboutImg = LoadImageFromMemory(".png", about_about_png, about_about_png_len);
    maze->aboutTex = LoadTextureFromImage(aboutImg);
    UnloadImage(aboutImg);

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
            // 一级菜单
            for (int i = 0; i < 3; i++) {
                if (CheckCollisionPointRec(mouse, maze->menuButton[i].bounds)) {
                    if (i == 0) {
                        destroyMaze(maze);
                        maze = createMaze(DEFAULT_ROWS, DEFAULT_COLS);
                        loadProfile(maze);
                        maze->instantMode = false;
                        maze->genStack = initStack();           
                        maze->grid[0][0].visited = true;        
                        Position start = {0, 0};                
                        push(maze->genStack, start);            
                        maze->state = GENERATING;               
                    } else if (i == 1) {
                        destroyMaze(maze);
                        maze = createMaze(DEFAULT_ROWS, DEFAULT_COLS);
                        loadProfile(maze);
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
                }
            }
            if (CheckCollisionPointRec(mouse, maze->profileButton.bounds)) {
                maze->state = PROFILE;
            }
            if (CheckCollisionPointRec(mouse, maze->aboutButton.bounds)) {
                maze->state = ABOUT;
            }
        } else if (maze->state == SELECTING_SIZE && 
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { // 保存逻辑
                Vector2 mouse = GetMousePosition();
                for (int i = 0; i < 3; i++) {
                    if (CheckCollisionPointRec(mouse, maze->sizeButton[i].bounds)) {
                        if (i == 0) {
                            destroyMaze(maze);
                            maze = createMaze(20, 26);
                            loadProfile(maze);
                            maze->difficulty = 0;
                            maze->saveFilename = "save_small.bin";
                            FILE *test = fopen(maze->saveFilename, "rb");
                            if (test) {
                                fclose(test);
                                maze->loadFilename = maze->saveFilename;
                                maze->confirmLoad = true;
                                maze->state = LOAD_OR_NEW;
                            } else {
                                maze->state = SELECTING_MODE;
                            }
                        } else if (i == 1) {
                            destroyMaze(maze);
                            maze = createMaze(40, 52);
                            loadProfile(maze);
                            maze->difficulty = 1;
                            maze->saveFilename = "save_medium.bin";
                            FILE *test = fopen(maze->saveFilename, "rb");
                            if (test) {
                                fclose(test);
                                maze->loadFilename = maze->saveFilename;
                                maze->confirmLoad = true;
                                maze->state = LOAD_OR_NEW;
                            } else {
                                maze->state = SELECTING_MODE;
                            }
                        } else if (i == 2) {
                            destroyMaze(maze);
                            maze = createMaze(60, 78);
                            loadProfile(maze);
                            maze->difficulty = 2;
                            maze->saveFilename = "save_large.bin";
                            FILE *test = fopen(maze->saveFilename, "rb");
                            if (test) {
                                fclose(test);
                                maze->loadFilename = maze->saveFilename;
                                maze->confirmLoad = true;
                                maze->state = LOAD_OR_NEW;
                            } else {
                                maze->state = SELECTING_MODE;
                            }
                        }
                    }
                }
        } else if (maze->state == SELECTING_MODE &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            for (int i = 0; i < 2; i++) {
                if (CheckCollisionPointRec(mouse, maze->modeButton[i].bounds)) {
                    if (i == 0) {
                        maze->challengeMode = false;
                        generateRandomizedMaze(maze);
                        breakCycles(maze);
                        maze->gamesPlayed++;
                        maze->state = PLAYING;
                        maze->startTime = GetTime();
                    } else if (i == 1) {
                        maze->challengeMode = true;
                        generateRandomizedMaze(maze);
                        breakCycles(maze);
                        placeItems(maze);
                        maze->gamesPlayed++;
                        maze->state = PLAYING;
                        maze->startTime = GetTime();
                    }
                }
            }
        } else if (maze->state != IDLE && maze->state != SELECTING_SIZE && maze->state != SELECTING_MODE && 
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { // 按钮事件
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
                    saveGame(maze, maze->saveFilename);
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
                    maze->grid[row][col].explored = false;
                    maze->grid[row][col].playerPath = false;
                    maze->grid[row][col].hasItem = false;
                }
            }

            maze->pathLen = 0;
            maze->visiblePathLen = 0;
            maze->totalItems = 0;
            maze->collectedCount = 0;

            if (maze->state == WON) {
                maze->playerRow = 0;
                maze->playerCol = 0;
                maze->playerStep = 0;
                generateRandomizedMaze(maze);
                breakCycles(maze);
                if (maze->challengeMode) placeItems(maze);
                loadProfile(maze);
                maze->gamesPlayed++;
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
            } else if (maze->state == SELECTING_MODE) {
                maze->state = SELECTING_SIZE;
            } else if (maze->state == SELECTING_SIZE) {
                maze->state = IDLE;
            } else if (maze->state != IDLE) {
                // 重置 + state = IDLE
                for (int row = 0; row < maze->rows; row++) {
                    for (int col = 0; col < maze->cols; col++) {
                        maze->grid[row][col].top = true;
                        maze->grid[row][col].left = true;
                        maze->grid[row][col].right = true;
                        maze->grid[row][col].bottom = true;
                        maze->grid[row][col].visited = false;
                        maze->grid[row][col].explored = false;
                        maze->grid[row][col].playerPath = false;
                        maze->grid[row][col].hasItem = false;
                    }
                }
                maze->pathLen = 0;
                maze->visiblePathLen = 0;
                maze->totalItems = 0;
                maze->collectedCount = 0;
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
                // dist初始化
                maze->dist = (int **)malloc(sizeof(int *) * maze->rows);
                for (int i = 0; i < maze->rows; i++) {
                    maze->dist[i] = (int *)malloc(sizeof(int) * maze->cols);
                }
                // 全部初始化为-1 表示没有前驱
                for (int i = 0; i < maze->rows; i++) {
                    for (int j = 0; j < maze->cols; j++) {
                        maze->prevRow[i][j] = -1;
                        maze->prevCol[i][j] = -1;
                        maze->dist[i][j] = -1;
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
                maze->dist[0][0] = 0;
                maze->state = SOLVING;  // 切换状态为解答中
            }
        }

        if (maze->state == SOLVING) {
            for (int i = 0; i < maze->animSpeed / 4; i++) {
                if (stepSolve(maze)) {
                    int row = maze->rows - 1;
                    int col = maze->cols - 1;
                    while (row != -1 && col != -1) {
                        maze->path[maze->pathLen].row = row;
                        maze->path[maze->pathLen].col = col;
                        maze->pathLen++;
        
                        int prevR = maze->prevRow[row][col];
                        int prevC = maze->prevCol[row][col];
                        row = prevR; 
                        col = prevC;
                    }   
                    maze->visiblePathLen = 1;
                    maze->state = BACKTRACK_ANIM;
                    break;
                }
            }
        }
        // visiblePathLen 从 1 增长 金线从终点往起点逆流
        if (maze->state == BACKTRACK_ANIM) {
            if (maze->visiblePathLen < maze->pathLen) {
                maze->visiblePathLen++;
            } else {
                maze->state = SOLVED;
            }
        }
        // 玩家模式
        if (maze->state == PLAYING) {
            // 鼠标拖拽逻辑
            int centerX = maze->offsetX + maze->playerCol * maze->cellSize + maze->cellSize / 4;
            int centerY = maze->offsetY + maze->playerRow * maze->cellSize + maze->cellSize / 4;
            Rectangle playerRect = {centerX, centerY, maze->cellSize, maze->cellSize}; // 判定箱 防止点其他地方使方框移动
            static bool dragging = false;

            // 只在按下的那一帧检测鼠标是否在玩家方块上
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && 
                CheckCollisionPointRec(GetMousePosition(), playerRect)) {
                dragging = true;
            }
            if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                dragging = false;
            }
            if (dragging) {
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    Vector2 mouse = GetMousePosition();
                    // 玩家屏幕坐标
                    int centerX = maze->offsetX + maze->playerCol * maze->cellSize + maze->cellSize / 2;
                    int centerY = maze->offsetY + maze->playerRow * maze->cellSize + maze->cellSize / 2;
                    float dx = mouse.x - centerX;         // 正值往右、负值往左
                    float dy = mouse.y - centerY;         // 正值往下、负值往上
                    float threshold = maze->cellSize / 2; // 阈值 超过格子/2移动
                    // 冷却
                    static int dragDelay = 0;
                    if (dragDelay > 0) {
                        dragDelay--;
                    } else {
                        // 拖拽逻辑
                        if (fabs(dx) > fabs(dy) && fabs(dx) > threshold) {
                            // 横向 右或左
                            if (dx > 0) {
                                if (!maze->grid[maze->playerRow][maze->playerCol].right &&
                                    maze->playerCol + 1 < maze->cols) {
                                    maze->playerCol++;
                                    maze->playerStep++;
                                    maze->grid[maze->playerRow][maze->playerCol].playerPath = true;
                                    maze->walkPlayed = true;
                                }
                            } else {
                                if (!maze->grid[maze->playerRow][maze->playerCol].left &&
                                    maze->playerCol - 1 >= 0) {
                                    maze->playerCol--;
                                    maze->playerStep++;
                                    maze->grid[maze->playerRow][maze->playerCol].playerPath = true;     
                                    maze->walkPlayed = true;
                                }
                            }
                        } else if (fabs(dy) > threshold) { // 纵向 下或上
                            if (dy > 0) {
                                if (!maze->grid[maze->playerRow][maze->playerCol].bottom &&
                                    maze->playerRow + 1 < maze->rows) {
                                    maze->playerRow++;
                                    maze->playerStep++;
                                    maze->grid[maze->playerRow][maze->playerCol].playerPath = true;
                                    maze->walkPlayed = true;
                                }
                            } else {
                                if (!maze->grid[maze->playerRow][maze->playerCol].top &&
                                    maze->playerRow - 1 >= 0) {
                                    maze->playerRow--;
                                    maze->playerStep++;
                                    maze->grid[maze->playerRow][maze->playerCol].playerPath = true;
                                    maze->walkPlayed = true;
                                }
                            }
                        }
                        dragDelay = 4; // 等4帧再允许下一步
                    }
                }
            }

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
                        maze->walkPlayed = true;
                    }
                }
                if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
                    if (!maze->grid[maze->playerRow][maze->playerCol].bottom && 
                        maze->playerRow + 1 < maze->rows) {
                        maze->playerRow++;
                        maze->playerStep++;
                        maze->grid[maze->playerRow][maze->playerCol].playerPath = true;
                        maze->walkPlayed = true;
                    }
                }
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                    if (!maze->grid[maze->playerRow][maze->playerCol].left && 
                        maze->playerCol - 1 >= 0) {
                        maze->playerCol--;
                        maze->playerStep++;
                        maze->grid[maze->playerRow][maze->playerCol].playerPath = true;
                        maze->walkPlayed = true;
                    }
                }
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                    if (!maze->grid[maze->playerRow][maze->playerCol].right && 
                        maze->playerCol + 1 < maze->cols) {
                        maze->playerCol++;
                        maze->playerStep++;
                        maze->grid[maze->playerRow][maze->playerCol].playerPath = true;
                        maze->walkPlayed = true;
                    }
                }
            }

            if (maze->grid[maze->playerRow][maze->playerCol].hasItem) {
                maze->collectedCount++;
                maze->grid[maze->playerRow][maze->playerCol].hasItem = false;
                PlaySound(maze->collectSound);
            }
            // 全收集完
            if (maze->collectedCount == maze->totalItems && !maze->allCollectedPlayed && maze->challengeMode) {
                PlaySound(maze->allCollectedSound);
                maze->allCollectedPlayed = true;
            }
            
            static bool firstFrame = true;
            int walls = 0;
            if (maze->grid[maze->playerRow][maze->playerCol].top) walls++;
            if (maze->grid[maze->playerRow][maze->playerCol].bottom) walls++;
            if (maze->grid[maze->playerRow][maze->playerCol].left) walls++;
            if (maze->grid[maze->playerRow][maze->playerCol].right) walls++;
            if (walls == 3 && !maze->wallPlayed && !firstFrame) {
                PlaySound(maze->wallSound);
                maze->wallPlayed = true;
            } 
            if (walls < 3) {
                maze->wallPlayed = false;
            }

            static double lastWalkSound = 0;
            if (GetTime() - lastWalkSound > 0.15 && maze->walkPlayed) {
                PlaySound(maze->walkSound);
                lastWalkSound = GetTime();
            }
            maze->walkPlayed = false;
            // 终点检查
            if (maze->playerRow == maze->rows - 1 && maze->playerCol == maze->cols - 1) {
                if (maze->challengeMode && maze->collectedCount < maze->totalItems) {
                    maze->showCollectHint = true;
                } else {
                    maze->pathLen = 0;
                    mazeSolver(maze);
                    maze->elaspedTime = GetTime() - maze->startTime; // 计时结束
                    if (maze->elaspedTime < maze->bestTime[maze->difficulty] || 
                        maze->bestTime[maze->difficulty] == 0) {
                        maze->bestTime[maze->difficulty] = maze->elaspedTime;
                        maze->isNewBest = true;
                    }
                    // 评级
                    float ratio = (float)maze->playerStep / (maze->pathLen - 1);
                    if (ratio <= 1.1)       maze->grade = 'S';
                    else if (ratio <= 1.5)  maze->grade = 'A';
                    else if (ratio <= 2.0)  maze->grade = 'B';
                    else                    maze->grade = 'C';

                    PlaySound(maze->winSound);
                    maze->state = WON;
                    maze->showCollectHint = false;
                    /* 存入玩家档案 */
                    maze->gamesWon++;
                    GameRecord record;
                    record.rows = maze->rows;
                    record.cols = maze->cols;
                    record.steps = maze->playerStep;
                    record.time = maze->elaspedTime;
                    record.grade = maze->grade;
                    record.challengeMode = maze->challengeMode;
                    record.won = true;
                    maze->history[maze->historyCount++] = record;
                    saveProfile(maze);
                }
            } else {
                maze->showCollectHint = false;
            }
            firstFrame = false;
        }
        // 进入存档或者开新存档
        if (maze->state == LOAD_OR_NEW) {
            if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_ENTER)) {
                loadGame(maze, maze->loadFilename);
                maze->startTime = GetTime() - maze->elaspedTime;
                loadProfile(maze);
                maze->gamesPlayed++;
                maze->state = PLAYING;
                maze->confirmLoad = false;
            }
            if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) {
                maze->state = SELECTING_MODE;
                maze->confirmLoad = false;
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

        if (maze->confirmReturn) {
            if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_ENTER)) {
                maze->state = SELECTING_SIZE;
                maze->confirmReturn = false;
            }
            if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) {
                maze->confirmReturn = false;
            }
        }

        if (maze->confirmQuit) {
            if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_ENTER)) {
                break;
            }
            if (IsKeyPressed(KEY_N)) {
                maze->confirmQuit = false;
            }
        }
    }

    destroyMaze(maze);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}