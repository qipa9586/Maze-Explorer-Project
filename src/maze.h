#ifndef MAZE_H
#define MAZE_H
#include <stdbool.h>
#include <raylib.h>

/* 核心数据结构 */
// 定义窗口分辨率、格子大小、行和列
#define LENGTH 1600    
#define WIDTH 1200     
#define CELL_SIZE 30  // px
#define ROWS 40
#define COLS 52

// GUI相关宏定义
#define OFFSET_X ((LENGTH - COLS * CELL_SIZE) / 2) // 水平居中：窗口宽减迷宫宽，左右平分空白
#define OFFSET_Y ((WIDTH - ROWS * CELL_SIZE) / 2)  // 垂直居中：窗口高减迷宫高，上下平分空白
#define WALL_COLOR (Color) {200, 200, 200, 255}
#define BG_COLOR (Color) {30, 30, 30, 255}
#define FONT_PATH "C:/Windows/Fonts/FiraCode-Bold.ttf"

// 队列 采用链队
typedef struct QUEUE {
    int row, col;
    int prevRow, prevCol; // 回溯
    struct QUEUE *next;
} QueueNode;

typedef struct LinkedQueue {
    QueueNode *front;
    QueueNode *rear;
} Queue;

// 显式栈数据结构 采用链栈 DFS不用系统递归栈用非递归法更灵活
typedef struct POSITION {
    int col;
    int row;
} Position;

typedef struct STACK {
    Position pos;
    struct STACK *next;
} StackNode;

// 单格子
typedef struct CELL {
    bool top, right, left, bottom;
    bool visited;
    bool isPath; // true: BFS 找到的路径
} Cell;

// 迷宫
typedef struct MAZE {
    int rows, cols;
    Cell **grid;
    Position *path; // BFS 路径的格子坐标
    int pathLen;    // 路径长度
} Maze;

/* 函数声明 */
void generateRandomizedMaze(Maze *maze);        // 运用DFS实现随机生成迷宫
void mazeSolver(Maze *maze);                    // 运用BFS实现解迷宫
Maze *createMaze(int rows, int cols);
void destroyMaze(Maze *maze);

void drawMaze(Maze *maze, Font font);

#endif