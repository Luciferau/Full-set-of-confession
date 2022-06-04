#include "head.h"
#include "star.h"

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



