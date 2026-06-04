# raylib 函数速查手册 5.5

## 窗口与生命周期

```c
void InitWindow(int width, int height, const char *title);   // 创建窗口
bool WindowShouldClose(void);                                  // 点了 X 返回 true
void CloseWindow(void);                                        // 关闭窗口释放资源
void SetTargetFPS(int fps);                                    // 锁帧率，0=不限
```

**模式**：`InitWindow` → `while(!WindowShouldClose()) { ... }` → `CloseWindow()`

---

## 鼠标

```c
bool IsMouseButtonPressed(int button);    // 点下瞬间触发一次
bool IsMouseButtonDown(int button);       // 按住时每帧触发
bool IsMouseButtonReleased(int button);   // 松开瞬间
Vector2 GetMousePosition(void);           // 返回 {x, y}
int GetMouseX(void);                      // 只取 x
int GetMouseY(void);                      // 只取 y
```

按钮常量：`MOUSE_LEFT_BUTTON` `MOUSE_RIGHT_BUTTON` `MOUSE_MIDDLE_BUTTON`

**Pressed vs Down**：点按钮切页面用 `Pressed`（一次），游戏移动用 `Down`（持续）。

---

## 键盘

```c
bool IsKeyPressed(int key);     // 按下瞬间
bool IsKeyDown(int key);        // 按住时每帧
bool IsKeyReleased(int key);    // 松开瞬间
int GetKeyPressed(void);        // 返回刚按下的键码
```

常用键名：`KEY_UP` `KEY_DOWN` `KEY_LEFT` `KEY_RIGHT` `KEY_W` `KEY_A` `KEY_S` `KEY_D` `KEY_R` `KEY_M` `KEY_P` `KEY_H` `KEY_SPACE` `KEY_ESCAPE` `KEY_ENTER`

---

## 绘图基础

### 每帧结构

```c
BeginDrawing();
    // 所有绘图函数
EndDrawing();
```

`BeginDrawing` 设画布，`EndDrawing` 提交到屏幕。不在二者之间的绘图调用无效。

---

## 矩形

```c
void DrawRectangle(int x, int y, int w, int h, Color c);           // 填充矩形
void DrawRectangleRec(Rectangle rec, Color c);                     // 用 Rectangle 结构体
void DrawRectangleLines(int x, int y, int w, int h, Color c);      // 空心矩形边框
void DrawRectangleLinesEx(Rectangle rec, float thick, Color c);    // 可调粗线空心
```

`Rectangle` = `{ float x; float y; float width; float height; }`

---

## 直线

```c
void DrawLine(int startX, int startY, int endX, int endY, Color c);              // 细线（1px）
void DrawLineEx(Vector2 start, Vector2 end, float thick, Color c);               // 可调粗线
void DrawLineBezier(Vector2 start, Vector2 end, float thick, Color c);           // 贝塞尔曲线
```

`Vector2` = `{ float x; float y; }`。迷宫墙用 `DrawLine`，路径线用 `DrawLineEx` 加粗。

---

## 圆

```c
void DrawCircle(int centerX, int centerY, float radius, Color c);
void DrawCircleV(Vector2 center, float radius, Color c);
void DrawCircleLines(int cx, int cy, float r, Color c);   // 空心圆
```

---

## 文字

```c
void DrawText(const char *text, int x, int y, int fontSize, Color c);
void DrawTextEx(Font font, const char *text, Vector2 pos, float size, float spacing, Color c);
```

**区别**：`DrawText` 用内置默认字体，`DrawTextEx` 可传自定义 `Font`。英文用前者够用。

---

## 文字格式化（raylib 内置）

```c
const char *TextFormat(const char *fmt, ...);   // 类似 sprintf，返回静态字符串
```

```c
// 帧数+状态合一行
DrawText(TextFormat("Frame: %d State: %d", frame, state), 10, 10, 20, GREEN);
```

`TextFormat` 返回的是内部静态缓冲区，多次调用会覆盖。赋给 `const char *` 只用一次没问题，不要存指针跨帧用。

---

## 颜色

**内置常量**：`BLACK` `WHITE` `RED` `GREEN` `BLUE` `YELLOW` `GOLD` `ORANGE` `PURPLE` `PINK` `BROWN` `LIGHTGRAY` `GRAY` `DARKGRAY` `DARKBLUE` `DARKGREEN` `DARKBROWN` `BEIGE` `SKYBLUE` `MAGENTA` `MAROON` `LIME`

**自定义**（RGBA，每通道 0-255）：

```c
Color c = { R, G, B, A };
// 半透明蓝
Color transBlue = { 0, 0, 255, 128 };
// 深色背景
Color bg = { 15, 20, 35, 255 };
```

---

## 碰撞检测（鼠标悬停）

```c
bool CheckCollisionPointRec(Vector2 point, Rectangle rec);    // 点是否在矩形内
bool CheckCollisionRecs(Rectangle a, Rectangle b);            // 矩形碰撞
bool CheckCollisionPointCircle(Vector2 p, Vector2 c, float r); // 点在圆内
```

---

## 输入与事件

```c
float GetFrameTime(void);      // 上一帧耗时（秒），做帧无关动画用
int GetFPS(void);              // 当前 FPS

void SetExitKey(int key);      // 设置退出键，0=禁用，默认 ESC
```

---

## 随机数（raylib 内置）

```c
void SetRandomSeed(unsigned int seed);
int GetRandomValue(int min, int max);   // [min, max] 闭区间
```

等价 `rand()` 但不用 `<stdlib.h>`。种子仍建议在 `main` 开头调 `srand(time(NULL))`。

---

## 计时器

```c
double GetTime(void);           // 从 InitWindow 开始的秒数
```

做速度控制时可替代帧计数器。

---

## 图像/纹理（进阶）

```c
Texture2D LoadTexture(const char *fileName);
void UnloadTexture(Texture2D tex);
void DrawTexture(Texture2D tex, int x, int y, Color tint);
void DrawTextureEx(Texture2D tex, Vector2 pos, float rot, float scale, Color tint);
```

---

## 字体（自定义 TTF）

```c
Font LoadFont(const char *fileName);
Font LoadFontEx(const char *fileName, int fontSize, int *codepoints, int count);
void UnloadFont(Font font);
int *LoadCodepoints(const char *text, int *count);     // UTF-8 串→Unicode 码点数组
void UnloadCodepoints(int *codepoints);
```

**中文用 `LoadFontEx` + `LoadCodepoints`**，传 text 的码点只加载用到的字。

---

## 日志

```c
void TraceLog(int level, const char *fmt, ...);
```

级别：`LOG_INFO` `LOG_WARNING` `LOG_ERROR` `LOG_DEBUG`

---

## 录音/音频（MiniAudio 后端）

```c
Sound LoadSound(const char *fileName);
void PlaySound(Sound s);
void UnloadSound(Sound s);
Music LoadMusicStream(const char *fileName);
void PlayMusicStream(Music m);
void UpdateMusicStream(Music m);
void UnloadMusicStream(Music m);
```

---

## 常用组合模式

### 按钮（自定义）

```c
Rectangle btn = { x, y, w, h };
bool hover = CheckCollisionPointRec(GetMousePosition(), btn);
Color c = hover ? LIGHTGRAY : GRAY;
DrawRectangleRec(btn, c);
DrawText("Click Me", btn.x + 10, btn.y + 10, 20, BLACK);

if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    // 点击逻辑
}
```

### 居中元素

水平居中：`(屏幕宽 - 内容宽) / 2`  
垂直居中：`(屏幕高 - 内容高) / 2`

### 帧计数器控制速度

```c
static int frame = 0;
frame++;
if (frame % skipFrames == 0) {
    // 做一步
}
```

### 状态机驱动界面

```c
switch (state) {
    case MENU: drawMenu(); break;
    case GAME: drawGame(); break;
    case WIN: drawWinScreen(); break;
}
```
