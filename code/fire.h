#pragma once
#include "head.h"
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
