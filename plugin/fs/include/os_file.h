#ifndef OS_FILE_H
#define OS_FILE_H
#define O_RDONLY	0x4
#define O_WRONLY	0x2
#define O_RDWR		0x6

#ifndef IN_OSFILE

extern void acoral_fs_init(void);

extern acoral_32 acoral_unlink(acoral_char *pathname);

extern acoral_32 acoral_open(acoral_char *pathname,acoral_32 oflag);

extern acoral_32 acoral_close(acoral_32 fd);

//extern uint8 OSFileGetCh(uint8 *Ch, HANDLE Handle);
/*********************************************************************************************************
** 函数名称: OSFileGetCh
** 功能描述: 从文件读一个字节
**
** 输　入: Ch:返回读到的数据
**        Handle:文件句柄
** 输　出: RETURN_OK:成功
**        其它参考fat.h中关于返回值的说明 
** 全局变量: 
** 调用模块: OSQPost,OSTaskSuspend
********************************************************************************************************/

extern acoral_32 acoral_read(acoral_32 fd,void *buf,acoral_u32 nbytes);

//extern uint8 OSFilePutCh(uint8 Ch, HANDLE Handle);
/*********************************************************************************************************
** 函数名称: OSFilePutCh
** 功能描述: 写一个字节到文件
**
** 输　入: Ch:要写的数据
**        Handle:文件句柄
** 输　出: RETURN_OK:成功
**        其它参考fat.h中关于返回值的说明 
** 全局变量: 
** 调用模块: OSQPost,OSTaskSuspend
********************************************************************************************************/

extern acoral_32 acoral_write(acoral_32 fd,const void *buf,acoral_u32 nbytes);

extern acoral_32 acoral_closeall(void);

//extern uint8 OSFileEof(HANDLE Handle);
/*********************************************************************************************************
** 函数名称: OSFileEof
** 功能描述: 判断文件是否到读\写到文件尾
**
** 输　入: Handle:文件句柄
**
** 输　出: 0:否
**        1:是 
** 全局变量: 
** 调用模块: OSQPost,OSTaskSuspend
**
********************************************************************************************************/

extern acoral_u32 acoral_lseek(acoral_32 fd,acoral_32 offset,acoral_u8 whence);

extern acoral_32 acoral_mkdir(acoral_char *pathname,acoral_u8 mode);

extern acoral_32 acoral_rmdir(acoral_char *pathname);

extern acoral_32 acoral_chdir(acoral_char *pathname);


//extern uint8 OSChangeDrive(char *Drive);
/*********************************************************************************************************
** 函数名称: OSChangeDrive
** 功能描述: 改变当前逻辑盘
**
** 输　入: Drive:逻辑盘符字符串
**        
** 输　出: RETURN_OK:成功
**        NOT_FIND_DISK:逻辑盘不存在
**        PARAMETER_ERR:非法参数
** 全局变量: 
** 调用模块: OSQPost,OSTaskSuspend
**
** 作　者: 陈明计
** 日　期: 2003年9月8日
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

//extern uint8 OSGetFDTInfo(FDT *Rt,uint8 Drive, uint32 ClusIndex, uint32 Index);
/*********************************************************************************************************
** 函数名称: OSGetFDTInfo
** 功能描述: 获取指定目录指定文件（目录）信息
**
** 输　入: Rt：存储返回信息的指针
**        Drive：驱动器号
**        ClusIndex：目录首簇号
**        Index：文件（目录）在FDT中的位置
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 
** 调用模块: OSQPost,OSTaskSuspend
**
********************************************************************************************************/

extern acoral_32 acoral_mount(acoral_char *source,const char *target,
					   const acoral_char *filesystemtype,acoral_u32 mountflags,
					   const void *data);

extern acoral_32 acoral_umount(const char *target);
/*********************************************************************************************************
** 函数名称: OSRemoveFileDriver
** 功能描述: 删除一个底层驱动程序
**
** 输　入: Drive:逻辑驱动器号
**
** 输　出: 无
**         
** 全局变量: 
** 调用模块: OSQPost,OSTaskSuspend
**
********************************************************************************************************/

extern void acoral_flush(void);
//extern void OSAllCacheWriteBack(void);
/*********************************************************************************************************
** 函数名称: OSAllCacheWriteBack
** 功能描述: 把所有已改变的扇区写回逻辑盘
**
** 输　入: 无
**
** 输　出: NOT_RUN:没有执行
**        RETURN_OK:执行成功
**         
** 全局变量: 
** 调用模块: OSQPost,OSTaskSuspend
**
********************************************************************************************************/

//extern void OSFileTask(void *pdata);
/*********************************************************************************************************
** 函数名称: OSFileTask
** 功能描述: 文件系统服务任务
**
** 输　入: 无
**
** 输　出: 无
**         
** 全局变量: 
** 调用模块: OSQPost,OSTaskSuspend
**
********************************************************************************************************/
#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
#endif
