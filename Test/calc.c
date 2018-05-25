#include <linux/kernel.h>

asmlinkage int sys_calc(int a,int b){
	printk("%d + %d = %d\n",a,b,a+b);
	printk("%d - %d = %d\n",a,b,a-b);
	printk("%d * %d = %d\n",a,b,a*b);
	printk("%d / %d = %d\n",a,b,a/b);

	return 23;
}
