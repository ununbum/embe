#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/unistd.h>

#include <string.h>

#include "dev_driver.h"

#define MAX_DIGIT 4
#define DEVICE "/dev/dev_driver"


int main(int argc,char** argv)
{
	int dev;
	unsigned long retval;
	unsigned int tmp;
	unsigned char interval,count;
	int initial;
	int i;
	int cnt=0;

	if(argc != 4)
	{
		printf("parameters are too many or too less\n");
		printf("ex) /dev_test time_interval(1~100),count(1~100),initial number\n");
		return -1;
	}

	interval = atoi(argv[1]);
	if(interval < 1 || interval >100)
	{
		printf("time interval are 1~100 value\n");
		printf("please type again\n");
		return -1;
	}
	count = atoi(argv[2]);
	if(count <1 || count >100)
	{
		printf("count are 1~100 value\n");
		printf("please type again\n");
		return -1;
	}

	if(strlen(argv[3])!=MAX_DIGIT)
	{
		printf("initial number can has no more or less than 4 Digit\n");
		printf("please type again\n");
		return -1;
	}
	for(i=0;i<4;i++)
	{
		if(argv[3][i] == '0')
			cnt++;
	}
	if(cnt!=3)
	{
		printf("this device can make output with 3 zero\n");
		printf("but your input has less or many zero than 3\n");
		return -1;
	}
	for(i=0;i<4;i++)
	{
		if(argv[3][i] != '0')
			cnt=i;
	}
	if(argv[3][cnt]>0x38)
	{
		printf("this device can make output to 8 only\n");
		printf("but your input has bigger input than 8\n");
		return -1;
	}

	retval = syscall(378,interval,count,strtol(argv[3],NULL,16));


	
	dev = open(DEVICE,O_RDWR);

	if(dev<0)
	{
		printf("Device open error :  %s\n",DEVICE);
		exit(1);
	}

	ioctl(dev,IOCTL_TIMER,retval);
	ioctl(dev,IOCTL_FND,retval);
	ioctl(dev,IOCTL_DOT,retval);
	ioctl(dev,IOCTL_TEXT_LCD,retval);
	ioctl(dev,IOCTL_LED,retval);
	if(retval<0)
	{
		printf("Write error!\n");
		return -1;
	}

	close(dev);

}
