#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

//------------------------------Const-----------------------------------

#define HEIGHT 22
#define WIDTH 13

#define BOX_LENGTH 4

//旋转相关
#define ROTATE_UP 0
#define ROTATE_RIGHT 1
#define ROTATE_DOWN 2
#define ROTATE_LEFT 3

//地图相关
#define MAP_FREE 0
#define MAP_WAIL 1
#define MAP_BOX 2

//格子相关
#define BOX_HOLLOW 0
#define BOX_SOLID 1

//-------------------------globalVariable-------------------------------

HANDLE g_handle;//句柄

//旋转次数
int g_rotate_times = 0; 
int g_box_x = 0;
int g_box_y = WIDTH / 2 - 1;
int g_box_type = 0;
int g_score = 0;
int g_max_height = HEIGHT;

//全局地图
char g_map[HEIGHT][WIDTH] = { MAP_FREE };

//随机格子
char g_box[][BOX_LENGTH] = {
    //土
    BOX_HOLLOW,BOX_SOLID,BOX_HOLLOW,BOX_HOLLOW,
    BOX_SOLID,BOX_SOLID,BOX_SOLID,BOX_HOLLOW,
    BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,
    BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,
    //田
    BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,
    BOX_HOLLOW,BOX_SOLID,BOX_SOLID,BOX_HOLLOW,
    BOX_HOLLOW,BOX_SOLID,BOX_SOLID,BOX_HOLLOW,
    BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,
    //一
    BOX_SOLID,BOX_SOLID,BOX_SOLID,BOX_SOLID,
    BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,
    BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,
    BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,
    //Z
    BOX_SOLID,BOX_SOLID,BOX_HOLLOW,BOX_HOLLOW,
    BOX_HOLLOW,BOX_SOLID,BOX_SOLID,BOX_HOLLOW,
    BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,
    BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,
};

//当前下落格子
char g_cur_box[BOX_LENGTH][BOX_LENGTH] = { BOX_HOLLOW };
//下一个下落格子
char g_next_box[BOX_LENGTH][BOX_LENGTH] = { BOX_HOLLOW };

//------------------------------Function-----------------------------------

//获取格子对应的下标
int getBoxIndex(int i, int j);
//旋转方块
void rotateToBox(char *srcBox, char *destBox, int times);
//初始化地图
void initMap();
// 获取地图内容
char* getMapValue(int i, int j);
//画地图
void drawMap();
//清理格子
void clearBox();
//画格子
void drawBox();
//设置光标不显示
void showCursorVisible(bool flag);
//画字符
void writeChar(short x, short y, const char* pStr);

/**********************Logic***************************/

// 移动控制
void moveControl();
//固定到地图
void fixedToMap();
//消行
void elimination();
//随机格子
void randomBox();
//检测碰撞
int checkBump(int x, int y);
// 向左移动
void moveLeft();
// 向右移动
void moveRight();
// 向下移动
void moveDown();
//向上 旋转
void moveTurn();
//游戏开始前初始化工作
void gameInit();
//开始游戏
int startGame();

//--------------------------------------------------------------------------

//设置光标不显示
void showCursorVisible(bool flag) {
    CONSOLE_CURSOR_INFO cci = { 0 };
    cci.dwSize = 1;
    cci.bVisible = flag;//开关
    SetConsoleCursorInfo(g_handle, &cci);
}

//画字符 x y 与二维数组逻辑 i j相反
void writeChar(short x, short y, const char* pStr)
{
    //showCursorVisible(false);//不可显示
    SetConsoleCursorPosition(g_handle, { x * 2,y });
    printf("%s", pStr);
}

//获取格子对应的下标
int getBoxIndex(int i, int j)
{
    return i * BOX_LENGTH + j;
}

//旋转方块
void rotateToBox(char *srcBox,char *destBox,int times)
{
    int remainder = times % 4;
    if (ROTATE_UP == remainder) 
    {
        for (int i = 0; i < BOX_LENGTH; ++i)
        {
            for (int j = 0; j < BOX_LENGTH; ++j)
            {
                destBox[getBoxIndex(i,j)] = srcBox[getBoxIndex(i,j)];
            }
        }
    }
    else if (ROTATE_RIGHT == remainder)
    {
        for (int i = 0; i < BOX_LENGTH; ++i)
        {
            for (int j = BOX_LENGTH - 1; j >= 0; --j)
            {
                destBox[getBoxIndex(i, BOX_LENGTH - 1 - j)] = srcBox[getBoxIndex(j,i)];
            }
        }
    }
    else if (ROTATE_DOWN == remainder)
    {
        for (int i = BOX_LENGTH - 1; i >= 0; --i)
        {
            for (int j = BOX_LENGTH - 1; j >= 0; --j)
            {
                destBox[getBoxIndex(BOX_LENGTH - 1 - i, BOX_LENGTH - 1 - j)] = srcBox[getBoxIndex(i,j)];
            }
        }

    }
    else if (ROTATE_LEFT == remainder)
    {
        for (int i = BOX_LENGTH - 1; i >= 0; --i)
        {
            for (int j = 0; j < BOX_LENGTH; ++j)
            {
                destBox[getBoxIndex(BOX_LENGTH - 1 - i, j)] = srcBox[getBoxIndex(j, i)];
            }
        }
    }
}

// 获取地图内容
char* getMapValue(int i, int j)
{
    if (g_map[i][j] == MAP_FREE)
    {
        return "□";
    }
    else if (g_map[i][j] == MAP_BOX)
    {
        return "■";
    }
    else
    {
        return "★";
    }
}

//画地图
void drawMap()
{
    for (int i = 0; i < HEIGHT; ++i)
    {
        for (int j = 0; j < WIDTH; ++j)
        {
            writeChar(j, i, getMapValue(i,j));
        }
        printf("\r\n");
    }
}

//清理格子
void clearBox()
{
    for (int i = 0; i < BOX_LENGTH; ++i)
    {
        for (int j = 0; j < BOX_LENGTH; ++j)
        {
            if (g_cur_box[i][j] == BOX_SOLID)
            {
                writeChar(j + g_box_y, i + g_box_x, "□");
            }
        }
    }
}

//画格子
void drawBox()
{
    for (int i = 0; i < BOX_LENGTH; ++i)
    {
        for (int j = 0; j < BOX_LENGTH; ++j)
        {
            if (g_cur_box[i][j] == BOX_SOLID)
            {
                writeChar(j + g_box_y, i + g_box_x, "■");
            }
        }
    }
}

//初始化地图
void initMap()
{
    for (int i = 0; i < HEIGHT; ++i)
    {
        for (int j = 0; j < WIDTH; ++j)
        {
            if (j == 0 || j == WIDTH - 1 || i == HEIGHT - 1)
            {
                g_map[i][j] = MAP_WAIL;
            }
        }
    }
}


/**********************Logic***************************/

//随机格子
void randomBox()
{
    int totalType = sizeof(g_box) / BOX_LENGTH / BOX_LENGTH;
    g_box_type = rand() % totalType;
    g_rotate_times = rand() % 4;
    rotateToBox((char*)g_box+ g_box_type*BOX_LENGTH*BOX_LENGTH,
                                    (char*)g_cur_box, g_rotate_times);
    g_box_x = 0;
    g_box_y = WIDTH / 2 - 1;

    drawBox();
}

// 消行
void elimination()
{
    int times = 0;
    int mapX = g_box_x;
    while (times < BOX_LENGTH) {
        int flag = 1;
        for (int j = 1; j < WIDTH - 1; ++j) {
            if (g_map[mapX+ times][j] != MAP_BOX) {
                flag = 0;
                break;
            }
        }
        if (flag) //能消行
        {
            for (int i = mapX + times; i >= g_max_height; --i)
            {
                for (int j = 1; j < WIDTH - 1; ++j) {
                    if (i != g_max_height)
                    {
                        g_map[i][j] = g_map[i-1][j];
                    }
                    else
                    {
                        g_map[i][j] = MAP_FREE;
                    }
                    writeChar(j, i, getMapValue(i, j));
                }
            }
            --g_max_height;
        }
        times++;
    }
}

//固定到地图
void fixedToMap()
{
    for (int i = 0; i < BOX_LENGTH; ++i)
    {
        for (int j = 0; j < BOX_LENGTH; ++j)
        {
            if (g_cur_box[i][j] == BOX_SOLID)
            {
                g_map[g_box_x + i][g_box_y + j] = MAP_BOX;
            }
        }
    }
    g_max_height = min(g_max_height, g_box_x);
}

//检测碰撞
int checkBump(int x,int y)
{
    for (int i = 0; i < BOX_LENGTH; ++i)
    {
        for (int j = 0; j < BOX_LENGTH; ++j)
        {
            if (g_cur_box[i][j] == BOX_SOLID &&
                    g_map[x + i][y + j] != MAP_FREE)
            {
                return 1;
            }
        }
    }
    return 0;
}

// 向左移动
void moveLeft()
{
    if (!checkBump(g_box_x, g_box_y - 1))
    {
        clearBox();
        g_box_y--;
        drawBox();
    }
}

// 向右移动
void moveRight()
{
    if (!checkBump(g_box_x, g_box_y + 1))
    {
        clearBox();
        g_box_y++;
        drawBox();
    }
}

// 向下移动
void moveDown()
{
    if (!checkBump(g_box_x+1, g_box_y))
    {
        clearBox();
        g_box_x++;
        drawBox();
    }
    else
    {
        fixedToMap();
        elimination();
        randomBox();
    }
}

//向上 旋转
void moveTurn()
{
    clearBox();
    rotateToBox((char*)g_box + g_box_type*BOX_LENGTH*BOX_LENGTH,
        (char*)g_cur_box, g_rotate_times+1);
    if (!checkBump(g_box_x , g_box_y))
    {
        g_rotate_times++;
        drawBox();
    }
    else
    {
        //还原
        rotateToBox((char*)g_box + g_box_type*BOX_LENGTH*BOX_LENGTH,
            (char*)g_cur_box, g_rotate_times);
        drawBox();
    }
}

// 移动控制
void moveControl()
{
    while (1)
    {
        if (_kbhit())
        {
            char ch = _getch();
            switch (ch)
            {
                case 'W':
                case 'w':
                    moveTurn();
                    break;
                case 'S':
                case 's':
                    moveDown();
                    break;
                case 'A':
                case 'a':
                    moveLeft();
                    break;
                case 'D':
                case 'd':
                    moveRight();
                    break;
                case 'Q':
                case 'q':
                    return;
                default :
                    break;
            }
        }
        Sleep(100);
        //moveDown();
    }
}

//开始游戏
int startGame()
{
    system("cls");
    drawMap();
    randomBox();
    moveControl();
    
    return 1;
}

//游戏开始前初始化工作
void gameInit()
{
    g_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    srand((unsigned)time(NULL));
    initMap();
}


int main(int argc, char *argv)
{  
    gameInit();
    startGame();
    system("pause");
    return 0;
}