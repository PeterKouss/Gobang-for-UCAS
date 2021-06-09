#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FLUSH while (getchar() != '\n')

#define SIZE 15
#define CHARSIZE 2

#define SEARCHDEPTHTIME 6 //搜索层数
#define SERIESDEPTH 10	//算杀层数

#define ROOTNUMBER 15 //搜索根节点数
#define CHILDNUMBER 6 //搜索子节点数

#ifndef max
#define max(a, b) ((a > b) ? a : b)
#endif

#ifndef min
#define min(a, b) ((a < b) ? a : b)
#endif

#define oppcolor ((color == 1) ? 2 : 1)

//全局变量区
char aInitDisplayBoardArray[SIZE][SIZE * CHARSIZE + 1] =
	{
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + ",
		"+ + + + + + + + + + + + + + + "}; //棋盘使用Unicode画的，一个符号占两个char，所以要*2，+1是为了末尾的'\0'
char play1Pic[] = "●";		//"●"黑棋;
char play2Pic[] = "◎";		//"◎"白棋;
char play1CurrentPic[] = "▲";
char play2CurrentPic[] = "△";
char aDisplayBoardArray[SIZE][SIZE * CHARSIZE + 1]; //此数组用于显示
int aRecordBoard[SIZE + 1][SIZE + 1];				//此数组用于记录棋位 从(1,1)开始,记录位置即棋盘坐标

struct st
{
	int x;
	int y;
} step[SIZE * SIZE + 1];

struct po
{
	int x;
	int y;
	int attackpriority;				  //进攻评分
	int defencepriority;			  //防守评分
	int compriority;				  //综合评分
} testpoint[SIZE * SIZE + 1],		  //扫描中储存暂时点
	possiblepoint[SIZE * SIZE + 1],   //Genratepoint中的可行点
	rootpoint[SIZE * SIZE + 1],		  //Alphabeta扫描中的根点
	childpoint[17][SIZE * SIZE + 1],  //扫描中各层点
	attackpoint[SIZE * SIZE + 1],	 //Attackpoint中的可行点
	attackrootpoint[SIZE * SIZE + 1], //seriessearch中的根点
	bestpoint;						  //最佳落子点

int stepnumber = 0;										   //棋步记录
int upside = 0, rightside = 0, downside = 0, leftside = 0; //棋盘有子方形边界

int flex6, flex5, flex4, flex3, flex2, block4, block3, block2; //分别记录棋形数量

int temi; //用于Alphabeta中缓存一个i

//程序声明区
void initRecordBorard(void);
void recordtoDisplayArray(void);
void displayBoard(void);
void pvp(void);
void pvc(void);
int choosemode(void);
int chooseside(void);
int playagain(void);
int judgeforbidenmove();
int judgewin();
int judgerepetset();
void sidesize();
void sortquick(struct po *v, int left, int right);
void swap(struct po *v, int i, int j);
void backwards();
void manset(int color);
void pointsettype(int color, int x, int y);
void pointtypescan(int color, int i, int j, int a, int b);
void setpoint(int color, int x, int y);
void aiset(int color);
int findGenaratepoint(int color);
int forcemove(int count);
int pointpriority();
int evaluatepoint(int color, int x, int y);
int alphabeta(int depth, int alpha, int beta, int color, int computerplay);
int serieswin(int depth, int color, int computerplay);
void rootpointsearch(int color, int depth, int alpha, int beta);
int evaluateboard(int color);
int rootserieswin(int color, int depth);
int serieswin(int depth, int color, int computerplay);
int findattackpoint(int color);

//程序区
int main()
{
	do
	{
		printf("程序作者:杨立昆\n");
		if (choosemode() == 1)
		{
			pvp();
		}
		else
		{
			pvc();
		}
	} while (playagain() == 1);
	return 0;
}

//人人端口
void pvp(void)
{
	initRecordBorard();		//初始化记录数组aRecordBoard
	recordtoDisplayArray(); //将棋子位置读入输出模块
	displayBoard();			//打印棋盘
	while (1)
	{
		//黑棋落子
		manset(1); //人控黑棋落子(包括禁手)
		pointsettype(1, step[stepnumber].x, step[stepnumber].y);
		recordtoDisplayArray();
		displayBoard(); //打印棋盘
		if (judgeforbidenmove() == 1)
		{
			printf("Forbidenmove.White win!");
		}
		if (judgewin() == 1)
		{
			printf("Black win!\n");
			break;
		} //判断输赢(输入stepnumber为步数)(已判断则返回1,未判断则返回0)
		//白棋落子
		manset(2);
		pointsettype(2, step[stepnumber].x, step[stepnumber].y);
		recordtoDisplayArray();
		displayBoard(); //打印棋盘
		if (judgewin() == 1)
		{
			printf("White win!\n");
			break;
		} //判断输赢(输入stepnumber为步数)(已判断则返回1,未判断则返回0)
	}
}

//人机端口
void pvc(void)
{
	int returnchooseside = chooseside();
	if (returnchooseside == 1)
	{
		initRecordBorard();		//初始化记录数组aRecordBoard
		recordtoDisplayArray(); //将棋子位置读入输出模块
		displayBoard();			//打印棋盘
		while (1)
		{
			manset(1);
			pointsettype(1, step[stepnumber].x, step[stepnumber].y);
			recordtoDisplayArray();
			displayBoard(); //打印棋盘
			if (judgeforbidenmove() == 1)
			{
				printf("Forbidenmove.White win!");
				getchar();
			}
			if (judgewin() == 1)
			{
				printf("Black win!\n");
				break;
			} //判断输赢(输入stepnumber为步数)(已判断则返回1,未判断则返回0)
			aiset(2);
			pointsettype(2, step[stepnumber].x, step[stepnumber].y);
			recordtoDisplayArray();
			displayBoard(); //打印棋盘
			if (judgewin() == 1)
			{
				printf("White win!\n");
				break;
			} //判断输赢(输入stepnumber为步数)(已判断则返回1,未判断则返回0)
		}
	}
	else if (returnchooseside == 2)
	{
		initRecordBorard();		//初始化记录数组aRecordBoard
		recordtoDisplayArray(); //将棋子位置读入输出模块
		displayBoard();			//打印棋盘
		while (1)
		{
			aiset(1);
			pointsettype(1, step[stepnumber].x, step[stepnumber].y);
			recordtoDisplayArray();
			displayBoard(); //打印棋盘
			if (judgewin() == 1)
			{
				printf("Black win!\n");
				break;
			} //判断输赢(输入stepnumber为步数)(已判断则返回1,未判断则返回0)
			manset(2);
			pointsettype(2, step[stepnumber].x, step[stepnumber].y);
			recordtoDisplayArray();
			displayBoard(); //打印棋盘
			if (judgewin() == 1)
			{
				printf("White win!\n");
				break;
			} //判断输赢(输入stepnumber为步数)(已判断则返回1,未判断则返回0)
		}
	}
}

//ai落子端口
void aiset(int color)
{
	//启始第一步(执黑)
	if (stepnumber == 0)
	{
		setpoint(color, 8, 8);
		sidesize();
	}
	//第二步(执白)下在黑棋旁边
	else if (stepnumber == 1)
	{
		int rx = 0, ry = 0;
		srand(time(NULL));
		do
		{
			rx = step[stepnumber].x + (rand() % 3 - 1);
			ry = step[stepnumber].y + (rand() % 3 - 1);
		} while (judgerepetset(rx, ry) != 0);
		setpoint(color, rx, ry);
		sidesize();
	}
	//第三步(执黑)
	else if (stepnumber == 2)
	{
		int rx = 0, ry = 0;
		//如果白棋直指,下在它旁边(花月)
		if ((step[stepnumber].x == 8 && (step[stepnumber].y <= 9 && step[stepnumber].y >= 7)) || (step[stepnumber].y == 8 && (step[stepnumber].x <= 9 && step[stepnumber].x >= 7)))
		{
			srand(time(NULL));
			do
			{
				int d = (rand() % 3 - 1);
				rx = 8 + d;
				ry = 8 + d;
			} while (judgerepetset(rx, ry) != 0 || rx < 7 || rx > 9 || ry < 7 || ry > 9 || (rx != step[stepnumber].x && ry != step[stepnumber].y));
			setpoint(color, rx, ry);
			sidesize();
		}
		//如果白棋斜指,下在浦月位(浦月)
		else if ((step[stepnumber].x <= 9 && step[stepnumber].x >= 7 && step[stepnumber].y <= 9 && step[stepnumber].y >= 7) && step[stepnumber].x != 8 && step[stepnumber].y != 8)
		{
			srand(time(NULL));
			do
			{
				int d = (rand() % 3 - 1);
				int e = 0;
				do
				{
					e = ((d == 0) ? (rand() % 3 - 1) : 0);
				} while (d == 0 && e == 0);
				rx = step[stepnumber].x + 2 * d;
				ry = step[stepnumber].y + 2 * e;
			} while (judgerepetset(rx, ry) != 0 || rx < 7 || rx > 9 || ry < 7 || ry > 9);
			setpoint(color, rx, ry);
			sidesize();
		}
		//如果白棋没有下在中间区域
		else
		{
			rootpointsearch(color, SEARCHDEPTHTIME, -1000000, 1000000);
			setpoint(color, bestpoint.x, bestpoint.y);
			sidesize();
		}
	}
	//其余步骤进行搜索计算
	else if (stepnumber >= 2 && stepnumber <= 10)
	{
		rootpointsearch(color, SEARCHDEPTHTIME, -1000000, 1000000);
		setpoint(color, bestpoint.x, bestpoint.y);
		sidesize();
	}
	else
	{
		//己方算杀
		if (rootserieswin(color, SERIESDEPTH))
		{
			setpoint(color, bestpoint.x, bestpoint.y);
			sidesize();
			return;
		}
		//未能完成己方算杀,如果可下点数目有限(有必须落子情况),执行剪枝搜索
		if (findGenaratepoint(color) < 12)
		{
			rootpointsearch(color, SEARCHDEPTHTIME, -1000000, 1000000);
			setpoint(color, bestpoint.x, bestpoint.y);
			sidesize();
			return;
		}
		//对方算杀
		if (rootserieswin(oppcolor, SERIESDEPTH))
		{

			setpoint(color, bestpoint.x, bestpoint.y);
			sidesize();
			return;
		}
		//未能算杀或被算杀,利用搜索算法
		rootpointsearch(color, SEARCHDEPTHTIME, -1000000, 1000000);
		setpoint(color, bestpoint.x, bestpoint.y);
		sidesize();
	}
}

//搜索程序入口
void rootpointsearch(int color, int depth, int alpha, int beta)
{
	int rootcount;
	rootcount = findGenaratepoint(color); //找到可能落子点并记录数目
	if (rootcount == 1)					  //如果只有一个可能,直接落子
	{
		bestpoint = possiblepoint[1];
	}
	else //否则进行搜索
	{
		int val = -1000000;
		for (int i = 1; i <= rootcount; i++)
		{
			rootpoint[i] = possiblepoint[i];
		}
		for (int i = 1; i <= ROOTNUMBER && i <= rootcount; i++)
		{
			setpoint(color, rootpoint[i].x, rootpoint[i].y);
			sidesize();
			rootpoint[i].compriority = alphabeta(depth - 1, alpha, beta, color, 0); //递归入口
			val = max(val, rootpoint[i].compriority);
			alpha = max(alpha, val);
			backwards();
		}
		sortquick(rootpoint, 1, min(ROOTNUMBER, rootcount));
		bestpoint = rootpoint[1];
	}
}

//alphabeta剪枝
int alphabeta(int depth, int alpha, int beta, int color, int computerplay)
{
	if (depth == 0 || (childpoint[depth + 1][temi].compriority >= 1200 && (computerplay == 1))) //深度为0或赢了直接返回
	{
		return evaluateboard(color); //返回估值
	}
	if (computerplay == 1)
	{
		int val = -1000000;
		int childcount = 0;
		childcount = findGenaratepoint(color); //找到可下节点
		for (int i = 1; i <= childcount; i++)
		{
			childpoint[depth][i] = possiblepoint[i];
		}
		for (int i = 1; i <= CHILDNUMBER && i <= childcount; i++)
		{
			temi = i;
			setpoint(color, childpoint[depth][i].x, childpoint[depth][i].y); //下点
			sidesize();
			val = max(val, alphabeta(depth - 1, alpha, beta, color, 0)); //己方取最大
			alpha = max(alpha, val);
			backwards();
			if (beta <= alpha) //alpha剪裁
			{
				break;
			}
		}
		return val;
	}
	if (computerplay == 0)
	{
		int val = 1000000;
		int childcount = 0;
		childcount = findGenaratepoint(oppcolor);
		for (int i = 1; i <= childcount; i++)
		{
			childpoint[depth][i] = possiblepoint[i];
		}
		for (int i = 1; i <= CHILDNUMBER && i <= childcount; i++)
		{
			temi = i;
			setpoint(oppcolor, childpoint[depth][i].x, childpoint[depth][i].y);
			sidesize();
			val = min(val, alphabeta(depth - 1, alpha, beta, color, 1)); //对方取最小
			beta = min(beta, val);
			backwards();
			if (beta <= alpha) //beta剪裁
			{
				break;
			}
		}
		return val;
	}
}

//算杀程序入口
int rootserieswin(int color, int depth)
{
	int attackcount = 0;
	attackcount = findattackpoint(color); //找到最佳进攻点(已排序)
	for (int i = 1; i <= attackcount; i++)
	{
		attackrootpoint[i] = attackpoint[i];
	}
	for (int i = 1; i <= attackcount; i++)
	{
		setpoint(color, attackrootpoint[i].x, attackrootpoint[i].y);
		sidesize();
		if (serieswin(depth - 1, color, 0)) //递归入口
		{
			//可以算杀则拷贝到bestpoint
			bestpoint = attackrootpoint[i];
			backwards();
			return 1;
		}
		backwards();
	}
		return 0;
}

//算杀
int serieswin(int depth, int color, int computerplay)
{
	int evaluate = 0;
	//己方下棋时,胜利返回1,失败返回0
	if (computerplay)
	{
		evaluate = evaluateboard(color);
		if (evaluate >= 8000) //己方胜利
		{
			return 1;
		}
		else if (evaluate == -10000) //己方必输
		{
			return 0;
		}
	}
	//对方下棋时,对方胜利返回0,失败返回1
	else
	{
		evaluate = evaluateboard(oppcolor);
		if (evaluate == -10000) //对方必输
		{
			return 1;
		}
		else if (evaluate >= 8000) //对方必胜
		{
			return 0;
		}
	}
	if (depth == 0) //深度返回
	{
		return 0;
	}
	if (computerplay)
	{
		int win = 0;
		int attackcount = findattackpoint(color);
		for (int i = 1; i <= attackcount; i++)
		{
			childpoint[depth][i] = attackpoint[i];
		}
		for (int i = 1; i <= attackcount; i++)
		{
			setpoint(color, childpoint[depth][i].x, childpoint[depth][i].y);
			sidesize();
			win = win || serieswin(depth - 1, color, 0); //找到一点即算杀成功
			backwards();
			if (win)
			{
				break;
			}
		}
		return win;
	}
	else
	{
		int win = 1;
		int childcount = findGenaratepoint(oppcolor);
		for (int i = 1; i <= childcount; i++)
		{
			childpoint[depth][i] = possiblepoint[i];
		}
		for (int i = 1; i <= childcount; i++)
		{
			setpoint(oppcolor, childpoint[depth][i].x, childpoint[depth][i].y);
			sidesize();
			win = win && serieswin(depth - 1, color, 1); //一点可被防守则算杀失败
			backwards();
			if (!win)
			{
				break;
			}
		}
		return win;
	}
}

//找到形成杀棋位置(大于活三)
int findattackpoint(int color)
{
	int attackcount = 0;
	int tempval = 0;
	for (int i = upside - 3; i <= downside + 3; i++)
	{
		for (int j = leftside - 3; j <= rightside + 3; j++)
		{
			if (judgerepetset(j, i) != 0)
			{
				continue;
			}
			setpoint(color, j, i);
			pointsettype(color, j, i);
			backwards();
			if (color == 1 && judgeforbidenmove() != 0)
			{
				continue;
			}
			tempval = pointpriority();
			if (tempval >= 200 || flex3 > 0 || block4 > 0) //己方可杀点
			{
				attackcount++;
				attackpoint[attackcount].x = j;
				attackpoint[attackcount].y = i;
				attackpoint[attackcount].compriority = tempval;
			}
			setpoint(oppcolor, j, i);
			pointsettype(oppcolor, j, i);
			backwards();
			if (flex5 > 0) //对方下成五的点自己也要搜!!!(防止对方下成冲四,原本可以算杀,但算杀因为下成冲四,己方搜索进攻点搜不到对方成五点,而认为己方输,导致算杀失败的情况)!!!
			{
				attackcount++;
				attackpoint[attackcount].x = j;
				attackpoint[attackcount].y = i;
				attackpoint[attackcount].compriority = 5000;
			}
		}
	}
	sortquick(attackpoint, 1, attackcount);
	return attackcount;
}

//找到搜索重要节点
int findGenaratepoint(int color)
{
	int count = 0, movecount = 0;
	int tempval = 0;
	for (int i = upside - 3; i <= downside + 3; i++)
	{
		for (int j = leftside - 3; j <= rightside + 3; j++)
		{
			if (judgerepetset(j, i) != 0) //是否重复
			{
				continue;
			}
			setpoint(color, j, i);
			pointsettype(color, j, i);
			backwards();
			if (color == 1 && judgeforbidenmove() != 0) //是否禁手
			{
				continue;
			}
			tempval = evaluatepoint(color, j, i);
			if (tempval > 0) //存储有意义的点
			{
				count++;
				testpoint[count].x = j;
				testpoint[count].y = i;
				testpoint[count].compriority = tempval;
			}
		}
	}
	sortquick(testpoint, 1, count); //对点排序
	movecount = forcemove(count);   //是否有强制落子点
	if (movecount == 0)
	{
		for (int i = 1; i <= count; i++)
		{
			movecount++;
			possiblepoint[i] = testpoint[i]; //转存到possiblepoint
		}
	}
	return movecount; //返回点数量
}

//决定性落子
int forcemove(int count)
{
	//有五连及以上,直接返回
	if (testpoint[1].compriority >= 2400)
	{
		possiblepoint[1] = testpoint[1];
		return 1;
	}
	int movecount = 0;
	//对方有活三
	if (testpoint[1].compriority == 1200)
	{
		//找对方下一步能活四的点
		for (int i = 1; i <= count; i++)
		{
			if (testpoint[i].compriority == 1200)
			{
				movecount++;
				possiblepoint[movecount] = testpoint[i];
			}
			else
				break;
		}
		//找双方下一步能冲四的点
		for (int i = movecount; i <= count; i++)
		{
			if ((testpoint[i].compriority >= 50 && testpoint[i].compriority < 200))
			{
				movecount++;
				possiblepoint[movecount] = testpoint[i];
			}
		}
	}
	return movecount;
}

//全盘估值
int evaluateboard(int color)
{
	int countflex6[3];
	int countflex5[3];
	int countflex4[3];
	int countflex3[3];
	int countflex2[3];
	int countblock4[3];
	int countblock3[3];
	int countblock2[3];
	int count44[3];
	int count43[3];
	int count33[3];

	for (int i = 0; i < 3; i++)
	{
		countflex6[i] = 0;
		countflex5[i] = 0;
		countflex4[i] = 0;
		countflex3[i] = 0;
		countflex2[i] = 0;
		countblock4[i] = 0;
		countblock3[i] = 0;
		countblock2[i] = 0;
		count44[i] = 0;
		count43[i] = 0;
		count33[i] = 0;
	}

	for (int i = upside - 3; i <= downside + 3; i++)
	{
		for (int j = leftside - 3; j <= rightside + 3; j++)
		{
			if (judgerepetset(j, i) != 0)
			{
				continue;
			}
			setpoint(color, j, i);
			pointsettype(color, j, i);
			backwards();
			if (color == 1 && judgeforbidenmove() != 0)
			{
				continue;
			}
			countflex6[color] += flex6;
			countflex5[color] += flex5;
			countflex4[color] += flex4;
			countflex3[color] += flex3;
			countflex2[color] += flex2;
			countblock4[color] += block4;
			countblock3[color] += block3;
			countblock2[color] += block2;
			if (flex4 + block4 > 1)
			{
				count44[color] += 1;
			}
			if (flex3 > 0 && block4 > 0)
			{
				count43[color] += 1;
			}
			if (flex3 > 1)
			{
				count33[color] += 1;
			}

			setpoint(oppcolor, j, i);
			pointsettype(oppcolor, j, i);
			backwards();
			countflex6[oppcolor] += flex6;
			countflex5[oppcolor] += flex5;
			countflex4[oppcolor] += flex4;
			countflex3[oppcolor] += flex3;
			countflex2[oppcolor] += flex2;
			countblock4[oppcolor] += block4;
			countblock3[oppcolor] += block3;
			countblock2[oppcolor] += block2;
			if (flex4 + block4 > 1)
			{
				count44[oppcolor] += 1;
			}
			if (flex3 > 0 && block4 > 0)
			{
				count43[oppcolor] += 1;
			}
			if (flex3 > 1)
			{
				count33[oppcolor] += 1;
			}
		}
	}

	// 当前局面轮到己方下棋
	// 1.己方存在连五点，必胜
	// 2.对方存在两个连五点，无法阻挡，必败
	// 3.对方不能成五，己方存在活四点，必胜
	// 4.己方有四三或四四,对方无成五点,必胜
	// 5.己方双三,对方无成四,成五点,必胜
	if (countflex5[color] > 0 || countflex6[color] > 0)
	{
		return 10000;
	}
	if (countflex5[oppcolor] + countflex6[oppcolor] > 1)
	{
		return -10000;
	}
	if (countflex5[oppcolor] == 0 && countflex4[color] > 0)
	{
		return 10000;
	}
	if ((count43[color] > 0 || count44[color] > 0) && countflex5[oppcolor] == 0)
	{
		return 9000;
	}
	if (count33[color] > 0 && countflex4[oppcolor] == 0 &&countblock4[oppcolor] == 0&& countflex5[oppcolor] == 0)
	{
		return 8000;
	}

	// 没有分出胜负，就计算评价值
	int colorscore = 0, oppcolorscore = 0;
	colorscore += countflex6[color] * 1200;
	colorscore += countflex5[color] * 1200;
	colorscore += countflex4[color] * 1000;
	colorscore += count44[color] * 1000;
	colorscore += count43[color] * 800;
	colorscore += count33[color] * 800;
	colorscore += countflex3[color] * 100;
	colorscore += countflex2[color] * 30;
	colorscore += countblock4[color] * 200;
	colorscore += countblock3[color] * 30;
	colorscore += countblock2[color] * 5;

	oppcolorscore += countflex6[oppcolor] * 1200;
	oppcolorscore += countflex5[oppcolor] * 1200;
	oppcolorscore += countflex4[oppcolor] * 1000;
	oppcolorscore += count44[oppcolor] * 1000;
	oppcolorscore += count43[oppcolor] * 800;
	oppcolorscore += count33[oppcolor] * 800;
	oppcolorscore += countflex3[oppcolor] * 100;
	oppcolorscore += countflex2[oppcolor] * 30;
	oppcolorscore += countblock4[oppcolor] * 200;
	oppcolorscore += countblock3[oppcolor] * 30;
	oppcolorscore += countblock2[oppcolor] * 5;

	//因为轮到己方下，所以己方棋型更有威力，所以乘以1.2
	return (colorscore * 1.2) - oppcolorscore;
}

//单点估值
int evaluatepoint(int color, int x, int y)
{
	int attackpriority, defencepriority;
	setpoint(color, x, y);
	pointsettype(color, x, y);
	backwards();
	attackpriority = pointpriority(); //该点进攻分
	setpoint(oppcolor, x, y);
	pointsettype(oppcolor, x, y);
	backwards();
	defencepriority = pointpriority(); //该点防守分
	if (attackpriority >= 200 || defencepriority >= 200)
	{
		return max((attackpriority * 2), defencepriority);
	}
	else
	{
		return (attackpriority * 2) + defencepriority;
	}
}

//落点优先级
int pointpriority()
{
	int priority = 0;
	if (flex5 > 0 || flex6 > 0)
	{
		return 5000;
	}
	else if (flex4 > 0 || block4 > 1)
	{
		return 1200;
	}
	else if (block4 + flex4 > 0 && flex3 > 0)
	{
		return 1000;
	}
	else if (flex3 > 1)
	{
		return 200;
	}
	else if (block4 > 0)
	{
		priority += 50;
	}
	else if (flex3 > 0)
	{
		priority += 12;
	}
	else if (block3 > 0)
	{
		priority += 5;
	}
	else if (flex2 > 0)
	{
		priority += 6;
	}
	else if (block2 > 0)
	{
		priority += 1;
	}
	return priority;
}

//人落子
void manset(int color)
{
	int temx, temy;
	char c;
	while (1)
	{
		scanf("%d %c", &temy, &c); //读点
		temy = 16 - temy;
		temx = c - 'A' + 1;
		FLUSH;
		int returnjudgerepetset = judgerepetset(temx, temy);
		if (returnjudgerepetset == 1) //是否重复
		{
			printf("It has been set. Please reset.\n");
			continue;
		}
		else if (returnjudgerepetset == 2) //是否越界
		{
			printf("Out of range please reset.\n");
			continue;
		}
		else if (returnjudgerepetset == 0)
		{
			setpoint(color, temx, temy);
			sidesize();
			break;
		}
	}

} //输入坐标+判断禁手+判断重复及溢出(judgerepetset返回0为无重复)

//判断禁手
int judgeforbidenmove()
{
	if (flex6 > 0 || flex4 + block4 > 1 || flex3 > 1) //一子双三,一子双四,一子长连则禁手
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//判断输赢
int judgewin()
{
	if (flex6 > 0 || flex5 > 0) //成五或六即赢
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//判断越域和重复
int judgerepetset(int x, int y)
{
	if (x > SIZE || y > SIZE || x < 1 || y < 1)
	{
		return 2;
	}
	for (int i = 1; i <= stepnumber; i += 1)
	{
		if (x == step[i].x && y == step[i].y)
		{
			return 1;
		}
	}
	return 0;
}

//选模式
int choosemode(void)
{
	char c;
	printf("Choose mode:\nPvP please tap \"1\".\nPVC please tap \"2\".\n");
	while (1)
	{
		c = getchar();
		FLUSH;
		if (c == '1')
		{
			return 1;
		}
		else if (c == '2')
		{
			return 0;
		}
		else
		{
			printf("Please tap \"1\" or \"2\".\n");
		}
	}
	return 0;
}

//人机选边函数(人走黑棋return 1,人走白棋return 2.)
int chooseside(void)
{
	char c;
	printf("Choose side:\nBlack please tap \"1\".\nWhite please tap \"2\".\n");
	while (1)
	{
		c = getchar();
		FLUSH;
		if (c == '1')
		{
			return 1;
		}
		else if (c == '2')
		{
			return 2;
		}
		else
		{
			printf("Please tap \"1\" or \"2\".\n");
		}
	}
	return 0;
}

//是否继续
int playagain(void)
{
	char c;
	printf("Play again?\nYes please tap\"1\".\nQuit please tap\"2\".\n");
	while (1)
	{
		c = getchar();
		FLUSH;
		if (c == '1')
		{
			return 1;
		}
		else if (c == '2')
		{
			return 0;
		}
		else
		{
			printf("Please tap \"1\" or \"2\".\n");
		}
	}
}

//设置边界(随时设置当前棋盘边界,剪枝重要成分)
void sidesize()
{
	upside = min(step[stepnumber].y, upside);
	downside = max(step[stepnumber].y, downside);
	rightside = max(step[stepnumber].x, rightside);
	leftside = min(step[stepnumber].x, leftside);
}

//快速排序(对compority)
void sortquick(struct po *v, int left, int right)
{
	int i, last;
	if (left >= right)
	{
		return;
	}
	swap(v, left, (left + right) / 2);
	last = left;
	for (i = left + 1; i <= right; i++)
	{
		if (v[i].compriority > v[left].compriority)
		{
			swap(v, ++last, i);
		}
	}
	swap(v, left, last);
	sortquick(v, left, last - 1);
	sortquick(v, last + 1, right);
}

void swap(struct po *v, int i, int j)
{
	struct po temp;
	temp = v[i];
	v[i] = v[j];
	v[j] = temp;
}

//落子
void setpoint(int color, int x, int y)
{
	stepnumber++;
	step[stepnumber].x = x;
	step[stepnumber].y = y;
	aRecordBoard[step[stepnumber].y][step[stepnumber].x] = color;
}

//退棋
void backwards()
{
	aRecordBoard[step[stepnumber].y][step[stepnumber].x] = 0;
	step[stepnumber].x = 0;
	step[stepnumber].y = 0;
	stepnumber--;
}

//初始化函数
void initRecordBorard(void)
{
	stepnumber = 0; //初始化步数
		//通过双重循环，将aRecordBoard清0
	for (int i = 0; i <= SIZE + 1; i++)
		for (int j = 0; j <= SIZE + 1; j++)
			aRecordBoard[i][j] = 0;
}

//记录棋子位置函数
void recordtoDisplayArray(void)
{
	//将aRecordBoard中记录的棋子位置，转化到aDisplayBoardArray中
	int i, j;
	//第一步：将aInitDisplayBoardArray中记录的空棋盘，复制到aDisplayBoardArray中
	for (i = 0; i < SIZE; i++)
	{
		for (j = 0; j <= SIZE * CHARSIZE + 1; j++)
			aDisplayBoardArray[i][j] = aInitDisplayBoardArray[i][j];
		aDisplayBoardArray[i][j] = '\0';
	}
	//第二步：扫描aRecordBoard，当遇到非0的元素，将●或者◎复制到aDisplayBoardArray的相应位置上
	for (i = 0; i < SIZE; i++)
	{
		for (j = 0; j < SIZE; j++)
		{
			if (aRecordBoard[i + 1][j + 1] == 1)
			{
				aDisplayBoardArray[i][CHARSIZE * j] = play1Pic[0];
				aDisplayBoardArray[i][CHARSIZE * j + 1] = play1Pic[1];
				//注意：aDisplayBoardArray所记录的字符是中文字符，每个字符占2个字节。●和◎也是中文字符，每个也占2个字节。
				if (CHARSIZE == 3)
					aDisplayBoardArray[i][CHARSIZE * j + 2] = play1Pic[2];
			}
			else if (aRecordBoard[i + 1][j + 1] == 2)
			{
				aDisplayBoardArray[i][CHARSIZE * j] = play2Pic[0];
				aDisplayBoardArray[i][CHARSIZE * j + 1] = play2Pic[1];
				if (CHARSIZE == 3)
					aDisplayBoardArray[i][CHARSIZE * j + 2] = play2Pic[2];
			}
		}
	}
	if (stepnumber % 2 == 1)
	{
		aDisplayBoardArray[step[stepnumber].y - 1][CHARSIZE * (step[stepnumber].x - 1)] = play1CurrentPic[0];
		aDisplayBoardArray[step[stepnumber].y - 1][CHARSIZE * (step[stepnumber].x - 1) + 1] = play1CurrentPic[1];
	}
	else
	{
		aDisplayBoardArray[step[stepnumber].y - 1][CHARSIZE * (step[stepnumber].x - 1)] = play2CurrentPic[0];
		aDisplayBoardArray[step[stepnumber].y - 1][CHARSIZE * (step[stepnumber].x - 1) + 1] = play2CurrentPic[1];
	}
}

//显示棋盘函数
void displayBoard(void)
{
	int i;
	//第一步：清屏
	system("cls");
	//第二步：将aDisplayBoardArray输出到屏幕上
	printf("\n");
	for (i = 0; i < SIZE; i++)
		printf("%3d %s\n", 15 - i, aDisplayBoardArray[i]); //打印棋盘竖行号和主体
	printf("   ");
	for (i = 0; i < SIZE; i++)
		printf("%2c", 'A' + i); //打印横行号
	printf("\n");
	for (i = 1; i <= stepnumber; i++)
	{
		printf("%d. %s:(%d,%c)%c", i, (i % 2 == 0) ? "white" : "black", 16 - step[i].y, step[i].x + 'A' - 1, (i % 2 == 0) ? '\n' : '\t');
	} //打印历史步骤

	/*
	printf("flex6:%d\n", flex6);
	printf("flex5:%d\n", flex5);
	printf("flex4:%d\n", flex4);
	printf("flex3:%d\n", flex3);
	printf("flex2:%d\n", flex2);
	printf("block4:%d\n", block4);
	printf("block3:%d\n", block3);
	printf("block2:%d\n", block2);
	*/
}

//扫描棋形函数
void pointsettype(int color, int x, int y)
{
	flex6 = 0;
	flex5 = 0;
	flex4 = 0;
	flex3 = 0;
	flex2 = 0;
	block4 = 0;
	block3 = 0;
	block2 = 0;
	//扫描四个方向
	for (int j = x - 5; j <= x + 5; j++)
	{
		pointtypescan(color, y, j, 1, 0);
	}
	for (int i = y - 5; i <= y + 5; i++)
	{
		pointtypescan(color, i, x, 0, 1);
	}
	for (int j = x - 5, i = y - 5; j <= x + 5; i++, j++)
	{
		pointtypescan(color, i, j, 1, 1);
	}
	for (int j = x - 5, i = y + 5; j <= x + 5; i--, j++)
	{
		pointtypescan(color, i, j, 1, -1);
	}
}

//扫描某个点在某个方向形成的新棋形
void pointtypescan(int color, int i, int j, int a, int b) //color:1为黑棋,2为白棋
{
	if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] != color)
	{
		return;
	}
	if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
	{
		if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == color)
		{
			if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
			{
				if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
				{
					if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == color)
					{
						if (i + 5 * b >= 1 && i + 5 * b <= 15 && j + 5 * a >= 1 && j + 5 * a <= 15 && aRecordBoard[i + 5 * b][j + 5 * a] == color)
						{
							flex6++; //连六●●●●●●
							return;
						}
						else
						{
							flex5++; //连五●●●●●
							return;
						}
					}
				}
			}
		}
	} //连六和连五
	if (i - b >= 1 && i - b <= 15 && j - a >= 1 && j - a <= 15 && (aRecordBoard[i - b][j - a] == 0))
	{
		if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
		{
			if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == color)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
					{
						if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == 0)
						{
							flex4++; //活四+●●●●+
							return;
						}
						else if (((i + 3 * b == 15) || (j + 3 * a == 15)) || (aRecordBoard[i + 4 * b][j + 4 * a] != color && aRecordBoard[i + 4 * b][j + 4 * a] != 0))
						{
							block4++; //一端冲四+●●●●◎
							return;
						}
					}
				}
			}
		}
	}
	else if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && (aRecordBoard[i - b][j - a] != color && aRecordBoard[i - b][j - a] != 0 || (i - b == 0) || (j - a == 0)))
	{
		if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
		{
			if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == color)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
					{
						if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == 0)
						{
							block4++; //一端冲四◎●●●●+
							return;
						}
					}
				}
			}
		}
	} //活四和一端冲四
	if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
	{
		if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == color)
		{
			if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
			{
				if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == 0)
				{
					if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == color)
					{
						block4++; //中偏冲四●●●+●
						return;
					}
				}
			}
			else if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == 0)
			{
				if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
				{
					if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == color)
					{
						block4++; //中部冲四●●+●●
						return;
					}
				}
			}
		}
		else if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
		{
			if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
			{
				if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
				{
					if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == color)
					{
						block4++; //中偏冲四●+●●●
						return;
					}
				}
			}
		}
	} //中部冲四和中偏冲四
	if (i - b >= 1 && i - b <= 15 && j - a >= 1 && j - a <= 15 && (aRecordBoard[i - b][j - a] == 0))
	{
		if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
		{
			if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == color)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == 0)
					{
						//两侧界内
						if (i - 2 * b >= 1 && i - 2 * b <= 15 && j - 2 * a >= 1 && j - 2 * a <= 15 && i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && ((aRecordBoard[i + 4 * b][j + 4 * a] == 0 && aRecordBoard[i - 2 * b][j - 2 * a] != color) || (aRecordBoard[i - 2 * b][j - 2 * a] == 0 && aRecordBoard[i + 4 * b][j + 4 * a] != color)))
						{
							flex3++; //活三+●●●++或++●●●+
							return;
						}
						//一侧越界
						else if ((i - 2 * b <= 1 || i - 2 * b >= 15 || j - 2 * a <= 1 || j - 2 * a >= 15) && (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15) && aRecordBoard[i + 4 * b][j + 4 * a] == 0)
						{
							flex3++; //活三|+●●●++
							return;
						}
						//另一侧越界
						else if ((i + 4 * b <= 1 || i + 4 * b >= 15 || j + 4 * a <= 1 || j + 4 * a >= 15) && (i - 2 * b >= 1 && i - 2 * b <= 15 && j - 2 * a >= 1 && j - 2 * a <= 15) && aRecordBoard[i - 2 * b][j - 2 * a] == 0)
						{
							flex3++; //活三++●●●+|
							return;
						}
					}
				}
				else if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == 0)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
					{
						if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == 0)
						{
							flex3++; //跳活三+●●+●+
							return;
						}
					}
				}
			}
			else if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
					{
						if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == 0)
						{
							flex3++; //跳活三+●+●●+
							return;
						}
					}
				}
			}
		}
	} //活三和跳活三
	if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && (aRecordBoard[i - b][j - a] != color && aRecordBoard[i - b][j - a] != 0) || (i - b == 0) || (j - a == 0))
	{
		if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
		{
			if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == color)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == 0)
					{
						if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == 0)
						{
							block3++; //一端眠三◎●●●++
							return;
						}
					}
				}
			}
		}
	}
	else if (i - 2 * b >= 1 && i - 2 * b <= 15 && j - 2 * a >= 1 && j - 2 * a <= 15 && (aRecordBoard[i - 2 * b][j - 2 * a] == 0))
	{
		if (i - b >= 1 && i - b <= 15 && j - a >= 1 && j - a <= 15 && (aRecordBoard[i - b][j - a] == 0) && (i - b != 0) && (j - a != 0))
		{
			if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
			{
				if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == color)
				{
					if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
					{
						if ((i + 2 * b == 15) || (j + 2 * a == 15) || (aRecordBoard[i + 3 * b][j + 3 * a] != color && (aRecordBoard[i + 3 * b][j + 3 * a] != 0)))
						{
							block3++; //一端眠三++●●●◎
							return;
						}
					}
				}
			}
		}
	} //一端眠三
	if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && (aRecordBoard[i - b][j - a] != color && aRecordBoard[i - b][j - a] != 0) || (i - b == 0) || (j - a == 0))
	{
		if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
		{
			if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == color)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == 0)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
					{
						if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == 0)
						{
							block3++; //一跳眠三◎●●+●+
							return;
						}
					}
				}
			}
			else if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
					{
						if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == 0)
						{
							block3++; //二跳眠三◎●+●●+
							return;
						}
					}
				}
			}
		}
	}
	else if (i - b >= 1 && i - b <= 15 && j - a >= 1 && j - a <= 15 && (aRecordBoard[i - b][j - a] == 0))
	{
		if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
		{
			if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
					{
						if ((i + 3 * b == 15) || (j + 3 * a == 15) || (aRecordBoard[i + 4 * b][j + 4 * a] != color && aRecordBoard[i + 4 * b][j + 4 * a] != 0))
						{
							block3++; //一跳眠三+●+●●◎
							return;
						}
					}
				}
			}
			else if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == color)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == 0)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
					{
						if ((i + 3 * b == 15) || (j + 3 * a == 15) || (aRecordBoard[i + 4 * b][j + 4 * a] != color && aRecordBoard[i + 4 * b][j + 4 * a] != 0))
						{
							block3++; //二跳眠三+●●+●◎
							return;
						}
					}
				}
			}
		}
	} //一跳眠三和二跳眠三
	if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
	{
		if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
		{
			if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == 0)
			{
				if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
				{
					if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == color)
					{
						block3++; //空二眠三●++●●
						return;
					}
				}
			}
		}
		else if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == color)
		{
			if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == 0)
			{
				if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == 0)
				{
					if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == color)
					{
						block3++; //空二眠三●●++●
						return;
					}
				}
			}
		}
	} //空二眠三
	if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
	{
		if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
		{
			if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
			{
				if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == 0)
				{
					if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == color)
					{
						block3++; //间二眠三●+●+●
						return;
					}
				}
			}
		}
	} //间二眠三
	if (i - b >= 1 && i - b <= 15 && j - a >= 1 && j - a <= 15 && ((i - b == 1) || (j - a == 1) || (aRecordBoard[i - 2 * b][j - 2 * a] != color && aRecordBoard[i - 2 * b][j - 2 * a] != 0)))
	{
		if ((aRecordBoard[i - b][j - a] == 0) && (i - b != 0) && (j - a != 0))
		{
			if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
			{
				if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == color)
				{
					if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
					{
						if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == 0)
						{
							if ((i + 3 * b == 15) || (j + 3 * a == 15) || (aRecordBoard[i + 4 * b][j + 4 * a] != color && aRecordBoard[i + 4 * b][j + 4 * a] != 0))
							{
								block3++; //二端眠三◎+●●●+◎
								return;
							}
						}
					}
				}
			}
		}
	} //二端眠三
	if (i - 2 * b >= 1 && i - 2 * b <= 15 && j - 2 * a >= 1 && j - 2 * a <= 15 && aRecordBoard[i - 2 * b][j - 2 * a] == 0)
	{
		if (i - b >= 1 && i - b <= 15 && j - a >= 1 && j - a <= 15 && aRecordBoard[i - b][j - a] == 0)
		{
			if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
			{
				if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == color)
				{
					if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == 0)
					{
						if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == 0)
						{
							flex2++; //双端活二++●●++
							return;
						}
					}
				}
			}
		}
	} //双端活二
	if (i - b >= 1 && i - b <= 15 && j - a >= 1 && j - a <= 15 && (aRecordBoard[i - b][j - a] == 0))
	{
		if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
		{
			if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == 0)
					{
						flex2++; //间一活二+●+●+
						return;
					}
				}
			}
		}
	} //间一活二
	if (i - b >= 1 && i - b <= 15 && j - a >= 1 && j - a <= 15 && aRecordBoard[i - b][j - a] == 0)
	{
		if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
		{
			if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == 0)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
					{
						if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == 0)
						{
							block2++; //间二活二+●++●+
							return;
						}
					}
				}
			}
		}
	} //间二活二
	if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && (aRecordBoard[i - b][j - a] != color && aRecordBoard[i - b][j - a] != 0) || (i - b == 0) || (j - a == 0))
	{
		if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
		{
			if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == color)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == 0)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == 0)
					{
						if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == 0)
						{
							block2++; //一端眠二◎●●+++
							return;
						}
					}
				}
			}
		}
	}
	else if (i - b >= 1 && i - b <= 15 && j - a >= 1 && j - a <= 15 && (aRecordBoard[i - b][j - a] == 0))
	{
		if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == 0)
		{
			if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
					{
						if ((i + 3 * b == 15) || (j + 3 * a == 15) || (aRecordBoard[i + 4 * b][j + 4 * a] != color && aRecordBoard[i + 4 * b][j + 4 * a] != 0))
						{
							block2++; //一端眠二+++●●◎
							return;
						}
					}
				}
			}
		}
	} //一端眠二
	if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && (aRecordBoard[i - b][j - a] != color && aRecordBoard[i - b][j - a] != 0) || (i - b == 0) || (j - a == 0))
	{
		if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
		{
			if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == 0)
					{
						if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == 0)
						{
							block2++; //间一眠二◎●+●++
							return;
						}
					}
				}
			}
		}
	}
	else if (i - 2 * b >= 1 && i - 2 * b <= 15 && j - 2 * a >= 1 && j - 2 * a <= 15 && (aRecordBoard[i - b][j - a] == 0))
	{
		if (i - b >= 1 && i - b <= 15 && j - a >= 1 && j - a <= 15 && (aRecordBoard[i - b][j - a] == 0))
		{
			if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
			{
				if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
				{
					if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == color)
					{
						if ((i + 2 * b == 15) || (j + 2 * a == 15) || (aRecordBoard[i + 3 * b][j + 3 * a] != color && aRecordBoard[i + 3 * b][j + 3 * a] != 0))
						{
							block2++; //间一眠二++●+●◎
							return;
						}
					}
				}
			}
		}
	} //间一眠二
	if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && (aRecordBoard[i - b][j - a] != color && aRecordBoard[i - b][j - a] != 0) || (i - b == 0) || (j - a == 0))
	{
		if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
		{
			if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == 0)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
					{
						if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == 0)
						{
							block2++; //间二眠二◎●++●+
							return;
						}
					}
				}
			}
		}
	}
	else if (i - b >= 1 && i - b <= 15 && j - a >= 1 && j - a <= 15 && (aRecordBoard[i - b][j - a] == 0))
	{
		if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
		{
			if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
			{
				if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == 0)
				{
					if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == color)
					{
						if ((i + 3 * b == 15) || (j + 3 * a == 15) || (aRecordBoard[i + 4 * b][j + 4 * a] != color && aRecordBoard[i + 4 * b][j + 4 * a] != 0))
						{
							block2++; //间二眠二+●++●◎
							return;
						}
					}
				}
			}
		}
	} //间二眠二
	if (i >= 1 && i <= 15 && j >= 1 && j <= 15 && aRecordBoard[i][j] == color)
	{
		if (i + b >= 1 && i + b <= 15 && j + a >= 1 && j + a <= 15 && aRecordBoard[i + b][j + a] == 0)
		{
			if (i + 2 * b >= 1 && i + 2 * b <= 15 && j + 2 * a >= 1 && j + 2 * a <= 15 && aRecordBoard[i + 2 * b][j + 2 * a] == 0)
			{
				if (i + 3 * b >= 1 && i + 3 * b <= 15 && j + 3 * a >= 1 && j + 3 * a <= 15 && aRecordBoard[i + 3 * b][j + 3 * a] == 0)
				{
					if (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && aRecordBoard[i + 4 * b][j + 4 * a] == color)
					{
						block2++; //间三眠二●+++●
						return;
					}
				}
			}
		}
	} //间三眠二
}
