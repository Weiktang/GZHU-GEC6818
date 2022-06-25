main:main.o LCD.o touch.o process.o music.o
	arm-linux-gcc $^ -o ./Make/main -lm -lpthread
	rm -rf main.o LCD.o touch.o process.o music.o

%.o:%.c
	arm-linux-gcc $< -o $@ -c -I./header -lm -lpthread

clean:
	rm -rf *.o ./Make/main