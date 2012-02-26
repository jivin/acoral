#include"acoral.h"
acoral_dev_ops_t sd_ops;
acoral_32 sd_write_fat_sector(void *data,acoral_size size,acoral_size offset,acoral_time tm_out){
	acoral_u32 sdblock_num=size/512;
	acoral_u32 sdblock_index=offset/512;
	acoral_32 ret;
	acoral_u8 *write_data=(acoral_u8 *)data;
	for(;sdblock_num>0;sdblock_num--)
	{
		ret=SD_WriteBlock(sdblock_index, write_data);
		sdblock_index++;
		write_data=write_data+512;
	}
	return ret;
}

acoral_32 sd_read_fat_sector(void *data,acoral_size size,acoral_size offset,acoral_time tm_out){
	acoral_u32 sdblock_num=size/512;
	acoral_u32 sdblock_index=offset/512;
	acoral_32 ret;
	acoral_u8 *read_data=(acoral_u8 *)data;	
	for(;sdblock_num>0;sdblock_num--)
	{
		ret=SD_ReadBlock(sdblock_index, read_data);
		sdblock_index++;
		read_data=read_data+512;
	}
	return ret;
}

void sd_open(){
	SD_Initialize();
}

void sd_init(){
	sd_ops.write=sd_write_fat_sector;
	sd_ops.read=sd_read_fat_sector;
	sd_ops.open=sd_open;
	sd_ops.config=NULL;
	sd_ops.close=NULL;
  	acoral_drv_register(&sd_ops,"disk");
}
