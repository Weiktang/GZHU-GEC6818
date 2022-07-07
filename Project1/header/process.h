#ifndef _PROCESS_H_

#define _PROCESS_H_ 1

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include "music.h"
#include "pic.h"

void process(int control_bmp); //控制界面
//功能处理
void process1();// LED控制
void process2();// 报警控制
void process3();// 液晶屏检测
void process4(struct pic* header, struct pic** current);// 数码相册
void process5(int* STOP,struct music* music_header, struct music** current);// 音乐播放
void process6(bool* air);// 家电控制
void process7(void); // 测试功能模块

void kill_myself();

#endif