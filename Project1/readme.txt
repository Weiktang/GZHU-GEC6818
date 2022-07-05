//TODO:
mixer 无法调音
普通字符设备驱动 led_drv3.ko 和 beep_drv3.ko无法共用，因为二者申请的地址中GPIOC部分有冲突。


暂使用驱动：led_drv.ko,beep_drv.ko,button_drv.ko,watchdog_drv2.ko
