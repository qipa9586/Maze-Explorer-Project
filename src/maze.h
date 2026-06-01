#ifndef MAZE_H
#define MAZE_H
#include <stdbool.h>

/* 核心数据结构 */
// 定义窗口分辨率、格子大小、行和列
#define LENGTH 800    
#define WIDTH 600     
#define CELL_SIZE 30  // px
#define ROWS 20
#define COLS 26

// GUI相关宏定义
#define OFFSET_X ((LENGTH - COLS * CELL_SIZE) / 2) // 水平居中：窗口宽减迷宫宽，左右平分空白
#define OFFSET_Y ((WIDTH - ROWS * CELL_SIZE) / 2)  // 垂直居中：窗口高减迷宫高，上下平分空白
#define WALL_COLOR (Color) {200, 200, 200, 255}
#define BG_COLOR (Color) {30, 30, 30, 255}

// 单格子
typedef struct CELL {
    bool top;
    bool right;
    bool left;
    bool bottom;
    bool visited;
} Cell;

// 迷宫
typedef struct MAZE {
    int rows;
    int cols;
    Cell **grid;
} Maze;

// 队列
typedef struct QUEUE {
    int row;
    int col;
    int prevRow; // 回溯
    int prevCol;
} QueueNode;

// 显式栈结构 DFS不用系统递归栈用非递归法更灵活
typedef struct POSITION {
    int col;
    int row;
} Position;

typedef struct STACK {
    Position pos;
    struct STACK *next;
} StackNode;

/* 函数声明 */
void generateRandomizedMaze(Maze *maze);        // 运用DFS实现随机生成迷宫
void mazeSolver(Maze *maze);                    // 运用BFS实现解迷宫
Maze *createMaze(int rows, int cols);
void destroyMaze(Maze *maze);

void drawMaze(Maze *maze);

#endif