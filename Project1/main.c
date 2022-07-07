#include "header/LCD.h"
#include "header/touch.h"
#include "header/process.h"
#include "header/music.h"
#include "header/watch_dog.h"

int main()
{
	// 看门猪
	signal(2, (void*)wd_siganl);// "ctrl + c"退出则关闭看门猪
	// pthread_t id; // 开启线程
    // int res = pthread_create(&id, NULL, wd_thread, NULL);// watch_dog init(thread)

	// 控制终端
	int main_bmp, control_bmp;
	int x, y;

	main_bmp = open("/Project/pic/system.bmp", O_RDONLY);
	control_bmp = open("/Project/pic/control.bmp", O_RDONLY);
	bmp_process(main_bmp);
	while(1)//从主界面进入控制界面
	{	
		touch_print_px(&x, &y);
		if((x >=300) && (x <= 520) && (y >= 330) && ( y<=370))
		{
			bmp_process(control_bmp);
			break;
		}
	}	
	process(control_bmp);

	return 0;
}