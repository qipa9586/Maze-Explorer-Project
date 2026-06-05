#include <raylib.h>
#include <stdio.h>
#include "maze.h"

void drawToolbar(Maze *maze) {
    const char *hint = "";
    if (maze->state != IDLE && maze->state != SELECTING_SIZE) {
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
            DrawText(maze->saveButton.label, maze->saveButton.bounds.x + 10, 
                    maze->saveButton.bounds.y + 4, 20, BLACK);
            hint = "M:Back | ? : Solve";
        } else if (maze->state == WON) {
            hint = "R:New  M:Menu";
        } else if (maze->instantMode) {
            hint = "R:New | M:Menu | Space:Solve";
        }

        DrawText(hint, 190, 8, 20, LIGHTGRAY);
        DrawText("F12:Screenshot", 1400, 8, 20, LIGHTGRAY);
        return;
    }
}

void drawMaze(Maze *maze) {
    // 画菜单 闲置状态画菜单
    if (maze->state == IDLE || maze->state == SELECTING_SIZE) {
        if (maze->state == IDLE) {
            ClearBackground((Color){15, 20, 35, 255});
            DrawText("MAZE EXPLORER", LENGTH / 2 - 335, 100, 80, WHITE);
            DrawText("Made By ZHM   Version 2.1.2", LENGTH / 2 - 100, 180, 35, WHITE);
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
                DrawText(maze->menuButton[i].label, maze->menuButton[i].bounds.x + 60, maze->menuButton[i].bounds.y + 25, 40, BLACK);
            }
        } else if (maze->state == SELECTING_SIZE) {
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

            // 看到 BFS 的扩张
            if (cell.explored) {
                DrawRectangle(x, y, maze->cellSize, maze->cellSize, (Color){50, 50, 80, 255}); // 深蓝紫
            }

            if (col == 0 && row == 0) {
                // 起点颜色为绿色
                DrawCircle(cx, cy, radius, GREEN);
            } else if (col == maze->cols - 1 && row == maze->rows - 1) {
                // 终点颜色为红色
                DrawCircle(cx, cy, radius, RED);
            }

            // 画玩家轨迹 放墙逻辑前面为了不让墙被覆盖
            if (cell.playerPath) {
                DrawRectangle(x, y, maze->cellSize, maze->cellSize, (Color){100, 100, 0, 128}); // 暗金色半透明
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

    // BFS 找到的路径
    // 每个格子的中心点坐标 = 迷宫偏移 + 格子位置 × 格子尺寸 + 半格（到中心）
    for (int i = 0; i < maze->pathLen - 1; i++) {
        int x1 = maze->offsetX + maze->path[i].col * maze->cellSize + maze->cellSize / 2;  
        int x2 = maze->offsetX + maze->path[i + 1].col * maze->cellSize + maze->cellSize / 2;
        int y1 = maze->offsetY + maze->path[i].row * maze->cellSize + maze->cellSize / 2;
        int y2 = maze->offsetY + maze->path[i + 1].row * maze->cellSize + maze->cellSize / 2;
        DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, 3.0f, GOLD);
    }

    // 本来想到终点把点给变个颜色想想还是算了 直接被覆盖了
    // int cx_start = OFFSET_X + CELL_SIZE / 2;
    // int cy_start = OFFSET_Y + CELL_SIZE / 2;
    // int cx_end = OFFSET_X + (maze->cols - 1) * CELL_SIZE + CELL_SIZE / 2;
    // int cy_end = OFFSET_Y + (maze->rows - 1) * CELL_SIZE + CELL_SIZE / 2;
    // DrawCircle(cx_start, cy_start, CELL_SIZE / 4, RED);
    // DrawCircle(cx_end, cy_end, CELL_SIZE / 4, GREEN);

    // 画玩家格子
    if (maze->state == PLAYING) {
        int cx = maze->offsetX + maze->playerCol * maze->cellSize + maze->cellSize / 4;
        int cy = maze->offsetY + maze->playerRow * maze->cellSize + maze->cellSize / 4;
        DrawRectangle(cx, cy, maze->cellSize / 2, maze->cellSize / 2, ORANGE);
    }
    // 老提示 不要了先丢这
    /* if (maze->state != IDLE && maze->state != PLAYING && maze->state != SELECTING_SIZE) {
        DrawText("R:Regenerate | M:Menu | Space:Solve | ESC:Quit", 10, WIDTH - 25, 20, GREEN);
    } else if (maze->state == PLAYING) {
        DrawText("M:Back | ?:HINT | ESC:Quit", 10, WIDTH - 25, 20, GREEN);
    } */

    int centerX = LENGTH / 2;
    int centerY = WIDTH / 2;
    if (maze->confirmReturn) {
        DrawRectangle(centerX - 325, centerY - 40, 725, 80, (Color){0, 0, 0, 200});
        DrawText("Return to size select ? (Y/N)", centerX - 275, centerY - 20, 40, YELLOW);
        if (IsKeyPressed(KEY_Y) || IsKeyPressed(KEY_ENTER)) {
            maze->state = SELECTING_SIZE;
            maze->confirmReturn = false;
        }
        if (IsKeyPressed(KEY_N)) {
            maze->confirmReturn = false;
        }
    }
}