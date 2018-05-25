#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/ioctl.h>


#include "fpga_dot_font.h"

#define IOM_dev_MAJOR 242 // ioboard fpga device major number
#define IOM_dev_NAME "dev_driver"

#define IOCTL_FND _IOW(IOM_dev_MAJOR,0,int)
#define IOCTL_DOT _IOW(IOM_dev_MAJOR,1,int)
#define IOCTL_LED _IOW(IOM_dev_MAJOR,2,int)
#define IOCTL_TEXT_LCD _IOW(IOM_dev_MAJOR,3,int)
#define IOCTL_TIMER _IOW(IOM_dev_MAJOR,4,int)



#define IOM_FND_ADDRESS 0x08000004
#define IOM_LED_ADDRESS 0x08000016
#define IOM_TEXT_LCD_ADDRESS 0x08000090
#define IOM_DOT_ADDRESS 0x08000210

static unsigned char *iom_fpga_fnd_addr;
static unsigned char *iom_fpga_dot_addr;
static unsigned char *iom_fpga_led_addr;
static unsigned char *iom_fpga_text_lcd_addr;
static int dev_usage =0;
/*
static struct struct_mydata{
	struct timer_list timer;
	int count;
};*/

//struct struct_mydata mydata;

int iom_dev_open(struct inode *minode, struct file *mfile);
long iom_dev_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);
int iom_dev_release(struct inode *minode,struct file *mfile);

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
	unsigned char value[10];



	switch(ioctl_num) {
		case IOCTL_FND:
				 fnd_loc = res & 0xF;
				 fnd_num = (res >> 4) &0xF;
				 printk("%d %d \n\n",fnd_loc,fnd_num);
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
				 fnd_num = (res >> 4) &0xF;
				 printk("%d \n\n",fnd_num);
				 memcpy(value,fpga_number[fnd_num],sizeof(fpga_number[fnd_num]));
				 for(i=0;i<10;i++)
				 {
					value_short = value[i] & 0x7f;
				 printk("%d \n\n",value_short);
					outw(value_short,(unsigned int)iom_fpga_dot_addr+i*2);
				 }
				 break;
		case IOCTL_LED:
				 fnd_num = (res >> 4) &0xF;
				 value_short = 1 >> fnd_num;
		 	   outw(value_short,(unsigned int)iom_fpga_led_addr);
				 break;
		case IOCTL_TEXT_LCD:
				 break;
		case IOCTL_TIMER:
				 break;
	}
	return 0;
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

//	init_timer(&(mydata.timer));
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
	//del_timer_sync(&mydata.timer);

	unregister_chrdev(IOM_dev_MAJOR,IOM_dev_NAME);
}

module_init(iom_dev_init);
module_exit(iom_dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huins");

