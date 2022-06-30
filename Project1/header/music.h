#ifndef _MUSIC_H_

#define _MUSIC_H_ 1

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

#define music_dir "/Project/music/"

struct music{
    char name[50];
    struct music* next;
    struct music* last;
};


struct music* print_dir(char *path, int depth);// ��ȡ�����б�
void show_music(struct music* header);// ��ʾ�����б�
void music(int statue, struct music* header, struct music** current,int* STOP);// madplay����
void voice(unsigned int dat, int mask);// ��������


#endif

/*ioctl()
 SOUND_MIXER_READ  ����ȡ����ͨ���������С
 SOUND_MIXER_WRITE ���û���ͨ���������С
        ������
            SOUND_MIXER_VOLUME ����������
            SOUND_MIXER_BASS ��������
            SOUND_MIXER_TREBLE ��������
            SOUND_MIXER_SYNTH FM �ϳ���
            SOUND_MIXER_PCM ��D/A ת����
            SOUND_MIXER_SPEAKER PC ����
            SOUND_MIXER_LINE ��Ƶ������
            SOUND_MIXER_MIC ��˷�����
            SOUND_MIXER_CD CD ����
            SOUND_MIXER_IMIX ��������
            SOUND_MIXER_ALTPCM ��D/A ת����
            SOUND_MIXER_RECLEV ¼������
            SOUND_MIXER_IGAIN ��������
            SOUND_MIXER_OGAIN �������
            SOUND_MIXER_LINE1 �����ĵ�1 ����
            SOUND_MIXER_LINE2 �����ĵ�2 ����
            SOUND_MIXER_LINE3 �����ĵ�3 ����
*/