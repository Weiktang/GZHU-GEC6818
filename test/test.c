#include <unistd.h>
#include <fcntl.h>

#define MIXER_DEV "/dev/dsp"

#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <stdio.h>

typedef enum {
    AUDIO_VOLUME_SET,
    AUDIO_VOLUME_GET,
} audio_volume_action;

/*
  Drawbacks. Sets volume on both channels but gets volume on one. Can be easily adapted.
 */
int audio_volume(audio_volume_action action, long* outvol)
{
    int ret = 0;
    int fd, devs;

    if ((fd = open(MIXER_DEV, O_WRONLY)) > 0)
    {
        if(action == AUDIO_VOLUME_SET) {
            if(*outvol < 0 || *outvol > 100)
                return -2;
            *outvol = (*outvol << 8) | *outvol;
            ioctl(fd, SOUND_MIXER_WRITE_VOLUME, outvol);
        }
        else if(action == AUDIO_VOLUME_GET) {
            ioctl(fd, SOUND_MIXER_READ_VOLUME, outvol);
            *outvol = *outvol & 0xff;
        }
        close(fd);
        return 0;
    }
    return -1;
}

int main(void)
{
    long vol = -1;
    int i = audio_volume(AUDIO_VOLUME_GET, &vol);
    printf("Ret %i\n", i);
    printf("Master volume is %i\n", vol);

    vol = 50;
    printf("Ret %i\n", audio_volume(AUDIO_VOLUME_SET, &vol));
    if(i == 0)
    {
        printf("hello world\n");
    }

    return 0;
}