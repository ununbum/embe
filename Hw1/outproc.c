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
#define MAX_BUFF 32
#define LINE_BUFF 16
#define FND_DEVICE "/dev/fpga_fnd"
#define LED_DEVICE "/dev/mem"
#define TEXT_DEVICE "/dev/fpga_text_lcd"
#define DOT_DEVICE "/dev/fpga_dot"

#define FPGA_BASE_ADDRESS 0x08000000
#define LED_ADDR 0x16

/*******************************
  child_process#2
process_name : output_process
function : output all fpga
execute by main process by execr
*********************************/



int * shmaddr;
int led_flag=-1;
int fd;
time_t start,end;
unsigned long *fpga_addr=0;
unsigned char *led_addr=0;

void output_led(void)		//LED use mmap, and controlled by shmaddr[14]
{

	if(shmaddr[14]<0)
		*led_addr = 128;
	else if(shmaddr[14]==1) 
	{ 
		if(led_flag==1)
		{
			*led_addr = 16;
		}
		else
		{
			*led_addr = 32;
		}
		if(difftime(time(NULL),start)>=1)
		{
			led_flag*=-1;
			time(&start);
		}
	}
	else
	{
		if(shmaddr[14]==10)
			*led_addr = 128;
		if(shmaddr[14]==8)
			*led_addr = 64;
		if(shmaddr[14]==4)
			*led_addr = 32;
		if(shmaddr[14]==2)
			*led_addr = 16;
		if(shmaddr[14]==0)
			*led_addr = 0; 
	}
	
}
void output_fnd(void)		//FND controlled by shmaddr[10~13]
{
	int dev;
	int res;
	unsigned char data[4];
	unsigned char retval;
	int i;
	

	dev = open(FND_DEVICE,O_RDWR);
	if(dev<0)
	{
		printf("fail on open FND Device file\n");
		close(dev);
		exit(1);
	}


	data[0]=shmaddr[10];
	data[1]=shmaddr[11];
	data[2]=shmaddr[12];
	data[3]=shmaddr[13];
	

	retval=write(dev,&data,4);	


	close(dev);

	return;
}
void output_text(void)	// TEXT controlled by shmaddr[15~46]
{
	int i;
	int dev;
	int str_size;
	int chk_size;

	unsigned char string[32];
	for(i=0;i<32;i++)
		string[i] = shmaddr[i+15];
	dev = open(TEXT_DEVICE,O_WRONLY);
	if(dev<0){
		printf("fail on open TEXT Device file\n");
		exit(1);
	}

	write(dev,string,MAX_BUFF);
	close(dev);
}
void output_dot(void)	//DOT controlled by shmaddr[47~56]
{
	int i;
	int dev;
	int str_size;
	dev = open(DOT_DEVICE,O_WRONLY);
	if(dev<0){
		printf("fail on open DOT Device file\n");
		exit(1);
	}

	write(dev,shmaddr+47,10);
	
//	for(i=0;i<10;i++)
//		printf("%d",shmaddr[i+47]);
	close(dev);
}
void open_led(void)		//mmap LED Device
{
	fd = open(LED_DEVICE,O_RDWR | O_SYNC);
	if(fd<0){
		printf("LED_FILE error!!\n");
		close(fd);
		exit(1);
	}
	fpga_addr = (unsigned long *)mmap(NULL,4096,PROT_WRITE,MAP_SHARED,fd,FPGA_BASE_ADDRESS);
	if(fpga_addr==MAP_FAILED)
	{
		perror("fail on read LED Memory\n");
		close(fd);
		exit(1);
	}
	led_addr = (unsigned char*)((void*)fpga_addr+LED_ADDR);

}
int main(int argc,char **argv)		//output all shmaddr[10~56],FND,LED,TEXT,DOT
{
	int dev;
	shmaddr = (int*)shmat(atoi(argv[1]),(int*)NULL,0);
	printf("output\n\n");
	open_led();
	time(&start);
	while(1)
	{
		if(shmaddr[57]==158)
			break;
			output_fnd();
			output_led();
			output_dot();
			output_text();
	}

	munmap(led_addr,4096);
	close(fd);
	printf("output end\n");
	return 1;
}
