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
int select_idx=0;
//mode2

int cnt=0;
int ind=-1;
int prev_num=-1;
int key_num;
int mode_flag=0;
int double_key=0;

unsigned char input_mode[2][10]={
	{0x1c,0x36,0x63,0x63,0x63,0x7f,0x7f,0x63,0x63,0x63},
	{0x0c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x3f,0x3f}
};

unsigned char image[10]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char row=0,col=0x70;
unsigned char cursor_flag=-1;
time_t cursor;


//mode 3,4

unsigned char input[9][3]={
	{'.','Q','Z'},
	{'A','B','C'},
	{'D','E','F'},
	{'G','H','I'},
	{'J','K','L'},
	{'M','N','O'},
	{'P','R','S'},
	{'T','U','V'},
	{'W','X','Y'},
};

void mode1(void)
{
	int i;
	int raw_hour,raw_min;
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	raw_hour=timeinfo->tm_hour;
	raw_min=timeinfo->tm_min;
	if(shmaddr[14]==-1)
	{
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
	}

	hour=((raw_min+elapse_min)/60+raw_hour+elapse_hour)%24;
	min=(raw_min+elapse_min)%60;


	//printf("%d : %d\n",elapse_hour,elapse_min);
	shmaddr[10]=hour/10;
	shmaddr[11]=hour%10;
	shmaddr[12]=min/10;
	shmaddr[13]=min%10;

}

void mode2(void)
{
	int selector[4]={10,8,4,2};
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
	shmaddr[10] = (cnt%10000)/1000;
	shmaddr[11] = (cnt%1000)/100;
	shmaddr[12] = (cnt%100)/10;
	shmaddr[13] = cnt%10;
}
void mode4(void)
{
	int i,j;

	for(i=0;i<10;i++)
	{
		if(shmaddr[i]==0)
		{
			if(i==0)		//reset
			{
				row=0;
				col=0x70;
				cnt=0;
				image[10]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
			}
			else if(i==1)	//up
			{
				if(row==0)
					return;
				row--;	
			}
			else if(i==3)	//left
			{
				if(col==0x00)
					return;
				col=col>>1;
			}
			else if(i==5)	//rigth
			{
				if(col==0x70)
					return;
				col=col<<1;
			}
			else if(i==7)	//down
			{
				if(row==9)
					return;
				row++
			}
			else if(i==2)	//cursor
			{
				cursor_flag*=-1;
			}
			else if(i==6)	//clear
			{
				image[10]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
			}
			else if(i==8)	//reverse
			{
				for(j=0;j<10;j++)
					image[j]^=image[j];
			}
			else if(i==4)	//draw
			{
				image[row]=image[row]|col;
			}
			shmaddr[i]=0;
			for(i=0;i<10;i++)
			{
				shmaddr[i+47]=image[i];
				if(i==row && cursor_flag==-1 && (difftime(time(NULL),cursor)>60))
				{
					shmaddr[i+47]=shmaddr[i+47]^col;
					time(&cursor);
				}
			}
		}
	}

}
int main()
{
	int i;
	int in_process,out_process;
	int mode=1;
	int init=1;
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
					for(i=0;i<=57;i++)
						shmaddr[i]=0;
					memset(shmaddr+15,' ',128);

					hour=0;
					min=0;

					res=0;
					mul=10;
					select_idx=0;

					cnt=0;
					ind=-1;
					prev_num=0;
					key_num=0;
					mode_flag=0;

					image[10]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
					if(mode==1)
						shmaddr[14]=-1;
					row=0;
					col=0x70;
					cursor_flag=-1;
					time(&cursor);
					init=0;
				}
			//	printf("mode : %d\n",mode);	
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
