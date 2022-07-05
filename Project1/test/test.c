#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "header/watch_dog.h"

int main(void)
{
	int count =0;
	wd_start();
	while(1)
	{
		count++;
		sleep(1);
		if(count % 4 == 0)
			wd_alive();		
		
		printf("reset %d\n", count);
	}
	return 0;
	
}