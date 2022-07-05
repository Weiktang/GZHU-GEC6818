#ifndef _WD_H_

#define _WD_H_ 1

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <signal.h>

#include <linux/ioctl.h>

#define WD_OPEN _IO('W', 0X20)
#define WD_CLOSE _IO('W', 0x21)
#define WD_ALIVE _IO('W', 0x22)
 
int wd_start(void); // start watchdog
int wd_stop(void);  // stop watchdog
int wd_alive(void); // keep alive
void* wd_thread(void* temp); // keep alive thread
void* wd_siganl(void); // for signal

#endif