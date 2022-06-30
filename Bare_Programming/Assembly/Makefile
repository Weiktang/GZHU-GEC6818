# 将目标文件链接成二进制文件
# 1. 使用链接器将目标文件链接成elf格式的可执行文件，链接地址是0x40000000, 让代码段(text)段在0x40000000（内存的首地址）地址上运行
# 2. 利用文件格式转换工具将elf格式的可执行文件转换成bin格式。
# 3. 利用反汇编工具将elf格式的可执行文件生成dis格式的反汇编文件
led_beep.bin:led_beep.o 
	arm-linux-ld -Ttext 0x40000000 -o led_beep.elf $^ 
	arm-linux-objcopy -O binary led_beep.elf led_beep.bin
	arm-linux-objdump -D led_beep.elf > led_beep.dis

#将汇编源文件编译成目标文件
%.o:%.S
	arm-linux-gcc -o $@ $< -c

#将C源文件编译成目标文件
%.o:%.c
	arm-linux-gcc -o $@ $< -c

clean:
	rm -rf *.o *.elf *.bin *.dis