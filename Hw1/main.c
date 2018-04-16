#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
unsigned char *shmaddr;

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
unsigned char buffer[10];
unsigned char row=0,col=0x70;
unsigned char on=1;
int cursor_flag=-1;
time_t cursor;
clock_t tic;

//mode 3,4


int self_test=-1;
char text[33]="Fpga_self_testerPress_2_and_3_SW";
char test[10]="testing...";
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
	

//mode 5

/***********************************
module name : mode1
parameter : none
function : show time on FND and light
and light LED by edit or fix
************************************/
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
			if(i==0)		//button 1 set flag for edit time
			{
				if(shmaddr[14]==10)
					shmaddr[14]=1;
				else
					shmaddr[14]=10;
				shmaddr[0]=0;
			}
			if(shmaddr[14]==1)
			{
				if(i==1)		//button 2 initialize to raw_time
				{
					elapse_hour=0;
					elapse_min=0;
					shmaddr[1]=0;
				}
				else if(i==2)	//button 3 increase 1hour
				{
					elapse_hour++;
					shmaddr[2]=0;
				}
				else if(i==3)	//button 4 increase 1 minute
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
	shmaddr[13]=min%10;	//shared memory for FND
}


/***********************************
module name : mode2
parameter : none
function : Count number and change
to Decimal,Octal,tetramal,Binary
and show to FND and LED
***********************************/

void mode2(void)
{
	int selector[4]={10,8,4,2};		//change multiplier
	int tmp;
	int i;
	for(i = 0;i<4;i++)
	{
		if(shmaddr[i]==1)
		{
			if(i==0)
			{
				select_idx++;		//change multiplier
				if(select_idx==4)
					select_idx=0;
				mul=selector[select_idx];
			}
			else if(i==1)			//increase second digit
				res+=mul*mul;
			else if(i==2)			//increase third digit
				res+=mul;
			else if(i==3)			//increase fourth digit
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
/***********************************
module name : mode3
parameter : none
function : edit text editor each 
button has 3 charater and some keys
have special fuction like clear,
space,change to number
***********************************/
void mode3(void)
{
	int i=0;
	int j=0;
	int func_flag=0;
	unsigned char buf=' ';		//charater to input

	memcpy(shmaddr+47,input_mode[mode_flag],10);		//for show input mode on Dot matrix
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
	if(double_key!=100000)		//for detecting double key input
		return;
	else
		double_key=0;

	if(i==1 && j==2)
	{
		memset(shmaddr+15,' ',32);
		cnt+=2;
		ind=-1;
		buf=' ';
		prev_num=0;
		double_key=0;
		func_flag=1;
	}
	else if(i==4 && j==5)
	{
		if(mode_flag==0)
			mode_flag=1;
		else
			mode_flag=0;
		cnt+=2;
		func_flag=1;
	}
	else if(i==7 && j==8)
	{
		buf=' ';
		ind++;
		cnt+=2;
		func_flag=1;
	}
	else if(j==10)		//if only one key is input
	{
		if(mode_flag==0)
		{
			if(prev_num==i)	//change character
			{
				cnt++;
				buf = input[i][key_num++];
				if(key_num==3)
					key_num=0;
			}
			else	//input to string and buf is another character
			{
				ind++;
				cnt++;
				buf = input[i][key_num];
			}
		}
		else			//for number input
		{
			ind++;
			cnt++;
			buf = i+49;
		}
	}
	prev_num=i;
	shmaddr[i]=0;
	shmaddr[j]=0;
	if(ind>=0 && func_flag==0)
	{
		if(ind>=32)		//if string is longer than 32 character, then push from right to left
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
	//show how many button that user pressed
}
/***********************************
module name : mode4
parameter : none
function : draw to Dot matrix each
button has each funciton like move
cursor or draw on cursor, clear,
and reverse picture
***********************************/
void mode4(void)
{
	int i,j;

	for(i=0;i<10;i++)
	{
		if(shmaddr[i]==1)
		{
			cnt++;
			if(i==0)		//reset
			{
				row=0;
				col=0x40;
				cnt=0;
				memset(image,0x00,10);
			}
			else if(i==1)	//up
			{
				if(row==0)
				{
					shmaddr[i]=0;
					return;
				}
				row--;	
			}
			else if(i==3)	//left
			{
				if(col==0x40)
				{
					shmaddr[i]=0;
					return;
				}
				col = col<<1;
			}
			else if(i==5)	//rigth
			{
				if(col==0x01)
				{
					shmaddr[i]=0;
					return;
				}
				col = col>>1;
			}
			else if(i==7)	//down
			{
				if(row==9)
				{
					shmaddr[i]=0;
					return;
				}
				row++;
			}
			else if(i==2)	//cursor
			{
				cursor_flag*=-1;
			}
			else if(i==6)	//clear
			{
				printf("clear!\n");
				memset(image,0,10);
			}
			else if(i==8)	//reverse
			{
				printf("revers!\n");
				for(j=0;j<10;j++)
					image[j]=~image[j];
			}
			else if(i==4)	//draw
			{
				image[row] = image[row]^col;
			}
			shmaddr[i]=0;
			memcpy(buffer,image,10);
		}
	}
	if(cursor_flag==-1)
	{
		if(difftime(time(NULL),cursor)>=1)	//each 1sec, cursor blink
		{
			time(&cursor);
			buffer[row]=buffer[row]^col;	//buffer make Dot can show at once
		}
	}
	memcpy(shmaddr+47,buffer,10);	
	shmaddr[10] = (cnt%10000)/1000;
	shmaddr[11] = (cnt%1000)/100;
	shmaddr[12] = (cnt%100)/10;
	shmaddr[13] = cnt%10;	//show how many times user pressed button
}
/***********************************
module name : mode5
parameter : none
function : test the fpga if there are
some error on display
***********************************/
void mode5(void)		//fpga_self_tester
{
	int i,j;

	if(self_test==-1)
	{
		memset(shmaddr+10,8,4);
		shmaddr[14]=11;
		if(clock()-tic>=1500000)
			sprintf(text,"Fpga_self_testerPress_2_and_3_SW");
		memcpy(shmaddr+15,text,32);
		memset(shmaddr+47,0x7f,10);		//on all light on fpga display
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
		else	//if button pressed, show which key are pressed and act
		{
			if(i==0)
				sprintf(text,"%dst_key_Pressed Press_2_and_3_SW",i+1);
			else if(i==1)
				sprintf(text,"%dnd_key_Pressed Press_2_and_3_SW",i+1);
			else if(i==1)
				sprintf(text,"%drd_key_Pressed Press_2_and_3_SW",i+1);
			else 
				sprintf(text,"%dth_key_Pressed Press_2_and_3_SW",i+1);
			memcpy(shmaddr+15,text,32);
			if(i==1 && j==2)	//if 2 and 3 button are pressed, display some work
			{
				self_test=1;		
				memset(shmaddr+10,0,4);
				shmaddr[14]=11;
				memset(shmaddr+15,' ',32);
				memset(shmaddr+47,0x00,10);
				cnt=-1;
				tic=clock();
			}
			shmaddr[i]=0;
			shmaddr[j]=0;
			double_key=0;
			tic=clock();
		}
	}
	else if(clock()-tic>=500000)		//some work	each action change by 0.5sec
	{
		cnt++;
		if(cnt<=2)
			shmaddr[14]=cnt+12;
		else if(cnt==3)
		{
			shmaddr[14]=12+cnt;
			shmaddr[10]=8;
		}
		else if(cnt==4)
		{
			shmaddr[14]=0;
			shmaddr[10]=0;
			shmaddr[11]=8;
			memset(shmaddr+47,0x40,10);
		}
		else if(cnt==5)
		{
			shmaddr[14]=0;
			shmaddr[10]=0;
			shmaddr[11]=0;
			shmaddr[12]=8;
			memset(shmaddr+47,0x38,10);
		}
		else if(cnt==6)
		{

			shmaddr[14]=0;
			shmaddr[10]=0;
			shmaddr[11]=0;
			shmaddr[12]=0;
			shmaddr[13]=8;
			memset(shmaddr+47,0x07,10);
		}
		else if(cnt<=49)
		{
			shmaddr[14]=0;
			shmaddr[10]=0;
			shmaddr[11]=0;
			shmaddr[12]=0;
			shmaddr[13]=0;
			memset(shmaddr+47,0x00,10);
			memset(shmaddr+15,' ',32);
			if(cnt<=17)
				memcpy(shmaddr+15,test+(strlen(test)-(cnt-7)),cnt-7);
			else if(cnt<=39)
				memcpy(shmaddr+15+cnt-17,test,strlen(test));
			else
				memcpy(shmaddr+15+cnt-17,test,strlen(test)-(cnt-39));	
		}
		else
		{
			cnt=0;
			self_test=-1;
		}
		tic=clock();
	}

}
/***********************************
module name : main
parameter : none
function : fork child process and 
calculate all functions act by using
shared memory
***********************************/
int main()
{
	int i,j;
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
					if(mode==6)
						mode=1;
				}
				else if(shmaddr[57]==114)
				{
					mode--;
					init=1;
					if(mode==0)
						mode=5;
				}
				if(init==1)			//init all shmaddr and global var
				{
					for(i=0;i<=57;i++)
						shmaddr[i]=0;
					memset(shmaddr+15,' ',32);

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
					on=1;

					if(mode==1)
						shmaddr[14]=10;
					memset(image,0,10);
					memset(buffer,0,10);
					row=0;
					col=0x40;
					cursor_flag=-1;
					double_key=0;
					sprintf(text,"Fpga_self_testerPress_2_and_3_SW");

					self_test=-1;

					time(&cursor);
					tic=clock();
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
									mode4();
									break;
					case 5 :
									mode5();
									break;
					deafault :
									continue;
				}
			}
			printf("end\n");
		}
	}
}
