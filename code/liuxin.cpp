#include "head.h"
#include "star.h"

void initMeteor(int i)//��ʼ������
{
	meteor[i].x = rand() % (1200 * 2) - 1200;//(0~-1000,1200)
	meteor[i].y = rand() % 20 - 200; //(-78,-200)
	meteor[i].speed = rand() % 2 + 1;
}
void drawMeteor()//������
{
	for (int i = 0; i < NUM; i++)
	{
		putimage(meteor[i].x, meteor[i].y, img_meteor, SRCPAINT);
	}
}

void moveMeteor()//�ƶ�����
{
	//�ı�����Ϳ���ʵ���ƶ���б��x,y��Ҫ�仯
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



