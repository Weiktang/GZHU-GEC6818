#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/io.h>

/* 混杂设备
#include <linux/miscdevice.h>
struct miscdevice
{
    int minor;
    const char* name;
    const struct file_operations* fops;
    struct list_head list;
    struct device* parent;
    struct device* this_device;
    const char* nodename;
    umode_t mode;
}
*/

// GPIOC7 LED10
#define LEDCON  0xC001C004  //使能寄存器
#define LEDDAT  0xC001C000 //输出寄存器

static int * __gp_gpmdat;

static int led_open (struct inode *p_inode, struct file *p_file)
{
    int *p_gpmcon = NULL;

    p_gpmcon = ioremap(LEDCON, 4);
    writel(0x20180, p_gpmcon);

    __gp_gpmdat = ioremap(LEDDAT, 4);

    return 0;
}
static long led_ioctl (struct file *p_file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
    case 0:
        writel(0x1FE7F, __gp_gpmdat);
        break;

    case 1:
        writel(0x3FF7F, __gp_gpmdat);
        break;

    default:
        return -1;
    }
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .unlocked_ioctl = led_ioctl,
};


static struct miscdevice led_dev = {
    .minor = MISC_DYNAMIC_MINOR, // MISC_DYNAMIC_MINOR —— 自动分配次设备号、
    .name = "led_drv2", //  次设备的名字，也是设备文件的名字
    .fops = &fops,  //  文件操作集，驱动程序给予应用层的接口
};

static __init int led_init (void)
{
    misc_register(&led_dev);

    return 0;
}

static __exit void led_exit (void)
{
    misc_deregister(&led_dev);
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
