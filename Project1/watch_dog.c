#include "header/watch_dog.h"

int wd_start(void)
{
    int fd_wd = open("/dev/watch_dog_drv", O_RDWR);
    if(fd_wd == -1)
    {
        perror("open watch dog");
        return -1;
    }

    int ret = ioctl(fd_wd, WD_OPEN);
    if(ret == -1)
    {
        perror("ioctl watch dog");
        return -2;
    }
    close(fd_wd);

    wd_alive();
    return 0;
}

int wd_stop(void)
{
    int fd_wd = open("/dev/watch_dog_drv", O_RDWR);
    if(fd_wd == -1)
    {
        perror("open watch dog");
        return -1;
    }

    int ret = ioctl(fd_wd, WD_CLOSE);
    if(ret == -1)
    {
        perror("ioctl watch dog");
        return -2;
    }

    close(fd_wd);

    return 0;

}

int wd_alive(void)
{
    int fd_wd = open("/dev/watch_dog_drv", O_RDWR);
    if(fd_wd == -1)
    {
        perror("open watch dog");
        return -1;
    }

    int ret = ioctl(fd_wd, WD_ALIVE);
    if(ret == -1)
    {
        perror("ioctl watch dog");
        return -2;
    }

    close(fd_wd);

    return 0;
}

void* wd_thread(void* temp)
{
    int count =0;
    wd_start();
    printf("start watch_dog successfully\n");
    while(1)
    {
        count++;
        sleep(1);
        if(count % 3 == 0)
        {
            wd_alive();	
        }
    }    
}

void* wd_siganl(void)
{
    wd_stop();
    _exit(0);
}
