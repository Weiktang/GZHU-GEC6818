#ifndef _LED_H_

#define _LED_H_ 1

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define LED1    7 
#define LED2    8 
#define LED3    9 
#define LED4    10 

#define LED_ON    0 // low
#define LED_OFF    1 // high

void led_ctl(unsigned char led_n, unsigned char statue);
void led_read(unsigned char* led_statue);

#endif