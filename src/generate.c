#include <stdlib.h>
#include <time.h>
#include "maze.h"

/* 链栈基本操作 */
StackNode *initStack() {
    StackNode *s = (StackNode *)malloc(sizeof(StackNode));
    s->next = NULL;
    return s;
}

void push(StackNode *top, Position pos) {
    StackNode *p = (StackNode *)malloc(sizeof(StackNode));
    p->pos = pos;
    p->next = top->next;
    top->next = p;
}

void pop(StackNode *top, Position *pos) {
    if (top == NULL || top->next == NULL) return;

    StackNode *del = top->next;
    *pos = del->pos;
    top->next = del->next;
    free(del);
}

Position peek(StackNode *top) {
    return top->next->pos;
}

bool isStackEmpty(StackNode *top) {
    return top->next == NULL;
}

/* 随机化DFS生成迷宫 使用 while 直接一锤子生成迷宫*/
void generateRandomizedMaze(Maze *maze) {
    StackNode *stack = initStack();

    maze->grid[0][0].visited = true; // 原点默认已被访问过
    
    Position start = {0, 0};
    push(stack, start);  // 原点先进显式栈

    // 方向（偏移）数组 四个方向找上下左右邻居
    int dRows[] = {-1, 1, 0, 0};  // 方向偏移：行上-1 下+1 左0   右0
    int dCols[] = {0, 0, -1, 1};  // 方向偏移：列上0  下0  左-1  右+1
    
    // 主循环 栈非空时
    while (!isStackEmpty(stack)) {
        Position curr = peek(stack);

        /* 1. 收集未访问过的邻居 */
        int unvisited[4] = {0, 1, 2, 3};  // 0上1下2左3右
        int count = 0;
        int nextRow, nextCol;

        for (int i = 0; i < 4; i++) {  // 四个方向相邻格子位置
            nextRow = curr.row + dRows[i];
            nextCol = curr.col + dCols[i];
            // 若(nr, nc)没越界且没被访问过
            if (nextRow >= 0 && nextRow < maze->rows && 
                nextCol >= 0 && nextCol < maze->cols) {
                    if (!maze->grid[nextRow][nextCol].visited) {
                        unvisited[count++] = i;  // 记录方向编号
                }
            }
        }

        /* 2. 分支判断 */
        if (!count) {
            Position tmp_pos;
            pop(maze->genStack, &tmp_pos);  // 死胡同走不通，丢掉这个位置并回溯
            continue;
        }
        
        /* 3. 随机选一个未访问过的邻居 Fisher-Yates 洗牌算法 */
        for (int i = count - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int tmp = unvisited[j];
            unvisited[j] = unvisited[i];
            unvisited[i] = tmp;
        }
        int chosen = unvisited[0];
        nextRow = curr.row + dRows[chosen];
        nextCol = curr.col + dCols[chosen];
        
        /* 
         * 4. 拆墙（格子的上下左右墙，两格各改一次）
         * case 0: 此格的上边界，相邻的下边界; case 1: 此格的下边界，相邻的上边界;
         * case 2: 此格的左边界，相邻的右边界; case 3: 此格的右边界，相邻的左边界;
         */
        switch (chosen) {
            case 0: {
                maze->grid[curr.row][curr.col].top = false;
                maze->grid[nextRow][nextCol].bottom = false;
            } break;

            case 1: {
                maze->grid[curr.row][curr.col].bottom = false;
                maze->grid[nextRow][nextCol].top = false;
            } break;

            case 2: {
                maze->grid[curr.row][curr.col].left = false;
                maze->grid[nextRow][nextCol].right = false;
            } break;

            case 3: {
                maze->grid[curr.row][curr.col].right = false;
                maze->grid[nextRow][nextCol].left = false;
            } break;
        
            default:
                break;
        }

        /* 5. 进入邻居 */
        maze->grid[nextRow][nextCol].visited = true;
        Position pos_push;
        pos_push.row = nextRow;
        pos_push.col = nextCol;
        push(stack, pos_push);
    }
}

/* 逐帧动画生成迷宫 可视化生成 */
bool stepGenerate(Maze *maze) {
    int dRows[] = {-1, 1, 0, 0};
    int dCols[] = {0, 0, -1, 1};
    // 栈空 生成完毕
    if (isStackEmpty(maze->genStack)) {
        return true;
    }

    Position curr = peek(maze->genStack);

    // 原 generate 函数的 while 循环变为单次迭代 再利用动画帧栈即可实现可视化生成
    /* 1. 收集未访问过的邻居 */
    int unvisited[4] = {0, 1, 2, 3}; // 0上1下2左3右
    int count = 0;
    int nextRow, nextCol;

    for (int i = 0; i < 4; i++) { 
        nextRow = curr.row + dRows[i];
        nextCol = curr.col + dCols[i];
        // 若(nr, nc)没越界且没被访问过
        if (nextRow >= 0 && nextRow < maze->rows &&
            nextCol >= 0 && nextCol < maze->cols) {
            if (!maze->grid[nextRow][nextCol].visited) {
                unvisited[count++] = i; // 记录方向编号
            }
        }
    }

    if (!count) {
        Position tmp_pos;
        pop(maze->genStack, &tmp_pos); // 死胡同走不通，丢掉这个位置并回溯
        return false; // 还未完成 返回 false 
    }

    /* 3. 随机选一个未访问过的邻居 Fisher-Yates 洗牌算法 */
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = unvisited[j];
        unvisited[j] = unvisited[i];
        unvisited[i] = tmp;
    }
    int chosen = unvisited[0];
    nextRow = curr.row + dRows[chosen];
    nextCol = curr.col + dCols[chosen];

    /* 4. 拆墙（格子的上下左右墙，两格各改一次） */
    switch (chosen) {
        case 0: {
            maze->grid[curr.row][curr.col].top = false;
            maze->grid[nextRow][nextCol].bottom = false;
        } break;

        case 1: {
            maze->grid[curr.row][curr.col].bottom = false;
            maze->grid[nextRow][nextCol].top = false;
        } break;

        case 2: {
            maze->grid[curr.row][curr.col].left = false;
            maze->grid[nextRow][nextCol].right = false;
        } break;

        case 3: {
            maze->grid[curr.row][curr.col].right = false;
            maze->grid[nextRow][nextCol].left = false;
        } break;

        default:
            break;
    }

    /* 5. 进入邻居 */
    maze->grid[nextRow][nextCol].visited = true;
    Position pos_push;
    pos_push.row = nextRow;
    pos_push.col = nextCol;
    push(maze->genStack, pos_push);

    // 还未完成 返回 false
    return false;
}