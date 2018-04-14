#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
int *shmaddr;

int elapse_hour=0,elapse_min=0;
int hour=0,min=0;
//mode1

int res=0,mul=10;
<<<<<<< HEAD
//mode2

int cnt=0;
int ind=0;
int prev_num;
int key_num;
int mode_flag=0;

int input_mode[2][10]={
=======
int select_idx=0;
//mode2

int cnt=0;
int ind=-1;
int prev_num=-1;
int key_num;
int mode_flag=0;
int double_key=0;

unsigned char input_mode[2][10]={
>>>>>>> dc89ad93ac5dc6d410ab05eda2a834785edad678
	{0x1c,0x36,0x63,0x63,0x63,0x7f,0x7f,0x63,0x63,0x63},
	{0x0c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x3f,0x3f}
};

//mode 3,4

<<<<<<< HEAD
char input[9][3]={
=======
unsigned char input[9][3]={
>>>>>>> dc89ad93ac5dc6d410ab05eda2a834785edad678
	{'.','Q','Z'},
	{'A','B','C'},
	{'D','E','F'},
	{'G','H','I'},
	{'J','K','L'},
	{'M','N','O'},
	{'P','R','S'},
	{'T','U','V'},
<<<<<<< HEAD
	{'W','X','Y'}
=======
	{'W','X','Y'},
>>>>>>> dc89ad93ac5dc6d410ab05eda2a834785edad678
};

void mode1(void)
{
	int i;
<<<<<<< HEAD
	int raw_hour=hour,raw_min=min;
	time_t rawtime;
	struct tm *timeinfo;
	if(shmaddr[14]==-1)
	{
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		raw_hour=timeinfo->tm_hour;
		raw_min=timeinfo->tm_min;
=======
	int raw_hour,raw_min;
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	raw_hour=timeinfo->tm_hour;
	raw_min=timeinfo->tm_min;
	if(shmaddr[14]==-1)
	{
>>>>>>> dc89ad93ac5dc6d410ab05eda2a834785edad678
		for(i=9;i>0;i--)
			shmaddr[i]=0;
	}
	for(i=0;i<5;i++)
	{
		if(shmaddr[i]==1)
		{
			if(i==0)
			{
				shmaddr[14]*=-1;
				shmaddr[0]=0;
			}
			if(shmaddr[14]==1)
			{
				if(i==1)
				{
					elapse_hour=0;
					elapse_min=0;
					shmaddr[1]=0;
				}
				else if(i==2)
				{
					elapse_hour++;
					shmaddr[2]=0;
				}
				else if(i==3)
				{
					elapse_min++;
					shmaddr[3]=0;
				}	
			}
		}
<<<<<<< HEAD
	}	
	hour=((raw_min+elapse_min)/60+raw_hour+elapse_hour)%24;
	min=(raw_min+elapse_min)%60;

=======
	}

	hour=((raw_min+elapse_min)/60+raw_hour+elapse_hour)%24;
	min=(raw_min+elapse_min)%60;


	//printf("%d : %d\n",elapse_hour,elapse_min);
>>>>>>> dc89ad93ac5dc6d410ab05eda2a834785edad678
	shmaddr[10]=hour/10;
	shmaddr[11]=hour%10;
	shmaddr[12]=min/10;
	shmaddr[13]=min%10;

}

void mode2(void)
{
	int selector[4]={10,8,4,2};
<<<<<<< HEAD
	int select_idx=0;
=======
>>>>>>> dc89ad93ac5dc6d410ab05eda2a834785edad678
	int tmp;
	int i;
	for(i = 0;i<4;i++)
	{
		if(shmaddr[i]==1)
		{
			if(i==0)
			{
				select_idx++;
				if(select_idx==4)
					select_idx=0;
				mul=selector[select_idx];
			}
			else if(i==1)
				res+=mul*mul;
			else if(i==2)
				res+=mul;
			else if(i==3)
				res++;
			shmaddr[i]=0;
		}

		shmaddr[14]=mul;
		tmp=res;
		shmaddr[13]=tmp%mul;
		tmp/=mul;
		shmaddr[12]=tmp%mul;
		tmp/=mul;
		shmaddr[11]=tmp%mul;
		shmaddr[10]=0;
	}
}
void mode3(void)
{
<<<<<<< HEAD
	int i;
	int j=0;
	int buf;		//function key

	for(i=0;i<10;i++)
	{
		if(shmaddr[i]==1)
			break;
	}
	for(j=i;j<10;j++)
	{
		if(shmaddr[j]==1)
			break;
=======
	int i=0;
	int j=0;
	unsigned char buf=' ';		//function key

	memcpy(shmaddr+47,input_mode[mode_flag],10);
	for(i=0;i<10;i++)
	{
		if(shmaddr[i]==1)
		{
			break;
		}
	}
	if(i!=10)
	{
		for(j=i+1;j<10;j++)
		{
			if(shmaddr[j]==1)
			{
				break;
			}
		}
>>>>>>> dc89ad93ac5dc6d410ab05eda2a834785edad678
	}
	else
		return;
	double_key++;
	if(double_key!=100000)
		return;
	else
		double_key=0;

	if(i==1 && j==2)
	{
		printf("clear\n");
		for(i=15;i<=47;i++)
			shmaddr[i]=' ';	
		cnt+=2;
		ind=-1;
		buf=' ';
		prev_num=0;
		double_key=0;
		shmaddr[1]=0;
		shmaddr[2]=0;
		return;
	}
	else if(i==4 && j==5)
	{
		printf("change\n");
		if(mode_flag==0)
			mode_flag=1;
		else
			mode_flag=0;
		cnt+=2;
		shmaddr[i]=0;
		shmaddr[j]=0;
		return;
	}
	else if(i==7 && j==8)
	{
		printf("space\n");
		buf=' ';
		ind++;
		cnt+=2;
		shmaddr[i]=0;
		shmaddr[j]=0;
		return;
	}
	else if(j==10)
	{
		if(mode_flag==0)
		{
			if(prev_num==i)
			{
				cnt++;
				buf = input[i][key_num++];
				if(key_num==3)
					key_num=0;
			}
			else
			{
				printf("%d %d\n",i,prev_num);
				ind++;
				cnt++;
				buf = input[i][key_num];
			}
		}
		else
		{
			ind++;
			cnt++;
			buf = i+49;
		}
	}
	prev_num=i;
	shmaddr[i]=0;
	shmaddr[j]=0;
	if(ind>=0)
	{
		if(ind>=32)
		{
			for(i=0;i<31;i++)
				shmaddr[i+15]=shmaddr[i+16];
			shmaddr[46]=buf;
		}
		else
			shmaddr[ind+15] = buf;
	}


}
void mode4(void)
{

	if(i==2 && j==3)
	{
		for(i=15;i<47;i++)
			shmaddr[i]=' ';
		
		cnt+=2;
	}
	else if(i==5 && j==6)
	{
		if(mode_flag==0)
			mode_flag=1;
		else
			mode_flag=0;
		cnt+=2;
	}
	else if(i==8 && j==9)
	{
		buf=' ';
		ind++;
		cnt+=2;
	}
	else if(i!=10 && j==10)
	{
		if(prev_num!=i)
		{
			ind++;
			cnt++;
			buf = input[i][key_num++];
			if(key_num==3)
				key_num=0;
		}
		cnt++;
	}
	if(ind>=0)
	{
		if(ind>=32)
		{
			for(i=0;i<31;i++)
				shmaddr[i+15]=shmaddr[i+16];
			shmaddr[46]=buf;
		}
		else
			shmaddr[ind+15] = buf;
	}

	prev_num=i;
	
	memcpy(shmaddr+47,input_mode[mode_flag],10);

}
void mode4(void)
{

}
int main()
{
	int i;
	int in_process,out_process;
	int mode=1;
<<<<<<< HEAD
	int init=0;
=======
	int init=1;
>>>>>>> dc89ad93ac5dc6d410ab05eda2a834785edad678
	int shmid = shmget(IPC_PRIVATE,1024,IPC_CREAT|0644);
	char str[256];


	sprintf(str, "%d", shmid);
	
	shmaddr=shmat(shmid,(int*)NULL,0);
	if((in_process=fork())==-1)			//fork fail
	{
		perror("error on fork!\n");
		exit(1);
	}
	else if(in_process == 0){			//fork input_process
		char *argc[] = {"./inproc", str, NULL};
		execve(argc[0],argc,NULL);
	}
	else								//fork output_process
	{
		
		if((out_process=fork())==-1)
		{
			perror("error on fork!\n");
			exit(1);
		}
		else if (out_process==0){		//fork output_process
			char *argc[] = {"./outproc", str, NULL};
			execve(argc[0],argc,NULL);
		}
		else							//do functional action
		{
			while(shmaddr[57]!=158)		//if back key is evented, exit program
			{
				if(shmaddr[57]==115)
				{
					mode++;
					init=1;
					if(mode==5)
						mode=1;
				}
				else if(shmaddr[57]==114)
				{
					mode--;
					init=1;
					if(mode==0)
						mode=4;
				}
				if(init==1)			//init all shmaddr and global var
				{
<<<<<<< HEAD
					for(i=0;i<57;i++)
						shmaddr[i]=0;
					memset(shmaddr+15,' ',32);

					elapse_hour=0;
					elapse_min=0;
=======
					for(i=0;i<=57;i++)
						shmaddr[i]=0;
					memset(shmaddr+15,' ',128);

>>>>>>> dc89ad93ac5dc6d410ab05eda2a834785edad678
					hour=0;
					min=0;

					res=0;
					mul=10;
<<<<<<< HEAD

					cnt=0;
					ind=0;
=======
					select_idx=0;

					cnt=0;
					ind=-1;
>>>>>>> dc89ad93ac5dc6d410ab05eda2a834785edad678
					prev_num=0;
					key_num=0;
					mode_flag=0;

<<<<<<< HEAD
					init=0;
				}
			
=======
					if(mode==1)
						shmaddr[14]=-1;

					init=0;
				}
			//	printf("mode : %d\n",mode);	
>>>>>>> dc89ad93ac5dc6d410ab05eda2a834785edad678
				switch(mode)
				{
					case 1 :
									mode1();
									break;
					case 2 :
									mode2();
									break;
					case 3 :
									mode3();
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
