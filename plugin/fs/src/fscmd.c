#include "../include/fs.h"

/*********************************************************************************************************
** 函数名称: FSCommand
** 功能描述: 底层驱动程序与上层的接口程序
**
** 输　入: Cammand：DISK_INIT：驱动程序初始化
**                 DISK_CLOSE：关闭驱动器（移除驱动程序）                 
**                 DISK_CREATE_BOOT_SECTOR：重建引导扇区
**                 DISK_READ_SECTOR：读扇区
**                 DISK_WRITE_SECTOR：写扇区
**        Parameter：剩余参数
** 输　出: DISK_READ_OK：读扇区完成
**        DISK_READ_NOT_OK：读扇区失败
**        DISK_WRITE_OK：写扇区完成
**        DISK_WRITE_NOT_OK：写扇区失败
**        DISK_INIT_OK：初始化完成
**        DISK_INIT_NOT_OK：初始化失败
**        BAD_DISK_COMMAND：无效的命令
** 全局变量: 无
** 调用模块: 无
********************************************************************************************************/
        acoral_u16 FsCommand(acoral_u8 Cammand, void *Parameter)
{
    acoral_u16 rt;
    acoral_id dev_id;
    acoral_u8 drive;
    Disk_RW_Parameter * Dp;
    Disk_Info *DiskInfo1;
    
    
    Dp = (Disk_RW_Parameter *)Parameter;
    drive=Dp->Drive;
    switch (Cammand)
    {
        case DISK_INIT:										/*设备初始化*/
            rt = DISK_INIT_NOT_OK;
        	DiskInfo1 = &DiskInfo[drive];
        	if (DiskInfo1 != NULL)
        	{
				DiskInfo1->DiakCommand = FsCommand;
				dev_id=acoral_dev_open((acoral_char *)(Dp->Buf));
				if(dev_id!=ACORAL_DEV_ERR_ID)
				{
					DiskInfo1->DevId= dev_id;
					/*if(acoral_dev_config(dev_id,GET_DISK_INFO,NULL,0)==0)
					{
						if(SD_Initialize()==0);
							return DISK_INIT_OK;
					}
					else
						acoral_dev_close(dev_id);*/
					rt = DISK_INIT_OK;
				}
		/*	DiskInfo1->RsvdForLow = SDGetBlockSize();
			if(SD_Initialize()==SD_NO_ERR);*/
					
        	}
		
            break;
        	 
        case DISK_CLOSE:
			acoral_dev_close(DiskInfo[drive].DevId);
            break;
        case DISK_READ_SUPER_SECTOR:								/*读超级块扇区*/
	    	rt = DISK_READ_OK;
            if(acoral_dev_read(DiskInfo[drive].DevId,(void *)Dp->Buf,512,0,0)!=0)
            { 
                rt = DISK_READ_NOT_OK;
            }
            break;

        case DISK_READ_SECTOR:									/*读扇区*/
            rt = DISK_READ_OK;
            if(acoral_dev_read(
            	DiskInfo[drive].DevId,
            	(void *)Dp->Buf,
            	DiskInfo[drive].BytsPerSec,
            	Dp->SectorIndex*DiskInfo[drive].BytsPerSec,
            	0)!=0)
            { 
                rt = DISK_READ_NOT_OK;
            }
            break;
        case DISK_WRITE_SECTOR:									/*写扇区*/
            rt = DISK_WRITE_OK;
            if(acoral_dev_write(
            	DiskInfo[drive].DevId,
            	(void *)Dp->Buf,
            	DiskInfo[drive].BytsPerSec,
            	Dp->SectorIndex*DiskInfo[drive].BytsPerSec,
            	0)!=0)
            {
               rt = DISK_WRITE_NOT_OK;
            }

            break;
        default:
            rt = BAD_DISK_COMMAND;
            break;
    }
    return rt;
}


/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
