
#define bool int
#define true 1
#define false 0

#include "stdio.h"
#include "rand.h"
#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
 
#define ROW 10   //16行
#define COL 10   //16列
#define MINE 16  //40颗

char mine[ROW + 2][COL + 2];  //雷数组
char show[ROW + 2][COL + 2];  //向玩家展示的数组
int flag = 1;
int flag1 =0;

void start(void);

//判断是否赢了的函数，用mine和show两个数组作对比
static int is_win(char mine[][COL + 2], char show[][COL + 2])
{
    int i, j;
    for(i = 1; i <= ROW; i++)
    {
        for(j = 1; j <= COL; j++)
        {
            if(mine[i][j] != '*' && mine[i][j] != '/')   //跳过‘*’雷，和‘/’
            {
                if(mine[i][j] == '0' || mine[i][j] != show[i][j])  //如果mine数组中还有‘0’表示还没有扫完雷，或者mine[i][j] !=show[i][j],意思是还有的格子没有被点到
                {
                    return 0;  //那么返回0表示，没有赢呢
                }
            }
        }
    }
    return 1;   //能走到这里，那么肯定是赢了
}
//无雷区域自动展开的函数，核心函数，这里用的是递归，效率比较低下，后期有待改进
//该函数的出口条件是，扫描当前的格子已经不是‘0’无雷了，那么就停止递归，也就是碰到边界，或者碰到数字，或者碰到‘*’雷停止递归
//否则，以自己为中心向他的8个方向开始递归
static void open_mine(char mine[][COL + 2], char show[][COL + 2], int x, int y)
{
    while(mine[x][y] == '0')  //这个是循环条件也是出口条件，满足条件时继续递归，不满足条件时结束递归
    {
        mine[x][y] = '/';  //只要进来说明这个坐标的格子不是雷，立马把对应坐标的mine数组改为‘/’，表明这个地方扫描过了，防止，重新排查
        show[x][y] = ' ';  //没有雷的话，就向用户展示‘ ’， 而不是‘0’
        open_mine(mine, show, x, y - 1);
        open_mine(mine, show, x, y + 1);
        open_mine(mine, show, x - 1, y);
        open_mine(mine, show, x + 1, y);
        open_mine(mine, show, x - 1, y - 1);
        open_mine(mine, show, x - 1, y + 1);
        open_mine(mine, show, x + 1, y - 1);
        open_mine(mine, show, x + 1, y + 1);
    }
    if(mine[x][y] != '*' && mine[x][y] != '/')   //如果是数字，那么就显示数字，这个数字表示以这个数字为中心的格子周围有几个雷
    {
        show[x][y] = mine[x][y];  
    }
}
//计算周围雷数的函数，返回数目
static int mine_count(char mine[][COL + 2], int x, int y)
{
    int count = 0;
    mine[x - 1][y - 1] == '*' ? count++ : count;
    mine[x - 1][y    ] == '*' ? count++ : count;
    mine[x - 1][y + 1] == '*' ? count++ : count;
    mine[x    ][y - 1] == '*' ? count++ : count;
    mine[x    ][y + 1] == '*' ? count++ : count;
    mine[x + 1][y - 1] == '*' ? count++ : count;
    mine[x + 1][y    ] == '*' ? count++ : count;
    mine[x + 1][y + 1] == '*' ? count++ : count;
    return count;
}
//这个函数是在雷埋好之后调用的，负责把每个格子周围的雷叔写入mine数组，一次性计算好，不需要等到玩家点击的时候才去计算周围的雷数，也方便后面，空白区域的自动展开
static void set_num(char mine[][COL + 2])
{
    int i, j;
    for(i = 1; i <= ROW; i++)
    {
        for(j = 1; j <= COL; j++)
        {
            if(mine[i][j] == '0')
            {
                mine[i][j] = mine_count(mine, i, j) + '0';   //计算每个格子周围的雷数，写入到雷数组中去
            }
        }
    }
}
//这是个埋雷的函数，这里为什么要传递x,y参数呢，是因为避免，玩家第一次点击的时候就踩雷，所以跳过这个坐标来初始化
static void set_mines(char mine[][COL + 2], int x, int y)
{
    //srand(1234);
    int sum = 0;
    int xx, yy;
    while(sum != MINE)
    {
        xx = rand() % ROW + 1;
        yy = rand() % COL + 1;
        if(mine[xx][yy] == '0' && xx != x && xx != y)
        {
            mine[xx][yy] = '*';
            sum++;
        }
    }
}
//这个函数负责打印，雷图或者打印show，传入的参数flag为1表示打印雷图，为2表示打印show
static void print_board(char board[][COL + 2], int flag)
{
    printf("  ");
    int i, j;
    for(i = 1; i <= COL; i++)
    {
        printf("%3d", i);
    }
    printf("\n");
    for(i = 1; i <= ROW; i++)
    {
        printf("%2d|", i);
        for(j = 1; j <= COL; j++)
        {
            if(1 == flag)  //进入这个if条件，说明是打印雷图
            {
                if(board[i][j] != '*')
                {
                    if(board[i][j] == '0')  //没有雷显示 ‘ ’空格，不显示‘0’，做一个小小的处理
                    {
                        printf("  |");
                    }
                    else
                    {
                        printf(" %c|", board[i][j]);  //这个情况是打印数字的情况
                    }
                }
                else  //进入这个else，表示是打印雷
                {
                    printf(" *|");
                }
            }
            else  //进入这个else表示打印show
            {
                if(board[i][j] == '0')  //如果这个地方不是雷，做一下处理，输出‘ ’ 空格，而不输出‘0’
                {
                    printf("  |");
                }
                else
                {
                    printf(" %c|", board[i][j]);  //进入这里表示点到的地方是一个数字，那么显示这个数字
                }
            }
        }
        printf("\n");
    }
}
//初始化雷图
static void init_mine(char mine[][COL + 2])
{
    int i, j;
    for(i = 1; i <= ROW; i++)
    {
        mine[i][0] = '/';  //把 左 边界初始化为 ‘/’
        mine[i][COL + 1] = '/';  //把 右 边界初始化为 ‘/’
        for(j = 1; j <= COL; j++)
        {
            mine[i][j] = '0';  
        }
    }
    for(i = 0; i < COL + 2; i++)
    {
        mine[0][i] = '/';  //把 上 边界初始化为 ‘/’
        mine[ROW + 1][i] = '/';  //把 下 边界初始化为 ‘/’
    }
}
//初始化向玩家展示的数组
static void init_show(char show[][COL + 2])
{
    int i, j;
    for(i = 0; i < ROW + 2; i++)
    {
        for(j = 0; j < COL + 2; j++)
        {
            show[i][j] = '-'; //全部初始化为‘-’
        }
    }
}

void PlayerInput(int fd_stdin,int fd_stdout)
{
    int x,y;
    int n;
    //int pos = -1,x,y;
    char szCmd[80]={0};
    printf("Please Input The Line Position where you put your Chessman (x): ");
    n = read(fd_stdin,szCmd,80);
    szCmd[1] = 0;
    atoi(szCmd,&x);
    printf("Please Input The Column Position where you put your Chessman (y): ");
    n = read(fd_stdin,szCmd,80);
    szCmd[1] = 0;
    atoi(szCmd,&y);
    if(x >= 1 && x <= ROW && y >= 1 && y <= COL)
        {
            if(flag)   //在玩家，输入坐标之后才开始初始化雷图，防止，第一次就踩雷
            {
                flag = 0;
                set_mines(mine, x, y);
                set_num(mine);
            }
            if(mine[x][y] == '*')  //这个表示玩家踩到雷，游戏结束
            {
                clear();
                printf("game over!\n");
                print_board(mine, 1);
                flag1=1;
                goto L4;
            }
            if(mine[x][y] != '0' && mine[x][y] != '*')   //这个说明点击的地方是个数字，说明这个周围有雷，数字是多少，表示周围有多少颗雷
            {
                show[x][y] = mine_count(mine, x, y) + '0';
            }
            else  //那么这种情况就是，一片无雷区域，那么open_mine这个函数，去自动展开
            {
                open_mine(mine, show, x, y);
            }
        }
        else
        {
            printf("Input Error！\n");  //走到这里表示玩家没有输入正确坐标重新输入
        }
L4:     return 0;
}

static void game(int fd_stdin,int fd_stdout)
{
    
    init_mine(mine);  //初始化雷数组
    init_show(show);  //初始化show数组
    do
    {
        if(is_win(mine, show))
        {
            clear();
            print_board(show, 2);
            print_board(mine, 1);
            printf("You Win！\n");
            flag1=1;
            goto L6;
        }
        clear();
        print_board(show, 2);
        //print_board(mine, 1);
        //printf("input <x, y>:");
        PlayerInput(fd_stdin,fd_stdout);
        //scanf("%d%d", &x, &y);
    }while(flag1 == 0);
L6: return 0;
}

//游戏选择界面
static void menu(void)
{
    clear();
    printf("################################\n");
    printf("###       MineSweeper       ####\n");
    printf("################################\n");
    printf("###       1.Start Game      ####\n");
    printf("###       0.Quit  Game      ####\n");
    printf("################################\n");
}
 

int minesweeper(int fd_stdin,int fd_stdout)
{
    init_mine(mine);  //初始化雷数组
    init_show(show);  //初始化show数组
    flag=1;
    flag1=0;
    do
    {
        int select;
        char buf[80]={0};
        menu();   
        read(fd_stdin,buf,2);
    	//buf[1] = 0;
       	atoi(buf,&select); 
        //scanf("%d", &select);
        if (select == 1)
        {
            game(fd_stdin,fd_stdout); //选择1开始游戏
            break;
        }
        else if (select==0)
        {
            goto L5; //选择0退出游戏
        }
        else
        {
            break;
        }
    }while(1);
L5: return 0;
}


