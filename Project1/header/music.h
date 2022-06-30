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


struct music* print_dir(char *path, int depth);// 获取音乐列表
void show_music(struct music* header);// 显示音乐列表
void music(int statue, struct music* header, struct music** current,int* STOP);// madplay控制
void voice(unsigned int dat, int mask);// 音量控制


#endif

/*ioctl()
 SOUND_MIXER_READ  来读取混音通道的增益大小
 SOUND_MIXER_WRITE 设置混音通道的增益大小
        参数：
            SOUND_MIXER_VOLUME 主音量调节
            SOUND_MIXER_BASS 低音控制
            SOUND_MIXER_TREBLE 高音控制
            SOUND_MIXER_SYNTH FM 合成器
            SOUND_MIXER_PCM 主D/A 转换器
            SOUND_MIXER_SPEAKER PC 喇叭
            SOUND_MIXER_LINE 音频线输入
            SOUND_MIXER_MIC 麦克风输入
            SOUND_MIXER_CD CD 输入
            SOUND_MIXER_IMIX 放音音量
            SOUND_MIXER_ALTPCM 从D/A 转换器
            SOUND_MIXER_RECLEV 录音音量
            SOUND_MIXER_IGAIN 输入增益
            SOUND_MIXER_OGAIN 输出增益
            SOUND_MIXER_LINE1 声卡的第1 输入
            SOUND_MIXER_LINE2 声卡的第2 输入
            SOUND_MIXER_LINE3 声卡的第3 输入
*/