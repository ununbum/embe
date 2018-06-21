#include <jni.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "android/log.h"

#define dev_driver "/dev/dev_driver"

#define LOG_TAG "MyTag"
#define LOGV(...)   __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

jint JNICALL Java_com_example_androidex_MainActivity2_open_1fpga(JNIEnv * env, jobject this)
{
	int fd = open(dev_driver,O_RDWR);
	if(fd<0)
			return;

	LOGV("device file open");

	return fd;
}
jint JNICALL Java_com_example_androidex_MainActivity2_read_1fpga(JNIEnv * env, jobject this,jint fd)
{
	char buf[2];
	return read(fd,buf,2);
}
void JNICALL Java_com_example_androidex_MainActivity2_write_1fpga(JNIEnv * env, jobject this)
{
	int fd;
	int retn;
	char buf[256];
	fd = open(dev_driver,O_RDWR);

	retn = write(fd,buf,2);

}
jint JNICALL Java_com_example_androidex_MainActivity2_close_1fpga(JNIEnv * env, jobject this,jint fd)
{
	LOGV("device file close");
	return close(fd);
}




