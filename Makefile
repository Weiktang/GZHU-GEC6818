main:main.o LCD.o touch.o process.o music.o led.o beep.o
	arm-linux-gcc $^ -o ./Make/main -lm -lpthread
	rm -rf *.o

%.o:%.c
	arm-linux-gcc $< -o $@ -c -I./header -lm -lpthread

clean:
	rm -rf *.o ./Make/main