#include "header/beep.h"

void beep_ctl(unsigned char statue)
{
    int fd_beep = open("/dev/beep_drv",O_RDWR);
    if(fd_beep == -1)
    {
        perror("beep open");
    }
    char beep_buf; 
    beep_buf = statue;

    write(fd_beep, &beep_buf, sizeof(beep_buf));

    close(fd_beep);
}

void beep_read(unsigned char* beep_statue)
{
    int fd_beep = open("/dev/beep_drv",O_RDWR);
    if(fd_beep == -1)
    {
        perror("beep open");
    }
    unsigned char beep_buf;
    read(fd_beep, &beep_buf, sizeof(beep_buf));
    *beep_statue = beep_buf;
    close(fd_beep);
}