#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <string.h>

#define MAX_DIGIT 4
#define FND_DEVICE "/dev/fpga_fnd"
#define LED_DEVICE "/dev/mem"
#define FPGA_BASE_ADDRESS 0x08000000
#define LED_ADDR 0x16

int * shmaddr;
int led_flag=0;
void clock(void);
int main(int argc,char **argv)
{
	int dev;
	shmaddr = (int*)shmat(atoi(argv[1]),(int*)NULL,0);
	printf("output\n\n");
	while(1)
	{
		if(shmaddr[0]==158)
			break;
		sleep(1);
		clock();
	}
}
void clock(void)
{
	int dev;
	int fd;
	unsigned long *fpga_addr=0;
	unsigned char *led_addr=0;
	unsigned char data[4];
	unsigned char retval;
	int i;
	

	dev = open(FND_DEVICE,O_RDWR);

	fd = open(LED_DEVICE,O_RDWR | O_SYNC);

	fpga_addr = (unsigned long *)mmap(NULL,4096,PROT_READ | PROT_WRITE,MAP_SHARED,fd,FPGA_BASE_ADDRESS);

	led_addr = (unsigned char*)((void*)fpga_addr+LED_ADDR);
	
	if(shmaddr[13]<0)
		*led_addr = 128;
	else
	{ 
		if(led_flag==1)
		{
			*led_addr = 32;
			led_flag=0;
		}
		else
		{
			*led_addr = 64;
			led_flag=1;
		}
	}

	if(dev<0)
		printf("fuck you\n\n");
	data[0]=shmaddr[11]/10;
	data[1]=shmaddr[11]%10;
	data[2]=shmaddr[12]/10;
	data[3]=shmaddr[12]%10;

	retval=write(dev,&data,4);	


	munmap(led_addr,4096);
	close(dev);

	return;
}
