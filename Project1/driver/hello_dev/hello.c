#include <linux/kernel.h>
#include <linux/module.h>

static int __init GEC6818_hello_module_init(void)//��ʼ��
{
    printk(KERN_INFO "Hello, GEC6818module is installed !\n");
    return 0;
}

static void __exit GEC6818_hello_module_cleanup(void)//ж��
{
    printk(KERN_INFO "Good-bye, GEC6818module was removed!\n");
}
module_init(GEC6818_hello_module_init);
module_exit(GEC6818_hello_module_cleanup);
MODULE_LICENSE("GPL");