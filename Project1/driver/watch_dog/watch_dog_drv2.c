#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/ioctl.h>

#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/soc.h>


#define WD_OPEN     _IO('W', 0X20)
#define WD_CLOSE    _IO('W', 0x21)
#define WD_ALIVE    _IO('W', 0x22)


static unsigned int watch_dog_major = 0;//主设备号(=0,动态分配;  !=0，静态注册)
static unsigned int watch_dog_minor = 0;//次设备号
static dev_t watch_dog_dev_num; // 设备号
// 资源申请指针
static struct resource * wt_res;
// 定义看门狗寄存器指针
static volatile unsigned int __iomem *WT_BASE; // 
static volatile unsigned int __iomem *WTCON;
static volatile unsigned int __iomem *WTCNT;
// 内核设备注册
static struct class *gec6818_watch_dog_class;
static struct device *gec6818_watch_dog_device;
//  定义一个cdev（字符设备）
static struct cdev gec6818_watch_dog_dev;


//  给cdev定义一个文件操作集
static int gec6818_watch_dog_open(struct inode *inode, struct file *filp)
{
	printk("watch dog driver is openning\n");
	return 0;	
}
static int gec6818_watch_dog_close(struct inode *inode, struct file *filp)
{
	printk("watch dog driver is closing\n");
	return 0;		
}

static long gec6818_watch_dog_ioctl(struct file *folp, unsigned int cmd, unsigned long arg)
{
	switch(cmd){
	case WD_OPEN:
		*WTCON |= 0x01<<2; // 开启中断使能
		*WTCON |= 1<<5; // 开启定时器
        *WTCON |= 0x01; // 开启复位信号
		break;
	case WD_CLOSE:
		*WTCON &= ~(0x01<<2); // 关闭中断使能
		*WTCON &= ~(1<<5); // turn off 定时器
        *WTCON &= ~(0x01); // 关闭复位信号
		break;
	case WD_ALIVE:
		*WTCNT = 0xC350; // 写入数据5s复位
		break;
	}
	return 0;
}	

static const struct file_operations gec6818_watch_dog_fops = {
	.owner = THIS_MODULE,
	.open = gec6818_watch_dog_open,
	.unlocked_ioctl = gec6818_watch_dog_ioctl,
	.release = gec6818_watch_dog_close,
};

//module的入口函数----驱动的安装函数
//module_init()-->gec6818_led_init()-->完成驱动的安装
static int __init gec6818_watch_dog_init(void)
{
	int ret;
    // cdev申请设备号
    if(watch_dog_major == 0)// 动态注册
    {
        ret = alloc_chrdev_region(&watch_dog_dev_num, watch_dog_minor, 1, "watch_dog_dev");
    }
    else// 静态注册
    {
        watch_dog_dev_num = MKDEV(watch_dog_major, watch_dog_minor);
        ret = register_chrdev_region(watch_dog_dev_num, 1, "watch_dog_dev");
    }
    if(ret < 0)
    {
        printk("register_chrdev_region failed\n");
        goto watch_dog_num_err;
    }
	// cdev初始化
    cdev_init(&gec6818_watch_dog_dev,  &gec6818_watch_dog_fops);

    // cdev注册内核
    ret = cdev_add(&gec6818_watch_dog_dev, watch_dog_dev_num, 1);
    if(ret < 0)
    {
        printk("cdev_add failed\n");
        goto cdev_add_err;
    }

    // 创建class  
	gec6818_watch_dog_class = class_create(THIS_MODULE, "watch_dog_class");
	if(gec6818_watch_dog_class == NULL)
    {
		printk("create class error\n");
		goto class_err;
	}
	
	// 创建device
	gec6818_watch_dog_device = device_create(gec6818_watch_dog_class, NULL,watch_dog_dev_num, NULL, "watch_dog_drv");  // /dev/watch_dog_dev
	if(gec6818_watch_dog_device == NULL)
    {
		printk("device create error\n");
		goto device_err;
	}
	
    //打开PCLK时钟
	nxp_soc_peri_reset_set(RESET_ID_WDT);
	nxp_soc_peri_reset_set(RESET_ID_WDT_POR);

    // 申请寄存器的物理地址
    printk(KERN_INFO "gec6818 watch_dog driver init1...\n");
    release_mem_region(0xC0019000, 0x400);	// 这里地址要改
    printk(KERN_INFO "gec6818 watch_dog driver init9...\n");
    wt_res = request_mem_region(0xC0019000, 0x0f, "gec6818_watch_dog");
    if(wt_res == NULL)
    {
		printk(KERN_INFO "wt resource error\n");
	}
    if(wt_res == NULL)
    {
		printk("wt resource error");
		goto wt_res_err;
	}
    // IO内存动态映射，得到虚拟地址
    printk(KERN_INFO "gec6818 watch_dog driver init2...\n");
    WT_BASE = ioremap(0xC0019000,0x0f);
    if(WT_BASE == NULL)
    {
        printk("wt ioremap error\n");
        goto wt_remap_err;
    }
    // 通过虚拟地址访问寄存器。
    printk(KERN_INFO "gec6818 watch_dog driver init3...\n");
    WTCON = WT_BASE + 0;
    WTCNT = WT_BASE + 2;
    printk("wtcon=%p,wtcnt=%p\n", WTCON,WTCNT);
    // 看门狗寄存器初始化工作--> 开启定时器，写入分配系数，关闭定时器中断和复位信号
    //(复位信号在子线程中开启并由子线程写入定时器数据防止复位)
    // 这里直接装驱动可能会导致一些牛马问题
    *WTCON |= 0x63<<8; // 第一次分频 99
    *WTCON |= 0x2<<3; // 第二次分频 64
    *WTCNT = 0xC350; // 写入数据5s复位

    return 0;
	
wt_remap_err:
	iounmap(WT_BASE);	
wt_res_err:
	release_mem_region(0xC0019000, 0x0f);		
device_err:
	device_destroy(gec6818_watch_dog_class, watch_dog_dev_num);
class_err:
	class_destroy(gec6818_watch_dog_class);
cdev_add_err:
	cdev_del(&gec6818_watch_dog_dev);	
watch_dog_num_err:
	unregister_chrdev_region(watch_dog_dev_num, 1);
	return ret;

}
//module的出口函数----驱动卸载函数
//module_exit()-->gec6818_led_exit()-->完成驱动的卸载
static void __exit gec6818_watch_dog_exit(void)
{
    device_destroy(gec6818_watch_dog_class, watch_dog_dev_num);
	class_destroy(gec6818_watch_dog_class);
	iounmap(WT_BASE);
	release_mem_region(0xC0019000, 0x0f);	// 这里地址要改
	cdev_del(&gec6818_watch_dog_dev);
	unregister_chrdev_region(watch_dog_dev_num, 1);
	printk(KERN_INFO "gec6818 watch_dog driver exit...\n");
}


//module入口和出口
module_init(gec6818_watch_dog_init);//#insmod watch_dog_dev.ko-->调用函数module_init()
module_exit(gec6818_watch_dog_exit);//#remmod watch_dog_dev.ko-->调用函数module_exit()

//module的描述
MODULE_AUTHOR(" ");
MODULE_DESCRIPTION("watch_dog driver for GEC6818");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("V1.0");
