#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "cfg_type.h"

#define	LED1	7
#define	LED2	8
#define	LED3	9
#define	LED4	10

#define	LED_ON	0
#define	LED_OFF	1



struct gpio_info {
	unsigned int gpio_num;
	const char *gpio_name;
};

struct gpio_info gec6818_led[4] = {
	{
		.gpio_num = PAD_GPIO_E + 13,
		.gpio_name = "led7",
	},
	{
		.gpio_num = PAD_GPIO_C + 17,
		.gpio_name = "led8",
	},
	{
		.gpio_num = PAD_GPIO_C + 8,
		.gpio_name = "led9",
	},
	{
		.gpio_num = PAD_GPIO_C +7,
		.gpio_name = "led10",
	},

};


static int gec6818_led_open(struct inode *inode, struct file *filp)
{
	printk("led driver is openning\n");
	return 0;	
}
static int gec6818_led_close(struct inode *inode, struct file *filp)
{
	printk("led driver is closing\n");
	return 0;		
}
//struct file * ---内核中，指向驱动文件的指针
//const char __user * ---- 应用程序写下来的数据
//size_t  ---- 应用程序下下来数据的字节数
//loff_t * ----- 文件指针的偏移
static ssize_t gec6818_led_write(struct file *folp, const char __user *buf, size_t len, loff_t *off)
{
	unsigned char led_flag[2]; // 1--LED_NUM; 0--LED_STATUS
	if(len != 2)
		return -EINVAL;  //返回给应用程序write()的一个错误码
	if(copy_from_user(led_flag, buf, len))
		return -EFAULT;
	switch(led_flag[1]){
	case LED1:
		gpio_set_value(gec6818_led[0].gpio_num, led_flag[0]);
		break;

	case LED2:
		gpio_set_value(gec6818_led[1].gpio_num, led_flag[0]);
		break;
		
	case LED3:
		gpio_set_value(gec6818_led[2].gpio_num, led_flag[0]);
		break;
		
	case LED4:
		gpio_set_value(gec6818_led[3].gpio_num, led_flag[0]);
		break;
	default:
		return -EINVAL;
	}
	
	return len;	
}

static ssize_t gec6818_led_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	char led_flag[4]; //0--LED1,1--LED2,2--LED3,3--LED; 0--LED ON, 1--LED OFF
	int i=0;
	if(len != 4)
		return -EINVAL;
		
	for(i=0;i<4;i++){
		led_flag[i] = gpio_get_value(gec6818_led[i].gpio_num);
	}
	
	if(copy_to_user(buf, led_flag,len))
		return -EFAULT;
	
	return len;	
}


//定义文件操作集，给应用程序提供统一的接口

static const struct file_operations gec6818_led_fops = {
	.owner = THIS_MODULE,
	.open = gec6818_led_open,
	.write = gec6818_led_write,
	.read = gec6818_led_read,
	.release = gec6818_led_close,
};

//定义混杂设备，并初始化
static struct miscdevice  gec6818_led_misc = {	
	.minor = MISC_DYNAMIC_MINOR,
	.name = "led_drv",
	.fops = &gec6818_led_fops,
};


//module的入口函数----驱动的安装函数
//module_init()-->gec6818_led_init()-->完成驱动的安装
static int __init gec6818_led_init(void)
{
	//从内存申请资源、注册驱动
	int ret;
	int i;
	for(i=0;i<4;i++)
	{
		gpio_free(gec6818_led[i].gpio_num);
		ret = gpio_request(gec6818_led[i].gpio_num, gec6818_led[i].gpio_name);
		if(ret < 0)
		{
			printk("request GPIOE13 failed\n");
			goto gpio_failed;
		}		
		gpio_direction_output(gec6818_led[i].gpio_num, 1); //LED off
	}

	//混杂设备注册
	ret = misc_register(&gec6818_led_misc);
	if(ret < 0)
	{
		printk("misc register failed\n");
		goto misc_reg_failed;
	}
	
	printk(KERN_INFO "gec6818 led driver init...\n");
	
	return 0;
	
misc_reg_failed:	
gpio_failed: 
	while(i--)
		gpio_free(gec6818_led[i].gpio_num);
	
	return ret;
}
//module的出口函数----驱动卸载函数
//module_exit()-->gec6818_led_exit()-->完成驱动的卸载
static void __exit gec6818_led_exit(void)
{
	//释放资源、注销驱动
	int i;
	for(i=0;i<4;i++)
		gpio_free(gec6818_led[i].gpio_num);
	
	//混杂设备的注销
	misc_deregister(&gec6818_led_misc);
	
	printk(KERN_INFO "gec6818 led driver exit...\n");
}


//module入口和出口
module_init(gec6818_led_init);//#insmod led_drv.ko-->调用函数module_init()
module_exit(gec6818_led_exit);//#remmod led_drv.ko-->调用函数module_exit()

//module的描述
MODULE_AUTHOR(" ");
MODULE_DESCRIPTION("LED driver for GEC6818");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("V1.0");