#ifndef FS_CMD_H
#define FS_CMD_H
extern acoral_u16 FsCommand(acoral_u8 Command, void *Parameter);
/*********************************************************************************************************
** 函数名称: SDMMCCammand
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
#endif
