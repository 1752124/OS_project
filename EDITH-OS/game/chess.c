
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"






/*======================================================================*/



int QP[10][10]; //棋盘格局


void Init1()   //初始化函数，当前的棋盘格局总是保存在States[0]中
{
    	int i,j;
    	for(i=0;i<10;i++)
        	for(j=0;j<10;j++)
            		QP[i][j]=0; //将棋盘清空
	
}

void DisplayBoard(int QP[10][10])
{
	int i = 0;
	int j = 0;
 
	for (i = 0; i < 10; i++)
	{
		printf("  %d ", i+1); //打印棋盘 x 轴坐标提示
	}
	printf("\n");
 
	for (j = 0; j < 10; j++)
	{
		printf("---|"); //打印第一行棋盘
	}
	printf("\n");
 
	for (i = 0; i < 10; i++)
	{
		for (j  = 0; j < 10; j++)
		{
			if (QP[i][j] == -1)
            		{
                		printf(" %c |", 1);
            		}
            		else if (QP[i][j] == 1)
            		{
                		printf(" %c |", 2);
            		}
            		else
            		{
                		printf("   |");
            		}
			//printf(" %c |", QP[i][j]); //打印竖标
		}
		printf(" %d ", i+1); //打印棋盘 y 轴坐标提示
		printf("\n");
 
		for (j = 0; j < 10; j++)
		{
			printf("---|"); //打印横标
		}
		printf("\n");
	}
}


//用户1通过此函数来输入落子的位置，
//比如，用户输入3 1，则表示用户在第3行第1列落子。
void UserInput1(int fd_stdin,int fd_stdout)
{
	printf("Player1:\n");
    int n;
    int pos = -1,x,y;
    char szCmd[80]={0};
L1: printf("Please Input The Line Position where you put your Chessman (x): ");
    n = read(fd_stdin,szCmd,80);
    szCmd[1] = 0;
    atoi(szCmd,&x);
    printf("Please Input The Column Position where you put your Chessman (y): ");
    n = read(fd_stdin,szCmd,80);
    szCmd[1] = 0;
    atoi(szCmd,&y);
    if(x>0&&x<16&&y>0&&y<16&&QP[x-1][y-1]==0)
        QP[x-1][y-1]=1;
    else
    {
        printf("Input Error!\n");
        goto L1;
    }

}

//用户2输入
void UserInput2(int fd_stdin,int fd_stdout)
{
	printf("Player2\n");
    int n;
    int pos = -1,x,y;
    char szCmd[80]={0};
L2: printf("Please Input The Line Position where you put your Chessman (x): ");
    n = read(fd_stdin,szCmd,80);
    szCmd[1] = 0;
    atoi(szCmd,&x);
    printf("Please Input The Column Position where you put your Chessman (y): ");
    n = read(fd_stdin,szCmd,80);
    szCmd[1] = 0;
    atoi(szCmd,&y);
    if(x>0&&x<16&&y>0&&y<16&&QP[x-1][y-1]==0)
        QP[x-1][y-1]=-1;
    else
    {
        printf("Input Error!\n");
        goto L2;
    }

}

//判断平局
static int IsFull(int QP[10][10], int row, int col)
{
	int i = 0;
	int j = 0;
 
	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 10; j++)
		{
			if (QP[i][j] == 0)
			{
				return 0;
			}
		}
	}
 
	//棋盘没有空位置了还没有判断出输赢，则平局
	return 1;
}

//判断输赢
int Win(int QP[10][10], int row, int col)
{
	int i = 0;
	int j = 0;
 
	// 横线上五子连成一线，赢家产生
	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 10 - 4; j++)
		{
			if (QP[i][j] == QP[i][j + 1]
				&& QP[i][j + 1] == QP[i][j + 2]
				&& QP[i][j + 2] == QP[i][j + 3]
				&& QP[i][j + 3] == QP[i][j + 4]
				&& QP[i][j] != 0)
			{
				return QP[i][j];
			}
		}
	}
 
	// 竖线上五子连成一线，赢家产生
	for (j = 0; j < 10; j++)
	{
		for (i = 0; i < 10 - 4; i++)
		{
			if (QP[i][j] == QP[i+1][j]
				&& QP[i+1][j] == QP[i+2][j] 
				&& QP[i+2][j] == QP[i+3][j]
				&& QP[i+3][j] == QP[i+4][j] 
				&& QP[i][j] != 0)
			{
				return QP[i][j];
			}
		}
	}
 
	// 斜线上五子连成一线，赢家产生
	for (i = 0; i < 10 - 4; i++)
	{
		if (QP[i][i] == QP[i+1][i+1]
			&& QP[i+1][i+1] == QP[i+2][i+2]
			&& QP[i + 2][i + 2] == QP[i + 3][i + 3]
			&& QP[i + 3][i + 3] == QP[i + 4][i + 4]
			&& QP[i][i] != 0)
		{
			return QP[i][i];
		}
 
		if (QP[i][i+4] == QP[i+1][i+3]
			&& QP[i+1][i+3] == QP[i+2][i+2]
			&& QP[i + 2][i + 2] == QP[i + 3][i + 1]
			&& QP[i + 3][i + 1] == QP[i + 4][i]
			&& QP[i][i + 4] != 0)
		{
			return QP[i][i+4];
		}
	}
 
	//游戏平局
	if (IsFull(QP, 10, 10))
	{
		return -2;
	}
 
	//游戏结束
	return 0;
 
}


void chess(int fd_stdin,int fd_stdout)
{
    	char buf[80]={0};
    	Init1();
	printf("##################################\n");
   	printf("##Welcome To The Fivechess Game!##\n");
	printf("######     1.Start Game     ######\n");
	printf("######     0.Quit  Game     ######\n");
	printf("##################################\n");
	while (1)
	{
        int i = -1;
        printf("choose:");
	    read(fd_stdin,buf,2);
    	//buf[1] = 0;
       	atoi(buf,&i);
        if (i == 0){       //退出
                goto L3;
        	}
            if (i == 1){
                goto L4;
            }
            else{
                printf("Input Error!\n");
            }
    }
	L4: DisplayBoard(QP);
       	int ret = 0;
        while(1)
	{
        	UserInput1(fd_stdin, fd_stdout);
		ret = Win(QP, 10, 10);
		if (ret != 0)
		{
			break;
		}
		DisplayBoard(QP); //打印棋盘
		printf("\n");
                UserInput2(fd_stdin, fd_stdout);
		ret = Win(QP, 10, 10);
		if (ret != 0)
		{
			break;
		}
		DisplayBoard(QP); //打印棋盘
		printf("\n");
	}
	// 判断输赢或平局
	if (ret == -2)
	{printf("Draw!\n"); 
		DisplayBoard(QP); //打印棋盘
	}
	else if (ret == -1)
	{
		
		printf("Player2 win!\n");
		DisplayBoard(QP); //打印棋盘
	}
	else if (ret == 1)
	{
		printf("Player1 win!\n");
		DisplayBoard(QP); //打印棋盘
	}
L3:return 0;

}
