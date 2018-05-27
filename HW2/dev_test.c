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

	retval = syscall(378,atoi(argv[1]),atoi(argv[2]),strtol(argv[3],NULL,16));


	
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
