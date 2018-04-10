CC = arm-none-linux-gnueabi-gcc

Hw1 : inproc main outproc
inproc : inproc.c
			$(CC) --static -o inproc inproc.c
main : main.c
			$(CC) --static -o main main.c
outproc : outproc.c
			$(CC) --static -o outproc outproc.c

clean :
			rm inproc main outproc
