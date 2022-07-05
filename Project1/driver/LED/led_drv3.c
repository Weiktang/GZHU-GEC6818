#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/ioport.h>
#include <linux/device.h>

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


static unsigned int led_major = 0;//���豸��(=0,��̬����;  !=0����̬ע��)
static unsigned int led_minor = 0;//���豸��
static dev_t led_dev_num;

static struct resource * gpioc_res;
static struct resource * gpioe_res;

static volatile unsigned int __iomem *GPIOC_BASE_VA;
static volatile unsigned int __iomem *GPIOCOUT;
static volatile unsigned int __iomem *GPIOCOUTENB;
static volatile unsigned int __iomem *GPIOCALTFN0;
static volatile unsigned int __iomem *GPIOCALTFN1;

static volatile unsigned int __iomem *GPIOE_BASE_VA;
static volatile unsigned int __iomem *GPIOEOUT;
static volatile unsigned int __iomem *GPIOEOUTENB;
static volatile unsigned int __iomem *GPIOEALTFN0;
// static volatile unsigned int __iomem *GPIOEALTFN1;

static struct class *gec6818_led_class;
static struct device *gec6818_led_device;

//  ����һ��cdev���ַ��豸��
static struct cdev gec6818_led_dev;


//  ��cdev����һ���ļ�������
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

static ssize_t gec6818_led_write(struct file *folp, const char __user *buf, size_t len, loff_t *off)
{
	unsigned char led_flag[2]; // 1--LED_NUM; 0--LED_STATUS
	if(len != 2)
		return -EINVAL;  //���ظ�Ӧ�ó���write()��һ��������
	if(copy_from_user(led_flag, buf, len))
		return -EFAULT;

	switch(led_flag[1])
    {
        case LED1:
            if(led_flag[0] == LED_ON)
                *GPIOEOUT &= ~(1<<13);
            else if(led_flag[0] == LED_OFF)
                *GPIOEOUT |= (1<<13);
            break;

        case LED2:
            if(led_flag[0] == LED_ON)
                *GPIOCOUT &= ~(1<<17);
            else if(led_flag[0] == LED_OFF)
                *GPIOCOUT |= (1<<17);
            break;
            
        case LED3:
            if(led_flag[0] == LED_ON)
                *GPIOCOUT &= ~(1<<8);
            else if(led_flag[0] == LED_OFF)
                *GPIOCOUT |= (1<<8);
            break;
            
        case LED4:
            if(led_flag[0] == LED_ON)
                *GPIOCOUT &= ~(1<<7);
            else if(led_flag[0] == LED_OFF)
                *GPIOCOUT |= (1<<7);
            break;

        default:
            return -EINVAL;
	}
	
	return len;	
}

static ssize_t gec6818_led_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	char led_flag[4]; //[0]--LED1,[1]--LED2,[2]--LED3,[3]--LED4; 0--LED ON, 1--LED OFF
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
static const struct file_operations gec6818_led_fops = {
	.owner = THIS_MODULE,
	.open = gec6818_led_open,
	.write = gec6818_led_write,
	.read = gec6818_led_read,
	.release = gec6818_led_close,
};

//module����ں���----�����İ�װ����
//module_init()-->gec6818_led_init()-->��������İ�װ
static int __init gec6818_led_init(void)
{
	int ret;
    // cdev�����豸��
    if(led_major == 0)// ��̬ע��
    {
        ret = alloc_chrdev_region(&led_dev_num, led_minor, 1, "led_dev");
    }
    else// ��̬ע��
    {
        led_dev_num = MKDEV(led_major, led_minor);
        ret = register_chrdev_region(led_dev_num, 1, "led_dev");
    }
    if(ret < 0)
    {
        printk("register_chrdev_region failed\n");
        goto led_num_err;
    }
	// cdev��ʼ��
    cdev_init(&gec6818_led_dev,  &gec6818_led_fops);

    // cdevע���ں�
    ret = cdev_add(&gec6818_led_dev, led_dev_num, 1);
    if(ret < 0)
    {
        printk("cdev_add failed\n");
        goto cdev_add_err;
    }

    // ����class  
	gec6818_led_class = class_create(THIS_MODULE, "leds_class");
	if(gec6818_led_class == NULL)
    {
		printk("create class error\n");
		goto class_err;
	}
	
	// ����device
	gec6818_led_device = device_create(gec6818_led_class, NULL,led_dev_num, NULL, "led_drv");  // /dev/led_drv
	if(gec6818_led_device == NULL)
    {
		printk("device create error\n");
		goto device_err;
	}
	

    // ����Ĵ����������ַ
    gpioc_res = request_mem_region(0xC001C000, 0x1000, "gpioc_mem");
    if(gpioc_res == NULL)
    {
		printk("gpioc resource error");
		goto gpioc_res_err;
	}
	gpioe_res = request_mem_region(0xC001E000, 0x1000,   "gpioe_led");
	if(gpioe_res == NULL)
    {
		printk("gpioe resource error");
		goto gpioe_res_err;
	}
    // IO�ڴ涯̬ӳ�䣬�õ������ַ
	GPIOC_BASE_VA = ioremap(0xC001C000, 0x1000);
	if(GPIOC_BASE_VA == NULL)
    {
		printk("gpioc ioremap error\n");
		goto gpioc_remap_err;
	}
	
	GPIOE_BASE_VA = ioremap(0xC001E000, 0x1000);
	if(GPIOE_BASE_VA == NULL)
    {
		printk("gpioe ioremap error\n");
		goto gpioe_remap_err;
	}

    // ͨ�������ַ���ʼĴ�����
	GPIOEOUT = GPIOE_BASE_VA + 0;
	GPIOEOUTENB = GPIOE_BASE_VA + 1;
	GPIOEALTFN0 = GPIOE_BASE_VA + 8;
	
	*GPIOEALTFN0 &= ~(3<<26); //����ΪIO����
	*GPIOEOUTENB |= (1<<13); //GPIOE13Ϊ���
	*GPIOEOUT |= (1<<13); 


    GPIOCOUT = GPIOC_BASE_VA + 0;
	GPIOCOUTENB = GPIOC_BASE_VA + 1;
	GPIOCALTFN1 = GPIOC_BASE_VA + 9;
	
	*GPIOCALTFN1 |= (1<<1); //����ΪIO����
	*GPIOCOUTENB |= (1<<17); //GPIOC17Ϊ���
	*GPIOCOUT |= (1<<17); 

    GPIOCOUT = GPIOC_BASE_VA + 0;
	GPIOCOUTENB = GPIOC_BASE_VA + 1;
	GPIOCALTFN0 = GPIOC_BASE_VA + 8;
	
	*GPIOCALTFN0 |= (1<<16); //����ΪIO����
	*GPIOCOUTENB |= (1<<8); //GPIOC8Ϊ���
	*GPIOCOUT |= (1<<8); 

    GPIOCOUT = GPIOC_BASE_VA + 0;
	GPIOCOUTENB = GPIOC_BASE_VA + 1;
	GPIOCALTFN0 = GPIOC_BASE_VA + 8;
	
	*GPIOCALTFN0 |= (1<<14); //����ΪIO����
	*GPIOCOUTENB |= (1<<7); //GPIOC7Ϊ���
	*GPIOCOUT |= (1<<7); 

	printk(KERN_INFO "gec6818 led driver init...\n");

    return 0;
	
gpioe_remap_err:
	iounmap(GPIOE_BASE_VA);	
gpioc_remap_err:
	iounmap(GPIOC_BASE_VA);	
gpioe_res_err:
	release_mem_region(0xC001E000, 0x1000);	
gpioc_res_err:
	release_mem_region(0xC001C000, 0x1000);	
device_err:
	device_destroy(gec6818_led_class, led_dev_num);
class_err:
	class_destroy(gec6818_led_class);
cdev_add_err:
	cdev_del(&gec6818_led_dev);	
led_num_err:
	unregister_chrdev_region(led_dev_num, 1);
	return ret;

}
//module�ĳ��ں���----����ж�غ���
//module_exit()-->gec6818_led_exit()-->���������ж��
static void __exit gec6818_led_exit(void)
{
    device_destroy(gec6818_led_class, led_dev_num);
	class_destroy(gec6818_led_class);
	iounmap(GPIOC_BASE_VA);
	iounmap(GPIOE_BASE_VA);
	release_mem_region(0xC001E000, 0x1000);	
	release_mem_region(0xC001C000, 0x1000);	
	cdev_del(&gec6818_led_dev);
	unregister_chrdev_region(led_dev_num, 1);
	printk(KERN_INFO "gec6818 led driver exit...\n");
}


//module��ںͳ���
module_init(gec6818_led_init);//#insmod led_drv.ko-->���ú���module_init()
module_exit(gec6818_led_exit);//#remmod led_drv.ko-->���ú���module_exit()

//module������
MODULE_AUTHOR(" ");
MODULE_DESCRIPTION("LED driver for GEC6818");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("V1.0");