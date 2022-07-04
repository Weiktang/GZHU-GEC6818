#include "button.h"

void button_read(unsigned char* button_statue)
{
    int fd_button = open("/dev/button_drv",O_RDWR);
    if(fd_button == -1)
    {
        perror("button open");
    }
    char button_buf[4];
    read(fd_button, button_buf, sizeof(button_buf));
    for(int i = 0;i < 4;i++)
    {
        button_statue[i] = button_buf[i];
    }
    close(fd_button);
}