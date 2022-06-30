#define GPIOC_ALTFN0 (*(volatile unsigned int *)0xC001C020)
#define GPIOC_OUTENB (*(volatile unsigned int *)0xC001C004)
#define GPIOC_OUT    (*(volatile unsigned int *)0xC001C000)

void delay(int val);

int _start(void)
{
	//��GPIO���á�������Ϊ��ͨI/O����(ALT function1 ���� 01)
	GPIOC_ALTFN0 |= (1<<28);// beep GPIOC14

	//��IO��������Ϊ���
	GPIOC_OUTENB |= (1<<14);

    GPIOC_OUT &= ~(1<<14); 
	
	while(1)
	{
		GPIOC_OUT |= (1<<14);// �ߵ�ƽ �����
		delay(0x300000);
		GPIOC_OUT &= ~(1<<14);// �͵�ƽ �����
		delay(0x300000);
	}
	return 0;
}

void delay(int val)
{
	for(int i=0;i<val;i++);
}

// beep GPIOC14