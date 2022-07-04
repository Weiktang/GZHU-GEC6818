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


#define	Button2	2 // GPIOA28 K2
#define	Button3	3 // GPIOB30 K3
#define	Button4	4 // GPIOB31 K4
#define	Button6	6 // GPIOB9  K6

#define Button_ON  0
#define Button_OFF 1


struct gpio_info {
	unsigned int gpio_num;
	const char *gpio_name;
};

struct gpio_info gec6818_Button[4] = {
	{
		.gpio_num = PAD_GPIO_A + 28,
		.gpio_name = "Button2",
	},
	{
		.gpio_num = PAD_GPIO_B + 30,
		.gpio_name = "Button3",
	},
	{
		.gpio_num = PAD_GPIO_B + 31,
		.gpio_name = "Button4",
	},
	{
		.gpio_num = PAD_GPIO_B +9,
		.gpio_name = "Button6",
	},
};


static unsigned int Button_major = 0;//���豸��(=0,��̬����;  !=0����̬ע��)
static unsigned int Button_minor = 0;//���豸��
static dev_t Button_dev_num;

static struct resource * gpioa_res;
static struct resource * gpiob_res;

static volatile unsigned int __iomem *GPIOA_BASE_VA;
static volatile unsigned int __iomem *GPIOAOUTENB;
// static volatile unsigned int __iomem *GPIOAALTFN0;
static volatile unsigned int __iomem *GPIOAALTFN1;

static volatile unsigned int __iomem *GPIOB_BASE_VA;
static volatile unsigned int __iomem *GPIOBOUTENB;
static volatile unsigned int __iomem *GPIOBALTFN0;
static volatile unsigned int __iomem *GPIOBALTFN1;

static struct class *gec6818_Button_class;
static struct device *gec6818_Button_device;

//  ����һ��cdev���ַ��豸��
static struct cdev gec6818_Button_dev;


//  ��cdev����һ���ļ�������
static int gec6818_Button_open(struct inode *inode, struct file *filp)
{
	printk("Button driver is openning\n");
	return 0;	
}
static int gec6818_Button_close(struct inode *inode, struct file *filp)
{
	printk("Button driver is closing\n");
	return 0;		
}

static ssize_t gec6818_Button_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	char Button_flag[4];//[0]--Button1,[1]--Button2,[2]--Button3,[3]--Button4; 0--Button ON, 1--Button OFF
	int i=0;
    if(len != 4)
		return -EINVAL;

    for(i=0;i<4;i++)
    {
        Button_flag[i] = gpio_get_value(gec6818_Button[i].gpio_num);
	}
    
	
	if(copy_to_user(buf, Button_flag, len))
		return -EFAULT;
	
	return len;		
}
static const struct file_operations gec6818_Button_fops = {
	.owner = THIS_MODULE,
	.open = gec6818_Button_open,
	.read = gec6818_Button_read,
	.release = gec6818_Button_close,
};

//module����ں���----�����İ�װ����
//module_init()-->gec6818_Button_init()-->��������İ�װ
static int __init gec6818_Button_init(void)
{
	int ret;
    // cdev�����豸��
    if(Button_major == 0)// ��̬ע��
    {
        ret = alloc_chrdev_region(&Button_dev_num, Button_minor, 1, "Button_dev");
    }
    else// ��̬ע��
    {
        Button_dev_num = MKDEV(Button_major, Button_minor);
        ret = register_chrdev_region(Button_dev_num, 1, "Button_dev");
    }
    if(ret < 0)
    {
        printk("register_chrdev_region faiButton\n");
        goto Button_num_err;
    }
	// cdev��ʼ��
    cdev_init(&gec6818_Button_dev,  &gec6818_Button_fops);

    // cdevע���ں�
    ret = cdev_add(&gec6818_Button_dev, Button_dev_num, 1);
    if(ret < 0)
    {
        printk("cdev_add fail Button\n");
        goto cdev_add_err;
    }

    // ����class  
	gec6818_Button_class = class_create(THIS_MODULE, "Buttons_class");
	if(gec6818_Button_class == NULL)
    {
		printk("create class error\n");
		goto class_err;
	}
	
	// ����device
	gec6818_Button_device = device_create(gec6818_Button_class, NULL,Button_dev_num, NULL, "button_drv");  // /dev/button_drv
	if(gec6818_Button_device == NULL)
    {
		printk("device create error\n");
		goto device_err;
	}
	

    // ����Ĵ����������ַ
     gpioa_res = request_mem_region(0xC001A000, 0x1000, "gpioa_mem");
    if(gpioa_res == NULL)
    {
		printk("gpioa resource error");
		goto gpioa_res_err;
	}
	gpiob_res = request_mem_region(0xC001B000, 0x1000,   "gpiob_mem");
	if(gpiob_res == NULL)
    {
		printk("gpiob resource error");
		goto gpiob_res_err;
	}

    // IO�ڴ涯̬ӳ�䣬�õ������ַ
	GPIOA_BASE_VA = ioremap(0xC001A000, 0x1000);
	if(GPIOA_BASE_VA == NULL)
    {
		printk("gpioa ioremap error\n");
		goto gpioa_remap_err;
	}
	
	GPIOB_BASE_VA = ioremap(0xC001B000, 0x1000);
	if(GPIOB_BASE_VA == NULL)
    {
		printk("gpiob ioremap error\n");
		goto gpiob_remap_err;
	}

    // ͨ�������ַ���ʼĴ�����
	GPIOAOUTENB = GPIOA_BASE_VA + 1;
	GPIOAALTFN1 = GPIOA_BASE_VA + 9;

    *GPIOAALTFN1 &= ~(3<<24); //����ΪIO����
	*GPIOAOUTENB &= ~(1<<28); //GPIOA28Ϊ����

	GPIOBOUTENB = GPIOB_BASE_VA + 1;
	GPIOBALTFN1 = GPIOB_BASE_VA + 9;

    *GPIOBALTFN1 |= (1<<28); //����ΪIO����
	*GPIOBOUTENB &= ~(1<<30); //GPIOB30Ϊ����

	GPIOBOUTENB = GPIOB_BASE_VA + 1;
	GPIOBALTFN1 = GPIOB_BASE_VA + 9;

    *GPIOBALTFN1 |= (1<<30); //����ΪIO����
	*GPIOBOUTENB &= ~(1<<31); //GPIOB31Ϊ����

	GPIOBOUTENB = GPIOB_BASE_VA + 1;
	GPIOBALTFN0 = GPIOB_BASE_VA + 8;

    *GPIOBALTFN0 &= ~(3<<18); //����ΪIO����
	*GPIOBOUTENB &= ~(1<<9); //GPIOB9Ϊ����	

	printk(KERN_INFO "gec6818 Button driver init...\n");

    return 0;
	

gpioa_remap_err:
	iounmap(GPIOA_BASE_VA);	
gpiob_remap_err:
	iounmap(GPIOB_BASE_VA);	
gpioa_res_err:
	release_mem_region(0xC001A000, 0x1000);	
gpiob_res_err:
	release_mem_region(0xC001B000, 0x1000);	
device_err:
	device_destroy(gec6818_Button_class, Button_dev_num);
class_err:
	class_destroy(gec6818_Button_class);
cdev_add_err:
	cdev_del(&gec6818_Button_dev);	
Button_num_err:
	unregister_chrdev_region(Button_dev_num, 1);
	return ret;

}
//module�ĳ��ں���----����ж�غ���
//module_exit()-->gec6818_Button_exit()-->���������ж��
static void __exit gec6818_Button_exit(void)
{
    device_destroy(gec6818_Button_class, Button_dev_num);
	class_destroy(gec6818_Button_class);
	iounmap(GPIOA_BASE_VA);
	iounmap(GPIOB_BASE_VA);
	release_mem_region(0xC001A000, 0x1000);	
	release_mem_region(0xC001B000, 0x1000);	
	cdev_del(&gec6818_Button_dev);
	unregister_chrdev_region(Button_dev_num, 1);
	printk(KERN_INFO "gec6818 Button driver exit...\n");
}


//module��ںͳ���
module_init(gec6818_Button_init);//#insmod Button_drv.ko-->���ú���module_init()
module_exit(gec6818_Button_exit);//#remmod Button_drv.ko-->���ú���module_exit()

//module������
MODULE_AUTHOR(" ");
MODULE_DESCRIPTION("Button driver for GEC6818");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("V1.0");