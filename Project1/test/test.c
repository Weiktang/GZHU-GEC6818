#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void show(char* led_buf)
{
    *led_buf = '0';
    *(led_buf+1)= '1';
    *(led_buf+2)= '2';
    *(led_buf+3)= '3';
}

void main()
{
    unsigned char a = 0;
    printf("%d\n", ~a);
}