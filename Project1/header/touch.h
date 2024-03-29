#ifndef _TOUCH_H_

#define _TOUCH_H_ 1

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h> // bool类型
#include <linux/input.h>

void touch_print_px(int* ABS_x, int* ABS_y);// 获取坐标
void *touch_print_px2(void* flag);// 线程函数
void *touch_print_px3(void* flag);// 线程函数--空调

#endif