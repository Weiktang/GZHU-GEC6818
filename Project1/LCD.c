#include "header/LCD.h"
#include "header/led.h"

void show(long color)
{
    int fd_lcd;
    int* lcd_buf = NULL;
    fd_lcd = open("/dev/fb0", O_RDWR); 
	if(fd_lcd == -1)
	{
		perror("open lcd");
	}
	lcd_buf = (int*)mmap(NULL, Buffer_Size*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd_lcd, 0);
    
	for(int i=0;i<Buffer_Size;i++)
	{   
		*(lcd_buf + i) = color; 
	}

	munmap(lcd_buf,Buffer_Size*4);
    close(fd_lcd);

}

void show_point(int x, int y,int *last_x,int *last_y)
{
    int fd_lcd;
    int* lcd_buf = NULL;

    fd_lcd = open("/dev/fb0", O_RDWR); 
	if(fd_lcd == -1)
	{
		perror("open lcd");
	}
	lcd_buf = (int*)mmap(NULL, Buffer_Size*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd_lcd, 0);
    

	// time 15点14分 tang
	if( (*last_x) & (*last_y) != 0)
	{
		int distance = sqrt(pow((*last_y - y),2)+pow((*last_x) - x,2));
		double rate =  (y - *last_y) / (x - *last_x);
		int point_address;
		for(int i =0;i<distance;i++)
		{
			point_address =(*last_x + rate*i)+(*last_y+rate*i);
			*(lcd_buf + point_address) = Black; 
			*(lcd_buf + x + y*800) = Black; 		
		}
	}
	*last_x = x;
	*last_y = y;

	// *(lcd_buf + x + y*800) = Black; 
	
	munmap(lcd_buf,Buffer_Size*4);
    close(fd_lcd);

}

void show_Rect(int num, struct Rect rect[], long* color)
{
	int fd_lcd;
    int* lcd_buf = NULL;
    fd_lcd = open("/dev/fb0", O_RDWR); 
	if(fd_lcd == -1)
	{
		perror("open lcd");
	}
	lcd_buf = (int*)mmap(NULL, Buffer_Size*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd_lcd, 0);

	for(int i = 0;i < 800;i++)
	{
		for(int j = 0;j < 480;j++)
		{
			*(lcd_buf + i * 480 + j) = White; 
		}
	}
	for(int t =0;t<num;t++)
	{
		int adr = rect[t].x + rect[t].y*800;
		for(int i = 0;i < rect[t].h;i++)
		{
			int tmp = adr;
			for(int j = 0;j < rect[t].l;j++)
			{
				*(lcd_buf + adr) = color[t]; 
				adr++;
			}
			adr = tmp + 800;
			// printf("%d:%d\n",t,adr);
		}
	}

	munmap(lcd_buf,Buffer_Size*4);
    close(fd_lcd);
}

void bmp_process(int fd_bmp) // 传入bmp文件描述符，可读
{
    int fd_lcd; // lcd文件描述符
    char data_buff[Buffer_Size*3]; // 缓存数组
    memset(data_buff,0,sizeof(data_buff));

    // 移动文件指针，前面的54B的头信息不是RGB数据
    lseek(fd_bmp, 54, SEEK_SET);
	// 读取bmp数据到data_buff数组中
    // fread(data_buff,sizeof(data_buff),1,fd_bmp);
	read(fd_bmp, data_buff, sizeof(data_buff));


    fd_lcd = open("/dev/fb0", O_RDWR); 
    if(fd_lcd == -1)
    {
        perror("open lcd");
    }
    int *fb_base = NULL;
    fb_base= (int *)mmap(NULL, 800*480*4, PROT_READ | PROT_WRITE, MAP_SHARED,  fd_lcd, 0);
    for(int i =0;i<800*480;i++)
    {
		int address = 800*(479 - i/800) + i%800;
		*(fb_base+address) = (data_buff[3*i+2]<<16) +( data_buff[3*i+1]<<8) + (data_buff[3*i]<<0);
        // 可以修改为显存映射方式
    }
    // 关闭显存映射
    munmap(fb_base, Buffer_Size*4);
    close(fd_lcd);
}

void bmp_process2(int LED_n, int statue)//	LED专用
{
    int fd_lcd;
	int fd_bmp;
	int size = 84*108;
	// printf("size is : %d\n",size);
    fd_lcd = open("/dev/fb0", O_RDWR); 
	if(fd_lcd == -1)
	{
		perror("open lcd");
	}

    int* fb_base = NULL;
	fb_base = (int*)mmap(NULL, Buffer_Size*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd_lcd, 0);


	//图片缓存地址
	char data_buff[size*3]; // 
    memset(data_buff,0,sizeof(data_buff));
	if(statue == LED_ON)
	{
		fd_bmp = open("/Project/pic/led_on.bmp", O_RDONLY);
	}
	else if(statue == LED_OFF)
	{
		fd_bmp = open("/Project/pic/led_off.bmp", O_RDONLY);
	}
	else
	{
		printf("error LED statue!\n");
	}
    lseek(fd_bmp, 54, SEEK_SET);
	read(fd_bmp, data_buff, sizeof(data_buff));

	int base_address;
	if(LED_n == LED1)
	{
		base_address = 800*162 + 222; // 1
	}
	else if(LED_n == LED2)
	{
		base_address = 800*162 + 222+282; // 2
	}
	else if(LED_n == LED3)
	{
		base_address = 800*(162+157) + 222; // 3
	}
	else if(LED_n == LED4)
	{
		base_address = 800*(162+157) + 222+287; // 4
	}
	else
	{
		printf("error LED num!\n");
		goto ERROR;
	}

	for(int i =0;i<108;i++) 
    {
		for(int j=0;j<84;j++)
		{
			*(fb_base+base_address+(107-i)*800+j) = (0x00 << 24)+(data_buff[3*(84*i+j)+2]<<16) +\
                ( data_buff[3*(84*i+j)+1]<<8) + (data_buff[3*(84*i+j)]<<0);
		}
    }

    // printf("i close\n");
	// // 关闭显存映射，关闭文件
	munmap(fb_base,Buffer_Size*4);
	close(fd_lcd);
ERROR:
	munmap(fb_base,Buffer_Size*4);
	close(fd_lcd);
}

void show_Flag(int country)
{
	int fd_lcd;
    int* lcd_buf = NULL;
    fd_lcd = open("/dev/fb0", O_RDWR); 
	if(fd_lcd == -1)
	{
		perror("open lcd");
	}
	lcd_buf = (int*)mmap(NULL, Buffer_Size*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd_lcd, 0);
	switch(country)
	{
		case GERMAN:
		{
			for(int i=0;i<Buffer_Size;i++)
			{	
				if(i < 127680)
				{
					*(lcd_buf + i) = Black;
				}
				else
				{
					if(i < 255360)
					{
						*(lcd_buf + i) = Red;
					}
					else
					{
						if(i < 510720)
						{
							*(lcd_buf + i) = Yellow;
						}
					}
				}
			}
			break;
		}
		
		case RUSSIA:
		{
			 for(int i=0;i<Buffer_Size;i++)
			{	
				if(i < 127680)
				{
					*(lcd_buf + i) = White;
				}
				else
				{
					if(i < 255360)
					{
						*(lcd_buf + i) = Blue;
					}
					else
					{
						if(i < 510720)
						{
							*(lcd_buf + i) = Red;
						}
					}
				}
			}
			break;
		}

		case FRANCE:
		{
			for(int j = 0;j<480;j++)
			{	
				for(int i=0;i<800;i++)
				{
					if(i<266)
					{
						*(lcd_buf + i+ j * 800) = Blue;
					}
					else
					{
						if(i<266*2)
						{
							*(lcd_buf + i+ j * 800) = White;
						}
						else
						{
							*(lcd_buf + i+ j * 800) = Red;
						}
					}
				}
			}
			break;
		}
		
		case ITALY:
		{
			for(int j = 0;j<480;j++)
			{	
				for(int i=0;i<800;i++)
				{
					if(i<266)
					{
						*(lcd_buf + i + j * 800) = Green;
					}
					else
					{
						if(i<266*2)
						{
							*(lcd_buf + i + j * 800) = White;
						}
						else
						{
							*(lcd_buf + i + j * 800) = Red;
						}
					}
				}
			}
			break;
		}
		
		case JAPAN:
		{
			for(int i=0;i<480;i++)
			{
				for(int j =0;j<800;j++)
				{
					*(lcd_buf + j + i * 800) = White; 

					if( (abs(i-240)*abs(i-240) +abs(j-400)*abs(j-400))< (100*100))
					{
						*(lcd_buf + j + i * 800) = Red; 
					}
				}
			}
			break;
		}
		
		default:
			break;
		
	}	
	munmap(lcd_buf,Buffer_Size*4);
	close(fd_lcd);
}



