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

/* 状态机枚举 */
typedef enum STATE {
    IDLE, GENERATING, GENERATED, SOLVING, SOLVED, PLAYING, WON   
} MazeState;

// 单格子
typedef struct CELL {
    bool top, right, left, bottom;
    bool visited;
    bool isPath;   // true: BFS 找到的路径
    bool explored; // BFS 探索过的格子
} Cell;

// 迷宫
typedef struct MAZE {
    int rows, cols;
    Cell **grid;         // 网格
    Position *path;      // BFS 路径的格子坐标
    int pathLen;         // 路径长度
    MazeState state;     // 当前状态
    StackNode *genStack; // DFS 动画帧栈（跨帧持久）
    Queue *bfsQueue;     // BFS 动画队列（跨帧持久）
    int **prevRow, **prevCol;  // BFS 回溯数组（跨帧持久） 需要储存在结构体中不会丢失
    int playerRow, playerCol;  // 玩家当前位置
    int playerStep;            // 玩家当前步数
} Maze;

/* 函数声明 */
void generateRandomizedMaze(Maze *maze);        // 运用DFS实现随机生成迷宫
bool stepGenerate(Maze *maze);                  // 在上方函数基础上实现逐帧可视化生成迷宫

void mazeSolver(Maze *maze);                    // 运用BFS实现解开迷宫
bool stepSolve(Maze *maze);                     // 在上方函数基础上实现逐帧可视化解开迷宫

Maze *createMaze(int rows, int cols);
void destroyMaze(Maze *maze);
void drawMaze(Maze *maze, Font font);
/* 链栈基本操作声明 */
StackNode *initStack();
void push(StackNode *top, Position pos);
void pop(StackNode *top, Position *pos);
bool isStackEmpty(StackNode *top);
/* 链队基本操作声明 */
Queue *initQueue();
void enqueue(Queue *q, QueueNode node);
void dequeue(Queue *q, QueueNode *node);
bool isQueueEmpty(Queue *q);

#endif