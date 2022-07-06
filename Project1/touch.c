#include "header/touch.h"
#include <unistd.h>

void touch_print_px(int* x, int* y)
{
    int fd_touch;
    fd_touch = open("/dev/input/event0", O_RDWR);
    if(fd_touch == -1)
	{
		perror("open touch");
	}
    struct input_event *data = NULL;
    data = malloc(sizeof(struct input_event));
    
    for(int i =0;i<6;i++)
    {
        read(fd_touch, data, sizeof(struct input_event));
        if(data->type == EV_ABS)
        {
            if(data->code == ABS_X)
            {
                *x = data->value;
                *x = *x * 800/1024;
            }
            if(data->code == ABS_Y)
            {
                *y = data->value;
		        *y = *y * 480/600;
            }
        }
    }
    // printf("function: (%d, %d)\n", *x, *y);
    free(data);
    close(fd_touch);
}

// 多线程版本
void *touch_print_px2(void* flag)
{
    int fd_touch;
    int x,y = 0;
    fd_touch = open("/dev/input/event0", O_RDWR);
    if(fd_touch == -1)
	{
		perror("open touch");
	}
    struct input_event *data = NULL;
    data = malloc(sizeof(struct input_event));
    
    while(1)
    {
        x = 0;
        y = 0;
        for(int i =0;i<6;i++)
        {
            read(fd_touch, data, sizeof(struct input_event));
            if(data->type == EV_ABS)
            {
                if(data->code == ABS_X)
                {
                    x = data->value;
                    x = x * 800/1024;
                }
                if(data->code == ABS_Y)
                {
                    y = data->value;
                    y = y * 480/600;
                }

                if((x > 0) && (x <= 120) && (y > 0) && ( y<= 70))
                {
                    *(bool*) flag = true;
                    free(data);
                    close(fd_touch);
                    pthread_exit(0);
                }
            }
        }
    }
}

void *touch_print_px3(void* flag) // 空调专属
{
    int fd_touch;
    int x,y = 0;
    fd_touch = open("/dev/input/event0", O_RDWR);
    if(fd_touch == -1)
	{
		perror("open touch");
	}
    struct input_event *data = NULL;
    data = malloc(sizeof(struct input_event));
    
    while(1)
    {
        x = 0;
        y = 0;
        for(int i =0;i<6;i++)
        {
            read(fd_touch, data, sizeof(struct input_event));
            if(data->type == EV_ABS)
            {
                if(data->code == ABS_X)
                {
                    x = data->value;
                    x = x * 800/1024;
                }
                if(data->code == ABS_Y)
                {
                    y = data->value;
                    y = y * 480/600;
                }

                if((x > 0) && (x <= 120) && (y > 0) && ( y<= 70))// 退出
                {
                    *(int*) flag = 1;
                    free(data);
                    close(fd_touch);
                    pthread_exit(0);
                }
                if((x > 310) && (x <= 475) && (y > 370) && ( y<= 411))// 按下开关
                {
                    *(int*) flag = 2;
                }
            }
        }
        // printf("%d,%d\n",x,y);
    }
}