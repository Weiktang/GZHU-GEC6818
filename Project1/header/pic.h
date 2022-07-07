#ifndef _PIC_H_

#define _PIC_H_ 1

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <string.h>
#include <sys/soundcard.h>
#include <sys/stat.h>
#include <LCD.h>

#define pic_dir "/Project/pic2/"

struct pic
{
    char name[50];
    struct pic *next;
    struct pic *last;
};

struct pic *print_pic_dir(char *path, int depth);                              // 获取图片列表
void show_pic(struct pic *header);                                         // 显示图片列表
void pic(struct pic *header, struct pic **current); // 图片循环播放

#endif