#include <raylib.h>
#include <stdio.h>
#include "maze.h"

void drawToolbar(Maze *maze) {
    const char *hint = "";
    if (maze->state != IDLE && maze->state != SELECTING_SIZE && maze->state != ABOUT) {
        DrawRectangle(0, 0, LENGTH, TOOLBAR_H, (Color){25, 30, 45, 255});
        if ((maze->state == GENERATING || maze->state == GENERATED ||
            maze->state == SOLVING || maze->state == SOLVED) && !maze->instantMode) {
            Vector2 mouse = GetMousePosition();
            for (int i = 0; i < 4; i++) {
                // 按钮是否启用
                bool isActive = (maze->animSpeed == maze->speedButton[i].speed);
                Color btnColor = isActive ? SKYBLUE : GRAY;
                bool hovering = CheckCollisionPointRec(mouse, maze->speedButton[i].bounds);
                if (hovering && !isActive) {
                    btnColor = LIGHTGRAY;
                }

                DrawRectangleRec(maze->speedButton[i].bounds, btnColor);
                DrawText(maze->speedButton[i].label, maze->speedButton[i].bounds.x + 8, 
                    maze->speedButton[i].bounds.y + 3, 20, BLACK);
            }
            hint = "R:New | M:Menu | Space:Solve | Speed:";

        } else if (maze->state == PLAYING) {
            Vector2 mouse = GetMousePosition();
            bool hovering = CheckCollisionPointRec(mouse, maze->saveButton.bounds);
            Color btnColor = hovering ? LIGHTGRAY : GRAY;
            DrawRectangleRec(maze->saveButton.bounds, btnColor);
            DrawText(maze->saveButton.label, maze->saveButton.bounds.x + 22,
                     maze->saveButton.bounds.y + 3, 22, BLACK);
            hint = "M:Back | ? : Solve";
            if (maze->challengeMode) {
                // 金币图案放在toolbar 美观
                int itemsX = maze->saveButton.bounds.x + maze->saveButton.bounds.width + 20;
                int coinR = 10; // toolbar 里小金币，半径 10
                int fontSize = 18;
                DrawCircle(itemsX + coinR, fontSize, coinR, GOLD);
                DrawCircle(itemsX + coinR, fontSize, coinR * 0.6, (Color){255, 255, 200, 255}); // 内芯亮
                DrawText("$", itemsX + coinR - MeasureText("$", fontSize) / 2,
                         fontSize / 2, fontSize, (Color){200, 150, 30, 255});
                // 数字 获得指示器
                char items[64];
                sprintf(items, "Items: %d / %d", maze->collectedCount, maze->totalItems);
                DrawText(items, itemsX + coinR * 2 + 6, 8, 20, GOLD);
            }

        } else if (maze->state == WON) {
            hint = "R:New  M:Menu";
        } else if (maze->instantMode) {
            hint = "R:New | M:Menu | Space:Solve";
        }

        DrawText(hint, 190, 8, 20, LIGHTGRAY);
        // DrawText("F12:Screenshot", 1400, 8, 20, LIGHTGRAY); 截图用不了有bug
        return;
    }
}

void drawMaze(Maze *maze) {
    // 画菜单 闲置状态画菜单
    if (maze->state == IDLE || maze->state == SELECTING_SIZE || maze->state == SELECTING_MODE || 
        maze->state == PROFILE || maze->state == ABOUT) {
        if (maze->state == IDLE) {
            ClearBackground((Color){15, 20, 35, 255});
            DrawText("MAZE EXPLORER", LENGTH / 2 - 335, 100, 80, WHITE);
            DrawText("Made By ZHM   Version 2.2.6", LENGTH / 2 - 100, 180, 35, WHITE);
            // me Vicky With Claude Code 😄
            // 1. 获取鼠标位置
            Vector2 mouse = GetMousePosition();
            for (int i = 0; i < 3; i++) {
                // 2. 检测鼠标是否在矩形内
                bool hovering = CheckCollisionPointRec(mouse, maze->menuButton[i].bounds);
                // 3. 画按钮矩形（悬停时换个颜色）
                Color btnColor = hovering ? LIGHTGRAY : GRAY;
                DrawRectangleRec(maze->menuButton[i].bounds, btnColor);
                // 4. 画按钮文字
                DrawText(maze->menuButton[i].label, maze->menuButton[i].bounds.x + 60, 
                         maze->menuButton[i].bounds.y + 25, 40, BLACK);
            }
            bool hovering = CheckCollisionPointRec(mouse, maze->profileButton.bounds);
            Color btnColor = hovering ? LIGHTGRAY : GRAY;
            DrawRectangleRec(maze->profileButton.bounds, btnColor);
            DrawText(maze->profileButton.label, maze->profileButton.bounds.x + 60,
                     maze->profileButton.bounds.y + 25, 40, BLACK);

            hovering = CheckCollisionPointRec(mouse, maze->aboutButton.bounds);
            btnColor = hovering ? LIGHTGRAY : GRAY;
            DrawRectangleRec(maze->aboutButton.bounds, btnColor);
            DrawText(maze->aboutButton.label, maze->aboutButton.bounds.x + 30,
                     maze->aboutButton.bounds.y + 7, 20, WHITE);

        } else if (maze->state == SELECTING_SIZE) { // 二级菜单
            ClearBackground((Color){15, 20, 35, 255});
            DrawText("CHOOSE MAZE", LENGTH / 2 - 300, 100, 80, WHITE);
                
            // 画按钮
            Vector2 mouse = GetMousePosition();
            for (int i = 0; i < 3; i++) {
                bool hovering = CheckCollisionPointRec(mouse, maze->sizeButton[i].bounds);
                Color btnColor = hovering ? LIGHTGRAY : GRAY;
                DrawRectangleRec(maze->sizeButton[i].bounds, btnColor);
                DrawText(maze->sizeButton[i].label, maze->sizeButton[i].bounds.x + 60,
                         maze->sizeButton[i].bounds.y + 15, 40, BLACK);
            }
        } else if (maze->state == SELECTING_MODE) { // 三级菜单
            ClearBackground((Color){15, 20, 35, 255});
            DrawText("CHOOSE MODE", LENGTH / 2 - 300, 100, 80, WHITE);

            Vector2 mouse = GetMousePosition();
            for (int i = 0; i < 2; i++) {
                bool hovering = CheckCollisionPointRec(mouse, maze->modeButton[i].bounds);
                Color btnColor = hovering ? LIGHTGRAY : GRAY;
                DrawRectangleRec(maze->modeButton[i].bounds, btnColor);
                DrawText(maze->modeButton[i].label, maze->modeButton[i].bounds.x + 60,
                    maze->modeButton[i].bounds.y + 15, 40, BLACK);
            }
        } else if (maze->state == PROFILE) {    // 玩家档案界面
            ClearBackground((Color){15, 20, 35, 255});
            Vector2 mouse = GetMousePosition();
            // 画退出按钮
            bool hovering = CheckCollisionPointRec(mouse, maze->backButton.bounds);
            Color btnColor = hovering ? LIGHTGRAY : GRAY;
            DrawRectangleRec(maze->backButton.bounds, btnColor);
            DrawText(maze->backButton.label, maze->backButton.bounds.x + 50,
                     maze->backButton.bounds.y + 12, 30, BLACK);
            if (CheckCollisionPointRec(mouse, maze->backButton.bounds) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || 
                IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
                maze->state = IDLE;
            }

            DrawText("PLAYER PROFILE", LENGTH / 2 - 285, 60, 60, WHITE);
            // 战绩 
            char stats[128];
            sprintf(stats, "Games: %d     Won: %d     Won Rate: %.0f%%", 
                    maze->gamesPlayed, maze->gamesWon, 
                    maze->gamesPlayed > 0 ? (float)(maze->gamesWon / maze->gamesPlayed) * 100.0 : 0);
            DrawText(stats, LENGTH / 2 - MeasureText(stats, 30) / 2, 140, 30, WHITE);
            
            char bestRecord[128];
            DrawText("Best Records: ", LENGTH / 2 - 100, 200, 28, GOLD);
            const char *labels[] = {"Small", "Medium", "Large"};
            for (int i = 0; i < 3; i++) {
                if (maze->bestTime[i] == 0.0) {
                    sprintf(bestRecord, "%-6s ( %2dx%-2d ):  ---", 
                        labels[i], 
                        i == 0 ? 20 : i == 1 ? 40 : 60, // 迷宫大小
                        i == 0 ? 26 : i == 1 ? 52 : 78);
                } else {
                    sprintf(bestRecord, "%s ( %2dx%-2d ):  %.1fs", 
                        labels[i], 
                        i == 0 ? 20 : i == 1 ? 40 : 60, 
                        i == 0 ? 26 : i == 1 ? 52 : 78,
                        maze->bestTime[i]);
                }
                DrawText(bestRecord, LENGTH / 2 - 200, 245 + i * 35, 24, WHITE);
            }

            int x = 475;
            int y = 415;
            int xCol[] = {x, x + 80, x + 180, x + 360, x + 460, x + 635};
            DrawText("Recent Games:", LENGTH / 2 - 100, 370, 28, GOLD);
            DrawText("No.", xCol[0], y, 24, WHITE);
            DrawText("Size", xCol[1], y, 24, WHITE);
            DrawText("Player Steps", xCol[2], y, 24, WHITE);
            DrawText("Grade", xCol[3], y, 24, WHITE);
            DrawText("Elasped Time", xCol[4], y, 24, WHITE);

            int startIndex = maze->historyCount > 5 ? maze->historyCount - 5 : 0;
            int displayCount = maze->historyCount > 5 ? 5 : maze->historyCount;
            for (int i = 0; i < displayCount; i++) {
                GameRecord *G = &maze->history[startIndex + i];
                const char *sizeLabel = G->rows == 20 ? "Small" : G->rows == 40 ? "Medium" : "Large";
                // 排版
                char buf[32];
                y += 35;

                sprintf(buf, "#%d", startIndex + i + 1);
                DrawText(buf, xCol[0], y, 24, WHITE);

                DrawText(sizeLabel, xCol[1], y, 24, WHITE);

                sprintf(buf, "%d steps", G->steps);
                DrawText(buf, xCol[2], y, 24, WHITE);

                sprintf(buf, "%c", G->grade);
                DrawText(buf, xCol[3], y, 24, WHITE);

                sprintf(buf, "%.1fs", G->time);
                DrawText(buf, xCol[4], y, 24, WHITE);

                DrawText(G->won ? "Win" : "Lose", xCol[5], y, 24, WHITE);
            }

            if (maze->historyCount == 0) {
                DrawText("No games played yet.", LENGTH / 2 - 210, 415, 24, WHITE);
            }
            
        } else if (maze->state == ABOUT) {
            ClearBackground((Color){15, 20, 35, 255});
            DrawTexture(maze->aboutTex, 0, 0, WHITE);

            Vector2 mouse = GetMousePosition();
            bool hovering = CheckCollisionPointRec(mouse, maze->backButton.bounds);
            Color btnColor = hovering ? LIGHTGRAY : GRAY;
            DrawRectangleRec(maze->backButton.bounds, btnColor);
            DrawText(maze->backButton.label, maze->backButton.bounds.x + 50,
                     maze->backButton.bounds.y + 12, 30, BLACK);
            if (CheckCollisionPointRec(mouse, maze->backButton.bounds) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || 
                IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
                maze->state = IDLE;
            }
        }

        return;
    }

    for (int row = 0; row < maze->rows; row++) {
        for (int col = 0; col < maze->cols; col++) {
            // 每个格子的像素坐标:
            int x = maze->offsetX + col * maze->cellSize; // 格子左上角x
            int y = maze->offsetY + row * maze->cellSize; // 格子左上角y

            Cell cell = maze->grid[row][col];

            /*
             * 画格子底色 (区分起点/终点/普通) && 画墙
             * 使用到的函数:
             * DrawRectangle(startX, startY, endX, endY, color);  画矩形-格子底色
             * DrawLine(startX, startY, endX, endY, color);       画直线-墙
             * DrawCircle(centerX, centerY, radius, color);       画圆形-起点终点
             */ 

            int cx = x + maze->cellSize / 2;  // 圆心 x
            int cy = y + maze->cellSize / 2;  // 圆心 y
            int radius = maze->cellSize / 4;  // 圆的半径

            // 先画格子填充（底色）
            // 普通格子为灰色
            DrawRectangle(x, y, maze->cellSize, maze->cellSize, DARKGRAY);

            // 看到 BFS 的扩张 (热力图版) 凉色近 暖色远 运用的是队列 层序得到每一层然后染色
            if (cell.explored) {
                float t = (float)maze->dist[row][col] / maze->maxDist;
                Color heat = ColorFromHSV((1.0f - t) * 240.0f, 0.8f, 1.0f);
                DrawRectangle(x, y, maze->cellSize, maze->cellSize, heat);
            }

            // 找到的路径格子为深紫底
            if (maze->pathLen > 0) {
                int len = (maze->state == BACKTRACK_ANIM) ? maze->visiblePathLen : maze->pathLen;
                for (int i = 0; i < len; i++) {
                    if (maze->path[i].row == row && maze->path[i].col == col) {
                        DrawRectangle(x, y, maze->cellSize, maze->cellSize, (Color){90, 40, 200, 210});
                    }
                }
            }
            // 画玩家轨迹 放墙逻辑前面为了不让墙被覆盖
            if (cell.playerPath) {
                DrawRectangle(x, y, maze->cellSize, maze->cellSize, (Color){100, 100, 0, 128}); // 暗金色半透明
            }
            // 画金币
            if (cell.hasItem) {
                int coinRadius = maze->cellSize / 4;
                DrawCircle(cx, cy, coinRadius, GOLD);
                DrawCircle(cx, cy, coinRadius * 0.6, (Color){255, 255, 200, 255});  // 内芯亮
                int fontSize = maze->cellSize / 3;
                DrawText("$", cx - MeasureText("$", fontSize) / 2, cy - fontSize / 2, fontSize, (Color){200, 150, 30, 255});
            }

            // 再画墙（画在格子底色上面）
            if (cell.top) {
                DrawLine(x, y, x + maze->cellSize, y, WALL_COLOR);
            }
            if (cell.bottom) {
                DrawLine(x, y + maze->cellSize, x + maze->cellSize, y + maze->cellSize, WALL_COLOR);
            }
            if (cell.left) {
                DrawLine(x, y, x, y + maze->cellSize, WALL_COLOR);
            }
            if (cell.right) {
                DrawLine(x + maze->cellSize, y, x + maze->cellSize, y + maze->cellSize, WALL_COLOR);
            }
        }
    }
    // BFS 找到的金线路径
    // 每个格子的中心点坐标 = 迷宫偏移 + 格子位置 × 格子尺寸 + 半格（到中心）
    if (maze->pathLen > -1) {
        int len = (maze->state == BACKTRACK_ANIM) ? maze->visiblePathLen : maze->pathLen;
        for (int i = 0; i < len - 1; i++) {
            int x1 = maze->offsetX + maze->path[i].col * maze->cellSize + maze->cellSize / 2;
            int x2 = maze->offsetX + maze->path[i + 1].col * maze->cellSize + maze->cellSize / 2;
            int y1 = maze->offsetY + maze->path[i].row * maze->cellSize + maze->cellSize / 2;
            int y2 = maze->offsetY + maze->path[i + 1].row * maze->cellSize + maze->cellSize / 2;
            DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, 4.5f, (Color){255, 210, 60, 255}); // 金色
        }
    }
    // 起点 (0, 0) 绿色
    int startX = maze->offsetX + maze->cellSize / 2;
    int startY = maze->offsetY + maze->cellSize / 2;
    DrawCircle(startX, startY, maze->cellSize / 4, GREEN);

    // 终点 (rows-1, cols-1) 红色
    int ex = maze->offsetX + (maze->cols - 1) * maze->cellSize + maze->cellSize / 2;
    int ey = maze->offsetY + (maze->rows - 1) * maze->cellSize + maze->cellSize / 2;
    DrawCircle(ex, ey, maze->cellSize / 4, RED);

    // 画玩家格子
    if (maze->state == PLAYING) {
        int centerX = maze->offsetX + maze->playerCol * maze->cellSize + maze->cellSize / 4;
        int centerY = maze->offsetY + maze->playerRow * maze->cellSize + maze->cellSize / 4;
        int sideLength = maze->cellSize / 2;
        DrawRectangle(centerX, centerY, sideLength, sideLength, ORANGE);
        Rectangle playerRect = {centerX, centerY, sideLength, sideLength}; // 玩家格子碰撞箱

        if (CheckCollisionPointRec(GetMousePosition(), playerRect)) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND); // 鼠标变手指
        } else {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
    }

    int centerX = LENGTH / 2;
    int centerY = WIDTH / 2;
    if (maze->confirmReturn) {
        DrawRectangle(centerX - 325, centerY - 40, 725, 80, (Color){0, 0, 0, 200});
        DrawText("Return to size select ? (Y/N)", centerX - 275, centerY - 20, 40, YELLOW);
    }

    if (maze->confirmQuit) {
        DrawRectangle(LENGTH / 2 - 340, WIDTH / 2 - 40, 710, 80, (Color){0, 0, 0, 200});
        DrawText("Do you want to QUIT ? (Y/N)", LENGTH / 2 - 290, WIDTH / 2 - 20, 40, YELLOW);
    }

    if (maze->confirmLoad) {
        DrawRectangle(centerX - 325, centerY - 40, 725, 80, (Color){0, 0, 0, 200});
        DrawText("Continue saved game ? (Y/N)", centerX - 275, centerY - 20, 40, YELLOW);
    }

    if (maze->showSaved) {
        if (GetTime() - maze->savedTime > 1.0) {
            maze->showSaved = false;
        } else {
            DrawRectangle(centerX - 100, centerY - 40, 200, 80, (Color){0, 0, 0, 200});
            DrawText("Saved !", centerX - MeasureText("Saved !", 40) / 2, centerY - 20, 40, YELLOW);
        }
    }

    if (maze->showCollectHint) {
        int centerX = LENGTH / 2;
        int centerY = WIDTH / 2;
        DrawRectangle(centerX - 250, centerY - 40, 500, 80, (Color){0, 0, 0, 200});
        DrawText("Collect all items first !",
                 centerX - MeasureText("Collect all items first !", 35) / 2,
                 centerY - 20, 35, GOLD);
    }

    if (maze->showSaved) {
        int centerX = LENGTH / 2;
        int centerY = WIDTH / 2;
        DrawRectangle(centerX - 100, centerY - 40, 200, 80, (Color){0, 0, 0, 200});
        DrawText("Saved !", centerX - MeasureText("Saved !", 40) / 2, centerY - 20, 40, YELLOW);
    }

    // 获胜提示信息
    if (maze->state == WON) {
        // 居中
        int boxW = 400, boxH = 310;
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

        sprintf(steps, "Time: %.1f s", maze->elaspedTime);
        DrawText(steps, winX + boxW / 2 - MeasureText(steps, 30) / 2, winY + 200, 30, WHITE);

        if (maze->isNewBest) {
            DrawText("New Best !", winX + boxW / 2 - MeasureText("New Best !", 30) / 2, winY + 220, 30, GOLD);
        }

        if (maze->grade != '\0') {
            Color gradeColor;
            const char *gradeText;
            switch (maze->grade) {
            case 'S': {
                gradeColor = GOLD;
                gradeText = "S";
            } break;

            case 'A': {
                gradeColor = RED;
                gradeText = "A";
            } break;

            case 'B': {
                gradeColor = YELLOW;
                gradeText = "B";
            } break;

            default: {
                gradeColor = WHITE;
                gradeText = "C";
            } break;
            }
            // 光环 (S专属)
            if (maze->grade == 'S') {
                DrawText("S", winX + boxW / 2 - MeasureText("S", 110) / 2, winY + 230, 110, (Color){255, 215, 0, 40});
                DrawText("S", winX + boxW / 2 - MeasureText("S", 95) / 2, winY + 238, 95, (Color){255, 215, 0, 70});
                DrawText("S", winX + boxW / 2 - MeasureText("S", 80) / 2, winY + 245, 80, (Color){255, 215, 0, 150});
            }

            DrawText(gradeText, winX + boxW / 2 - MeasureText(gradeText, 70) / 2, winY + 250, 70, gradeColor);
        }
    }
}