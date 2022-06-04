#include "head.h"
#include "fire.h"
 

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
	if (!fires[i].draw) {
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
				pMem[yy * 1200 + xx] = BGR(fires[i].pix[x1][y1]);	// 显存操作绘制烟花
		}
	}
	fires[i].draw = false;
}

// 项目初始化
void init() {
	// 创建窗口
	initgraph(1200, 800);

	// 播放背景音乐
	mciSendString("play fire/ring.mp3 repeat", 0, 0, 0);

	for (int i = 0; i < NUM; i++) {	// 初始化烟花和烟花弹
		initFire(i);
	}

	loadFireImages();

	// 这个函数用于获取绘图设备的显示缓冲区指针。
	pMem = GetImageBuffer();		// 获取窗口显存指针


	// 打开音效并设置别名
	char cmd[128];
	for (int i = 0; i < 13; i++) {
		sprintf_s(cmd, sizeof(cmd), "open fire/shoot.mp3 alias s%d", i);
		mciSendString(cmd, 0, 0, 0); // 打开13次

		sprintf_s(cmd, sizeof(cmd), "open fire/bomb.wav alias f%d", i);
		mciSendString(cmd, 0, 0, 0); // 打开13次
	}


}


void testFire() {
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
	while (fires[n].r <= fires[n].max_r) {
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

void daoJiShi() {
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

// 绽放烟花
void showFire() {
	// 烟花个阶段绽放时间间隔，制作变速绽放效果
	// 为什么数组大小定义为16？
	// 目前烟花的最大半径是155，准备以半径/10可刻度，不同的半径，绽放速度不同
	// 半径越大，绽放越慢
	//              10 20 30 40 50 
	int drt[16] = { 5, 5, 5, 5, 5, 6, 25, 25, 25, 25, 55, 55, 55, 55, 55 };

	for (int i = 0; i < NUM; i++) {
		fires[i].t2 = timeGetTime();

		// 增加爆炸半径，绽放烟花，增加时间间隔做变速效果
		if (fires[i].t2 - fires[i].t1 > fires[i].dt
			&& fires[i].show == true) {
			// 更新烟花半径
			if (fires[i].r < fires[i].max_r) {
				fires[i].r++;
				fires[i].dt = drt[fires[i].r / 10];
				fires[i].draw = true;
			}

			// 销毁烟花，并重新初始化该序号的飞弹和烟花
			if (fires[i].r >= fires[i].max_r) {
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


// 烟花弹升空
void shoot() {
	for (int i = 0; i < 13; i++) {
		bullets[i].t2 = timeGetTime();

		if (bullets[i].t2 - bullets[i].t1 > bullets[i].dt && bullets[i].shoot == true) {
			// 擦除
			putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);

			// 更新烟花弹的位置和图片状态
			if (bullets[i].y > bullets[i].topY) {
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
			if (bullets[i].y <= bullets[i].topY) {
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


void clearImage() {
	for (int i = 0; i < 2000; i++)
	{
		int px1 = rand() % 1200; // 0..1199
		int py1 = rand() % 800;  // 0.799

		pMem[py1 * 1200 + px1] = BLACK;
		pMem[py1 * 1200 + px1 + 1] = BLACK;	// 对显存赋值擦出像素点		
	}
}

