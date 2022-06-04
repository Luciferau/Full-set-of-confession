#include <stdio.h>
#include <conio.h>
#include <easyx.h>
#include<mmsystem.h>
#include <graphics.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <cmath>
#include <Windows.h>
#pragma comment (lib, "WS2_32.lib")
#pragma comment(lib,"winmm.lib")

#define SNOW_COUNT_MAX 200
#define SERVER_PORT  2021
#define MUN 10
#define NUM 13
#define PI 3.1415
#define WIDTH 1200
#define HEIGHT 800

IMAGE img_bk[3];
IMAGE img_meteor[3];
IMAGE jetImg;
DWORD* pMem;
IMAGE love;
/*
 SOCKADDR client;
int nSize = sizeof(client);
SOCKET clientSock;
SOCKET serverSocket;
sockaddr_in sockAddr;  
char buff[4096];
* 
*/


SOCKET webInitTCP(const char* ip, sockaddr_in* addr, unsigned short port);

 struct vector2 
 {
	vector2(int _x = 0, int _y = 0) :x(_x), y(_y) 
	{

	}

	vector2(int* data) :x(data[0]), y(data[1]) 
	{

	}
	long long x;
	long long y;

};

//流星结构体
struct Meteor //流星
{
	int x;
	int y;
	int speed;//速度
};


struct Meteor meteor[NUM];

 // 烟花结构
struct Fire
{
	int r;					// 当前爆炸半径
	int max_r;				// 爆炸中心距离边缘最大半径
	int x, y;				// 爆炸中心在窗口的坐标
	int cent2LeftTopX, cent2LeftTopY;		// 爆炸中心相对图片左上角的坐标
	int width, height;		// 图片的宽高
	int pix[240][240];		// 储存图片像素点
	bool show;				// 是否绽放
	bool draw;				// 开始输出像素点
	DWORD t1, t2, dt;		// 绽放速度
}fires[NUM];


// 烟花弹结构
struct Bullet
{
	int x, y;				// 烟花弹的当前坐标
	int topX, topY;				// 最高点坐标------将赋值给 FIRE 里面的 x, y
	int height;				// 烟花高度
	bool shoot;				// 是否可以发射

	DWORD t1, t2, dt;		// 发射速度
	IMAGE img[2];			// 储存花弹一亮一暗图片
	unsigned char n : 1;	// 图片下标 n++
}bullets[NUM];

struct snow
{
	vector2 startPoint;
	vector2 endPoint;
	vector2 p1;//x,y,z,
	vector2 p2;
	vector2 curPos;
	float t;//0,,1
	bool visible;
	bool isHeart;
	bool hearted; //已经构成心形 int heartTime;
	int heartTime;
};

struct snow snows[SNOW_COUNT_MAX];

const char* biaobai[] = 
{
	"I",
	"LOVE",
	"YOU"
	
};
int heartPoints[][2] = {
	1007, 180, 981,164,
	959,151, 938,138, 919,
	133, 904,128, 892,128,
	877,128, 861,130, 837,
	138, 818,147, 806,158,
	794,170, 782,186, 775,
	200, 770,219, 767,236,
	763,259, 764,277,768,301,
	773,321, 782,342, 792,359,
	806,376, 820,392, 834,407,
	850,419, 867,431, 880,443,
	897,454, 914,464, 931,474,
	946,485, 964,495, 979,505,
	1002,515, 1022,507, 1042,496,
	1058,488, 1076,478, 1093,468,
	1112,456, 1131,442, 1148,429,
	1167,415, 1188,398, 1204,377,
	1219,358, 1231,336, 1241,309,
	1245,282, 1247,254, 1244,231,
	1237,205, 1227,185, 1210,164,
	1190,149, 1166,136, 1142,130,
	1118,129, 1094,132, 1074,137,
	1054,147, 1039,157, 1024,169 
};

//加法
vector2 operator +(vector2 x, vector2 y) 
{
	return vector2(x.x + y.x, x.y + y.y);
}

//减法
vector2 operator -(vector2 x, vector2 y) 
{
	return vector2(x.x - y.x, x.y - y.y);
}

// 乘法
vector2 operator *(vector2 x, vector2 y) 
{
	return vector2(x.x * y.x - x.y * y.y, 
		x.y * y.x + x.x * y.y);
}

// 乘法
vector2 operator *(float x, vector2 y)
{
	return vector2(x * y.x, x * y.y);
}

//叉积
long long cross(vector2 x, vector2 y)
{
	return x.y * y.x - x.x * y.y;
}

//数量积 点积
long long dot(vector2 x, vector2 y)
{
	return x.x * y.x + x.y * y.y;
}

//四舍五入除法
long long dv(long long a, long long b) 
{	//注意重名！！！ 
	return b < 0 ? dv(-a, -b)
		: (a < 0 ? -dv(-a, b)
			: (a + b / 2) / b);
}

//模长平方
long long len(vector2 x) 
{
	return x.x * x.x + x.y * x.y;

}

//模长
long long dis(vector2 x) 
{
	return sqrt(x.x * x.x + x.y * x.y); 

}

//向量除法
vector2 operator /(vector2 x, vector2 y) 
{
	long long l = len(y);
	return vector2(dv(dot(x, y), l), 
		dv(cross(x, y), l));
}

//向量膜
vector2 operator %(vector2 x, vector2 y) 
{ 
	return x - ((x / y) * y);

}

//向量GCD 
vector2 gcd(vector2 x, vector2 y) 
{
	return len(y) ? gcd(y, x % y) : x;

}


//塞贝尔曲线
vector2 calcBezierPoint(float t, vector2 p0, vector2 p1, vector2 p2, vector2 p3)
{
	float u = 1 - t;
	float tt = t * t;
	float uu = u * u;
	float uuu = uu * u;
	float ttt = tt * t;

	vector2 p = uuu * p0;
	p = p + 3 * uu * t * p1;
	p = p + 3 * u * tt * p2;
	p = p + ttt * p3;

	return p;
}


SOCKET webInitTCP(const char* ip, sockaddr_in* addr, unsigned short port)
{
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (err != 0)
	{
		return -1;
	}

	SOCKET retSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	addr->sin_family = PF_INET;
	addr->sin_addr.S_un.S_addr = (ip == 0) ? 0 : inet_addr(ip);
	addr->sin_port = htons(port);

	return retSocket;
}

int getDelay() {
	static unsigned long long lastTime = 0;
	unsigned long long currentTime = GetTickCount();
	if (lastTime == 0) 
	{
		lastTime = currentTime;
		return 0;
	}

	else 
	{
		int ret = currentTime - lastTime;
		lastTime = currentTime;
		return ret;
	}
}


// 载入PNG图并去透明部分
void putimagePNG(int  picture_x, int picture_y, IMAGE* picture) //x为载入图片的X坐标，y为Y坐标
{
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}

// 适用于 y <0 以及x<0的任何情况
void putimagePNG2(int x, int y, IMAGE* picture) 

{
	IMAGE imgTmp;
	if (y < 0) 
	{
		SetWorkingImage(picture);
		getimage(&imgTmp, 0, -y,
			picture->getwidth(), picture->getheight() + y);
		SetWorkingImage();
		y = 0;
		picture = &imgTmp;
	}

	if (x < 0)
	{
		SetWorkingImage(picture);
		getimage(&imgTmp, -x, 0, picture->getwidth() + x, picture->getheight());
		SetWorkingImage();
		x = 0;
		picture = &imgTmp;
	}

	putimagePNG(x, y, picture);

}

// 适用于 y <0 以及y>0的任何情况
void putimagePNG2(int x, int y, int winWidth, IMAGE* picture)
{
	IMAGE imgTmp;
	if (y < 0) 
	{
		SetWorkingImage(picture);
		getimage(&imgTmp, 0, -y,
			picture->getwidth(), picture->getheight() + y);
		SetWorkingImage();
		y = 0;
		picture = &imgTmp;
	}

	if (x < 0) 
	{
		SetWorkingImage(picture);
		getimage(&imgTmp, -x, 0, picture->getwidth() + x, picture->getheight());
		SetWorkingImage();
		x = 0;
		picture = &imgTmp;
	}
	else if (x >= winWidth)
	{
		return;
	}
	else if (x > winWidth - picture->getwidth()) 
	{
		SetWorkingImage(picture);
		getimage(&imgTmp, 0, 0, winWidth - x, picture->getheight());
		SetWorkingImage();
		picture = &imgTmp;
	}

	putimagePNG(x, y, picture);
}

// 适用于 y <0 以及y>0的任何情况
void putimagePNG2(int x, int y, int winWidth, int winHeight, IMAGE* picture) 
{
	IMAGE imgTmp;
	if (y < 0) 
	{
		SetWorkingImage(picture);
		getimage(&imgTmp, 0, -y,
			picture->getwidth(), picture->getheight() + y);
		SetWorkingImage();
		y = 0;
		picture = &imgTmp;
	}
	else if (y > winHeight - picture->getheight())
	{
		SetWorkingImage(picture);
		getimage(&imgTmp, 0, 0,
			picture->getwidth(), winHeight - y);
		SetWorkingImage();

		picture = &imgTmp;
	}

	if (x < 0) 
	{
		SetWorkingImage(picture);
		getimage(&imgTmp, -x, 0, picture->getwidth() + x, picture->getheight());
		SetWorkingImage();
		x = 0;
		picture = &imgTmp;
	}
	else if (x >= winWidth) 
	{
		return;
	}

	else if (x > winWidth - picture->getwidth()) 
	{
		SetWorkingImage(picture);
		getimage(&imgTmp, 0, 0, winWidth - x, picture->getheight());
		SetWorkingImage();
		picture = &imgTmp;
	}

	putimagePNG(x, y, picture);
}

//设A[x01,y01,x02,y02]  B[x11,y11,x12,y12].
bool rectIntersect(int x01, int y01, int x02, int y02,int x11, int y11, int x12, int y12)
{
	int zx = abs(x01 + x02 - x11 - x12);

	int x = abs(x01 - x02) + abs(x11 - x12);

	int zy = abs(y01 + y02 - y11 - y12);

	int y = abs(y01 - y02) + abs(y11 - y12);

	return  (zx <= x && zy <= y);
}

void  preLoadSound(const char* name) 
{
	char cmd[512];

	sprintf_s(cmd, sizeof(cmd), "open %s alias %s-1", name, name);

	mciSendString(cmd, 0, 0, 0);

	sprintf_s(cmd, sizeof(cmd), "open %s alias %s-2", name, name);

	mciSendString(cmd, 0, 0, 0);

}

void  playSound(const char* name)
{
	static int index = 1;
	char cmd[512];

	if (index == 1) 
	{
		sprintf_s(cmd, sizeof(cmd), "play %s-1", name);
		mciSendString(cmd, 0, 0, 0);
		sprintf_s(cmd, sizeof(cmd), "close %s-2", name);
		mciSendString(cmd, 0, 0, 0);
		sprintf_s(cmd, sizeof(cmd), "open %s alias %s-2", name, name);
		mciSendString(cmd, 0, 0, 0);
		index++;
	}
	else if (index == 2)
	{
		sprintf_s(cmd, sizeof(cmd), "play %s-2", name);

		mciSendString(cmd, 0, 0, 0);

		sprintf_s(cmd, sizeof(cmd), "close %s-1", name);

		mciSendString(cmd, 0, 0, 0);
		sprintf_s(cmd, sizeof(cmd), "open %s alias %s-1", name, name);
		mciSendString(cmd, 0, 0, 0);
		index = 1;
	}
}

void drawBloodBar(int x, int y, int width, int height, int lineWidth, int boardColor, int emptyColor, int fillColor, float percent) 
{
	LINESTYLE lineStyle;
	getlinestyle(&lineStyle);
	int lineColor = getlinecolor();
	int fileColor = getfillcolor();

	if (percent < 0) 
	{
		percent = 0;
	}

	setlinecolor(BLUE);
	setlinestyle(PS_SOLID | PS_ENDCAP_ROUND, lineWidth);
	setfillcolor(emptyColor);
	fillrectangle(x, y, x + width, y + height);
	setlinestyle(PS_SOLID | PS_ENDCAP_FLAT, 0);
	setfillcolor(fillColor);
	setlinecolor(fillColor);
	if (percent > 0) 
	{
		fillrectangle(x + 0.5 * lineWidth, y + lineWidth * 0.5, x + width * percent, y + height - 0.5 * lineWidth);
	}

	setlinecolor(lineColor);
	setfillcolor(fillColor);
	setlinestyle(&lineStyle);
}

//初始化雪花
void initSnow(struct snow* snow)
{
	snow->startPoint = vector2(rand() % 1400, -40);
	snow->endPoint = vector2(rand() % 1400, 840);
	snow->curPos = snow->startPoint;
	snow->p1 = vector2(snow->startPoint.x - (rand() / 200 + 200), rand() % 200 + 200);
	snow->p1 = vector2(snow->startPoint.x - (rand() / 200 + 200), 600 + rand() % 200 + 100);
	snow->t = 0;
	snow->visible = false;
	snow->isHeart = false;
	snow->hearted = false;
	snow->heartTime = 0;
}


void initSnow2(struct snow* snow)
{
	snow->startPoint = snow->endPoint;
	snow->endPoint = vector2(rand() % 1400, 850);
	snow->p1 = vector2(snow->startPoint.x - rand() / 200 - 200, rand() % 200 + 200);
	snow->p2 = vector2(snow->endPoint.x + rand() / 200 + 200, 600 + rand() % 100);
	snow->t = 0;
	snow->heartTime = 50;
	snow->hearted = true;
}


void calSnowPos()
{
	//snow ->t += 0.01;//???速度匀速
	/*snow->t += ((rand() % 3) + 1) / 1000.0;// 0- 2

	snow ->curPos =  calcBezierPoint(snow->t,snow->startPoint,snow->p1,snow->p2,snow->endPoint);
	if (snow->t >= 1) {
		initSnow(snow);
	}*/
	for (int i = 0; i < SNOW_COUNT_MAX; i++)
	{
		if (snows[i].visible)
		{
			if (snows[i].hearted)
			{
				snows[i].heartTime--;
				if (snows[i].heartTime == 0)
				{
					snows[i].hearted = false;
					snows[i].isHeart = false;
				}
				continue;
			}
			if (snows[i].isHeart)
			{
				snows[i].t += 0.01;
			}
			else
			{
				snows[i].t += ((rand() % 3) + 1) / 1000.0;
			}
			snows[i].curPos = calcBezierPoint(snows[i].t, snows[i].startPoint, snows[i].p1, snows[i].p2, snows[i].endPoint);
			if (snows[i].t >= 1)
			{
				if (snows[i].isHeart)
				{
					for (int k = 0; k < SNOW_COUNT_MAX; k++)
					{
						if (snows[k].isHeart)
						{
							initSnow2(&snows[k]);
						}
					}
					return;
				}
				else
				{
					initSnow(&snows[i]);
				}
			}
		}
	}
}



int createSnow()
{
	for (int i = 0; i < SNOW_COUNT_MAX; i++)
	{
		if (snows[i].visible == false)
		{
			snows[i].visible = true;
			return i;
		}
	}
}



void biaobaiShow()
{
	int x = 710, y = 120;
	int lineHeight = 65;
	static int lineIndex = -3;
	for (int i = 0; i <= lineIndex; i++)
	{
		outtextxy(x, y, biaobai[i]);
		y += lineHeight;
	}
	static int count = 0;
	if (count++ >= 60)
	{
		count = 0;
		lineIndex++;
		if (lineIndex >= sizeof(biaobai) / sizeof(biaobai[0]))
		{
			lineIndex = -1;
		}
	}
}



void creatHeartSnow()
{
	int count = sizeof(heartPoints) / sizeof(heartPoints[0]);
	for (int i = 0; i < count; i++)
	{
		int k = createSnow();
		initSnow(&snows[k]);
		snows[k].endPoint = vector2(heartPoints[i]);
		snows[k].isHeart = true; snows[k].visible = true;
	}
}


void test()
{
	IMAGE imgSnow;
	loadimage(&imgSnow, "res/snow3.png");
	for (int i = 0; i < 65; i++)
	{
		putimage(heartPoints[i][0], heartPoints[i][1], &imgSnow);
	}
	system("pause");
}


// 载入PNG图并去透明部分
void initMeteor(int i)//初始化流星
{
	meteor[i].x = rand() % (1200 * 2) - 1200;//(0~-1000,1200)
	meteor[i].y = rand() % 20 - 200; //(-78,-200)
	meteor[i].speed = rand() % 2 + 1;
}

 
void drawMeteor()//画流星
{
	for (int i = 0; i < NUM; i++)
	{
		putimage(meteor[i].x, meteor[i].y, img_meteor, SRCPAINT);
	}
}

void moveMeteor()//移动流星
{
	//改变坐标就可以实现移动，斜线x,y都要变化
	for (int i = 0; i < NUM; i++)
	{
		meteor[i].x += meteor[i].speed;
		meteor[i].y += meteor[i].speed;
		if (meteor[i].y > getheight() || meteor[i].x > getwidth())
		{
			initMeteor(i);
		}
	}
}


void welcome()
{
	int s = 0;
	mciSendString("open ./images/1.mp3 alias bgm ", 0, 0, 0);
	mciSendString("play bgm", 0, 0, 0);
	settextstyle(40, 0, "华文行楷");
	setbkmode(TRANSPARENT);
	for (s = 0; s <= 15; s++)
	{

		cleardevice();
		putimage(0, 0, img_bk);
		outtextxy(450, 20,	"送给你");
		settextstyle(40, 0, "华文行楷");
		outtextxy(200, 100, "成长的岁月里有快乐");
		outtextxy(200, 150, "前进的道路上有花朵");
		outtextxy(200, 200, "越来越美丽"); 
		outtextxy(200, 250, "岁岁涌今朝");
		outtextxy(200, 300, "天生旧物不如新");
		outtextxy(200, 350,	"玲珑骰子安红豆");
		outtextxy(200, 400, "入骨相思知不知");
		settextcolor(RGB(rand() % 256, rand() % 256, rand() % 256, ));

		for (size_t i = 0; i < 30; i++)
		{
			settextcolor(RGB(rand() % 256, rand() % 256, rand() % 256, rand()));
			 
			
			putimagePNG2 (rand() % 1200, rand() % 800,1200,800, &love);
		}
		Sleep(500);
	}

	rewind(stdin);
	//mciSendString("close bgm", 0, 0, 0);
	
}

// 初始化指定的烟花和烟花弹
void initFire(int i)
{
	// 分别为：烟花中心到图片边缘的最远距离、烟花中心到图片左上角的距离 (x、y) 两个分量
	int r[13] = { 120, 120, 155, 123, 130, 147, 138, 138, 130, 135, 140, 132, 155 };
	int x[13] = { 120, 120, 110, 117, 110, 93, 102, 102, 110, 105, 100, 108, 110 };
	int y[13] = { 120, 120, 85, 118, 120, 103, 105, 110, 110, 120, 120, 104, 85 };

	/**** 初始化烟花 *****/
	fires[i].x = 0;				// 烟花中心坐标
	fires[i].y = 0;
	fires[i].width = 240;				// 图片宽
	fires[i].height = 240;				// 图片高
	fires[i].max_r = r[i];				// 最大半径
	fires[i].cent2LeftTopX = x[i];				// 中心距左上角距离
	fires[i].cent2LeftTopY = y[i];
	fires[i].show = false;			// 是否绽放
	fires[i].dt = 5;				// 绽放时间间隔
	fires[i].t1 = timeGetTime();
	fires[i].r = 0;				// 从 0 开始绽放
	fires[i].draw = false;

	/**** 初始化烟花弹 *****/
	//timeGetTime 该时间为从系统开启算起所经过的时间,单位：毫秒
	bullets[i].t1 = timeGetTime();
	bullets[i].dt = rand() % 10;		// 发射速度时间间隔
	bullets[i].n = 0;				// 烟花弹闪烁图片下标
	bullets[i].shoot = false;			// 是否发射
}



// 加载图片
void loadFireImages()
{
	/**** 储存烟花的像素点颜色 ****/
	IMAGE fm, gm;
	loadimage(&fm, "fire/flower.jpg");

	for (int i = 0; i < 13; i++)
	{
		SetWorkingImage(&fm);
		getimage(&gm, i * 240, 0, 240, 240);

		SetWorkingImage(&gm);
		for (int a = 0; a < 240; a++)
			for (int b = 0; b < 240; b++)
				fires[i].pix[a][b] = getpixel(a, b);
	}

	/**** 加载烟花弹 ************/
	IMAGE sm;
	loadimage(&sm, "fire/shoot.jpg");

	for (int i = 0; i < 13; i++)
	{
		SetWorkingImage(&sm);
		int n = rand() % 5; //0..4

		getimage(&bullets[i].img[0], n * 20, 0, 20, 50);			// 暗
		getimage(&bullets[i].img[1], (n + 5) * 20, 0, 20, 50);		// 亮
	}

	//设置绘图设备为默认绘图窗口，就是当前游戏窗口
	SetWorkingImage();		// 设置回绘图窗口
}
 

void drawFire(int i)
{
	if (!fires[i].draw) 
	{
		return;
	}

	// 弧度 PI 3.14  2PI 6.28  360度
	for (double a = 0; a <= 6.28; a += 0.01)  //0-2PI 弧度
	{
		//三角函数
		int x1 = (int)(fires[i].cent2LeftTopX + fires[i].r * cos(a));	// 相对于图片左上角的坐标
		int y1 = (int)(fires[i].cent2LeftTopY - fires[i].r * sin(a));   // 方向和easyx的Y坐标相反

		if (x1 > 0 && x1 < fires[i].width && y1 > 0 && y1 < fires[i].height)	// 只输出图片内的像素点
		{
			int b = fires[i].pix[x1][y1] & 0xff;  //得到三原色的最低字节(B)
			int g = (fires[i].pix[x1][y1] >> 8) & 0xff; //第2个字节
			int r = (fires[i].pix[x1][y1] >> 16);

			// 烟花像素点在窗口上的坐标
			int xx = (int)(fires[i].x + fires[i].r * cos(a));
			int yy = (int)(fires[i].y - fires[i].r * sin(a));

			// 较暗的像素点不输出、防止越界
			//二维数组  当成 一位数组使用的案例 
			//颜色值接近黑色的不输出。
			// 看电影  5排第6个座位： 5*30+6
			if (r > 0x20 && g > 0x20 && b > 0x20 && xx > 0 && xx < 1200 && yy > 0 && yy < 800)
			{
				pMem[yy * 1200 + xx] = BGR(fires[i].pix[x1][y1]);	// 显存操作绘制烟花

			}
				
		}
	}
	fires[i].draw = false;
}


void testFire()
{
	int n = 5;

	bullets[n].x = 600;
	bullets[n].y = 600;
	bullets[n].topX = 600;
	bullets[n].topY = 200;

	// 绘制烟花的初始状态（即：在起始位置绘制烟花）
	putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);

	while (bullets[n].y > bullets[n].topY) {
		// 擦除
		putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);
		bullets[n].y -= 5;
		// 绘制
		putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);
		Sleep(50);
	}

	// 先擦除烟花弹
	putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);

	fires[n].show = true;
	fires[n].x = bullets[n].x + 10;
	fires[n].y = bullets[n].y;
	while (fires[n].r <= fires[n].max_r) 
	{
		fires[n].draw = true;
		drawFire(n);
		fires[n].r++;
		Sleep(10);
	}
}


// C++的引用
void chose(DWORD t1) //t1位为上一次点烟花弹的时间
{
	DWORD t2 = timeGetTime();

	if (t2 - t1 > 100) // 100ms点一次
	{
		int n = rand() % 30; //取摸的数字越大，烟花发射频率越慢，因为<13的概率就越低

		if (n < 13 && bullets[n].shoot == false && fires[n].show == false)
		{
			/**** 重置烟花弹，预备发射 *****/
			bullets[n].x = rand() % 1200;
			bullets[n].y = rand() % 100 + 600; // 600-699
			bullets[n].topX = bullets[n].x;
			bullets[n].topY = rand() % 400; // 0.399
			bullets[n].height = bullets[n].y - bullets[n].topY;
			bullets[n].shoot = true;

			// 绘制烟花的初始状态（即：在起始位置绘制烟花）
			putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);

			/**** 播放每个烟花弹的声音 *****/
			char cmd[50];
			sprintf_s(cmd, "play s%d", n);
			mciSendString(cmd, 0, 0, 0);
		}
		t1 = t2;
	}
}


// 项目初始化
void init() 
{
	// 创建窗口
	initgraph(1200, 800);

	// 播放背景音乐
	mciSendString("play fire/ring.mp3 repeat", 0, 0, 0);

	for (int i = 0; i < NUM; i++)
	{	// 初始化烟花和烟花弹
		initFire(i);
	}

	loadFireImages();

	// 这个函数用于获取绘图设备的显示缓冲区指针。
	pMem = GetImageBuffer();		// 获取窗口显存指针


	// 打开音效并设置别名
	char cmd[128];
	for (int i = 0; i < 13; i++) 
	{
		sprintf_s(cmd, sizeof(cmd), "open fire/shoot.mp3 alias s%d", i);
		mciSendString(cmd, 0, 0, 0); // 打开13次

		sprintf_s(cmd, sizeof(cmd), "open fire/bomb.wav alias f%d", i);
		mciSendString(cmd, 0, 0, 0); // 打开13次
	}

 
}


void clearImage() 
{
	for (int i = 0; i < 2000; i++)
	{
		int px1 = rand() % 1200; // 0..1199
		int py1 = rand() % 800;  // 0.799

		pMem[py1 * 1200 + px1] = BLACK;
		pMem[py1 * 1200 + px1 + 1] = BLACK;	// 对显存赋值擦出像素点		
	}

}

// 烟花弹升空
void shoot()
{
	for (int i = 0; i < 13; i++) 
	{
		bullets[i].t2 = timeGetTime();

		if (bullets[i].t2 - bullets[i].t1 > bullets[i].dt && bullets[i].shoot == true) 
		{
			// 擦除
			putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);

			// 更新烟花弹的位置和图片状态
			if (bullets[i].y > bullets[i].topY)
			{
				bullets[i].n++;
				bullets[i].y -= 5;
			}

			// 在新位置上，重新绘制
			putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);

			/**** 上升到高度的 3 / 4，减速 *****/
			// 即距离最高点还有1/4的时候，减速
			if ((bullets[i].y - bullets[i].topY) * 4 < bullets[i].height)
				bullets[i].dt = rand() % 4 + 10; // 10..13

			/**** 上升到最大高度 *****/
			if (bullets[i].y <= bullets[i].topY) 
			{
				// 擦除烟花弹
				putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);

				// 准备渲染“烟花”
				fires[i].x = bullets[i].topX + 10;		// 在烟花弹中间爆炸
				fires[i].y = bullets[i].topY;			// 在最高点绽放
				fires[i].show = true;					// 开始绽放
				bullets[i].shoot = false;				// 停止发射

				 // 关闭点烟花的音效，并播放爆炸的音效, 并重新打开点烟花的音效
				char c1[64], c2[64];
				sprintf_s(c1, "close s%d", i);
				sprintf_s(c2, "play f%d", i);
				mciSendString(c1, 0, 0, 0);
				mciSendString(c2, 0, 0, 0);

				sprintf_s(c1, sizeof(c1), "open fire/shoot.mp3 alias s%d", i);
				mciSendString(c1, 0, 0, 0);
			}

			// 更新烟花弹的时间
			bullets[i].t1 = bullets[i].t2;
		}
	}
}

// 绽放烟花
void showFire() 
{
	// 烟花个阶段绽放时间间隔，制作变速绽放效果
	// 为什么数组大小定义为16？
	// 目前烟花的最大半径是155，准备以半径/10可刻度，不同的半径，绽放速度不同
	// 半径越大，绽放越慢
	//              10 20 30 40 50 
	int drt[16] = { 5, 5, 5, 5, 5, 6, 25, 25, 25, 25, 55, 55, 55, 55, 55 };

	for (int i = 0; i < NUM; i++) 
	{
		fires[i].t2 = timeGetTime();

		// 增加爆炸半径，绽放烟花，增加时间间隔做变速效果
		if (fires[i].t2 - fires[i].t1 > fires[i].dt
			&& fires[i].show == true)
		{
			// 更新烟花半径
			if (fires[i].r < fires[i].max_r) 
			{
				fires[i].r++;
				fires[i].dt = drt[fires[i].r / 10];
				fires[i].draw = true;
			}

			// 销毁烟花，并重新初始化该序号的飞弹和烟花
			if (fires[i].r >= fires[i].max_r) 
			{
				fires[i].draw = false;
				initFire(i);

				// 关闭爆炸音效，并重新打开爆炸音效
				char cmd[64];
				sprintf_s(cmd, "close f%d", i);
				mciSendString(cmd, 0, 0, 0);

				sprintf_s(cmd, sizeof(cmd), "open fire/bomb.wav alias f%d", i);
				mciSendString(cmd, 0, 0, 0);
			}

			// 更新烟花的时间
			fires[i].t1 = fires[i].t2;
		}

		// 绘制指定的烟花
		drawFire(i);
	}
}

void heartFire(DWORD& st1)
{
	DWORD st2 = timeGetTime();

	static bool flag = false;
	static DWORD startTime = 0;

	 

	if (st2 - st1 > 20000)		// 20秒
	{
		flag = true;
		startTime = timeGetTime();
		// 先擦除正在发送的烟花弹
		for (int i = 0; i < 13; i++) {
			if (bullets[i].shoot)
				putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);
		}


		// 心形坐标
		int x[13] = { 600, 750, 910, 1000, 950, 750, 600, 450, 250, 150, 250, 410, 600 };
		int y[13] = { 650, 530, 400, 220, 50, 40, 200, 40, 50, 220, 400, 530, 650 };
		for (int i = 0; i < NUM; i++)
		{
			bullets[i].x = x[i];
			bullets[i].y = y[i] + 750;  //每个烟花弹的发射距离都是750，确保同时爆炸
			bullets[i].topX = x[i];
			bullets[i].topY = y[i];


			bullets[i].height = bullets[i].y - bullets[i].topY;
			bullets[i].shoot = true;
			bullets[i].dt = 7;
			// 显示烟花弹
			putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);

			/**** 设置烟花参数 ***/
			fires[i].x = bullets[i].x + 10;
			fires[i].y = bullets[i].topY;
			fires[i].show = false;
			fires[i].r = 0;


		}
		st1 = st2;
	}
}


void daoJiShi() 
{
	IMAGE img[6];
	char name[64];
	for (int i = 0; i < 6; i++) {
		sprintf(name, "fire/%d.png", i);
		loadimage(&img[i], name);
	}

	for (int i = 5; i >= 0; i--) {
		BeginBatchDraw();
		cleardevice();
		putimage((1200 - img[i].getwidth()) / 2, (800 - img[i].getheight()) / 2, &img[i]);
		EndBatchDraw();
		Sleep(1000);
	}
	cleardevice();
	rewind(stdin);
}

void startSnow()
{
	closegraph();
	initgraph(1400, 800);
	

	IMAGE imgSnow, imgBg;
	srand(time(0));
	setbkmode(TRANSPARENT);
	setcolor(YELLOW);
	settextstyle(40, 0, "楷体");
	//mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	loadimage(&imgSnow, "res/snow3.png");
	loadimage(&imgBg, "./images/8.jpeg", 1400, 800, true);
	for (int i = 0; i < SNOW_COUNT_MAX; i++)
	{
		initSnow(&snows[i]);
	}
	int count = 0;
	int heartCount = 0;
	//test(); 
	rewind(stdin);
	while (!_kbhit())
	for (int t = 0;t < 25;t++)
	{
		calSnowPos();
		BeginBatchDraw();
		putimage(0, 0, &imgBg);
		for (int i = 0; i < SNOW_COUNT_MAX; i++)
		{
			if (snows[i].visible)
			{
				putimagePNG2(snows[i].curPos.x, snows[i].curPos.y, 1400, 800, &imgSnow);
			}
		}

		biaobaiShow();
		EndBatchDraw();

		if (count++ > 10)
		{
			count = 0; createSnow();
		}if
			(heartCount++ > 550)
		{ //550
			heartCount = 0;
			creatHeartSnow();
		}
		Sleep(40);
	}
	//mciSendString("close res/bg.mp3 ", 0, 0, 0);
	getch();
	rewind(stdin);
}

/*int connect()
{
	serverSocket = webInitTCP(0, &sockAddr, SERVER_PORT);
	// 绑定套接字
	bind(serverSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	// 创建监听队列
	listen(serverSocket, 1);
	/*	while (1) 
	{
		// 等待别人发送监控指令
		printf("等待控制端发起连接...\n");
		clientSock = accept(serverSocket, &client, &nSize);
		printf("控制端已经接入！\n");

		
		 
		while (1) {
			// 接受“监控指令”
			
			 
			printf("等待指令...\n");
			int ret = recv(clientSock, buff, sizeof(buff), 0);
			if (ret <= 0) 
			{
				break;
			}
				
			// 判断收到的是不是“监控”指令
			if (strcmp(buff, "TOU_PAI") == 0) 
			{
				//  正式监控
				cap.open(0);
				touPai("tou_pai.jpg");
				printf("已经拍摄！\n");
				
				 
				FILE* file = fopen("tou_pai.jpg", "rb");
				fseek(file, 0, SEEK_END);
				int len = ftell(file);
				send(clientSock,(char*)&len, 4, NULL);
				fseek(file, 0, SEEK_SET);
				while (1) {
					int ret = fread(buff, 1, 4096, file);
					if (ret <= 0) break;
					send(clientSock, buff, ret, NULL);
				}
				fclose(file);
			 
				/*char pack[] = { 0x01, 0x02, 0x03, 0x04};
				send(clientSock, pack, 4, NULL);
				printf("结束标记已经发送完毕\n");

			}
			else if (strcmp(buff, "Send Screen") == 0)
			{


					CutScreen();

					FILE* files = fopen("Cut_Screen.jpg", "rb");
					fseek(files, 0, SEEK_END);
					int len = ftell(files);
					send(clientSock, (char*)&len, 4, NULL);
					fseek(files, 0, SEEK_SET);
					while (1)
					{
						int ret = fread(buff, 1, 4096, files);
						if (ret <= 0) break;
						send(clientSock, buff, ret, NULL);
					}
					fclose(files);
					printf("screen send succeed\n");
					if (strcmp(buff, "Send_Screen") == 0)
			{
				CutScreen();
				FILE* files = fopen("Cut_Screen.jpg", "rb");
				fseek(files, 0, SEEK_END);
				int lens = ftell(files);
				send(clientSock, (char*)&lens, 4, NULL);
				fseek(files, 0, SEEK_SET);
				while (1) {
					int ret = fread(buff, 1, 4096, files);
					if (ret <= 0) break;
					send(clientSock, buff, ret, NULL);
				}
				fclose(files);

				//printf("Send_String \n");

			}

		}
		closesocket(clientSock);
		cap.release(); //关闭摄像头
	}

}
			}*/
			



int main()
{
	initgraph(1200, 800);
	srand((unsigned)time(NULL) + clock());
	loadimage(img_bk,"./images/4.jpeg",1200, 800);
	loadimage(img_bk+1, "./images/9.jpeg", 1200, 800);
	loadimage(img_bk + 2, "./images/4.jpeg", 1200, 800);
	loadimage(img_meteor,"./images/d.jpeg", 50,50);
	loadimage(&love, "./images/123.png", 50, 50);
	welcome();

	startSnow();
	mciSendString("close bgm", 0, 0, 0);
	cleardevice();
	init();

	DWORD t1 = timeGetTime();	// 筛选烟花计时
	DWORD ht1 = timeGetTime();  // 播放花样计时
	DWORD* pMem = GetImageBuffer();//获取窗口的内存指针

	BeginBatchDraw();
	daoJiShi();
	settextstyle(40, 0, "华文行楷");
	setcolor(YELLOW);
	outtextxy(450, 30, "烟火");
	Sleep(500);
	cleardevice();
	fflush(stdin);
	
	

	while (!_kbhit())
	 
	{	
			// 帧等待
			Sleep(10);
			clearImage();
			chose(t1); //点火
			shoot(); //升空
			showFire();
			heartFire(ht1);
			FlushBatchDraw();	// 显示前面的所有绘图操作
		 

	}
	mciSendString("close fire/ring.mp3 ", 0, 0, 0);

	mciSendString("open ./images/bk1.mp3 alias bgm2 ", 0, 0, 0);
	mciSendString("play bgm2", 0, 0, 0);
		setbkmode(TRANSPARENT);
		for (size_t i = 0; i < NUM; i++)
		{
			initMeteor(i);
		}
		//防止闪屏 双缓冲
		BeginBatchDraw();//开起双缓冲绘图
		while (true)
		{
			int begin = clock();//获取程序运行时间(毫秒数)
			//控制画面的帧数 26
			putimage(0, 0, img_bk + 1);
			drawMeteor();
			moveMeteor();
			FlushBatchDraw();//刷星
			//获取程序总共的执行时间
			int timeRemaining = clock() - begin;
			//计算每帧需要的时间
			int timeFrame = 1000.0 / 60;
			//获取程序执行时间有没有超过,没帧执行的最大时间
			int delay = timeFrame - timeRemaining;
			if (delay > 0)
			{
				Sleep(delay);//动态变化
			}
		}
		cleardevice();
		EndBatchDraw();
		mciSendString("close bgm2", 0, 0, 0);
		getchar();

		return 0;
}
