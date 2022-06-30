#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>

#include "cfg_type.h"

#define GPIOC14_BEEP    (PAD_GPIO_C +14)
#define BEEP_ON  1
#define BEEP_OFF 0

static int __init gec6818_beep_dev_init(void) 
{
    // 从内存申请资源、注册驱动
    int ret;
    gpio_free(GPIOC14_BEEP);
    // int gpio_request(unsigned gpio, const char* label);
    if((ret = gpio_request(GPIOC14_BEEP, "GPIOC14")) < 0)
    {
        printk("request error\n");
        return ret;
    }
    // int gpio_dirction_output(unsigned gpio, LED_ON);
    gpio_direction_output(GPIOC14_BEEP, BEEP_ON);

    printk(KERN_INFO "GEC6818 led driver init ...\n"); 
    return 0;
}

static void __exit gec6818_beep_dev_exit(void) 
{
    // 释放资源、注销驱动
    // int gpio_dirction_output(unsigned gpio, LED_OFF);
    // gpio_free(unsigned gpio);
    gpio_direction_output(GPIOC14_BEEP, BEEP_OFF);
    gpio_free(GPIOC14_BEEP);
    printk(KERN_INFO "GEC6818 led driver exit ...\n");

}

module_init(gec6818_beep_dev_init);
module_exit(gec6818_beep_dev_exit);

MODULE_DESCRIPTION("LED Driver for GEC6818");
MODULE_LICENSE("GPL v2");