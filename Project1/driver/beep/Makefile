INSTALLDIR = /home/weiktang/driver/LED

ifneq ($(KERNELRELEASE),)
obj-m:=beep_drv3.o
else
KERNELDIR:=/home/weiktang/kernel/6818GEC/kernel
CROSS_COMPILE:=/home/weiktang/kernel/6818GEC/prebuilts/gcc/linux-x86/arm/arm-eabi-4.8/bin/arm-eabi-

PWD:=$(shell pwd)

default:
	$(MAKE)  ARCH=arm  CROSS_COMPILE=$(CROSS_COMPILE) -C  $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *.order .*.cmd *.mod.c *.symvers .tmp_versions

cleanall:
	rm -rf *.o *.order .*.cmd *.mod.c *.symvers .tmp_versions *.ko
endif

install: beep_drv3.ko
	mkdir -p $(INSTALLDIR)
	cp --target-dir=$(INSTALLDIR) beep_drv3.ko
