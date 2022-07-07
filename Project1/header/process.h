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

void process(int control_bmp); //���ƽ���
//���ܴ���
void process1();// LED����
void process2();// ��������
void process3();// Һ�������
void process4(struct pic* header, struct pic** current);// �������
void process5(int* STOP,struct music* music_header, struct music** current);// ���ֲ���
void process6(bool* air);// �ҵ����
void process7(void); // ���Թ���ģ��

void kill_myself();

#endif