#ifndef _BEEP_H_

#define _BEEP_H_ 1

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define BEEP_ON 1
#define BEEP_OFF 0

void beep_ctl(unsigned char statue);
void beep_read(unsigned char *beep_statue);

#endif