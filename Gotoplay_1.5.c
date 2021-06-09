#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FLUSH while (getchar() != '\n')

#define SIZE 15
#define CHARSIZE 2

#define SEARCHDEPTHTIME 6 //��������
#define SERIESDEPTH 10	//��ɱ����

#define ROOTNUMBER 15 //�������ڵ���
#define CHILDNUMBER 6 //�����ӽڵ���

#ifndef max
#define max(a, b) ((a > b) ? a : b)
#endif

#ifndef min
#define min(a, b) ((a < b) ? a : b)
#endif

#define oppcolor ((color == 1) ? 2 : 1)

//ȫ�ֱ�����
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
		"+ + + + + + + + + + + + + + + "}; //����ʹ��Unicode���ģ�һ������ռ����char������Ҫ*2��+1��Ϊ��ĩβ��'\0'
char play1Pic[] = "��";		//"��"����;
char play2Pic[] = "��";		//"��"����;
char play1CurrentPic[] = "��";
char play2CurrentPic[] = "��";
char aDisplayBoardArray[SIZE][SIZE * CHARSIZE + 1]; //������������ʾ
int aRecordBoard[SIZE + 1][SIZE + 1];				//���������ڼ�¼��λ ��(1,1)��ʼ,��¼λ�ü���������

struct st
{
	int x;
	int y;
} step[SIZE * SIZE + 1];

struct po
{
	int x;
	int y;
	int attackpriority;				  //��������
	int defencepriority;			  //��������
	int compriority;				  //�ۺ�����
} testpoint[SIZE * SIZE + 1],		  //ɨ���д�����ʱ��
	possiblepoint[SIZE * SIZE + 1],   //Genratepoint�еĿ��е�
	rootpoint[SIZE * SIZE + 1],		  //Alphabetaɨ���еĸ���
	childpoint[17][SIZE * SIZE + 1],  //ɨ���и����
	attackpoint[SIZE * SIZE + 1],	 //Attackpoint�еĿ��е�
	attackrootpoint[SIZE * SIZE + 1], //seriessearch�еĸ���
	bestpoint;						  //������ӵ�

int stepnumber = 0;										   //�岽��¼
int upside = 0, rightside = 0, downside = 0, leftside = 0; //�������ӷ��α߽�

int flex6, flex5, flex4, flex3, flex2, block4, block3, block2; //�ֱ��¼��������

int temi; //����Alphabeta�л���һ��i

//����������
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

//������
int main()
{
	do
	{
		printf("��������:������\n");
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

//���˶˿�
void pvp(void)
{
	initRecordBorard();		//��ʼ����¼����aRecordBoard
	recordtoDisplayArray(); //������λ�ö������ģ��
	displayBoard();			//��ӡ����
	while (1)
	{
		//��������
		manset(1); //�˿غ�������(��������)
		pointsettype(1, step[stepnumber].x, step[stepnumber].y);
		recordtoDisplayArray();
		displayBoard(); //��ӡ����
		if (judgeforbidenmove() == 1)
		{
			printf("Forbidenmove.White win!");
		}
		if (judgewin() == 1)
		{
			printf("Black win!\n");
			break;
		} //�ж���Ӯ(����stepnumberΪ����)(���ж��򷵻�1,δ�ж��򷵻�0)
		//��������
		manset(2);
		pointsettype(2, step[stepnumber].x, step[stepnumber].y);
		recordtoDisplayArray();
		displayBoard(); //��ӡ����
		if (judgewin() == 1)
		{
			printf("White win!\n");
			break;
		} //�ж���Ӯ(����stepnumberΪ����)(���ж��򷵻�1,δ�ж��򷵻�0)
	}
}

//�˻��˿�
void pvc(void)
{
	int returnchooseside = chooseside();
	if (returnchooseside == 1)
	{
		initRecordBorard();		//��ʼ����¼����aRecordBoard
		recordtoDisplayArray(); //������λ�ö������ģ��
		displayBoard();			//��ӡ����
		while (1)
		{
			manset(1);
			pointsettype(1, step[stepnumber].x, step[stepnumber].y);
			recordtoDisplayArray();
			displayBoard(); //��ӡ����
			if (judgeforbidenmove() == 1)
			{
				printf("Forbidenmove.White win!");
				getchar();
			}
			if (judgewin() == 1)
			{
				printf("Black win!\n");
				break;
			} //�ж���Ӯ(����stepnumberΪ����)(���ж��򷵻�1,δ�ж��򷵻�0)
			aiset(2);
			pointsettype(2, step[stepnumber].x, step[stepnumber].y);
			recordtoDisplayArray();
			displayBoard(); //��ӡ����
			if (judgewin() == 1)
			{
				printf("White win!\n");
				break;
			} //�ж���Ӯ(����stepnumberΪ����)(���ж��򷵻�1,δ�ж��򷵻�0)
		}
	}
	else if (returnchooseside == 2)
	{
		initRecordBorard();		//��ʼ����¼����aRecordBoard
		recordtoDisplayArray(); //������λ�ö������ģ��
		displayBoard();			//��ӡ����
		while (1)
		{
			aiset(1);
			pointsettype(1, step[stepnumber].x, step[stepnumber].y);
			recordtoDisplayArray();
			displayBoard(); //��ӡ����
			if (judgewin() == 1)
			{
				printf("Black win!\n");
				break;
			} //�ж���Ӯ(����stepnumberΪ����)(���ж��򷵻�1,δ�ж��򷵻�0)
			manset(2);
			pointsettype(2, step[stepnumber].x, step[stepnumber].y);
			recordtoDisplayArray();
			displayBoard(); //��ӡ����
			if (judgewin() == 1)
			{
				printf("White win!\n");
				break;
			} //�ж���Ӯ(����stepnumberΪ����)(���ж��򷵻�1,δ�ж��򷵻�0)
		}
	}
}

//ai���Ӷ˿�
void aiset(int color)
{
	//��ʼ��һ��(ִ��)
	if (stepnumber == 0)
	{
		setpoint(color, 8, 8);
		sidesize();
	}
	//�ڶ���(ִ��)���ں����Ա�
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
	//������(ִ��)
	else if (stepnumber == 2)
	{
		int rx = 0, ry = 0;
		//�������ֱָ,�������Ա�(����)
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
		//�������бָ,��������λ(����)
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
		//�������û�������м�����
		else
		{
			rootpointsearch(color, SEARCHDEPTHTIME, -1000000, 1000000);
			setpoint(color, bestpoint.x, bestpoint.y);
			sidesize();
		}
	}
	//���ಽ�������������
	else if (stepnumber >= 2 && stepnumber <= 10)
	{
		rootpointsearch(color, SEARCHDEPTHTIME, -1000000, 1000000);
		setpoint(color, bestpoint.x, bestpoint.y);
		sidesize();
	}
	else
	{
		//������ɱ
		if (rootserieswin(color, SERIESDEPTH))
		{
			setpoint(color, bestpoint.x, bestpoint.y);
			sidesize();
			return;
		}
		//δ����ɼ�����ɱ,������µ���Ŀ����(�б����������),ִ�м�֦����
		if (findGenaratepoint(color) < 12)
		{
			rootpointsearch(color, SEARCHDEPTHTIME, -1000000, 1000000);
			setpoint(color, bestpoint.x, bestpoint.y);
			sidesize();
			return;
		}
		//�Է���ɱ
		if (rootserieswin(oppcolor, SERIESDEPTH))
		{

			setpoint(color, bestpoint.x, bestpoint.y);
			sidesize();
			return;
		}
		//δ����ɱ����ɱ,���������㷨
		rootpointsearch(color, SEARCHDEPTHTIME, -1000000, 1000000);
		setpoint(color, bestpoint.x, bestpoint.y);
		sidesize();
	}
}

//�����������
void rootpointsearch(int color, int depth, int alpha, int beta)
{
	int rootcount;
	rootcount = findGenaratepoint(color); //�ҵ��������ӵ㲢��¼��Ŀ
	if (rootcount == 1)					  //���ֻ��һ������,ֱ������
	{
		bestpoint = possiblepoint[1];
	}
	else //�����������
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
			rootpoint[i].compriority = alphabeta(depth - 1, alpha, beta, color, 0); //�ݹ����
			val = max(val, rootpoint[i].compriority);
			alpha = max(alpha, val);
			backwards();
		}
		sortquick(rootpoint, 1, min(ROOTNUMBER, rootcount));
		bestpoint = rootpoint[1];
	}
}

//alphabeta��֦
int alphabeta(int depth, int alpha, int beta, int color, int computerplay)
{
	if (depth == 0 || (childpoint[depth + 1][temi].compriority >= 1200 && (computerplay == 1))) //���Ϊ0��Ӯ��ֱ�ӷ���
	{
		return evaluateboard(color); //���ع�ֵ
	}
	if (computerplay == 1)
	{
		int val = -1000000;
		int childcount = 0;
		childcount = findGenaratepoint(color); //�ҵ����½ڵ�
		for (int i = 1; i <= childcount; i++)
		{
			childpoint[depth][i] = possiblepoint[i];
		}
		for (int i = 1; i <= CHILDNUMBER && i <= childcount; i++)
		{
			temi = i;
			setpoint(color, childpoint[depth][i].x, childpoint[depth][i].y); //�µ�
			sidesize();
			val = max(val, alphabeta(depth - 1, alpha, beta, color, 0)); //����ȡ���
			alpha = max(alpha, val);
			backwards();
			if (beta <= alpha) //alpha����
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
			val = min(val, alphabeta(depth - 1, alpha, beta, color, 1)); //�Է�ȡ��С
			beta = min(beta, val);
			backwards();
			if (beta <= alpha) //beta����
			{
				break;
			}
		}
		return val;
	}
}

//��ɱ�������
int rootserieswin(int color, int depth)
{
	int attackcount = 0;
	attackcount = findattackpoint(color); //�ҵ���ѽ�����(������)
	for (int i = 1; i <= attackcount; i++)
	{
		attackrootpoint[i] = attackpoint[i];
	}
	for (int i = 1; i <= attackcount; i++)
	{
		setpoint(color, attackrootpoint[i].x, attackrootpoint[i].y);
		sidesize();
		if (serieswin(depth - 1, color, 0)) //�ݹ����
		{
			//������ɱ�򿽱���bestpoint
			bestpoint = attackrootpoint[i];
			backwards();
			return 1;
		}
		backwards();
	}
		return 0;
}

//��ɱ
int serieswin(int depth, int color, int computerplay)
{
	int evaluate = 0;
	//��������ʱ,ʤ������1,ʧ�ܷ���0
	if (computerplay)
	{
		evaluate = evaluateboard(color);
		if (evaluate >= 8000) //����ʤ��
		{
			return 1;
		}
		else if (evaluate == -10000) //��������
		{
			return 0;
		}
	}
	//�Է�����ʱ,�Է�ʤ������0,ʧ�ܷ���1
	else
	{
		evaluate = evaluateboard(oppcolor);
		if (evaluate == -10000) //�Է�����
		{
			return 1;
		}
		else if (evaluate >= 8000) //�Է���ʤ
		{
			return 0;
		}
	}
	if (depth == 0) //��ȷ���
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
			win = win || serieswin(depth - 1, color, 0); //�ҵ�һ�㼴��ɱ�ɹ�
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
			win = win && serieswin(depth - 1, color, 1); //һ��ɱ���������ɱʧ��
			backwards();
			if (!win)
			{
				break;
			}
		}
		return win;
	}
}

//�ҵ��γ�ɱ��λ��(���ڻ���)
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
			if (tempval >= 200 || flex3 > 0 || block4 > 0) //������ɱ��
			{
				attackcount++;
				attackpoint[attackcount].x = j;
				attackpoint[attackcount].y = i;
				attackpoint[attackcount].compriority = tempval;
			}
			setpoint(oppcolor, j, i);
			pointsettype(oppcolor, j, i);
			backwards();
			if (flex5 > 0) //�Է��³���ĵ��Լ�ҲҪ��!!!(��ֹ�Է��³ɳ���,ԭ��������ɱ,����ɱ��Ϊ�³ɳ���,���������������Ѳ����Է������,����Ϊ������,������ɱʧ�ܵ����)!!!
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

//�ҵ�������Ҫ�ڵ�
int findGenaratepoint(int color)
{
	int count = 0, movecount = 0;
	int tempval = 0;
	for (int i = upside - 3; i <= downside + 3; i++)
	{
		for (int j = leftside - 3; j <= rightside + 3; j++)
		{
			if (judgerepetset(j, i) != 0) //�Ƿ��ظ�
			{
				continue;
			}
			setpoint(color, j, i);
			pointsettype(color, j, i);
			backwards();
			if (color == 1 && judgeforbidenmove() != 0) //�Ƿ����
			{
				continue;
			}
			tempval = evaluatepoint(color, j, i);
			if (tempval > 0) //�洢������ĵ�
			{
				count++;
				testpoint[count].x = j;
				testpoint[count].y = i;
				testpoint[count].compriority = tempval;
			}
		}
	}
	sortquick(testpoint, 1, count); //�Ե�����
	movecount = forcemove(count);   //�Ƿ���ǿ�����ӵ�
	if (movecount == 0)
	{
		for (int i = 1; i <= count; i++)
		{
			movecount++;
			possiblepoint[i] = testpoint[i]; //ת�浽possiblepoint
		}
	}
	return movecount; //���ص�����
}

//����������
int forcemove(int count)
{
	//������������,ֱ�ӷ���
	if (testpoint[1].compriority >= 2400)
	{
		possiblepoint[1] = testpoint[1];
		return 1;
	}
	int movecount = 0;
	//�Է��л���
	if (testpoint[1].compriority == 1200)
	{
		//�ҶԷ���һ���ܻ��ĵĵ�
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
		//��˫����һ���ܳ��ĵĵ�
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

//ȫ�̹�ֵ
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

	// ��ǰ�����ֵ���������
	// 1.������������㣬��ʤ
	// 2.�Է�������������㣬�޷��赲���ذ�
	// 3.�Է����ܳ��壬�������ڻ��ĵ㣬��ʤ
	// 4.����������������,�Է��޳����,��ʤ
	// 5.����˫��,�Է��޳���,�����,��ʤ
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

	// û�зֳ�ʤ�����ͼ�������ֵ
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

	//��Ϊ�ֵ������£����Լ������͸������������Գ���1.2
	return (colorscore * 1.2) - oppcolorscore;
}

//�����ֵ
int evaluatepoint(int color, int x, int y)
{
	int attackpriority, defencepriority;
	setpoint(color, x, y);
	pointsettype(color, x, y);
	backwards();
	attackpriority = pointpriority(); //�õ������
	setpoint(oppcolor, x, y);
	pointsettype(oppcolor, x, y);
	backwards();
	defencepriority = pointpriority(); //�õ���ط�
	if (attackpriority >= 200 || defencepriority >= 200)
	{
		return max((attackpriority * 2), defencepriority);
	}
	else
	{
		return (attackpriority * 2) + defencepriority;
	}
}

//������ȼ�
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

//������
void manset(int color)
{
	int temx, temy;
	char c;
	while (1)
	{
		scanf("%d %c", &temy, &c); //����
		temy = 16 - temy;
		temx = c - 'A' + 1;
		FLUSH;
		int returnjudgerepetset = judgerepetset(temx, temy);
		if (returnjudgerepetset == 1) //�Ƿ��ظ�
		{
			printf("It has been set. Please reset.\n");
			continue;
		}
		else if (returnjudgerepetset == 2) //�Ƿ�Խ��
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

} //��������+�жϽ���+�ж��ظ������(judgerepetset����0Ϊ���ظ�)

//�жϽ���
int judgeforbidenmove()
{
	if (flex6 > 0 || flex4 + block4 > 1 || flex3 > 1) //һ��˫��,һ��˫��,һ�ӳ��������
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//�ж���Ӯ
int judgewin()
{
	if (flex6 > 0 || flex5 > 0) //���������Ӯ
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//�ж�Խ����ظ�
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

//ѡģʽ
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

//�˻�ѡ�ߺ���(���ߺ���return 1,���߰���return 2.)
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

//�Ƿ����
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

//���ñ߽�(��ʱ���õ�ǰ���̱߽�,��֦��Ҫ�ɷ�)
void sidesize()
{
	upside = min(step[stepnumber].y, upside);
	downside = max(step[stepnumber].y, downside);
	rightside = max(step[stepnumber].x, rightside);
	leftside = min(step[stepnumber].x, leftside);
}

//��������(��compority)
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

//����
void setpoint(int color, int x, int y)
{
	stepnumber++;
	step[stepnumber].x = x;
	step[stepnumber].y = y;
	aRecordBoard[step[stepnumber].y][step[stepnumber].x] = color;
}

//����
void backwards()
{
	aRecordBoard[step[stepnumber].y][step[stepnumber].x] = 0;
	step[stepnumber].x = 0;
	step[stepnumber].y = 0;
	stepnumber--;
}

//��ʼ������
void initRecordBorard(void)
{
	stepnumber = 0; //��ʼ������
		//ͨ��˫��ѭ������aRecordBoard��0
	for (int i = 0; i <= SIZE + 1; i++)
		for (int j = 0; j <= SIZE + 1; j++)
			aRecordBoard[i][j] = 0;
}

//��¼����λ�ú���
void recordtoDisplayArray(void)
{
	//��aRecordBoard�м�¼������λ�ã�ת����aDisplayBoardArray��
	int i, j;
	//��һ������aInitDisplayBoardArray�м�¼�Ŀ����̣����Ƶ�aDisplayBoardArray��
	for (i = 0; i < SIZE; i++)
	{
		for (j = 0; j <= SIZE * CHARSIZE + 1; j++)
			aDisplayBoardArray[i][j] = aInitDisplayBoardArray[i][j];
		aDisplayBoardArray[i][j] = '\0';
	}
	//�ڶ�����ɨ��aRecordBoard����������0��Ԫ�أ�������ߡ��Ƶ�aDisplayBoardArray����Ӧλ����
	for (i = 0; i < SIZE; i++)
	{
		for (j = 0; j < SIZE; j++)
		{
			if (aRecordBoard[i + 1][j + 1] == 1)
			{
				aDisplayBoardArray[i][CHARSIZE * j] = play1Pic[0];
				aDisplayBoardArray[i][CHARSIZE * j + 1] = play1Pic[1];
				//ע�⣺aDisplayBoardArray����¼���ַ��������ַ���ÿ���ַ�ռ2���ֽڡ���͡�Ҳ�������ַ���ÿ��Ҳռ2���ֽڡ�
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

//��ʾ���̺���
void displayBoard(void)
{
	int i;
	//��һ��������
	system("cls");
	//�ڶ�������aDisplayBoardArray�������Ļ��
	printf("\n");
	for (i = 0; i < SIZE; i++)
		printf("%3d %s\n", 15 - i, aDisplayBoardArray[i]); //��ӡ�������кź�����
	printf("   ");
	for (i = 0; i < SIZE; i++)
		printf("%2c", 'A' + i); //��ӡ���к�
	printf("\n");
	for (i = 1; i <= stepnumber; i++)
	{
		printf("%d. %s:(%d,%c)%c", i, (i % 2 == 0) ? "white" : "black", 16 - step[i].y, step[i].x + 'A' - 1, (i % 2 == 0) ? '\n' : '\t');
	} //��ӡ��ʷ����

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

//ɨ�����κ���
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
	//ɨ���ĸ�����
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

//ɨ��ĳ������ĳ�������γɵ�������
void pointtypescan(int color, int i, int j, int a, int b) //color:1Ϊ����,2Ϊ����
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
							flex6++; //�����������
							return;
						}
						else
						{
							flex5++; //���������
							return;
						}
					}
				}
			}
		}
	} //����������
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
							flex4++; //����+�����+
							return;
						}
						else if (((i + 3 * b == 15) || (j + 3 * a == 15)) || (aRecordBoard[i + 4 * b][j + 4 * a] != color && aRecordBoard[i + 4 * b][j + 4 * a] != 0))
						{
							block4++; //һ�˳���+������
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
							block4++; //һ�˳��ġ�����+
							return;
						}
					}
				}
			}
		}
	} //���ĺ�һ�˳���
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
						block4++; //��ƫ���ġ���+��
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
						block4++; //�в����ġ��+���
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
						block4++; //��ƫ���ġ�+����
						return;
					}
				}
			}
		}
	} //�в����ĺ���ƫ����
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
						//�������
						if (i - 2 * b >= 1 && i - 2 * b <= 15 && j - 2 * a >= 1 && j - 2 * a <= 15 && i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15 && ((aRecordBoard[i + 4 * b][j + 4 * a] == 0 && aRecordBoard[i - 2 * b][j - 2 * a] != color) || (aRecordBoard[i - 2 * b][j - 2 * a] == 0 && aRecordBoard[i + 4 * b][j + 4 * a] != color)))
						{
							flex3++; //����+����++��++����+
							return;
						}
						//һ��Խ��
						else if ((i - 2 * b <= 1 || i - 2 * b >= 15 || j - 2 * a <= 1 || j - 2 * a >= 15) && (i + 4 * b >= 1 && i + 4 * b <= 15 && j + 4 * a >= 1 && j + 4 * a <= 15) && aRecordBoard[i + 4 * b][j + 4 * a] == 0)
						{
							flex3++; //����|+����++
							return;
						}
						//��һ��Խ��
						else if ((i + 4 * b <= 1 || i + 4 * b >= 15 || j + 4 * a <= 1 || j + 4 * a >= 15) && (i - 2 * b >= 1 && i - 2 * b <= 15 && j - 2 * a >= 1 && j - 2 * a <= 15) && aRecordBoard[i - 2 * b][j - 2 * a] == 0)
						{
							flex3++; //����++����+|
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
							flex3++; //������+���+��+
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
							flex3++; //������+��+���+
							return;
						}
					}
				}
			}
		}
	} //������������
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
							block3++; //һ�����������++
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
							block3++; //һ������++�����
							return;
						}
					}
				}
			}
		}
	} //һ������
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
							block3++; //һ����������+��+
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
							block3++; //�����������+���+
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
							block3++; //һ������+��+����
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
							block3++; //��������+���+���
							return;
						}
					}
				}
			}
		}
	} //һ�������Ͷ�������
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
						block3++; //�ն�������++���
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
						block3++; //�ն��������++��
						return;
					}
				}
			}
		}
	} //�ն�����
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
						block3++; //���������+��+��
						return;
					}
				}
			}
		}
	} //�������
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
								block3++; //����������+����+��
								return;
							}
						}
					}
				}
			}
		}
	} //��������
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
							flex2++; //˫�˻��++���++
							return;
						}
					}
				}
			}
		}
	} //˫�˻��
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
						flex2++; //��һ���+��+��+
						return;
					}
				}
			}
		}
	} //��һ���
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
							block2++; //������+��++��+
							return;
						}
					}
				}
			}
		}
	} //������
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
							block2++; //һ���߶�����+++
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
							block2++; //һ���߶�+++����
							return;
						}
					}
				}
			}
		}
	} //һ���߶�
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
							block2++; //��һ�߶����+��++
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
							block2++; //��һ�߶�++��+���
							return;
						}
					}
				}
			}
		}
	} //��һ�߶�
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
							block2++; //����߶����++��+
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
							block2++; //����߶�+��++���
							return;
						}
					}
				}
			}
		}
	} //����߶�
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
						block2++; //�����߶���+++��
						return;
					}
				}
			}
		}
	} //�����߶�
}
