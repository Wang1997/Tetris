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

//��ת���
#define ROTATE_UP 0
#define ROTATE_RIGHT 1
#define ROTATE_DOWN 2
#define ROTATE_LEFT 3

//��ͼ���
#define MAP_FREE 0
#define MAP_WAIL 1
#define MAP_BOX 2

//�������
#define BOX_HOLLOW 0
#define BOX_SOLID 1

//�������
#define SCORE_COMMON 10
#define SCORE_DIFF 20

#define KEY_DOWN(key) GetAsyncKeyState(key)==(SHORT)0x8001?1:0

//-------------------------globalVariable-------------------------------

HANDLE g_handle;//���

//��ת����
int g_rotate_times = 0; 
int g_next_rotate_times = 0;
int g_box_x = 0;
int g_box_y = WIDTH / 2 - 1;
int g_box_type = 0;
int g_next_box_type = 0;
int g_score = 0;
int g_max_height = HEIGHT;
int g_diffFlag = 0;

//ȫ�ֵ�ͼ
char g_map[HEIGHT][WIDTH] = { MAP_FREE };

//�������
char g_box[][BOX_LENGTH] = {
    //��
    BOX_HOLLOW,BOX_SOLID,BOX_HOLLOW,BOX_HOLLOW,
    BOX_SOLID,BOX_SOLID,BOX_SOLID,BOX_HOLLOW,
    BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,
    BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,
    //��
    BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,
    BOX_HOLLOW,BOX_SOLID,BOX_SOLID,BOX_HOLLOW,
    BOX_HOLLOW,BOX_SOLID,BOX_SOLID,BOX_HOLLOW,
    BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,BOX_HOLLOW,
    //һ
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

//��ǰ�������
char g_cur_box[BOX_LENGTH][BOX_LENGTH] = { BOX_HOLLOW };
//��һ���������
char g_next_box[BOX_LENGTH][BOX_LENGTH] = { BOX_HOLLOW };

//------------------------------Function-----------------------------------

//��ȡ���Ӷ�Ӧ���±�
int getBoxIndex(int i, int j);
//��ת����
void rotateToBox(char *srcBox, char *destBox, int times);
//��ʼ����ͼ
void initMap();
// ��ȡ��ͼ����
char* getMapValue(int i, int j);
//����Ϣ��
void drawInfo();
//���»���
void updateScore();
//����ͼ
void drawMap();
//�������
void clearBox();
//������
void drawBox();
//����һ������
void drawNextBox();
//���ù�겻��ʾ
void showCursorVisible(bool flag);
//���ַ�
void writeChar(short x, short y, const char* pStr);

/**********************Logic***************************/

// �ƶ�����
void moveControl();
//�̶�����ͼ
void fixedToMap();
//����
int elimination();
//�������
void randomBox();
//�����ײ
int checkBump(int x, int y);
// �����ƶ�
void moveLeft();
// �����ƶ�
void moveRight();
// �����ƶ�
void moveDown();
//���� ��ת
void moveTurn();
//��Ϸ��ʼǰ��ʼ������
void gameInit();
//��ʼ��Ϸ
int startGame();
//������Ϸ
void endGame();
//�˵�
void menu();

//--------------------------------------------------------------------------

//���ù�겻��ʾ
void showCursorVisible(bool flag) {
    CONSOLE_CURSOR_INFO cci = { 0 };
    cci.dwSize = 1;
    cci.bVisible = flag;//����
    SetConsoleCursorInfo(g_handle, &cci);
}

//���ַ� x y ���ά�����߼� i j�෴
void writeChar(short x, short y, const char* pStr)
{
    showCursorVisible(false);//������ʾ
    SetConsoleCursorPosition(g_handle, { x * 2,y });
    printf("%s", pStr);
}

//��ȡ���Ӷ�Ӧ���±�
int getBoxIndex(int i, int j)
{
    return i * BOX_LENGTH + j;
}

//��ת����
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

// ��ȡ��ͼ����
char* getMapValue(int i, int j)
{
    if (g_map[i][j] == MAP_FREE)
    {
        return "��";
    }
    else if (g_map[i][j] == MAP_BOX)
    {
        return "��";
    }
    else
    {
        return "��";
    }
}

//����Ϣ��
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
    writeChar(WIDTH + INFO_WIDTH/6, HEIGHT / 3 - 1, "���֣�0");
    writeChar(WIDTH + INFO_WIDTH/6, HEIGHT/2 - 1, "˵����");
    writeChar(WIDTH + INFO_WIDTH/3, HEIGHT/2, "����:��ת");
    writeChar(WIDTH + INFO_WIDTH/3, HEIGHT / 2 + 2, "����:����");
    writeChar(WIDTH + INFO_WIDTH/3, HEIGHT / 2 + 4, "����:����");
    writeChar(WIDTH + INFO_WIDTH/3, HEIGHT / 2 + 6, "����:����");

    
}

//���»���
void updateScore()
{
    char str[10] = "���֣�0";
    sprintf(str+6,"%d",g_score);
    writeChar(WIDTH + INFO_WIDTH / 6, HEIGHT / 3 - 1, str);
}

//����ͼ
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

//�������
void clearBox()
{
    for (int i = 0; i < BOX_LENGTH; ++i)
    {
        for (int j = 0; j < BOX_LENGTH; ++j)
        {
            if (g_cur_box[i][j] == BOX_SOLID)
            {
                writeChar(j + g_box_y, i + g_box_x, "��");
            }
        }
    }
}

//������
void drawBox()
{
    for (int i = 0; i < BOX_LENGTH; ++i)
    {
        for (int j = 0; j < BOX_LENGTH; ++j)
        {
            if (g_cur_box[i][j] == BOX_SOLID)
            {
                writeChar(j + g_box_y, i + g_box_x, "��");
            }
        }
    }
}

// ����һ������
void drawNextBox()
{
    writeChar(WIDTH, 1, "��һ��:");
    for (int i = 0; i < BOX_LENGTH; ++i)
    {
        for (int j = 0; j < BOX_LENGTH; ++j)
        {
            if (g_next_box[i][j] == BOX_SOLID)
            {
                writeChar(j + WIDTH + 2, i + 2, "��");
            }
            else
            {
                writeChar(j + WIDTH + 2, i + 2, "  ");
            }
        }
    }
}

//��ʼ����ͼ
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

//�������
void randomBox()
{
    //����һ����Ϊ��ǰ����
    g_box_type = g_next_box_type;
    g_rotate_times = g_next_rotate_times;
    
    rotateToBox((char*)g_box+ g_box_type*BOX_LENGTH*BOX_LENGTH,
                                    (char*)g_cur_box, g_rotate_times);
    g_box_x = 0;
    g_box_y = WIDTH / 2 - 1;

    //����������һ��
    int totalType = sizeof(g_box) / BOX_LENGTH / BOX_LENGTH;
    g_next_box_type = rand() % totalType;
    g_next_rotate_times = rand() % 4;
    rotateToBox((char*)g_box + g_next_box_type*BOX_LENGTH*BOX_LENGTH,
        (char*)g_next_box, g_next_rotate_times);

    drawBox();
    drawNextBox();
}

// ����
int elimination()
{
    int times = 0;
    int mapX = g_box_x;
    int counts = 0; //��������
    while (times < BOX_LENGTH) {
        int flag = 1;
        for (int j = 1; j < WIDTH - 1; ++j) {
            if (g_map[mapX+ times][j] != MAP_BOX) {
                flag = 0;
                break;
            }
        }
        if (flag) //������
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

//�̶�����ͼ
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

//�����ײ
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

// �����ƶ�
void moveLeft()
{
    if (!checkBump(g_box_x, g_box_y - 1))
    {
        clearBox();
        g_box_y--;
        drawBox();
    }
}

// �����ƶ�
void moveRight()
{
    if (!checkBump(g_box_x, g_box_y + 1))
    {
        clearBox();
        g_box_y++;
        drawBox();
    }
}

// �����ƶ�
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
        if (g_diffFlag && counts) //����
        {
            g_score += counts * SCORE_DIFF;
        }
        else if (counts) //��ͨ
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

//���� ��ת
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
        //��ԭ
        rotateToBox((char*)g_box + g_box_type*BOX_LENGTH*BOX_LENGTH,
            (char*)g_cur_box, g_rotate_times);
        drawBox();
    }
}

// �ƶ�����
void moveControl()
{
    while (1)
    {
        if (KEY_DOWN(0x26)) 
        { //��
            moveTurn();
        }
        else if (KEY_DOWN(0x28))
        { //��
            moveDown();
            if(g_diffFlag)
                moveDown();
        }
        else if (KEY_DOWN(0x25))
        { //��
            moveLeft();
        }
        else if (KEY_DOWN(0x27))
        { //��
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

//��ʼ��Ϸ
int startGame()
{
    menu();
    drawMap();
    drawInfo();
    randomBox();
    moveControl();
    
    return 1;
}

//������Ϸ
void endGame()
{
    writeChar(0, HEIGHT + 1, "��Ϸ����\r\n");
}

//��Ϸ��ʼǰ��ʼ������
void gameInit()
{
    g_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    srand((unsigned)time(NULL));

    keybd_event(VK_SHIFT, 0, 0, 0);
    //Sleep(100);
    keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);

    // ��ʼ����һ������
    int totalType = sizeof(g_box) / BOX_LENGTH / BOX_LENGTH;
    g_next_box_type = rand() % totalType;
    g_next_rotate_times = rand() % 4;

    initMap();
}

//�˵�
void menu()
{
    while (1)
    {
        system("cls");
        printf("0.��ģʽ\r\n");
        printf("1.����ģʽ\r\n");
        printf("��ѡ��ģʽ:");
        scanf("%d",&g_diffFlag);
        if (g_diffFlag != 1 && g_diffFlag != 0) {
            printf("�Բ���,��ѡ��0����1\r\n");
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