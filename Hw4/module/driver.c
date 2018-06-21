#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <mach/gpio.h>
#include <linux/platform_device.h>
#include <asm/gpio.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <linux/version.h>
#include <linux/cdev.h>


#include "fpga_dot_font.h"

#define IOM_FND_ADDRESS 0x08000004
#define IOM_DOT_ADDRESS 0x08000210


static unsigned char *fpga_fnd_addr;
static unsigned char *fpga_dot_addr;

static int inter_major=242, inter_minor=0;
static int result;
static dev_t inter_dev;
static struct cdev inter_cdev;

int cnt=0;
int mode=0;
int end=0;
static int inter_read(struct file*filp,char *buf,size_t count,loff_t *f_pos);
static int inter_open(struct inode *, struct file *);
static int inter_release(struct inode *, struct file *);
static int inter_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void wake_app(unsigned long timeout);
void start_timer(unsigned long timeout);
void write_fnd(void);


struct struct_timer{
	struct timer_list timer;
	int seconds;
};

struct struct_timer mytime;

irqreturn_t inter_handler1(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler2(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler3(int irq, void* dev_id, struct pt_regs* reg);

static int inter_usage=0;
int interruptCount=0;

wait_queue_head_t wq_write;
DECLARE_WAIT_QUEUE_HEAD(wq_write);

static struct file_operations inter_fops =
{
	.open = inter_open,
	.write = inter_write,
	.read = inter_read,
	.release = inter_release,
};


void write_fnd(void)
{
	unsigned char value[4];
	unsigned short int value_short=0;

	if(mode==0)
	{
		value[0] = cnt/1000;
		value[1] = cnt/100;
		value[2] = cnt/10;
		value[3] = cnt%10;
	}
	else
	{
		value[0] = 0;
		value[1] = 0;
		value[2] = mytime.seconds/10;
		value[3] = mytime.seconds%10;
	}
	value_short = value[0] << 12 | value[1] <<8 | value[2] << 4 | value[3];
	outw(value_short,(unsigned int)fpga_fnd_addr);
}

void write_dot(void)
{
	int i;
	unsigned char value[10];
	unsigned short int value_short=0;
	int fnd_num;
	if(mode==0)
	{
		fnd_num = cnt%10;
		memcpy(value,fpga_number[fnd_num],sizeof(fpga_number[fnd_num]));
	}
	else
	{
		if(mytime.seconds<10)
		{
			fnd_num = mytime.seconds;
			memcpy(value,fpga_number[fnd_num],sizeof(fpga_number[fnd_num]));
		}
		else
			memcpy(value,fpga_set_blank,sizeof(fpga_set_blank));
	}
	if(end==1)
			memcpy(value,fpga_set_blank,sizeof(fpga_set_blank));
	for(i=0;i<10;i++)
	{
		value_short = value[i]&0x7f;
		outw(value_short,(unsigned int)fpga_dot_addr+i*2);
	}
}

void wake_app(unsigned long timeout)
{

	mytime.seconds=0;
	write_fnd();
	del_timer_sync(&mytime.timer);
		__wake_up(&wq_write,1,1,NULL);
}
void start_timer(unsigned long timeout)
{
	struct struct_timer *pdata = (struct struct_timer*)timeout;
	pdata->seconds--;
	write_fnd();
	write_dot();
	if(pdata->seconds==0)
		end=1;
	else
	{
		mytime.timer.expires = get_jiffies_64() + HZ;
		mytime.timer.data = (unsigned long)&mytime;
		mytime.timer.function = start_timer;
		add_timer(&mytime.timer);
	}
}
static int inter_read(struct file *filp,char *buf,size_t count,loff_t *f_ops)
{
	return end;
}
irqreturn_t inter_handler1(int irq, void* dev_id, struct pt_regs* reg) {
	printk(KERN_ALERT "interrupt1!!! = %x\n", gpio_get_value(IMX_GPIO_NR(1, 11)));
	
	if(mode==0)
		mode=1;
	else
		mode=0;
	mytime.seconds=0;
	write_fnd();
	mytime.seconds=61;
	write_dot();
	return IRQ_HANDLED;
}

irqreturn_t inter_handler2(int irq, void* dev_id,struct pt_regs* reg) {
        printk(KERN_ALERT "interrupt3!!! = %x\n", gpio_get_value(IMX_GPIO_NR(2, 15)));
	if(mode==0)
		return IRQ_HANDLED;
	if(mytime.seconds>60)
		mytime.seconds=0;
	mytime.seconds++;
	write_fnd();
  return IRQ_HANDLED;
}

irqreturn_t inter_handler3(int irq, void* dev_id, struct pt_regs* reg) {
        printk(KERN_ALERT "interrupt4!!! = %x\n", gpio_get_value(IMX_GPIO_NR(5, 14)));     
	if(mode==0)
		return IRQ_HANDLED;
	mytime.timer.expires = get_jiffies_64() + HZ;
	mytime.timer.data = (unsigned long)&mytime;
	mytime.timer.function = start_timer;
	add_timer(&mytime.timer);
	
	return IRQ_HANDLED;
}


static int inter_open(struct inode *minode, struct file *mfile){
	int ret;
	int irq;

	if(inter_usage!=0)
		return 0;
	end=0;
	printk(KERN_ALERT "Open Module\n");
	write_fnd();
	write_dot();
	// int1
	gpio_direction_input(IMX_GPIO_NR(1,11));
	irq = gpio_to_irq(IMX_GPIO_NR(1,11));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret=request_irq(irq,(void*) inter_handler1, IRQF_TRIGGER_FALLING, "home", 0);

	// int3
	gpio_direction_input(IMX_GPIO_NR(2,15));
	irq = gpio_to_irq(IMX_GPIO_NR(2,15));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret=request_irq(irq, (void*)inter_handler2, IRQF_TRIGGER_FALLING, "volup", 0);

	// int4
	gpio_direction_input(IMX_GPIO_NR(5,14));
	irq = gpio_to_irq(IMX_GPIO_NR(5,14));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret=request_irq(irq, (void*)inter_handler3, IRQF_TRIGGER_FALLING, "voldown", 0);

	inter_usage=1;

	return 0;
}

static int inter_release(struct inode *minode, struct file *mfile){
	free_irq(gpio_to_irq(IMX_GPIO_NR(1, 11)), NULL);
	free_irq(gpio_to_irq(IMX_GPIO_NR(2, 15)), NULL);
	free_irq(gpio_to_irq(IMX_GPIO_NR(5, 14)), NULL);
	inter_usage=0;
	cnt=0;
	write_fnd();
	end=1;
	write_dot();
	end=0;
	mode=0;
	printk(KERN_ALERT "Release Module\n");
	return 0;
}



static int inter_write(struct file *filp,const char *buf,size_t count,loff_t *f_pos ){
	if(mode==0)
	{
		cnt++;
		write_fnd();
		write_dot();
	}

	printk("write\n");
	return 0;
}

static int inter_register_cdev(void)
{
	int error;
	inter_dev = MKDEV(inter_major, inter_minor);
	error = register_chrdev_region(inter_dev,1,"inter");
	if(error<0) {
		printk(KERN_WARNING "inter: can't get major %d\n", inter_major);
		return result;
	}
	printk(KERN_ALERT "major number = %d\n", inter_major);
	cdev_init(&inter_cdev, &inter_fops);
	inter_cdev.owner = THIS_MODULE;
	inter_cdev.ops = &inter_fops;

	fpga_fnd_addr = ioremap(IOM_FND_ADDRESS,0x4);
	fpga_dot_addr = ioremap(IOM_DOT_ADDRESS,0x10);


	error = cdev_add(&inter_cdev, inter_dev, 1);

	mytime.seconds = 0;

	init_timer(&(mytime.timer));
	if(error)
	{
		printk(KERN_NOTICE "inter Register Error %d\n", error);
	}
	return 0;
}

static int __init inter_init(void) {
	int result;
	if((result = inter_register_cdev()) < 0 )
		return result;
	printk(KERN_ALERT "Init Module Success \n");
	printk(KERN_ALERT "Device : /dev/inter, Major Num : 242\n");
	return 0;
}

static void __exit inter_exit(void) {
	cdev_del(&inter_cdev);
	iounmap(fpga_fnd_addr);
	iounmap(fpga_dot_addr);

	unregister_chrdev_region(inter_dev, 1);
	printk(KERN_ALERT "Remove Module Success \n");
}

module_init(inter_init);
module_exit(inter_exit);
	MODULE_LICENSE("GPL");
