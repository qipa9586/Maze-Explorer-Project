#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

/* 创建&初始化迷宫 */
Maze *createMaze(int rows, int cols) {
    // 先分配内存
    Maze *maze = (Maze *)calloc(1, sizeof(Maze));
    maze->rows = rows;
    maze->cols = cols;
    int availH = WIDTH - TOOLBAR_H;
    maze->cellSize = (LENGTH / cols < availH / rows) ? LENGTH / cols : availH / rows;  // 取两个方向中较小的那个做格子大小 保证迷宫不超出窗口
    maze->offsetX = (LENGTH - maze->cols * maze->cellSize) / 2;
    maze->offsetY = TOOLBAR_H + (availH - maze->rows * maze->cellSize) / 2;
    
    maze->grid = (Cell **)malloc(sizeof(Cell *) * maze->rows);
    for (int i = 0; i < maze->rows; i++) {
        maze->grid[i] = (Cell *)malloc(sizeof(Cell) * maze->cols);
    }
    maze->path = (Position *)malloc(sizeof(Position) * maze->rows * maze->cols);
    maze->pathLen = 0;
    maze->visiblePathLen = 0;

    // 初始化每个格子里面的值
    for (int i = 0; i < maze->rows; i++) {
        for (int j = 0; j < maze->cols; j++) {
            maze->grid[i][j].top = true;
            maze->grid[i][j].left = true;
            maze->grid[i][j].right = true;
            maze->grid[i][j].bottom = true;
            maze->grid[i][j].visited = false;
            maze->grid[i][j].explored = false;
            maze->grid[i][j].playerPath = false;
            maze->grid[i][j].hasItem = false;
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
    maze->dist = NULL;
    maze->maxDist = 0;
    maze->instantMode = false;
    maze->confirmReturn = false;
    maze->confirmQuit = false;
    maze->challengeMode = false;
    maze->showCollectHint = false;

    // 初始化一级菜单按钮
    int btnWidth = 600, btnHeight = 100, gap = 40;
    int startY = WIDTH / 2 - (btnHeight * 4 + gap * 3) / 2;
    // btnX = 水平居中：窗口宽减按钮宽，左右平分空白
    int btnX = LENGTH / 2 - btnWidth / 2 + 20;
    maze->menuButton[0] = (MenuButton) {{btnX, startY, btnWidth, btnHeight}, "Animated Generation", false};
    maze->menuButton[1] = (MenuButton) {{btnX, startY + btnHeight + gap, btnWidth, btnHeight}, "Instant Generation", false};
    maze->menuButton[2] = (MenuButton) {{btnX, startY + (btnHeight + gap) * 2, btnWidth, btnHeight}, "Player Exploration", false};
    maze->profileButton = (MenuButton) {{btnX, startY + (btnHeight + gap) * 3, btnWidth, btnHeight}, "Player Profile", false};
    maze->backButton    = (MenuButton) {{btnX + 5, WIDTH - 100, btnWidth - 50, 60}, "Back to Menu", false};
    // 二级菜单
    btnWidth = 450, btnHeight = 75, gap = 30;
    startY = WIDTH / 2 - (btnHeight * 3 + gap * 2) / 2;
    btnX = LENGTH / 2 - btnWidth / 2;
    maze->sizeButton[0] = (MenuButton) {{btnX, startY, btnWidth, btnHeight}, "SMALL", false};
    maze->sizeButton[1] = (MenuButton) {{btnX, startY + btnHeight + gap, btnWidth, btnHeight}, "MEDIUM", false};
    maze->sizeButton[2] = (MenuButton) {{btnX, startY + (btnHeight + gap) * 2, btnWidth, btnHeight}, "LARGE", false};
    // 速度/保存按钮
    btnWidth = 50, btnHeight = 28, gap = 10;
    startY = (TOOLBAR_H - btnHeight) / 2;
    maze->speedButton[0] = (SpeedButton) {{btnX, startY, btnWidth, btnHeight}, "1x", 6};
    maze->speedButton[1] = (SpeedButton) {{btnX + btnWidth + gap, startY, btnWidth, btnHeight}, "2x", 12};
    maze->speedButton[2] = (SpeedButton) {{btnX + (btnWidth + gap) * 2, startY, btnWidth, btnHeight}, "4x", 24};
    maze->speedButton[3] = (SpeedButton) {{btnX + (btnWidth + gap) * 3, startY, btnWidth, btnHeight}, "Max", 80};
    maze->animSpeed = 12;

    maze->saveButton = (MenuButton) {{btnX + (btnWidth + gap) * 4 + 40, startY, btnWidth + 50, btnHeight}, "SAVE", false};
    maze->confirmLoad = false;
    maze->showSaved = false;
    maze->savedTime = 0;
    maze->saveFilename = NULL;
    maze->loadFilename = NULL;
    // 三级菜单 模式选择
    btnWidth = 450, btnHeight = 75, gap = 30;
    startY = WIDTH / 2 - (btnHeight * 3 + gap * 2) / 2;
    btnX = LENGTH / 2 - btnWidth / 2;
    maze->modeButton[0] = (MenuButton) {{btnX, startY, btnWidth, btnHeight}, "Normal Mode", false};
    maze->modeButton[1] = (MenuButton) {{btnX, startY + btnHeight + gap, btnWidth, btnHeight}, "Challenge Mode", false};

    maze->difficulty = 0;
    maze->bestTime[0] = maze->bestTime[1] = maze->bestTime[2] = 0;
    maze->isNewBest = false;
    maze->grade = '\0';

    Wave w = LoadWave("assets/all_collected.wav"); maze->allCollectedSound = LoadSoundFromWave(w);  UnloadWave(w);
    w = LoadWave("assets/collect.wav");            maze->collectSound = LoadSoundFromWave(w);       UnloadWave(w);
    w = LoadWave("assets/walk.wav");               maze->walkSound = LoadSoundFromWave(w);          UnloadWave(w);
    w = LoadWave("assets/wall.wav");               maze->wallSound = LoadSoundFromWave(w);          UnloadWave(w);
    w = LoadWave("assets/win.wav");                maze->winSound = LoadSoundFromWave(w);           UnloadWave(w);
    maze->walkPlayed = maze->wallPlayed = maze->allCollectedPlayed = false;

    maze->totalItems = 0;
    
    return maze;
}

/* 销毁迷宫 */
void destroyMaze(Maze *maze) {
    for (int i = 0; i < maze->rows; i++) {
        free(maze->grid[i]);
    }
    if (maze->dist) {
        for (int i = 0; i < maze->rows; i++) {
            free(maze->dist[i]);
        }
        free(maze->dist);
    }
    if (maze->prevRow && maze->prevCol) {
        for (int i = 0; i < maze->rows; i++) {
            free(maze->prevRow[i]);
            free(maze->prevCol[i]);
        }
        free(maze->prevRow);
        free(maze->prevCol);
    }
    free(maze->grid);
    free(maze->path);
    maze->pathLen = 0;
    UnloadSound(maze->allCollectedSound);
    UnloadSound(maze->collectSound);
    UnloadSound(maze->walkSound);
    UnloadSound(maze->wallSound);
    UnloadSound(maze->winSound);
    free(maze);
}

bool saveGame(Maze *maze, const char *filename) {
    FILE *saveFile = fopen(filename, "wb");
    if (saveFile == NULL) {
        return false;
    }
    fwrite(&maze->rows, sizeof(int), 1, saveFile);
    fwrite(&maze->cols, sizeof(int), 1, saveFile);
    fwrite(&maze->totalItems, sizeof(int), 1, saveFile);
    fwrite(&maze->collectedCount, sizeof(int), 1, saveFile);
    fwrite(&maze->playerRow, sizeof(int), 1, saveFile);
    fwrite(&maze->playerCol, sizeof(int), 1, saveFile);
    fwrite(&maze->playerStep, sizeof(int), 1, saveFile);
    fwrite(&maze->elaspedTime, sizeof(double), 1, saveFile);
    fwrite(&maze->challengeMode, sizeof(bool), 1, saveFile);
    for (int row = 0; row < maze->rows; row++) {
        for (int col = 0; col < maze->cols; col++) {
            fwrite(&maze->grid[row][col].top,        sizeof(bool), 1, saveFile);
            fwrite(&maze->grid[row][col].bottom,     sizeof(bool), 1, saveFile);
            fwrite(&maze->grid[row][col].left,       sizeof(bool), 1, saveFile);
            fwrite(&maze->grid[row][col].right,      sizeof(bool), 1, saveFile);
            fwrite(&maze->grid[row][col].playerPath, sizeof(bool), 1, saveFile);
            fwrite(&maze->grid[row][col].hasItem,    sizeof(bool), 1, saveFile);
        }
    }
    maze->showSaved = true;
    maze->savedTime = GetTime();
    
    fclose(saveFile);
    return true;
}

bool loadGame(Maze *maze, const char *filename) {
    FILE *loadFile = fopen(filename, "rb");
    if (loadFile == NULL) {
        return false;
    }

    fread(&maze->rows, sizeof(int), 1, loadFile);
    fread(&maze->cols, sizeof(int), 1, loadFile);
    fread(&maze->totalItems, sizeof(int), 1, loadFile);
    fread(&maze->collectedCount, sizeof(int), 1, loadFile);
    fread(&maze->playerRow, sizeof(int), 1, loadFile);
    fread(&maze->playerCol, sizeof(int), 1, loadFile);
    fread(&maze->playerStep, sizeof(int), 1, loadFile);
    fread(&maze->elaspedTime, sizeof(double), 1, loadFile);
    fread(&maze->challengeMode, sizeof(bool), 1, loadFile);
    for (int row = 0; row < maze->rows; row++) {
        for (int col = 0; col < maze->cols; col++) {
            fread(&maze->grid[row][col].top,        sizeof(bool), 1, loadFile);
            fread(&maze->grid[row][col].bottom,     sizeof(bool), 1, loadFile);
            fread(&maze->grid[row][col].left,       sizeof(bool), 1, loadFile);
            fread(&maze->grid[row][col].right,      sizeof(bool), 1, loadFile);
            fread(&maze->grid[row][col].playerPath, sizeof(bool), 1, loadFile);
            fread(&maze->grid[row][col].hasItem,    sizeof(bool), 1, loadFile);
        }
    }
    int availH = WIDTH - TOOLBAR_H;
    maze->cellSize = (LENGTH / maze->cols < availH / maze->rows) ? LENGTH / maze->cols : availH / maze->rows;
    maze->offsetX = (LENGTH - maze->cols * maze->cellSize) / 2;
    maze->offsetY = TOOLBAR_H + (availH - maze->rows * maze->cellSize) / 2;

    fclose(loadFile);
    return true;
}

bool saveProfile(Maze *maze) {
    FILE *saveProfile = fopen("player_profile.bin", "wb");
    if (saveProfile == NULL) {
        return false;
    }

    fwrite(&maze->bestTime, sizeof(double), 3, saveProfile);
    fwrite(&maze->gamesPlayed, sizeof(int), 1, saveProfile);
    fwrite(&maze->gamesWon, sizeof(int), 1, saveProfile);
    int totalSteps = 0;
    for (int i = 0; i < maze->historyCount; i++) {
        totalSteps += maze->history[i].steps;
    }
    fwrite(&totalSteps, sizeof(int), 1, saveProfile);
    fwrite(&maze->historyCount, sizeof(int), 1, saveProfile);
    fwrite(&maze->history, sizeof(GameRecord), maze->historyCount, saveProfile);
    fclose(saveProfile);

    return true;
}

bool loadProfile(Maze *maze) {
    FILE *loadProfile = fopen("player_profile.bin", "rb");
    if (loadProfile == NULL) {
        return false;
    }

    fread(&maze->bestTime, sizeof(double), 3, loadProfile);
    fread(&maze->gamesPlayed, sizeof(int), 1, loadProfile);
    fread(&maze->gamesWon, sizeof(int), 1, loadProfile);
    int totalSteps ;
    fread(&totalSteps, sizeof(int), 1, loadProfile);
    fread(&maze->historyCount, sizeof(int), 1, loadProfile);
    fread(&maze->history, sizeof(GameRecord), maze->historyCount, loadProfile);
    fclose(loadProfile);

    return true;
}