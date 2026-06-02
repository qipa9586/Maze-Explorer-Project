#include <raylib.h>
#include <stdio.h>
#include "maze.h"

void drawMaze(Maze *maze, Font font) {
    for (int row = 0; row < maze->rows; row++) {
        for (int col = 0; col < maze->cols; col++) {
            // 每个格子的像素坐标:
            int x = OFFSET_X + col * CELL_SIZE; // 格子左上角x
            int y = OFFSET_Y + row * CELL_SIZE; // 格子左上角y

            Cell cell = maze->grid[row][col];

            /*
             * 画格子底色 (区分起点/终点/普通) && 画墙
             * 使用到的函数:
             * DrawRectangle(startX, startY, endX, endY, color);  画矩形-格子底色
             * DrawLine(startX, startY, endX, endY, color);       画直线-墙
             * DrawCircle(centerX, centerY, radius, color);       画圆形-起点终点
             */ 

            int cx = x + CELL_SIZE / 2;  // 圆心 x
            int cy = y + CELL_SIZE / 2;  // 圆心 y
            int radius = CELL_SIZE / 4;  // 圆的半径

            // 先画格子填充（底色）
            // 普通格子为灰色
            DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, DARKGRAY);

            // 看到 BFS 的扩张
            if (cell.explored) {
                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, (Color){50, 50, 80, 255}); // 深蓝紫
            }

            if (col == 0 && row == 0) {
                // 起点颜色为绿色
                DrawCircle(cx, cy, radius, GREEN);
            } else if (col == maze->cols - 1 && row == maze->rows - 1) {
                // 终点颜色为红色
                DrawCircle(cx, cy, radius, RED);
            }

            // 再画墙（画在格子底色上面）
            if (cell.top) {
                DrawLine(x, y, x + CELL_SIZE, y, WALL_COLOR);
            }
            if (cell.bottom) {
                DrawLine(x, y + CELL_SIZE, x + CELL_SIZE, y + CELL_SIZE, WALL_COLOR);
            }
            if (cell.left) {
                DrawLine(x, y, x, y + CELL_SIZE, WALL_COLOR);
            }
            if (cell.right) {
                DrawLine(x + CELL_SIZE, y, x + CELL_SIZE, y + CELL_SIZE, WALL_COLOR);
            }
        }
    }

    // BFS 找到的路径
    // 每个格子的中心点坐标 = 迷宫偏移 + 格子位置 × 格子尺寸 + 半格（到中心）
    for (int i = 0; i < maze->pathLen - 1; i++) {
        int x1 = OFFSET_X + maze->path[i].col * CELL_SIZE + CELL_SIZE / 2;  
        int x2 = OFFSET_X + maze->path[i + 1].col * CELL_SIZE + CELL_SIZE / 2;
        int y1 = OFFSET_Y + maze->path[i].row * CELL_SIZE + CELL_SIZE / 2;
        int y2 = OFFSET_Y + maze->path[i + 1].row * CELL_SIZE + CELL_SIZE / 2;
        DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, 3.0f, GOLD);
    }

    // int cx_start = OFFSET_X + CELL_SIZE / 2;
    // int cy_start = OFFSET_Y + CELL_SIZE / 2;
    // int cx_end = OFFSET_X + (maze->cols - 1) * CELL_SIZE + CELL_SIZE / 2;
    // int cy_end = OFFSET_Y + (maze->rows - 1) * CELL_SIZE + CELL_SIZE / 2;
    // DrawCircle(cx_start, cy_start, CELL_SIZE / 4, RED);
    // DrawCircle(cx_end, cy_end, CELL_SIZE / 4, GREEN);
}