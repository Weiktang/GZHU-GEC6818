#include "header/process.h"
#include "header/LCD.h"
#include "header/touch.h"
#include "header/led.h"
#include "header/beep.h"



void process(int control_bmp)
{
    int fd_bmp;
    int x, y;
    int STOP = 0;//是否暂停
    printf("获取音乐\n");
    struct  music* music_header;
    music_header =  print_dir(music_dir,0); // 获取列表头
    struct music* current = music_header;
    //显示音乐链表
    show_music(music_header);
    while(1)
    {   
        control_bmp = open("/Project/pic/control.bmp", O_RDONLY);
        bmp_process(control_bmp);
        x = 0;
        y = 0;
        touch_print_px(&x, &y);
        if((x >= 80) && (x <=240) && (y >= 130) && ( y<= 190))// 功能1
        {
            process1(fd_bmp);
        }
        else if((x >= 500) && (x <= 700) && (y >= 120) && ( y<= 170))// 功能2
        {
            process2(fd_bmp);
            close(fd_bmp);
        }
        else if((x >= 70) && (x <= 344) && (y >= 245) && ( y<= 275))// 功能3
        {
            process3(fd_bmp);
        }
        else if((x >= 500) && (x <= 695) && (y >= 230) && ( y<= 285))// 功能4
        {
            process4(fd_bmp);
        }
        else if((x >= 80) && (x <= 295) && (y >= 360) && ( y<= 390))// 功能5
        {
            process5(fd_bmp, &STOP, music_header, &current);
            close(fd_bmp);
        }
        else if((x >= 495) && (x <= 695) && (y >= 350) && ( y<= 400))// 功能6
        {
            process6(fd_bmp);
        }
    }
   
}

void process1(int fd_bmp)
{ 
    int x, y;
    int fd_led = open("/Project/pic/led_system.bmp", O_RDONLY);
    bmp_process(fd_led);
    //  读取LED灯状态
    unsigned char led_buf[4];
    led_read(led_buf);
    usleep(100);
    for(int i = 0;i < 4;i++)
    {
        if(led_buf[i] == LED_OFF || led_buf[i] == LED_ON)
        {
            bmp_process2(i + 7, led_buf[i]);// 修改部分图片
        }
        else
        {
            printf("Error, can't get the statue!\n");
        }
    }
    while(1)
    {
        touch_print_px(&x, &y);
        //  控制LED灯状态
        if((x >= 214) && (x <=290) && (y >= 164) && ( y <= 279))// LED1
        {
            led_buf[0] = !led_buf[0];
            led_ctl(LED1, led_buf[0]);
            led_read(led_buf);
            bmp_process2(LED1, led_buf[0]);//修改部分图片
        }
        else if((x >= 500) && (x <=570) && (y >= 175) && ( y <= 265))// LED2
        {
            led_buf[1] = !led_buf[1];
            led_ctl(LED2, led_buf[1]);
            led_read(led_buf);
            bmp_process2(LED2, led_buf[1]);//修改部分图片
        }
        else if((x >= 214) && (x <=290) && (y >= 360) && ( y <= 415))// LED3
        {
            led_buf[2] = !led_buf[2];
            led_ctl(LED3, led_buf[2]);
            led_read(led_buf);
            bmp_process2(LED3, led_buf[2]);//修改部分图片
        }
        else if((x >= 500) && (x <=570) && (y >= 360) && ( y <= 415))// LED4
        {
            led_buf[3] = !led_buf[3];
            led_ctl(LED4, led_buf[3]);
            led_read(led_buf);
            bmp_process2(LED4, led_buf[3]);//修改部分图片
        }
        else if((x > 0) && (x <= 130) && (y > 0) && ( y<= 70))// 退出LED界面
        {
            break;
        }
        else
        {
            continue;
        }
    }
}

void process2(int fd_bmp)
{
    int x, y;
    fd_bmp = open("/Project/pic/beap.bmp", O_RDONLY);
    bmp_process(fd_bmp);
    //  读取蜂鸣器状态
    unsigned char beep_buf;
    beep_read(&beep_buf);
    usleep(100);
    while(1)
    {
        touch_print_px(&x, &y);
        if((x > 0) && (x <= 130) && (y > 0) && ( y<= 70))// 返回
        {
            break;
        }
        else if((x >= 180) && (x <= 295) && (y >= 265) && ( y<= 360))
        {
            beep_buf = 1;
            if(beep_buf == 1)
            {
                beep_ctl(beep_buf);
            }
            // printf("loud\n");
        }
        else if((x >= 555) && (x <= 630) && (y >= 265) && ( y<= 360))
        {
            beep_buf = 0;
            if(beep_buf == 0)
            {
                beep_ctl(beep_buf);
            }
            // printf("low\n");
        }
        else
        {
            continue;
        }
    }
}

void process3(int fd_bmp)
{
    int x, y;
    int i = 0;
    bool flag= false;
    long color[6] = {Red, Green, Blue, Black, White, Yellow};
    pthread_t id; // 开启线程
    int res = pthread_create(&id, NULL, touch_print_px2, (void*)(&flag));
    
    while(1)
    {
        show(color[i]);
        sleep(1);
        i++;
        if(i == 6)
        {
            i = 0;
            show(color[i]);
        }
        if(flag)
        {
            break;
        }
    }

    // 线程销毁
}

void process4(int fd_bmp)
{
    int x, y;
    int i = 0;
    bool flag= false;
    pthread_t id; // 开启线程
    int res = pthread_create(&id, NULL, touch_print_px2, (void*)(&flag));
    while(1)
    {
        show_Flag(i);
        sleep(1);
        i++;
        if(i == 5)
        {
            i = 0;
            show_Flag(i);      
        }
        if(flag)
        {
            break;
        }
    }
}

void process5(int fd_bmp,int* STOP ,struct music* header, struct music** current)
{
    int x, y;
    fd_bmp = open("/Project/pic/music.bmp", O_RDONLY);
    bmp_process(fd_bmp);
    
    while(1)
    {
        touch_print_px(&x, &y);
        if((x > 0) && (x <= 120) && (y > 0) && ( y<= 70))// 返回
        {
            music(5,header, current, STOP);
            break;
        }
        else if((x >= 140) && (x <= 250) && (y >= 310) && ( y<= 350))
        {
            music(0,header, current, STOP);
            // printf("last\n");
        }
        else if((x >= 320) && (x <= 385) && (y >= 275) && ( y<= 350))
        {
            music(1,header, current, STOP);
            //printf("stop\n");
        }
        else if((x >= 465) && (x <= 517) && (y >= 270) && ( y<= 355))
        {
            music(2,header, current, STOP);
            // printf("start\n");
        }
        else if((x >= 595) && (x <= 690) && (y >= 295) && ( y<= 327))
        {   
            music(3,header, current, STOP);
            // printf("next\n");
        }
        else if((x >= 390) && (x <= 595) && (y >= 380) && ( y<= 480))
        {   
            music(4,header, current, STOP);
            // printf("KILL\n");
        }
        else if((x >= 700) && (x <= 790) && (y >= 380) && ( y<= 480))
        {   
            voice(10,1);
        }
        else if((x > 0) && (x <= 100) && (y >= 380) && ( y<= 480))
        {
            voice(10,0);
        }
        else
        {
            continue;
        }
    }
}

void process6(int fd_bmp)
{
    // wait to choose the function
    printf("control house\n");
}

