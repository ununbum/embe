#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>


#define BUFF_SIZE 64
#define MAX_BUTTON 9
#define KEY_RELEASE 0
#define KEY_PRESS 1


/*******************************
  child_process#1
process_name : inproc
function : input SW,MENU key
execute by main process by execr
********************************/


void read_mod(int shmid);
int main(int argc, char *argv[])
{
	key_t key;
	int shmid = atoi(argv[1]);
	read_mod(shmid);
	printf("input end\n");
}
void read_mod(int shmid)	//	shmaddr[0~9] : SW key, shmadrr[57] : MENU key
{
	struct input_event ev[BUFF_SIZE];
	unsigned char push_sw_buff[MAX_BUTTON];

	int fd,rd,value,size = sizeof(struct input_event);
	int dev,i;
	char name[256] = "Unknown";
	unsigned char * shmaddr=(unsigned char*)shmat(shmid,(int*)NULL,0);
	char * menu = "/dev/input/event0";
	char * nine_key = "/dev/fpga_push_switch";
	int input;
	int prev_input;
	int buf[9]={0,};

	shmaddr[1]=1;
	if((fd = open(menu,O_RDONLY|O_NONBLOCK))==-1){
		printf("%s , %s is not a valid device\n",menu,nine_key);
	}
	dev = open(nine_key,O_RDWR);
	while(1){
		if((rd =  read(fd,ev,size * BUFF_SIZE))>=size){		//read menu key
			if(ev[0].value==KEY_PRESS)
				shmaddr[57]=ev[0].code;
			if(shmaddr[57]==158)	//exit if 'back' key input
			{
				close(fd);
				close(dev);
				return;
			}
		}

		read(dev,&push_sw_buff,sizeof(push_sw_buff));
		for(i=0;i<MAX_BUTTON;i++){					//if key is pressed and released, then event occur
			if(buf[i]==1 && push_sw_buff[i]==0)
			{
					shmaddr[i] = KEY_PRESS;
					buf[i]=0;
			}
			else
					buf[i]=push_sw_buff[i];
		}
	}
}

