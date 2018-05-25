#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <string.h>

#define MAX_DIGIT 4
#define DEVICE "/dev/dev_driver"
#define IOCTL_FND _IOW(242,0,int)
#define IOCTL_DOT _IOW(242,1,int)


int main(void)
{
	int dev;
	long retval;
	unsigned int tmp = 0x83;
	dev = open(DEVICE,O_RDWR);
	if(dev<0)
	{
		printf("Device open error :  %s\n",DEVICE);
		exit(1);
	}


	retval = ioctl(dev,IOCTL_FND,tmp);
	retval = ioctl(dev,IOCTL_DOT,tmp);
	if(retval<0)
	{
		printf("Write error!\n");
		return -1;
	}
}
