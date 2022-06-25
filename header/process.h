#ifndef _PROCESS_H_

#define _PROCESS_H_ 1

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include "music.h"


void process(int control_bmp); //控制界面
//功能处理
void process1(int fd_bmp);// LED控制
void process2(int fd_bmp);// 报警控制
void process3(int fd_bmp);// 液晶屏检测
void process4(int fd_bmp);// 数码相册
void process5(int fd_bmp, int* STOP,long* volume,struct music* music_header, struct music** current);// 音乐播放
void process6(int fd_bmp);// 家电控制

#endif