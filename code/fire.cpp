#include "head.h"
#include "fire.h"
 

// ��ʼ��ָ�����̻����̻���
void initFire(int i)
{
	// �ֱ�Ϊ���̻����ĵ�ͼƬ��Ե����Զ���롢�̻����ĵ�ͼƬ���Ͻǵľ��� (x��y) ��������
	int r[13] = { 120, 120, 155, 123, 130, 147, 138, 138, 130, 135, 140, 132, 155 };
	int x[13] = { 120, 120, 110, 117, 110, 93, 102, 102, 110, 105, 100, 108, 110 };
	int y[13] = { 120, 120, 85, 118, 120, 103, 105, 110, 110, 120, 120, 104, 85 };

	/**** ��ʼ���̻� *****/
	fires[i].x = 0;				// �̻���������
	fires[i].y = 0;
	fires[i].width = 240;				// ͼƬ��
	fires[i].height = 240;				// ͼƬ��
	fires[i].max_r = r[i];				// ���뾶
	fires[i].cent2LeftTopX = x[i];				// ���ľ����ϽǾ���
	fires[i].cent2LeftTopY = y[i];
	fires[i].show = false;			// �Ƿ�����
	fires[i].dt = 5;				// ����ʱ����
	fires[i].t1 = timeGetTime();
	fires[i].r = 0;				// �� 0 ��ʼ����
	fires[i].draw = false;

	/**** ��ʼ���̻��� *****/
	//timeGetTime ��ʱ��Ϊ��ϵͳ����������������ʱ��,��λ������
	bullets[i].t1 = timeGetTime();
	bullets[i].dt = rand() % 10;		// �����ٶ�ʱ����
	bullets[i].n = 0;				// �̻�����˸ͼƬ�±�
	bullets[i].shoot = false;			// �Ƿ���
}

// ����ͼƬ
void loadFireImages()
{
	/**** �����̻������ص���ɫ ****/
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

	/**** �����̻��� ************/
	IMAGE sm;
	loadimage(&sm, "fire/shoot.jpg");

	for (int i = 0; i < 13; i++)
	{
		SetWorkingImage(&sm);
		int n = rand() % 5; //0..4

		getimage(&bullets[i].img[0], n * 20, 0, 20, 50);			// ��
		getimage(&bullets[i].img[1], (n + 5) * 20, 0, 20, 50);		// ��
	}

	//���û�ͼ�豸ΪĬ�ϻ�ͼ���ڣ����ǵ�ǰ��Ϸ����
	SetWorkingImage();		// ���ûػ�ͼ����
}

void drawFire(int i) 
{
	if (!fires[i].draw) {
		return;
	}

	// ���� PI 3.14  2PI 6.28  360��
	for (double a = 0; a <= 6.28; a += 0.01)  //0-2PI ����
	{
		//���Ǻ���
		int x1 = (int)(fires[i].cent2LeftTopX + fires[i].r * cos(a));	// �����ͼƬ���Ͻǵ�����
		int y1 = (int)(fires[i].cent2LeftTopY - fires[i].r * sin(a));   // �����easyx��Y�����෴

		if (x1 > 0 && x1 < fires[i].width && y1 > 0 && y1 < fires[i].height)	// ֻ���ͼƬ�ڵ����ص�
		{
			int b = fires[i].pix[x1][y1] & 0xff;  //�õ���ԭɫ������ֽ�(B)
			int g = (fires[i].pix[x1][y1] >> 8) & 0xff; //��2���ֽ�
			int r = (fires[i].pix[x1][y1] >> 16);

			// �̻����ص��ڴ����ϵ�����
			int xx = (int)(fires[i].x + fires[i].r * cos(a));
			int yy = (int)(fires[i].y - fires[i].r * sin(a));

			// �ϰ������ص㲻�������ֹԽ��
			//��ά����  ���� һλ����ʹ�õİ��� 
			//��ɫֵ�ӽ���ɫ�Ĳ������
			// ����Ӱ  5�ŵ�6����λ�� 5*30+6
			if (r > 0x20 && g > 0x20 && b > 0x20 && xx > 0 && xx < 1200 && yy > 0 && yy < 800)
				pMem[yy * 1200 + xx] = BGR(fires[i].pix[x1][y1]);	// �Դ���������̻�
		}
	}
	fires[i].draw = false;
}

// ��Ŀ��ʼ��
void init() {
	// ��������
	initgraph(1200, 800);

	// ���ű�������
	mciSendString("play fire/ring.mp3 repeat", 0, 0, 0);

	for (int i = 0; i < NUM; i++) {	// ��ʼ���̻����̻���
		initFire(i);
	}

	loadFireImages();

	// ����������ڻ�ȡ��ͼ�豸����ʾ������ָ�롣
	pMem = GetImageBuffer();		// ��ȡ�����Դ�ָ��


	// ����Ч�����ñ���
	char cmd[128];
	for (int i = 0; i < 13; i++) {
		sprintf_s(cmd, sizeof(cmd), "open fire/shoot.mp3 alias s%d", i);
		mciSendString(cmd, 0, 0, 0); // ��13��

		sprintf_s(cmd, sizeof(cmd), "open fire/bomb.wav alias f%d", i);
		mciSendString(cmd, 0, 0, 0); // ��13��
	}


}


void testFire() {
	int n = 5;

	bullets[n].x = 600;
	bullets[n].y = 600;
	bullets[n].topX = 600;
	bullets[n].topY = 200;

	// �����̻��ĳ�ʼ״̬����������ʼλ�û����̻���
	putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);

	while (bullets[n].y > bullets[n].topY) {
		// ����
		putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);
		bullets[n].y -= 5;
		// ����
		putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);
		Sleep(50);
	}

	// �Ȳ����̻���
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

// C++������
void chose(DWORD t1) //t1λΪ��һ�ε��̻�����ʱ��
{
	DWORD t2 = timeGetTime();

	if (t2 - t1 > 100) // 100ms��һ��
	{
		int n = rand() % 30; //ȡ��������Խ���̻�����Ƶ��Խ������Ϊ<13�ĸ��ʾ�Խ��

		if (n < 13 && bullets[n].shoot == false && fires[n].show == false)
		{
			/**** �����̻�����Ԥ������ *****/
			bullets[n].x = rand() % 1200;
			bullets[n].y = rand() % 100 + 600; // 600-699
			bullets[n].topX = bullets[n].x;
			bullets[n].topY = rand() % 400; // 0.399
			bullets[n].height = bullets[n].y - bullets[n].topY;
			bullets[n].shoot = true;

			// �����̻��ĳ�ʼ״̬����������ʼλ�û����̻���
			putimage(bullets[n].x, bullets[n].y, &bullets[n].img[bullets[n].n], SRCINVERT);

			/**** ����ÿ���̻��������� *****/
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



	if (st2 - st1 > 20000)		// 20��
	{
		flag = true;
		startTime = timeGetTime();
		// �Ȳ������ڷ��͵��̻���
		for (int i = 0; i < 13; i++) {
			if (bullets[i].shoot)
				putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);
		}


		// ��������
		int x[13] = { 600, 750, 910, 1000, 950, 750, 600, 450, 250, 150, 250, 410, 600 };
		int y[13] = { 650, 530, 400, 220, 50, 40, 200, 40, 50, 220, 400, 530, 650 };
		for (int i = 0; i < NUM; i++)
		{
			bullets[i].x = x[i];
			bullets[i].y = y[i] + 750;  //ÿ���̻����ķ�����붼��750��ȷ��ͬʱ��ը
			bullets[i].topX = x[i];
			bullets[i].topY = y[i];


			bullets[i].height = bullets[i].y - bullets[i].topY;
			bullets[i].shoot = true;
			bullets[i].dt = 7;
			// ��ʾ�̻���
			putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);

			/**** �����̻����� ***/
			fires[i].x = bullets[i].x + 10;
			fires[i].y = bullets[i].topY;
			fires[i].show = false;
			fires[i].r = 0;


		}
		st1 = st2;
	}
}

// �����̻�
void showFire() {
	// �̻����׶�����ʱ������������������Ч��
	// Ϊʲô�����С����Ϊ16��
	// Ŀǰ�̻������뾶��155��׼���԰뾶/10�ɿ̶ȣ���ͬ�İ뾶�������ٶȲ�ͬ
	// �뾶Խ������Խ��
	//              10 20 30 40 50 
	int drt[16] = { 5, 5, 5, 5, 5, 6, 25, 25, 25, 25, 55, 55, 55, 55, 55 };

	for (int i = 0; i < NUM; i++) {
		fires[i].t2 = timeGetTime();

		// ���ӱ�ը�뾶�������̻�������ʱ����������Ч��
		if (fires[i].t2 - fires[i].t1 > fires[i].dt
			&& fires[i].show == true) {
			// �����̻��뾶
			if (fires[i].r < fires[i].max_r) {
				fires[i].r++;
				fires[i].dt = drt[fires[i].r / 10];
				fires[i].draw = true;
			}

			// �����̻��������³�ʼ������ŵķɵ����̻�
			if (fires[i].r >= fires[i].max_r) {
				fires[i].draw = false;
				initFire(i);

				// �رձ�ը��Ч�������´򿪱�ը��Ч
				char cmd[64];
				sprintf_s(cmd, "close f%d", i);
				mciSendString(cmd, 0, 0, 0);

				sprintf_s(cmd, sizeof(cmd), "open fire/bomb.wav alias f%d", i);
				mciSendString(cmd, 0, 0, 0);
			}

			// �����̻���ʱ��
			fires[i].t1 = fires[i].t2;
		}

		// ����ָ�����̻�
		drawFire(i);
	}
}


// �̻�������
void shoot() {
	for (int i = 0; i < 13; i++) {
		bullets[i].t2 = timeGetTime();

		if (bullets[i].t2 - bullets[i].t1 > bullets[i].dt && bullets[i].shoot == true) {
			// ����
			putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);

			// �����̻�����λ�ú�ͼƬ״̬
			if (bullets[i].y > bullets[i].topY) {
				bullets[i].n++;
				bullets[i].y -= 5;
			}

			// ����λ���ϣ����»���
			putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);

			/**** �������߶ȵ� 3 / 4������ *****/
			// ��������ߵ㻹��1/4��ʱ�򣬼���
			if ((bullets[i].y - bullets[i].topY) * 4 < bullets[i].height)
				bullets[i].dt = rand() % 4 + 10; // 10..13

			/**** ���������߶� *****/
			if (bullets[i].y <= bullets[i].topY) {
				// �����̻���
				putimage(bullets[i].x, bullets[i].y, &bullets[i].img[bullets[i].n], SRCINVERT);

				// ׼����Ⱦ���̻���
				fires[i].x = bullets[i].topX + 10;		// ���̻����м䱬ը
				fires[i].y = bullets[i].topY;			// ����ߵ�����
				fires[i].show = true;					// ��ʼ����
				bullets[i].shoot = false;				// ֹͣ����

				 // �رյ��̻�����Ч�������ű�ը����Ч, �����´򿪵��̻�����Ч
				char c1[64], c2[64];
				sprintf_s(c1, "close s%d", i);
				sprintf_s(c2, "play f%d", i);
				mciSendString(c1, 0, 0, 0);
				mciSendString(c2, 0, 0, 0);

				sprintf_s(c1, sizeof(c1), "open fire/shoot.mp3 alias s%d", i);
				mciSendString(c1, 0, 0, 0);
			}

			// �����̻�����ʱ��
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
		pMem[py1 * 1200 + px1 + 1] = BLACK;	// ���Դ渳ֵ�������ص�		
	}
}

