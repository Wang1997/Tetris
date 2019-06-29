#include <stdio.h>
#include <stdlib.h>

//------------------------------Const-----------------------------------

#define HEIGHT 22
#define WIDTH 13

#define BOX_LENGTH 4

#define ROTATE_UP 0
#define ROTATE_RIGHT 1
#define ROTATE_DOWN 2
#define ROTATE_LEFT 3

#define MAP_FREE 0
#define MAP_WAIL 1
#define MAP_BOX 2

//-------------------------globalVariable-------------------------------

//��ת����
int g_rotate_times = 0; 
int g_box_height = 0;
int g_box_width = WIDTH / 2;
int g_box_type = 0;
int g_score = 0;

//ȫ�ֵ�ͼ
char g_map[HEIGHT][WIDTH] = { MAP_FREE };

//�������
char g_box[][BOX_LENGTH] = {
    0,1,0,0,
    1,1,1,0,
    0,0,0,0,
    0,0,0,0
};

//��ǰ�������
char g_cur_box[][BOX_LENGTH] = { 0 };
//��һ���������
char g_next_box[][BOX_LENGTH] = { 0 };

//------------------------------Function-----------------------------------

//��ȡ���Ӷ�Ӧ���±�
int getBoxIndex(int i, int j);
//��ת����
void rotateToBox(char *srcBox, char *destBox, int times);
//��ʼ����ͼ
void initMap();
//����ͼ
void drawMap();
//��ʼ��Ϸ
int startGame();
//�������
void clearBox();
//������
void drawBox();

//--------------------------------------------------------------------------

int getBoxIndex(int i, int j)
{
    return i * BOX_LENGTH + j;
}


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


void drawMap()
{
    for (int i = 0; i < HEIGHT; ++i)
    {
        for (int j = 0; j < WIDTH; ++j)
        {
            if (g_map[i][j] == MAP_FREE)
            {
                printf("��");
            }
            else
            {
                printf("��");
            }
        }
        printf("\r\n");
    }
}

void clearBox()
{

}

void setPos(int x, int y)
{
    COORD point = {x,y};
    HANDLE HOutput
}

void drawBox()
{

}

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

int startGame()
{
    drawMap();
    return 1;
}


int main(int argc, char *argv)
{

    initMap();
    startGame();
    system("pause");
    return 0;
}