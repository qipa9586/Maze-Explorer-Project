#include <stdlib.h>
#include "maze.h"
#define max(a, b) a > b ? a : b 

/* 链队基本操作 */
Queue *initQueue() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->front = NULL;
    q->rear = NULL;

    return q;
}

void enqueue(Queue *q, QueueNode node) {
    QueueNode *tmp = (QueueNode *)malloc(sizeof(QueueNode));
    *tmp = node;
    tmp->next = NULL;

    // 空队列时
    if (q->front == NULL) {
        q->front = tmp;
        q->rear = tmp;
    } else {
        q->rear->next = tmp;
        q->rear = tmp;
    }
}   

void dequeue(Queue *q, QueueNode *node) {
    if (q->front == NULL) return;

    QueueNode *tmp = q->front;
    *node = *tmp;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(tmp);
}

bool isQueueEmpty(Queue *q) {
    return q->front == NULL;
}

/* BFS解出迷宫 */
void mazeSolver(Maze *maze) {
    /* 1. 重置visited */
    for (int i = 0; i < maze->rows; i++) {
        for (int j = 0; j < maze->cols; j++) {
            maze->grid[i][j].visited = false;
        }
    }

    /* 2. 创建队列 入队起点 visited标记 */
    Queue* queue = initQueue();
    QueueNode node;
    node.row = 0; node.col = 0; node.next = NULL;
    node.prevRow = -1; node.prevCol = -1; 
    enqueue(queue, node);
    maze->grid[0][0].visited = true;

    /* 
     * 3. 偏移数组 dRows & dCols
     * BFS 还是要检查四个邻居
     */
    int dRows[] = {-1, 1, 0, 0};
    int dCols[] = {0, 0, -1, 1};

    /* 4. prev 回溯数组 */
    int **prevRow = (int **)malloc(sizeof(int *) * maze->rows);
    int **prevCol = (int **)malloc(sizeof(int *) * maze->rows);
    for (int i = 0; i < maze->rows; i++) {
        prevRow[i] = (int *)malloc(sizeof(int) * maze->cols);
        prevCol[i] = (int *)malloc(sizeof(int) * maze->cols);
    }
    // 全部初始化为-1 表示没有前驱
    for (int i = 0; i < maze->rows; i++) {
        for (int j = 0; j < maze->cols; j++) {
            prevRow[i][j] = -1;
            prevCol[i][j] = -1;
        }
    }

    /* 5. 主循环 BFS */
    while (!isQueueEmpty(queue)) {
        /* 检查是否为终点 */
        QueueNode curr;
        dequeue(queue, &curr);
        if (curr.row == maze->rows - 1 && 
            curr.col == maze->cols - 1) {
            break;
        }

        /* 四个方向：墙通 + 没越界 + 未访问 -> 记录 prev 入队 */
        int nextRow, nextCol;
        for (int i = 0; i < 4; i++) {
            nextRow = curr.row + dRows[i];
            nextCol = curr.col + dCols[i];
            bool isAccessible = false;  // 墙是否可通行
            // 0上1下2左3右
            switch (i) {
                case 0: {
                    if (maze->grid[curr.row][curr.col].top == false) {
                        isAccessible = true;
                    }
                } break;
                case 1: {
                    if (maze->grid[curr.row][curr.col].bottom == false) {
                        isAccessible = true;
                    }
                } break;
                case 2: {
                    if (maze->grid[curr.row][curr.col].left == false) {
                        isAccessible = true;
                    }
                } break;
                case 3: {
                    if (maze->grid[curr.row][curr.col].right == false) {
                        isAccessible = true;
                    }
                } break;
                default: break;
            }

            if (nextRow >= 0 && nextRow < maze->rows && 
                nextCol >= 0 && nextCol < maze->cols) {
                    if (isAccessible && 
                        !maze->grid[nextRow][nextCol].visited) {
                        prevRow[nextRow][nextCol] = curr.row;
                        prevCol[nextRow][nextCol] = curr.col;
                        maze->grid[nextRow][nextCol].visited = true;
                        // 邻居入队
                        QueueNode neighbour;
                        neighbour.row = nextRow;        neighbour.col = nextCol;
                        neighbour.prevRow = curr.row;   neighbour.prevCol = curr.col;
                        neighbour.next = NULL;
                        enqueue(queue, neighbour);
                    }
                }
        }
    }

    /* 6. 从终点顺着 prev 回溯到起点 标记路径 */
    int row = maze->rows - 1, col = maze->cols - 1;
    while (row != -1 && col != -1) {
        maze->path[maze->pathLen].row = row;
        maze->path[maze->pathLen].col = col;
        maze->pathLen++;
        
        int prevR = prevRow[row][col];
        int prevC = prevCol[row][col];
        row = prevR; 
        col = prevC;
    }

    /* 7. 释放 prev 数组 */
    for (int i = 0; i < maze->rows; i++) {
        free(prevRow[i]);
        free(prevCol[i]);
    }
    free(prevRow);
    free(prevCol);
}

/* 逐帧动画解开迷宫 可视化生成 */
bool stepSolve(Maze *maze) {
    if (isQueueEmpty(maze->bfsQueue)) {
        return true;
    }

    // 如果当前是终点 -> return true
    QueueNode curr;
    dequeue(maze->bfsQueue, &curr);
    if (curr.row == maze->rows - 1 &&
        curr.col == maze->cols - 1) {
        return true;
    }

    // 偏移数组
    int dRows[] = {-1, 1, 0, 0};
    int dCols[] = {0, 0, -1, 1};

    int nextRow, nextCol;
    for (int i = 0; i < 4; i++) {
        nextRow = curr.row + dRows[i];
        nextCol = curr.col + dCols[i];
        bool isAccessible = false;

        switch (i) {
            case 0: {
                if (maze->grid[curr.row][curr.col].top == false) {
                    isAccessible = true;
                }
            } break;

            case 1: {
                if (maze->grid[curr.row][curr.col].bottom == false) {
                    isAccessible = true;
                }
            } break;

            case 2: {
                if (maze->grid[curr.row][curr.col].left == false) {
                    isAccessible = true;
                }
            } break;

            case 3: {
                if (maze->grid[curr.row][curr.col].right == false) {
                    isAccessible = true;
                }
            } break;

            default:
                break;
        }

        if (nextRow >= 0 && nextRow < maze->rows &&
            nextCol >= 0 && nextCol < maze->cols) {
                if (isAccessible &&
                    !maze->grid[nextRow][nextCol].visited) {
                        maze->prevRow[nextRow][nextCol] = curr.row;
                        maze->prevCol[nextRow][nextCol] = curr.col;
                        maze->grid[nextRow][nextCol].visited = true;
                        maze->grid[nextRow][nextCol].explored = true;
                        maze->dist[nextRow][nextCol] = maze->dist[curr.row][curr.col] + 1;
                        maze->maxDist = max(maze->maxDist, maze->dist[nextRow][nextCol]);
                        // 邻居入队
                        QueueNode neighbour;
                        neighbour.row = nextRow;        neighbour.col = nextCol;
                        neighbour.prevRow = curr.row;   neighbour.prevCol = curr.col;
                        neighbour.next = NULL;
                        enqueue(maze->bfsQueue, neighbour);
                    }
            }

    }

    return false;
}