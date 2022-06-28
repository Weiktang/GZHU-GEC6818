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
    // char led[4] = {'5','5','5','5'};
    // show(led);
    // printf("%c\n",*(led));
    // printf("%c\n",*(led+1));
    // printf("%c\n",*(led+2));
    // printf("%c\n",*(led+3));
    unsigned char a = 10;
    int b = a;
    printf("%d", b);
}