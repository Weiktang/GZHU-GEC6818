#ifndef _BEAP_H_

#define _BEAP_H_ 1

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <linux/input.h>

void touch_print_px(int *ABS_x, int *ABS_y);

#endif