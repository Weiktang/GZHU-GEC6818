#define GPIOE_ALTFN0 (*(volatile unsigned int *)0xC001E020)
#define GPIOE_OUTENB (*(volatile unsigned int *)0xC001E004)
#define GPIOE_OUT    (*(volatile unsigned int *)0xC001E000)

#define GPIOC_ALTFN0 (*(volatile unsigned int *)0xC001C020)
#define GPIOC_ALTFN1 (*(volatile unsigned int *)0xC001C024)
#define GPIOC_OUTENB (*(volatile unsigned int *)0xC001C004)
#define GPIOC_OUT    (*(volatile unsigned int *)0xC001C000)

void delay(int val);

int _start(void) //delay()需要是第一个定义的函数
{
	// 查询2.3.1章functionx的功能
	//将GPIO配置——设置为普通I/O功能
	//ALT function0 -- 00
	GPIOE_ALTFN0 &= ~(3<<26);// led7  GPIOE13
	//ALT function1 -- 01
	GPIOC_ALTFN1 |= (1<<1); // led8  GPIOC17
	GPIOC_ALTFN0 |= (1<<16);// led9  GPIOC8
	GPIOC_ALTFN0 |= (1<<14);// led10 GPIOC7

	//将IO功能设置为输出
	GPIOE_OUTENB |= (1<<13);
	GPIOC_OUTENB |= (1<<17);
	GPIOC_OUTENB |= (1<<8);
	GPIOC_OUTENB |= (1<<7);

	GPIOE_OUT |= (1<<13);
	GPIOC_OUT |= (1<<17);
	GPIOC_OUT |= (1<<8);
	GPIOC_OUT |= (1<<7);

	while(1)
	{	
		GPIOE_OUT &= ~(1<<13); // 低电平 亮灯
		delay(0x3000000);
		GPIOE_OUT |= (1<<13); // 高电平 灭灯
		delay(0x3000000);

		GPIOC_OUT &= ~(1<<17);
		delay(0x3000000);
		GPIOC_OUT |= (1<<17);
		delay(0x3000000);

		GPIOC_OUT &= ~(1<<8);
		delay(0x3000000);
		GPIOC_OUT |= (1<<8);
		delay(0x3000000);

		GPIOC_OUT &= ~(1<<7);
		delay(0x3000000);
		GPIOC_OUT |= (1<<7);
		delay(0x3000000);
	}
	return 0;
}

void delay(int val)
{
	for(int i=0;i<val;i++);
}

// led7 GPIOE13
// led8 GPIOC17
// led9 GPIOC8
// led10 GPIOC7
// uboot命令：loady 0x40000000 *.bin
//            go 0x40000000 *.bin