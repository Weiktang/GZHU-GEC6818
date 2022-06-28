#include "led.h"

void led_ctl(unsigned char led_n, unsigned char statue)
{
    int fd_led = open("/dev/led_drv",O_RDWR);
    if(fd_led == -1)
    {
        perror("led open");
    }

    char led_buf[2]; 
    led_buf[0] = statue;
    led_buf[1] = led_n;

    write(fd_led, led_buf, sizeof(led_buf));

    close(fd_led);
}

void led_read(unsigned char* led_statue)
{
    int fd_led = open("/dev/led_drv",O_RDWR);
    if(fd_led == -1)
    {
        perror("led open");
    }
    char led_buf[4];
    read(fd_led, led_buf, sizeof(led_buf));
    for(int i = 0;i < 4;i++)
    {
        led_statue[i] = led_buf[i];
    }
    close(fd_led);
}