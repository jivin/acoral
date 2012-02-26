#include<type.h>
#include<driver.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
#include<unistd.h>
int disk_fd;
acoral_32 disk_write(void *data,acoral_size size,acoral_size offset,acoral_time tm_out){
	lseek(disk_fd,offset,SEEK_SET);
	if(write(disk_fd,data,size)!=size)
		return -1;
	return 0;
}

acoral_32 disk_read(void *data,acoral_size size,acoral_size offset,acoral_time tm_out){
	lseek(disk_fd,offset,SEEK_SET);
	if(read(disk_fd,data,size)!=size)
		return -1;
	return 0;
}

acoral_32 disk_open(){
	disk_fd=open("acoral.disk",O_RDWR);
	if(disk_fd<0)
		return -1;
	return 0;
}

acoral_32 disk_close(){
	close(disk_fd);
	return 0;
}

acoral_dev_ops_t disk_ops={
  	.write=disk_write,
  	.read=disk_read,
  	.open=disk_open,
  	.close=disk_close,
};

void disk_init(){
  	acoral_drv_register(&disk_ops,"disk");
}
