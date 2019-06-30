#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

//------------------------------Define-----------------------------------

#define HEIGHT 24
#define WIDTH 13
#define INFO_WIDTH 8

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

//积分相关
#define SCORE_COMMON 10
#define SCORE_DIFF 20

#define KEY_DOWN(key) GetAsyncKeyState(key)==(SHORT)0x8001?1:0

//-------------------------globalVariable-------------------------------

HANDLE g_handle;//句柄

//旋转次数
int g_rotate_times = 0; 
int g_next_rotate_times = 0;
int g_box_x = 0;
int g_box_y = WIDTH / 2 - 1;
int g_box_type = 0;
int g_next_box_type = 0;
int g_score = 0;
int g_max_height = HEIGHT;
int g_diffFlag = 0;

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
//画信息框
void drawInfo();
//更新积分
void updateScore();
//画地图
void drawMap();
//清理格子
void clearBox();
//画格子
void drawBox();
//画下一个格子
void drawNextBox();
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
int elimination();
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
//结束游戏
void endGame();
//菜单
void menu();

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
    showCursorVisible(false);//不可显示
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

//画信息框
void drawInfo()
{
    for (int i = 0; i < HEIGHT; ++i)
    {
        for (int j = WIDTH; j <= WIDTH + INFO_WIDTH; ++j)
        {
            if(i == 0 || i == HEIGHT-1 )
                writeChar(j, i, "- ");
            else if(j == (WIDTH + INFO_WIDTH))
                writeChar(j, i, "|");
        }
    }
    writeChar(WIDTH + INFO_WIDTH/6, HEIGHT / 3 - 1, "积分：0");
    writeChar(WIDTH + INFO_WIDTH/6, HEIGHT/2 - 1, "说明：");
    writeChar(WIDTH + INFO_WIDTH/3, HEIGHT/2, "↑键:旋转");
    writeChar(WIDTH + INFO_WIDTH/3, HEIGHT / 2 + 2, "↓键:加速");
    writeChar(WIDTH + INFO_WIDTH/3, HEIGHT / 2 + 4, "←键:左移");
    writeChar(WIDTH + INFO_WIDTH/3, HEIGHT / 2 + 6, "→键:右移");

    
}

//更新积分
void updateScore()
{
    char str[10] = "积分：0";
    sprintf(str+6,"%d",g_score);
    writeChar(WIDTH + INFO_WIDTH / 6, HEIGHT / 3 - 1, str);
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

// 画下一个格子
void drawNextBox()
{
    writeChar(WIDTH, 1, "下一个:");
    for (int i = 0; i < BOX_LENGTH; ++i)
    {
        for (int j = 0; j < BOX_LENGTH; ++j)
        {
            if (g_next_box[i][j] == BOX_SOLID)
            {
                writeChar(j + WIDTH + 2, i + 2, "■");
            }
            else
            {
                writeChar(j + WIDTH + 2, i + 2, "  ");
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
    //将下一个作为当前格子
    g_box_type = g_next_box_type;
    g_rotate_times = g_next_rotate_times;
    
    rotateToBox((char*)g_box+ g_box_type*BOX_LENGTH*BOX_LENGTH,
                                    (char*)g_cur_box, g_rotate_times);
    g_box_x = 0;
    g_box_y = WIDTH / 2 - 1;

    //重新生成下一个
    int totalType = sizeof(g_box) / BOX_LENGTH / BOX_LENGTH;
    g_next_box_type = rand() % totalType;
    g_next_rotate_times = rand() % 4;
    rotateToBox((char*)g_box + g_next_box_type*BOX_LENGTH*BOX_LENGTH,
        (char*)g_next_box, g_next_rotate_times);

    drawBox();
    drawNextBox();
}

// 消行
int elimination()
{
    int times = 0;
    int mapX = g_box_x;
    int counts = 0; //消行行数
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
            ++counts;
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
    return counts;
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
        int counts = elimination();
        if (g_diffFlag && counts) //困难
        {
            g_score += counts * SCORE_DIFF;
        }
        else if (counts) //普通
        {
            g_score += counts * SCORE_COMMON;
        }
        if (counts)
        {
            updateScore();
        }
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
        if (KEY_DOWN(0x26)) 
        { //上
            moveTurn();
        }
        else if (KEY_DOWN(0x28))
        { //下
            moveDown();
            if(g_diffFlag)
                moveDown();
        }
        else if (KEY_DOWN(0x25))
        { //左
            moveLeft();
        }
        else if (KEY_DOWN(0x27))
        { //右
            moveRight();
        }
        else if (KEY_DOWN('Q'))
        { //Q
            return;
        }
        else 
        {
            if (g_diffFlag)
                moveDown();
        }
        if(g_max_height <= 0)
            break;
        if(g_diffFlag)
            Sleep(125);
    }
}

//开始游戏
int startGame()
{
    menu();
    drawMap();
    drawInfo();
    randomBox();
    moveControl();
    
    return 1;
}

//结束游戏
void endGame()
{
    writeChar(0, HEIGHT + 1, "游戏结束\r\n");
}

//游戏开始前初始化工作
void gameInit()
{
    g_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    srand((unsigned)time(NULL));

    keybd_event(VK_SHIFT, 0, 0, 0);
    //Sleep(100);
    keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);

    // 初始化下一个格子
    int totalType = sizeof(g_box) / BOX_LENGTH / BOX_LENGTH;
    g_next_box_type = rand() % totalType;
    g_next_rotate_times = rand() % 4;

    initMap();
}

//菜单
void menu()
{
    while (1)
    {
        system("cls");
        printf("0.简单模式\r\n");
        printf("1.困难模式\r\n");
        printf("请选择模式:");
        scanf("%d",&g_diffFlag);
        if (g_diffFlag != 1 && g_diffFlag != 0) {
            printf("对不起,请选择0或者1\r\n");
            system("pause");
        }
        else
        {
            break;
        }
    }
    system("cls");
}

int main(int argc, char *argv)
{  
    gameInit();
    startGame();
    endGame();
    system("pause");
    return 0;
}