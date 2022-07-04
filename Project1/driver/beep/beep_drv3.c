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


static unsigned int beep_major = 0;//���豸��(=0,��̬����;  !=0����̬ע��)
static unsigned int beep_minor = 0;//���豸��
static dev_t beep_dev_num;

static struct resource * gpioc_res;

static volatile unsigned int __iomem *GPIOC_BASE_VA;
static volatile unsigned int __iomem *GPIOCOUT;
static volatile unsigned int __iomem *GPIOCOUTENB;
static volatile unsigned int __iomem *GPIOCALTFN0;
// static volatile unsigned int __iomem *GPIOCALTFN1;

static struct class *gec6818_beep_class;
static struct device *gec6818_beep_device;

//  ����һ��cdev���ַ��豸��
static struct cdev gec6818_beep_dev;


//  ��cdev����һ���ļ�������
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
		return -EINVAL;  //���ظ�Ӧ�ó���write()��һ��������
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

//module����ں���----�����İ�װ����
//module_init()-->gec6818_beep_init()-->��������İ�װ
static int __init gec6818_beep_init(void)
{
	int ret;
    // cdev�����豸��
    if(beep_major == 0)// ��̬ע��
    {
        ret = alloc_chrdev_region(&beep_dev_num, beep_minor, 1, "beep_dev");
    }
    else// ��̬ע��
    {
        beep_dev_num = MKDEV(beep_major, beep_minor);
        ret = register_chrdev_region(beep_dev_num, 1, "beep_dev");
    }
    if(ret < 0)
    {
        printk("register_chrdev_region faibeep\n");
        goto beep_num_err;
    }
	// cdev��ʼ��
    cdev_init(&gec6818_beep_dev,  &gec6818_beep_fops);

    // cdevע���ں�
    ret = cdev_add(&gec6818_beep_dev, beep_dev_num, 1);
    if(ret < 0)
    {
        printk("cdev_add fail beep\n");
        goto cdev_add_err;
    }

    // ����class  
	gec6818_beep_class = class_create(THIS_MODULE, "beeps_class");
	if(gec6818_beep_class == NULL)
    {
		printk("create class error\n");
		goto class_err;
	}
	
	// ����device
	gec6818_beep_device = device_create(gec6818_beep_class, NULL,beep_dev_num, NULL, "beep_drv");  // /dev/beep_drv
	if(gec6818_beep_device == NULL)
    {
		printk("device create error\n");
		goto device_err;
	}
	

    // ����Ĵ����������ַ
    gpioc_res = request_mem_region(0xC001C000, 0x1000, "gec6818_beep");
    if(gpioc_res == NULL)
    {
		printk("gpioc resource error");
		goto gpioc_res_err;
	}

    // IO�ڴ涯̬ӳ�䣬�õ������ַ
	GPIOC_BASE_VA = ioremap(0xC001C000, 0x1000);
	if(GPIOC_BASE_VA == NULL)
    {
		printk("gpioc ioremap error\n");
		goto gpioc_remap_err;
	}

    // ͨ�������ַ���ʼĴ�����
    GPIOCOUT = GPIOC_BASE_VA + 0;
	GPIOCOUTENB = GPIOC_BASE_VA + 1;
	GPIOCALTFN0 = GPIOC_BASE_VA + 9;
	
	*GPIOCALTFN0 |= (1<<1); //����ΪIO����
	*GPIOCOUTENB |= (1<<17); //GPIOC17Ϊ���
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
//module�ĳ��ں���----����ж�غ���
//module_exit()-->gec6818_beep_exit()-->���������ж��
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


//module��ںͳ���
module_init(gec6818_beep_init);//#insmod beep_drv.ko-->���ú���module_init()
module_exit(gec6818_beep_exit);//#remmod beep_drv.ko-->���ú���module_exit()

//module������
MODULE_AUTHOR(" ");
MODULE_DESCRIPTION("beep driver for GEC6818");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("V1.0");