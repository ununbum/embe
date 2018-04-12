#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
int res=0,mul=10;
void mode1(int *shmaddr)
{
	int i;
	int raw_hour,raw_min;
	int min,hour;
	int elapse_hour,elapse_min;
	time_t rawtime;
	struct tm *timeinfo;

	if(shmaddr[11]==-1)
	{
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		raw_hour=timeinfo->tm_hour;
		raw_min=timeinfo->tm_min;
		for(i=10;i>2;i--)
			shmaddr[i]=0;
	}
	for(i=2;i<11;i++)
	{
		if(shmaddr[i]==1)
		{
			if(i==2)
			{
				shmaddr[11]*=-1;
				shmaddr[2]=0;
			}
			if(shmaddr[11]==1)
			{
				if(i==3)
				{
					time(&rawtime);
					timeinfo = localtime(&rawtime);
					raw_hour=timeinfo->tm_hour;
					raw_min=timeinfo->tm_min;
					elapse_hour=0;
					elapse_min=0;
					shmaddr[3]=0;
				}
				else if(i==4)
				{
					elapse_hour++;
					shmaddr[4]=0;
				}
				else if(i==5)
				{
					elapse_min++;
					shmaddr[5]=0;
				}	
			}
		}
	}
	
	for(i=10;i>5;i--)
		shmaddr[i]=0;

	hour=((raw_min+elapse_min)/60+raw_hour+elapse_hour)%24;
	min=(raw_min+elapse_min)%60;
	printf("%d : %d\n",hour,min);
	shmaddr[12]=min%10;
	shmaddr[13]=min/10;
	shmaddr[14]=hour%10;
	shmaddr[15]=hour/10;
}

void mode2(int *shmaddr)
{
	int selector[4]={10,1,1,2};
	int select_idx=0;
	int i;
	for(i = 10;i>1;i--)
	{
		if(shmaddr[i]==1)
		{
			if(i==2)
			{
				select_idx++;
				if(select_idx>=5)
					select_idx=0;
				mul=selector[select_idx];
			}
			else if(i==3)
				res+=mul*mul;
			else if(i==4)
				res+=mul;
			else if(i==5)
				res++;
			shmaddr[i]=0;
		}
		printf("%d\n",res);
		shmaddr[12] = res%mul;
		res/=mul;
		shmaddr[13] = res%mul;
		res/=mul;
		shmaddr[14] = res%mul;
		res/=mul;
		shmaddr[15] = 0;
	}

}
int main()
{
	int in_process,out_process;
	key_t key = ftok("/etc/passwd",1);
	int shmid = shmget(key,1024,IPC_CREAT|0644);
	char str[256];


	sprintf(str,"%d",shmid);	
	int *shmaddr;
	shmaddr=shmat(shmid,(int*)NULL,0);
	if((in_process=fork())==-1)
	{
		perror("error on fork!\n");
		exit(1);
	}
	else if(in_process == 0){
		char *argc[] = {"./inproc", str, NULL};
		execve(argc[0],argc,NULL);
	}
	else
	{
		
		if((out_process=fork())==-1)
		{
			perror("error on fork!\n");
			exit(1);
		}
		else if (out_process==0){
			char *argc[] = {"./outproc", str, NULL};
			execve(argc[0],argc,NULL);
		}
		else
		{
			while(shmaddr[0]!=158)
			{
				switch(shmaddr[1])
				{
					case 1 :
									mode1(shmaddr);
									break;
					case 2 :
									mode2(shmaddr);
									break;
					case 3 :
									break;
					case 4 :
									break;
					case 0 :
									continue;
				}
			}
			printf("end\n");
		}
	}
}
