#include <linux/ioctl.h>

#define IOM_dev_MAJOR 242 // ioboard fpga device major number
#define IOM_dev_NAME "dev_driver"


#define IOCTL_FND _IOW(IOM_dev_MAJOR,0,int)
#define IOCTL_DOT _IOW(IOM_dev_MAJOR,1,int)
#define IOCTL_LED _IOW(IOM_dev_MAJOR,2,int)
#define IOCTL_TEXT_LCD _IOW(IOM_dev_MAJOR,3,int)
#define IOCTL_TIMER _IOW(IOM_dev_MAJOR,4,int)

