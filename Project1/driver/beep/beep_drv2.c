#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

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
//struct file * ---�ں��У�ָ�������ļ���ָ��
//const char __user * ---- Ӧ�ó���д����������
//size_t  ---- Ӧ�ó������������ݵ��ֽ���
//loff_t * ----- �ļ�ָ���ƫ��
static ssize_t gec6818_beep_write(struct file *folp, const char __user *buf, size_t len, loff_t *off)
{
	unsigned char beep_flag; // BEEP_STATUS
	if(len != 1)
		return -EINVAL;  //���ظ�Ӧ�ó���write()��һ��������
	if(copy_from_user(&beep_flag, buf, len))
		return -EFAULT;
	gpio_set_value(gec6818_beep.gpio_num, beep_flag);
	
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


//�����ļ�����������Ӧ�ó����ṩͳһ�Ľӿ�

static const struct file_operations gec6818_beep_fops = {
	.owner = THIS_MODULE,
	.open = gec6818_beep_open,
	.write = gec6818_beep_write,
	.read = gec6818_beep_read,
	.release = gec6818_beep_close,
};

//��������豸������ʼ��
static struct miscdevice  gec6818_beep_misc = {	
	.minor = MISC_DYNAMIC_MINOR,
	.name = "beep_drv2",
	.fops = &gec6818_beep_fops,
};


//module����ں���----�����İ�װ����
//module_init()-->gec6818_beep_init()-->��������İ�װ
static int __init gec6818_beep_init(void)
{
	//���ڴ�������Դ��ע������
	int ret;
	gpio_free(gec6818_beep.gpio_num);
	ret = gpio_request(gec6818_beep.gpio_num, gec6818_beep.gpio_name);
	if(ret < 0)
	{
		printk("request GPIOC14 fail beep\n");
		goto gpio_faibeep;
	}		
	gpio_direction_output(gec6818_beep.gpio_num, BEEP_OFF); //BEEP off
	//�����豸ע��
	ret = misc_register(&gec6818_beep_misc);
	if(ret < 0)
	{
		printk("misc register fail beep\n");
		goto misc_reg_faibeep;
	}
	
	printk(KERN_INFO "gec6818 beep driver init...\n");
	
	return 0;
	
misc_reg_faibeep:	
gpio_faibeep: 
	gpio_free(gec6818_beep.gpio_num);
	
	return ret;
}
//module�ĳ��ں���----����ж�غ���
//module_exit()-->gec6818_beep_exit()-->���������ж��
static void __exit gec6818_beep_exit(void)
{
	//�ͷ���Դ��ע������
	gpio_free(gec6818_beep.gpio_num);
	
	//�����豸��ע��
	misc_deregister(&gec6818_beep_misc);
	
	printk(KERN_INFO "gec6818 beep driver exit...\n");
}


//module��ںͳ���
module_init(gec6818_beep_init);//#insmod beep_drv.ko-->���ú���module_init()
module_exit(gec6818_beep_exit);//#remmod beep_drv.ko-->���ú���module_exit()

//module������
MODULE_AUTHOR(" ");
MODULE_DESCRIPTION("BEEP driver for GEC6818");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("V1.0");