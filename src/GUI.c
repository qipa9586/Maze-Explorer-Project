#include <raylib.h>
#include "maze.h"

void drawMaze(Maze *maze) {
    for (int row = 0; row < maze->rows; row++) {
        for (int col = 0; col < maze->cols; col++) {
            // 每个格子的像素坐标:
            int x = OFFSET_X + col * CELL_SIZE; // 格子左上角x
            int y = OFFSET_Y + row * CELL_SIZE; // 格子左上角y

            Cell cell = maze->grid[row][col];

            /*
             * 画格子底色 (区分起点/终点/普通) && 画墙
             * 使用到的函数:
             * DrawRectangle(startX, startY, endX, endY, color);
             * DrawLine(startX, startY, endX, endY, color);
             */ 

            // 先画格子填充（底色）
            // 普通格子为灰色
            DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, DARKGRAY);
            if (col == 0 && row == 0) {
                // 起点颜色为绿色
                int s = CELL_SIZE / 3;  // 起点颜色方块有点大，缩小一点
                DrawRectangle(x + s, y + s, s, s, GREEN);
            } else if (col == maze->cols - 1 && row == maze->rows - 1) {
                // 终点颜色为红色
                int s = CELL_SIZE / 3;
                DrawRectangle(x + s, y + s, s, s, RED);
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
}