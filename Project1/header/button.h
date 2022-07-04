#ifndef _BUTTON_H_

#define _BUTTON_H_ 1

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define	Button2	2 // GPIOA28 K2
#define	Button3	3 // GPIOB30 K3
#define	Button4	4 // GPIOB31 K4
#define	Button6	6 // GPIOB9  K6

#define BUTTON_ON 0
#define BUTTON_OFF 1

void button_read(unsigned char *button_statue);

#endif