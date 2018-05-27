#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/unistd.h>


#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/version.h>


#include "fpga_dot_font.h"
#include "dev_driver.h"




#define IOM_FND_ADDRESS 0x08000004
#define IOM_LED_ADDRESS 0x08000016
#define IOM_TEXT_LCD_ADDRESS 0x08000090
#define IOM_DOT_ADDRESS 0x08000210

static unsigned char *iom_fpga_fnd_addr;
static unsigned char *iom_fpga_dot_addr;
static unsigned char *iom_fpga_led_addr;
static unsigned char *iom_fpga_text_lcd_addr;
static int dev_usage =0;
static unsigned char pos1=0,pos2=0,flag1,flag2,cnt=0;

static struct struct_mydata{
	struct timer_list timer;
	struct file *file;
	unsigned char time_tic;
	unsigned long input;
	int count;
};
struct struct_mydata mydata;

int iom_dev_open(struct inode *minode, struct file *mfile);
long iom_dev_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);
int iom_dev_release(struct inode *minode,struct file *mfile);
void change_input(unsigned long timeout);

static struct file_operations iom_dev_fops = 
{
	.open = iom_dev_open,
	.unlocked_ioctl = iom_dev_ioctl,
	.release = iom_dev_release
};

// define functions...
int iom_dev_open(struct inode *minode, struct file *mfile)
{
	if(dev_usage != 0) return -EBUSY;

	dev_usage=1;
	return 0;
}
int iom_dev_release(struct inode *minode,struct file *mfile)
{
	dev_usage =0;
	pos1=0;
	pos2=0;
	flag1=0;
	flag2=0;

	return 0;
}


long iom_dev_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	int i;
	unsigned int res = (unsigned int)ioctl_param;
	unsigned char fnd_loc;
	unsigned char fnd_num;
	unsigned short int time_tic;
	unsigned short int value_short =0;
	unsigned char value[33];



	fnd_num = (res >> 4) &0xF;
	switch(ioctl_num) {
		case IOCTL_FND:
				 fnd_loc = res & 0xF;
				 for(i=0;i<4;i++)
				 {
					 if(i==fnd_loc)
						 value[i] = fnd_num;
					 else
						 value[i]=0;
				 }
				 value_short = value[0] << 12 | value[1] << 8 | value[2] << 4 | value[3];
				 outw(value_short,(unsigned int)iom_fpga_fnd_addr);
				 break;
		case IOCTL_DOT:
				 if(fnd_num!=0)
				 memcpy(value,fpga_number[fnd_num],sizeof(fpga_number[fnd_num]));
				 else
					 memset(value,0,10);
				 for(i=0;i<10;i++)
				 {
					value_short = value[i] & 0x7f;
					outw(value_short,(unsigned int)iom_fpga_dot_addr+i*2);
				 }
				 break;
		case IOCTL_LED:
				 if(fnd_num!=0)
					 value_short = 128 >> (fnd_num-1);
				 else
					 value_short=0;
		 	   outw(value_short,(unsigned int)iom_fpga_led_addr);
				 break;
		case IOCTL_TEXT_LCD:
				 memset(value,0x20,sizeof(value));
				 if(fnd_num!=0)
				 {
				 	memcpy(value+pos1,"20131532",8);
				 	memcpy(value+pos2+16,"KimSeungHyeun",13);
				 }
				 else
				 {
					 pos1=0;
					 pos2=0;
					 flag1=0;
					 flag2=0;
				 }
				 for(i=0;i<33;i++)
				 {
					 value_short = (value[i] & 0xFF) << 8 | (value[i+1] & 0xFF);
					 outw(value_short,(unsigned int)iom_fpga_text_lcd_addr+i);
					 i++;
				 }
				 break;
		case IOCTL_TIMER:
				 mydata.input = res;

				 time_tic = (res >> 16) & 0xFF;
				 mydata.count = (res >> 8) & 0xFF;
				 mydata.file = file;
				 mydata.time_tic = time_tic;


				 del_timer_sync(&mydata.timer);


				 mydata.timer.expires = get_jiffies_64() + (time_tic* HZ/10);
				 mydata.timer.data = (unsigned long)&mydata;
				 mydata.timer.function = change_input;

				 add_timer(&mydata.timer);
				 break;
	}
	return 0;
}
void change_input(unsigned long timeout)
{
	struct struct_mydata *p_data = (struct struct_mydata*)timeout;
	unsigned long next_input;
	p_data->count--;
	if(p_data->count == 0)
	{
		iom_dev_ioctl(p_data->file,IOCTL_FND,0x00);
		iom_dev_ioctl(p_data->file,IOCTL_DOT,0x00);
		iom_dev_ioctl(p_data->file,IOCTL_LED,0x00);
		iom_dev_ioctl(p_data->file,IOCTL_TEXT_LCD,0x00);
		cnt=0;
		return;
	}
	
	if(flag1==0)
		pos1++;
	else
		pos1--;
	if(pos1==8)
		flag1=1;
	else if(pos1==0)
		flag1=0;
	if(flag2==0)
		pos2++;
	else
		pos2--;
	if(pos2==3)
		flag2=1;
	else if(pos2==0)
		flag2=0;
	
	next_input = p_data->input+0x10;
	cnt++;
	if((next_input & 0xf0) == 0x90)
		next_input-=0x80;

	if(cnt==8)
	{
		next_input+=0x1;
		if((next_input&0xf) == 0x4)
			next_input-=0x4;
		cnt=0;
	}

	p_data->input = next_input;

	mydata.timer.expires = get_jiffies_64() + (mydata.time_tic*HZ/10);
	mydata.timer.data = (unsigned long)&mydata;
	mydata.timer.function = change_input;

	add_timer(&mydata.timer);


	iom_dev_ioctl(p_data->file,IOCTL_FND,next_input);
	iom_dev_ioctl(p_data->file,IOCTL_DOT,next_input);
	iom_dev_ioctl(p_data->file,IOCTL_LED,next_input);
	iom_dev_ioctl(p_data->file,IOCTL_TEXT_LCD,next_input);
}


int __init iom_dev_init(void)
{
	int result;
	result = register_chrdev(IOM_dev_MAJOR,IOM_dev_NAME, &iom_dev_fops);

	if(result<0){
		printk(KERN_WARNING"Can't get any major\n");
		return result;
	}


	iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
	iom_fpga_dot_addr = ioremap(IOM_DOT_ADDRESS, 0x10);
	iom_fpga_led_addr = ioremap(IOM_LED_ADDRESS, 0x1);
	iom_fpga_text_lcd_addr = ioremap(IOM_TEXT_LCD_ADDRESS, 0x32);

	init_timer(&(mydata.timer));
	printk("init module, %s major number : %d\n",IOM_dev_NAME,IOM_dev_MAJOR);

	return 0;
}
void __exit iom_dev_exit(void)
{
	iounmap(iom_fpga_fnd_addr);
	iounmap(iom_fpga_dot_addr);
	iounmap(iom_fpga_led_addr);
	iounmap(iom_fpga_text_lcd_addr);

	dev_usage = 0;
	pos1=0;
	pos2=0;
	flag1=0;
	flag2=0;
	del_timer_sync(&mydata.timer);

	unregister_chrdev(IOM_dev_MAJOR,IOM_dev_NAME);
}

module_init(iom_dev_init);
module_exit(iom_dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huins");

