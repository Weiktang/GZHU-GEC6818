#include "header/process.h"
#include "header/LCD.h"
#include "header/touch.h"
#include "header/led.h"
#include "header/beep.h"
#include "header/button.h"
#include "header/watch_dog.h"


void process(int control_bmp)
{
    int x, y;
    // music
    int STOP = 0;//是否暂停
    printf("获取音乐\n");
    struct  music* music_header;
    music_header =  print_dir(music_dir,0); // 获取列表�?
    struct music* music_current = music_header;
    //显示音乐链表
    show_music(music_header);
    printf("------------------------------------\n");   

    // picture
    printf("获取图片\n");
    struct  pic* pic_header;
    pic_header =  print_pic_dir(pic_dir,0); // 获取列表�?
    struct pic* pic_current = pic_header;
    //显示音乐链表
    show_pic(pic_header);
    printf("------------------------------------\n");  

    //家庭控制
    bool air = false;

    while(1)
    {   
        control_bmp = open("/Project/pic/control.bmp", O_RDONLY);
        bmp_process(control_bmp);
        x = 0;
        y = 0;
        touch_print_px(&x, &y);
        if((x >= 80) && (x <=240) && (y >= 130) && ( y<= 190))// 功能1
        {
            process1();
        }
        else if((x >= 500) && (x <= 700) && (y >= 120) && ( y<= 170))// 功能2
        {
            process2();
        }
        else if((x >= 70) && (x <= 344) && (y >= 245) && ( y<= 275))// 功能3
        {
            process3();
        }
        else if((x >= 500) && (x <= 695) && (y >= 230) && ( y<= 285))// 功能4
        {
            process4(pic_header, &pic_current);
        }
        else if((x >= 80) && (x <= 295) && (y >= 360) && ( y<= 390))// 功能5
        {
            process5(&STOP, music_header, &music_current);
        }
        else if((x >= 495) && (x <= 695) && (y >= 350) && ( y<= 400))// 功能6
        {
            process6(&air);
        }
        // else if((x > 0) && (x <= 130) && (y > 0) && ( y<= 70))// 功能7
        // {   
        //     process7();
        // }
    }
   
}

void process1()
{ 
    int x, y;
    int fd_led = open("/Project/pic/led_system.bmp", O_RDONLY);
    bmp_process(fd_led);
    //  读取LED灯状态
    unsigned char led_buf[4];
    unsigned char temp[4];
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
            led_read(temp);
            if(temp[0] == led_buf[0])
            {
                bmp_process2(LED1, led_buf[0]);//修改部分图片
            }
            else
            {
                printf("Error, can't set the LED1 statue!\n");
                led_buf[0] = temp[0];
            }
            
        }
        else if((x >= 500) && (x <=570) && (y >= 175) && ( y <= 265))// LED2
        {
            led_buf[1] = !led_buf[1];
            led_ctl(LED2, led_buf[1]);
            led_read(temp);
            if(temp[1] == led_buf[1])
            {
                bmp_process2(LED2, led_buf[1]);//修改部分图片
            }
            else
            {
                printf("Error, can't set the LED2 statue!\n");
                led_buf[1] = temp[1];
            }
        }
        else if((x >= 214) && (x <=290) && (y >= 360) && ( y <= 415))// LED3
        {
            led_buf[2] = !led_buf[2];
            led_ctl(LED3, led_buf[2]);
            led_read(temp);
            if(temp[2] == led_buf[2])
            {
                bmp_process2(LED3, led_buf[2]);//修改部分图片
            }
            else
            {
                printf("Error, can't set the LED3 statue!\n");
                led_buf[2] = temp[2];
            }  
        }
        else if((x >= 500) && (x <=570) && (y >= 360) && ( y <= 415))// LED4
        {
            led_buf[3] = !led_buf[3];
            led_ctl(LED4, led_buf[3]);
            led_read(temp);
            if(temp[3] == led_buf[3])
            {
                bmp_process2(LED4, led_buf[3]);//修改部分图片
            }
            else
            {
                printf("Error, can't set the LED4 statue!\n");
                led_buf[3] = temp[3];
            }  
        }
        else if((x > 0) && (x <= 130) && (y > 0) && ( y<= 70))// 退出LED界面
        {
            close(fd_led);
            break;
        }
        else
        {
            continue;
        }
    }
}

void process2()
{
    int x, y;
    int fd_bmp = open("/Project/pic/beap.bmp", O_RDONLY);
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
            close(fd_bmp);
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

void process3()
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

void process4(struct pic* header, struct pic** current)
{
    int x, y;
    int i = 0;
    bool flag= false;
    pthread_t id; // 开启线程
    int res = pthread_create(&id, NULL, touch_print_px2, (void*)(&flag));
    while(1)
    {
        // 国旗
        // show_Flag(i);
        // sleep(1);
        // i++;
        // if(i == 5)
        // {
        //     i = 0;
        //     show_Flag(i);      
        // }

        // 图像
        pic(header, current);
        sleep(1);
        if(flag)
        {
            break;
        }
    }
}

void process5(int* STOP ,struct music* header, struct music** current)
{
    int x, y;
    int fd_bmp = open("/Project/pic/music.bmp", O_RDONLY);
    bmp_process(fd_bmp);
    
    while(1)
    {
        touch_print_px(&x, &y);
        if((x > 0) && (x <= 120) && (y > 0) && ( y<= 70))// 返回
        {
            music(5,header, current, STOP);
            close(fd_bmp);
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

void process6(bool* air)
{
    //初始化
    int fd_kt = open("/Project/pic/kongtiao.bmp", O_RDONLY);
    bmp_process(fd_kt);
    bmp_process3(*air);
    // show(White);
    
    //线程
    int flag= 0;
    pthread_t id; // 开启线程
    int res = pthread_create(&id, NULL, touch_print_px3, (void*)(&flag));
    // 按键检测
    unsigned char button_buf[4];
    unsigned char temp[4];
    button_read(button_buf);
    button_read(temp);
    if(*air == true) // 恢复上一次状态
    {
        temp[0] = 1; // 按键K2曾经按过
    }
    else
    {
        temp[3] = 1;  // 按键K6曾经按过
    }
    
    usleep(100);
    while(1)
    {
        if(flag == 1)
        {
            close(fd_kt);
            break;
        }
        else if(flag == 2)
        {
            if(*air)
            {
                *air = false;
                temp[3] = BUTTON_ON;  // 假装按键K6曾经按过
                temp[0] = BUTTON_OFF; 
                bmp_process3(*air);
                printf("Close the Air conditioner\n");
            }
            else
            {
                *air = true;
                temp[0] = BUTTON_ON; // 假装按键K2曾经按过
                temp[3] = BUTTON_OFF; 
                bmp_process3(*air);
                printf("Open the Air conditionert\n");
            }
            flag = 0; // 清零
        }
        button_read(button_buf);
        usleep(100);
        for(int i = 0;i < 4;i++)
        {
            if((button_buf[i] == BUTTON_ON) && (button_buf[i] != temp[i])) // 按下1次
            {
                temp[i] = button_buf[i];
                if(i == 3)
                {
                    if(*air == true)
                    {
                        printf("K%d is pressed\n", (i + 3));
                        printf("Close the Air conditioner\n");
                        *air = false;
                        bmp_process3(*air);
                    }
                    else
                    {
                        printf("K%d is pressed\n", (i + 3));
                        printf("The Air conditioner has closed\n");
                    }
                }
                else if(i == 0)
                {
                    if(*air == false)
                    {
                        printf("K%d is pressed\n", (i + 2));
                        printf("Open the Air conditionert\n");
                        *air = true;
                        bmp_process3(*air);
                    }
                    else
                    {
                        printf("K%d is pressed\n", (i + 2));
                        printf("The Air conditioner has opened\n");
                    }
                }
                else if(i == 1)
                {
                    printf("K%d is pressed\n", (i + 2));
                }
                else if(i == 2)
                {
                    printf("K%d is pressed\n", (i + 2));
                }
            }
            else if((button_buf[i] == BUTTON_OFF) && (button_buf[i] != temp[i]))// 按下后释放
            {
                temp[i] = button_buf[i];
                // if(i == 3)
                // {
                //     printf("K%d is released\n", (i + 3));
                // }
                // else
                // {
                //     printf("K%d is released\n", (i + 2));
                // }
            }
        }

    }
}

void process7()
{
    // 看门猪驱动测试
    // int count =0;
    // wd_start();
    // printf("start watch_dog end\n");
    // while(1)
    // {
    //     count++;
    //     sleep(1);
    //     if(count % 3 == 0)
    //     {
    //         wd_alive();	
    //         printf("alive watch_dog end\n");
    //     }
        
    //     printf("reset %d\n", count);
    //     fflush(stdout);
    //     if(count == 30)
    //     {
    //         wd_stop();
    //         printf("stop watch_dog end\n");
    //         break;
    //     }
    // }

    // 看门猪测试2 --- 模拟程序卡死       
    kill_myself();

    //TODO Test
    // printf("手写板模式 测试\n");
    // show(White);
    // while(1)
    // {
    //     int x, y;
    //     int last_x, last_y;
    //     while(1)
    //     {
    //         touch_print_px(&x,&y);
    //         show_point(x, y, &last_x, &last_y);
    //     }
        
    // }
    
}

void kill_myself()
{
    system("killall -KILL main");
}