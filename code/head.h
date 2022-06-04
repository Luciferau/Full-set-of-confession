#pragma once
#include <stdio.h>
#include <conio.h>
#include <easyx.h>
#include<mmsystem.h>
#include <graphics.h>
#include <math.h>
#include <time.h>
#pragma comment(lib,"winmm.lib")

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

void initMeteor(int i);
void drawMeteor();
void moveMeteor();
void welcome();
void initFire(int i);
void loadFireImages();
void drawFire(int i);
void init();
void testFire();
void chose(DWORD t1);
void daoJiShi();
void heartFire(DWORD& st1);
void showFire();
void shoot();
void clearImage();
void INIT();

