/*===============================
 *        邮箱测试
 *       by pegasus
 *===============================*/
#include "acoral.h"
#include "os_file.h"
#include"../include/test.h"
acoral_u8 buf[100];
acoral_char name[]="\\lgui\\pics\\draw.bmp";
ACORAL_COMM_THREAD test()
{
	int i;
	int fd=acoral_open(name,O_RDONLY);
	if(fd<0)
		acoral_prints("open err\n");
	acoral_read(fd,buf,100);
	for(i=0;i<100;i++)
		acoral_print("%c",buf[i]);
}


void file_test()
{
    acoral_create_thread(test,ACORAL_PRINT_STACK_SIZE,NULL,NULL,20,1);
}

TEST_CALL(file_test)
