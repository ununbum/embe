#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>


int main()
{
	int in_process,out_process;
	int shmid = shmget(IPC_PRIVATE,1024,IPC_CREAT|0644);
	char str[256];
	time_t rawtime;
	struct tm *timeinfo;


	sprintf(str, "%d", shmid);
	
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
		else if(out_process == 0){
			char *argc[] = {"./outproc", str, NULL};
			execve(argc[0],argc,NULL);
		}
		else
		{
			int *shmaddr=shmat(shmid,(int*)NULL,0);
			int i;
			int setup_flag=-1;
			while(shmaddr[0]!=158)
			{
				usleep(400000);
				switch(shmaddr[1])
				{
					case 1 :
									printf("%d\n\n",shmaddr[0]);
									if(shmaddr[0]>=0)
									{
										time(&rawtime);
										timeinfo = localtime(&rawtime);
										shmaddr[11]=timeinfo->tm_hour;
										shmaddr[12]=timeinfo->tm_min;
										shmaddr[0]=-1;
									}
									for(i=2;i<11;i++)
									{
										printf("[%d] ",shmaddr[i]);
										if(shmaddr[i]==1)
										{
											if(i==2)
												setup_flag*=-1;
											if(setup_flag==1)
											{
												if(i==3)
												{
													time(&rawtime);
													timeinfo = localtime(&rawtime);
													shmaddr[11]=timeinfo->tm_hour;
													shmaddr[12]=timeinfo->tm_min;
												}
												else if(i==4)
													shmaddr[11]++;
												else if(i==5)
													shmaddr[12]++;
											}
										}
									}
									if(shmaddr[12]==60)
									{
										shmaddr[11]++;
										shmaddr[12]=0;
									}
									if(shmaddr[11]==24)
										shmaddr[11]=0;
									shmaddr[13]=setup_flag;
									printf("%d : %d\n",shmaddr[11],shmaddr[12]);
									break;
					case 2 :
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

