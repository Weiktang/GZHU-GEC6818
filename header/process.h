#ifndef _PROCESS_H_

#define _PROCESS_H_ 1

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include "music.h"


void process(int control_bmp); //���ƽ���
//���ܴ���
void process1(int fd_bmp);// LED����
void process2(int fd_bmp);// ��������
void process3(int fd_bmp);// Һ�������
void process4(int fd_bmp);// �������
void process5(int fd_bmp, int* STOP,long* volume,struct music* music_header, struct music** current);// ���ֲ���
void process6(int fd_bmp);// �ҵ����

#endif