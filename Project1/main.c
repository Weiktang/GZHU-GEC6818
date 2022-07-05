#include "header/LCD.h"
#include "header/touch.h"
#include "header/process.h"
#include "header/music.h"
#include "header/watch_dog.h"

int main()
{
	// ������
	signal(2, (void*)wd_siganl);// �����ֶ��˳���رտ�����
	pthread_t id; // �����߳�
    int res = pthread_create(&id, NULL, wd_thread, NULL);// watch_dog init(thread)

	// �����ն�
    int i = 0;
	int main_bmp, control_bmp;
	int x, y;
	int last_x,last_y = 0;
	struct Rect rect[rect_num] = {
		[0] = {
			.x = 100,
			.y = 60,
			.l = 500,
			.h = 320, 
		},
		[1] = {
			.x = 200,
			.y = 100,
			.l = 200,
			.h = 180, 
		},
		[2] = {
			.x = 300,
			.y = 160,
			.l = 50,
			.h = 50, 
		}		
	};
	main_bmp = open("/Project/pic/system.bmp", O_RDONLY);
	control_bmp = open("/Project/pic/control.bmp", O_RDONLY);
	bmp_process(main_bmp);
	while(1)//�������������ƽ���
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