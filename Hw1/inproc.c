#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <dirent.h>
//#include <terminos.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>


#define BUFF_SIZE 64
#define MAX_BUTTON 9
#define KEY_RELEASE 0
#define KEY_PRESS 1

void read_mod(int shmid);
int main(int argc, char *argv[])
{
	key_t key;
	int shmid = atoi(argv[1]);
	read_mod(shmid);
}
void read_mod(int shmid)
{
	struct input_event ev[BUFF_SIZE];
	unsigned char push_sw_buff[MAX_BUTTON];

	int fd,rd,value,size = sizeof(struct input_event);
	int dev,i;
	char name[256] = "Unknown";
	int * shmaddr=(int*)shmat(shmid,(int*)NULL,0);
	char * menu = "/dev/input/event0";
	char * nine_key = "/dev/fpga_push_switch";

	shmaddr[1]=1;
	if((fd = open(menu,O_RDONLY|O_NONBLOCK))==-1){
		printf("%s , %s is not a valid device\n",menu,nine_key);
	}
	dev = open(nine_key,O_RDWR);
	while(1){
		usleep(400000);
		if((rd =  read(fd,ev,size * BUFF_SIZE)) >= size){
			value = ev[0].value;
			shmaddr[0]=ev[0].code;
			if(shmaddr[0]==158)
				return;
			if(shmaddr[0]==115)
				shmaddr[1]=(shmaddr[1]+1)%4+1;
			else if(shmaddr[0]==114)
				shmaddr[1]=(shmaddr[1]-1)%4+1;
		}
		read(dev,&push_sw_buff,sizeof(push_sw_buff));
		for(i=0;i<MAX_BUTTON;i++){
			shmaddr[i+2] = push_sw_buff[i];
		}
	}
	close(dev);
}

