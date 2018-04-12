#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>

#include <string.h>

#define MAX_DIGIT 4
#define FND_DEVICE "/dev/fpga_fnd"
#define LED_DEVICE "/dev/mem"
#define FPGA_BASE_ADDRESS 0x08000000
#define LED_ADDR 0x16

int * shmaddr;
int led_flag=0;
clock_t start=0, end=0;
	unsigned char data[4];
void output_led(void);
void output_fnd(void);
int main(int argc,char **argv)
{
	int dev;
	shmaddr = (int*)shmat(atoi(argv[1]),(int*)NULL,0);
	printf("output\n\n");
	output_led();
	while(1)
	{
		if(shmaddr[0]==158)
			break;
		output_fnd();

		if(shmaddr[1]==1)
		{
			//output_fnd();
			if(clock()-start>=1000000)
			{
				output_led();
				start=clock();
			}
		}
		if(shmaddr[1]==2)
		{
			unsigned char res;
			//output_fnd();
			/*res = shmaddr[15];
			data[3] = res%shmaddr[14];
			res/=shmaddr[14];
			data[2] = res%shmaddr[14];
			res/=shmaddr[14];
			data[1] = res%shmaddr[14];
			data[0] = 0;*/
		}
	}
}
void output_led(void)
{
	int fd;
	unsigned long *fpga_addr=0;
	unsigned char *led_addr=0;

	fd = open(LED_DEVICE,O_RDWR|O_SYNC);

	fpga_addr = (unsigned long *)mmap(NULL,4096,PROT_READ | PROT_WRITE,MAP_SHARED,fd,FPGA_BASE_ADDRESS);

	led_addr = (unsigned char*)((void*)fpga_addr+LED_ADDR);
	
	if(fpga_addr==MAP_FAILED)
	{
		printf("fuck\n");
		close(fd);
		exit(1);
	}

	if(shmaddr[11]<0)
		*led_addr = 128;
	else
	{ 
		if(led_flag==1)
		{
			*led_addr = 16;
			led_flag=0;
		}
		else
		{
			*led_addr = 32;
			led_flag=1;
		}
	}
	munmap(led_addr,4096);
	close(fd);
}
void output_fnd(void)
{
	int dev;
	int res;
	//unsigned char data[4];
	unsigned char retval;
	int i;
	

	dev = open(FND_DEVICE,O_RDWR);
	if(dev<0)
	{
		printf("fuck you\n\n");
		close(dev);
		exit(1);
	}

	data[0]=shmaddr[15];
	data[1]=shmaddr[14];
	data[2]=shmaddr[13];
	data[3]=shmaddr[12];	
	//printf("%d%d%d%d\n",shmaddr[12],shmaddr[13],shmaddr[14],shmaddr[15]);
	retval=write(dev,&data,4);	
	close(dev);

	return;
}
