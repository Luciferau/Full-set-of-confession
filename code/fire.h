#pragma once
#include "head.h"
// �̻��ṹ
struct Fire
{
	int r;					// ��ǰ��ը�뾶
	int max_r;				// ��ը���ľ����Ե���뾶
	int x, y;				// ��ը�����ڴ��ڵ�����
	int cent2LeftTopX, cent2LeftTopY;		// ��ը�������ͼƬ���Ͻǵ�����
	int width, height;		// ͼƬ�Ŀ��
	int pix[240][240];		// ����ͼƬ���ص�

	bool show;				// �Ƿ�����
	bool draw;				// ��ʼ������ص�
	DWORD t1, t2, dt;		// �����ٶ�
}fires[NUM];

// �̻����ṹ
struct Bullet
{
	int x, y;				// �̻����ĵ�ǰ����
	int topX, topY;				// ��ߵ�����------����ֵ�� FIRE ����� x, y
	int height;				// �̻��߶�
	bool shoot;				// �Ƿ���Է���

	DWORD t1, t2, dt;		// �����ٶ�
	IMAGE img[2];			// ���滨��һ��һ��ͼƬ
	unsigned char n : 1;	// ͼƬ�±� n++
}bullets[NUM];
