#ifndef _TOUCH_H_

#define _TOUCH_H_ 1

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h> // bool类型
#include <linux/input.h>

void touch_print_px(int* ABS_x, int* ABS_y);
void *touch_print_px2(void* flag);// 线程函数

#endif