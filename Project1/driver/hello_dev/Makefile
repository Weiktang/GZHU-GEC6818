# butto_driver  makefile

INSTALLDIR = /mnt/e/Temporary\ works/internship/driver/

ifneq ($(KERNELRELEASE),)
obj-m:=hello.o
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

install: hello.ko
	mkdir -p $(INSTALLDIR)
	cp --target-dir=$(INSTALLDIR) hello.ko