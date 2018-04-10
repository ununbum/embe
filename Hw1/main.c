#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>

int *shmaddr;
void mode1(void)
{
	int i;
	int raw_hour,raw_min;
	int elapse_hour,elapse_min;
	time_t rawtime;
	struct tm *timeinfo;
	if(shmaddr[13]==-1)
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
				shmaddr[13]*=-1;
				shmaddr[2]=0;
			}
			if(shmaddr[13]==1)
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
	shmaddr[11]=((raw_min+elapse_min)/60+raw_hour+elapse_hour)%24;
	shmaddr[12]=(raw_min+elapse_min)%60;
}

void mode2(void)
{
	int selector[4]={10,8,4,2};
	int select_idx=0;
	int i;
	for(i = 10;i>1;i--)
	{
		if(shmaddr[i]==1)
		{
			if(i==2)
			{
				select_idx++;
				if(select_idx>5)
					select_idx=0;
				shmaddr[14]=selector[select_idx];
			}
			else if(i==3)
				shmaddr[15]+=shmaddr[14]*shmaddr[14];
			else if(i==4)
				shmaddr[15]+=shmaddr[14];
			else if(i==5)
				shmaddr[15]++;
			shmaddr[i]=0;
		}
	}

}
int main()
{
	int in_process,out_process;
	int shmid = shmget(IPC_PRIVATE,1024,IPC_CREAT|0644);
	char str[256];


	sprintf(str, "%d", shmid);
	
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
									mode1();
									break;
					case 2 :
									mode2();
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
