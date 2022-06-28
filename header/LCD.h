#ifndef _LCD_H_

#define _LCD_H_	1

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> // open
#include <unistd.h> //close
#include <string.h>
#include <math.h>
#include <sys/mman.h> // ÄÚ´æÓ³Éä

//long colors[6]={0x00FFFF00,0x0000FF00,0x000000FF,0x00000000,0x00FFFFFF,0x00FF0000};
#define Red 0x00FF0000
#define Green 0x0000FF00
#define Blue 0x000000FF
#define Black 0x00000000
#define White 0x00FFFFFF
#define Yellow 0x00FFFF00

#define GERMAN 0
#define RUSSIA 1
#define FRANCE 2
#define ITALY  3
#define JAPAN  4

#define Buffer_Size 800*480 
#define rect_num 3

struct Rect
{
	unsigned int x;
	unsigned int y;
	unsigned int l;
	unsigned int h;
};

void show_Rect(int num, struct Rect rect[], long* color);
void show_point(int x, int y,int *last_x,int *last_y);
void show(long int color);
void show_Flag(int country);
void bmp_process(int fd_bmp);
void bmp_process2(int LED_n, int statue); // LED ??

#endif