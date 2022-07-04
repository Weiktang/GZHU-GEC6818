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


#define	Beep	14

#define BEEP_ON  1
#define BEEP_OFF 0


struct gpio_info {
	unsigned int gpio_num;
	const char *gpio_name;
};

struct gpio_info gec6818_beep = {
	.gpio_num = PAD_GPIO_C +14,
	.gpio_name = "Beep",
};


static unsigned int beep_major = 0;//主设备号(=0,动态分配;  !=0，静态注册)
static unsigned int beep_minor = 0;//次设备号
static dev_t beep_dev_num;

static struct resource * gpioc_res;

static volatile unsigned int __iomem *GPIOC_BASE_VA;
static volatile unsigned int __iomem *GPIOCOUT;
static volatile unsigned int __iomem *GPIOCOUTENB;
static volatile unsigned int __iomem *GPIOCALTFN0;
// static volatile unsigned int __iomem *GPIOCALTFN1;

static struct class *gec6818_beep_class;
static struct device *gec6818_beep_device;

//  定义一个cdev（字符设备）
static struct cdev gec6818_beep_dev;


//  给cdev定义一个文件操作集
static int gec6818_beep_open(struct inode *inode, struct file *filp)
{
	printk("beep driver is openning\n");
	return 0;	
}
static int gec6818_beep_close(struct inode *inode, struct file *filp)
{
	printk("beep driver is closing\n");
	return 0;		
}

static ssize_t gec6818_beep_write(struct file *folp, const char __user *buf, size_t len, loff_t *off)
{
	unsigned char beep_flag;
	if(len != 1)
		return -EINVAL;  //返回给应用程序write()的一个错误码
	if(copy_from_user(&beep_flag, buf, len))
		return -EFAULT;

	switch(beep_flag)
    {
        case BEEP_ON:
            *GPIOCOUT |= (1<<14);
            break;

        case BEEP_OFF:
            *GPIOCOUT &= ~(1<<14);
            break;

        default:
            return -EINVAL;
	}
	
	return len;	
}

static ssize_t gec6818_beep_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	char beep_flag;
	if(len != 1)
		return -EINVAL;

    beep_flag = gpio_get_value(gec6818_beep.gpio_num);
	
	if(copy_to_user(buf, &beep_flag, len))
		return -EFAULT;
	
	return len;		
}
static const struct file_operations gec6818_beep_fops = {
	.owner = THIS_MODULE,
	.open = gec6818_beep_open,
	.write = gec6818_beep_write,
	.read = gec6818_beep_read,
	.release = gec6818_beep_close,
};

//module的入口函数----驱动的安装函数
//module_init()-->gec6818_beep_init()-->完成驱动的安装
static int __init gec6818_beep_init(void)
{
	int ret;
    // cdev申请设备号
    if(beep_major == 0)// 动态注册
    {
        ret = alloc_chrdev_region(&beep_dev_num, beep_minor, 1, "beep_dev");
    }
    else// 静态注册
    {
        beep_dev_num = MKDEV(beep_major, beep_minor);
        ret = register_chrdev_region(beep_dev_num, 1, "beep_dev");
    }
    if(ret < 0)
    {
        printk("register_chrdev_region faibeep\n");
        goto beep_num_err;
    }
	// cdev初始化
    cdev_init(&gec6818_beep_dev,  &gec6818_beep_fops);

    // cdev注册内核
    ret = cdev_add(&gec6818_beep_dev, beep_dev_num, 1);
    if(ret < 0)
    {
        printk("cdev_add fail beep\n");
        goto cdev_add_err;
    }

    // 创建class  
	gec6818_beep_class = class_create(THIS_MODULE, "beeps_class");
	if(gec6818_beep_class == NULL)
    {
		printk("create class error\n");
		goto class_err;
	}
	
	// 创建device
	gec6818_beep_device = device_create(gec6818_beep_class, NULL,beep_dev_num, NULL, "beep_drv");  // /dev/beep_drv
	if(gec6818_beep_device == NULL)
    {
		printk("device create error\n");
		goto device_err;
	}
	

    // 申请寄存器的物理地址
    gpioc_res = request_mem_region(0xC001C000, 0x1000, "gec6818_beep");
    if(gpioc_res == NULL)
    {
		printk("gpioc resource error");
		goto gpioc_res_err;
	}

    // IO内存动态映射，得到虚拟地址
	GPIOC_BASE_VA = ioremap(0xC001C000, 0x1000);
	if(GPIOC_BASE_VA == NULL)
    {
		printk("gpioc ioremap error\n");
		goto gpioc_remap_err;
	}

    // 通过虚拟地址访问寄存器。
    GPIOCOUT = GPIOC_BASE_VA + 0;
	GPIOCOUTENB = GPIOC_BASE_VA + 1;
	GPIOCALTFN0 = GPIOC_BASE_VA + 9;
	
	*GPIOCALTFN0 |= (1<<1); //设置为IO功能
	*GPIOCOUTENB |= (1<<17); //GPIOC17为输出
	*GPIOCOUT |= (1<<17); 

	printk(KERN_INFO "gec6818 beep driver init...\n");

    return 0;
	

gpioc_remap_err:
	iounmap(GPIOC_BASE_VA);	
gpioc_res_err:
	release_mem_region(0xC001C000, 0x1000);	
device_err:
	device_destroy(gec6818_beep_class, beep_dev_num);
class_err:
	class_destroy(gec6818_beep_class);
cdev_add_err:
	cdev_del(&gec6818_beep_dev);	
beep_num_err:
	unregister_chrdev_region(beep_dev_num, 1);
	return ret;

}
//module的出口函数----驱动卸载函数
//module_exit()-->gec6818_beep_exit()-->完成驱动的卸载
static void __exit gec6818_beep_exit(void)
{
    device_destroy(gec6818_beep_class, beep_dev_num);
	class_destroy(gec6818_beep_class);
	iounmap(GPIOC_BASE_VA);
	release_mem_region(0xC001C000, 0x1000);	
	cdev_del(&gec6818_beep_dev);
	unregister_chrdev_region(beep_dev_num, 1);
	printk(KERN_INFO "gec6818 beep driver exit...\n");
}


//module入口和出口
module_init(gec6818_beep_init);//#insmod beep_drv.ko-->调用函数module_init()
module_exit(gec6818_beep_exit);//#remmod beep_drv.ko-->调用函数module_exit()

//module的描述
MODULE_AUTHOR(" ");
MODULE_DESCRIPTION("beep driver for GEC6818");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("V1.0");