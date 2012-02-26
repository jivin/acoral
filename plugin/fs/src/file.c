#define IN_FILE
#include "../include/fs.h"

static MY_FILE FileInfo[MAX_OPEN_FILES];        /* 打开文件信息表 */

extern acoral_u32 _GetFDTInfo(acoral_char *FDTName, acoral_char *DirFileName);


/*********************************************************************************************************
** 函数名称: FileInit
** 功能描述: 初始化文件指针系统
**
** 输　入: 无
**
** 输　出: 无
**         
** 全局变量: 无
** 调用模块: 无
********************************************************************************************************/
        void FileInit(void)
{
    HANDLE i;
    
    for (i = 0; i < MAX_OPEN_FILES; i++)
    {
        FileInfo[i].Flags = 0;
    }
}

/*********************************************************************************************************
** 函数名称: FindOpenFile
** 功能描述: 查找已打开的指定文件的文件句柄，内部使用
**
** 输　入: Drive:文件所在的逻辑盘号
**        DirClus:文件所在目录的开始簇号
**        FileName:内部文件名
** 输　出: 文件句柄，Not_Open_FILE为没有打开
**         
** 全局变量: FileInfo
** 调用模块: _GetFDTInfo,GetDrive,FindFDTInfo,
********************************************************************************************************/
        HANDLE FindOpenFile(acoral_u8 Drive, acoral_u32 DirClus, acoral_char FileName[11])
{
    HANDLE Rt;
    MY_FILE *fp;
    
    fp = FileInfo;
    for (Rt = 0; Rt < MAX_OPEN_FILES; Rt++)
    {
        if (fp->Flags != 0)
        if (fp->Drive == Drive)
        if (fp->DirClus == DirClus)
        if (fp->Name[0] == FileName[0])
        if (fp->Name[1] == FileName[1])
        if (fp->Name[2] == FileName[2])
        if (fp->Name[3] == FileName[3])
        if (fp->Name[4] == FileName[4])
        if (fp->Name[5] == FileName[5])
        if (fp->Name[6] == FileName[6])
        if (fp->Name[7] == FileName[7])
        if (fp->Name[8] == FileName[8])
        if (fp->Name[9] == FileName[9])
        if (fp->Name[10] == FileName[10])
        {
            break;
        }
        fp++;
    }
    if (Rt >= MAX_OPEN_FILES)
    {
        Rt = Not_Open_FILE;
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: RemoveFile
** 功能描述: 删除文件
**
** 输　入: DirFileName:用户使用的文件名
**
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: strupr,_GetFDTInfo,GetDrive,FindFDTInfo,FATDelClusChain,DelFDT
********************************************************************************************************/
        acoral_u8 RemoveFile(acoral_char *DirFileName)
{
    acoral_u32 ClusIndex, ClusIndex1;
    acoral_u8 Drive, Rt;
    acoral_char DirName[12];
    FDT temp;
    
    strupr(DirFileName);                       /* 变为大写 */
    ClusIndex = _GetFDTInfo(DirName, DirFileName);
    Rt = PATH_NOT_FIND;
    if (ClusIndex != BAD_CLUS)
    {
        Drive = GetDrive(DirFileName);     /* 获取文件所在逻辑盘号 */
        Rt = FindFDTInfo(&temp, Drive, ClusIndex, DirName);
        if (Rt == RETURN_OK)
        {
            Rt = NOT_FIND_FILE;
            if ((temp.Attr & ATTR_DIRECTORY) == 0)  /* 是文件才删除 */
            {
                Rt = FILE_LOCK;
		/****************MODIFY BY GAOPAN*****************/
       //         if (FindOpenFile(Drive, ClusIndex, DirName) < MAX_OPEN_FILES)
                if (FindOpenFile(Drive, ClusIndex, DirName) == Not_Open_FILE)
		/****************MODIFY BY GAOPAN******END*******/

                {
                    /* 文件没有打开才删除 */
                    ClusIndex1 = temp.FstClusLO + ((acoral_u32)(temp.FstClusHI) << 16);
                    FATDelClusChain(Drive, ClusIndex1);
                    Rt = DelFDT(Drive, ClusIndex, DirName);
                }
            }
        }
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: FindOpenFileW
** 功能描述: 查找以读写方式打开的指定文件的文件句柄，内部使用
**
** 输　入: Drive:文件所在的逻辑盘号
**        DirClus:文件所在目录的开始簇号
**        FileName:内部文件名
** 输　出: 文件句柄，Not_Open_FILE为没有打开
**         
** 全局变量: FileInfo
** 调用模块: _GetFDTInfo,GetDrive,FindFDTInfo,
********************************************************************************************************/
        HANDLE FindOpenFileW(acoral_u8 Drive, acoral_u32 DirClus, acoral_char FileName[11])
{
    HANDLE Rt;
    MY_FILE *fp;
    
    fp = FileInfo;
    for (Rt = 0; Rt < MAX_OPEN_FILES; Rt++)
    {
        if ((fp->Flags & FILE_FLAGS_WRITE) != 0)
        if (fp->Drive == Drive)
        if (fp->DirClus == DirClus)
        if (fp->Name[0] == FileName[0])
        if (fp->Name[1] == FileName[1])
        if (fp->Name[2] == FileName[2])
        if (fp->Name[3] == FileName[3])
        if (fp->Name[4] == FileName[4])
        if (fp->Name[5] == FileName[5])
        if (fp->Name[6] == FileName[6])
        if (fp->Name[7] == FileName[7])
        if (fp->Name[8] == FileName[8])
        if (fp->Name[9] == FileName[9])
        if (fp->Name[10] == FileName[10])
        {
            break;
        }
        fp++;
    }
    if (Rt >= MAX_OPEN_FILES)
    {
        Rt = Not_Open_FILE;
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: _FileOpenR
** 功能描述: 只读方式打开文件，内部使用
**
** 输　入: DirFileName:用户使用的文件名
**
** 输　出: 文件句柄，Not_Open_FILE为不能打开
**         
** 全局变量: FileInfo
** 调用模块: _GetFDTInfo,GetDrive,FindFDTInfo,
********************************************************************************************************/
        HANDLE _FileOpenR(acoral_char *DirFileName)
{
    MY_FILE *fp;
    HANDLE Rt;
    FDT FileFDT;
    
    /* 查找空闲文件登记项 */
    for (Rt = 0; Rt < MAX_OPEN_FILES; Rt++)
    {
        if (FileInfo[Rt].Flags == 0)
        {
            break;
        }
    }
    
    if (Rt < MAX_OPEN_FILES)
    {
        fp = FileInfo + Rt;
        
        /* 获取目录开始簇号和文件名 */
        fp->DirClus = _GetFDTInfo(fp->Name, DirFileName);
        if (fp->DirClus < BAD_CLUS)
        {
            fp->Drive = GetDrive(DirFileName);     /* 获取文件所在逻辑盘号 */
            
            /* 获取文件信息 */
            if (FindFDTInfo(&FileFDT, fp->Drive, fp->DirClus, fp->Name) == RETURN_OK)
            {
                if ((FileFDT.Attr & ATTR_DIRECTORY) == 0)
                {
                    fp->Flags = FILE_FLAGS_READ;
                    fp->FileSize = FileFDT.FileSize;
                    fp->FstClus = FileFDT.FstClusLO | (acoral_u32)FileFDT.FstClusHI << 16;
                    fp->Clus = fp->FstClus;
                    fp->Offset = 0;
                    return Rt;
                }
            }
        }
    }
    return Not_Open_FILE;
}

/*********************************************************************************************************
** 函数名称: _FileOpenW
** 功能描述: 只写方式打开文件，内部使用
**
** 输　入: DirFileName:用户使用的文件名
**
** 输　出: 文件句柄，Not_Open_FILE为不能打开
**         
** 全局变量: FileInfo
** 调用模块: _GetFDTInfo,GetDrive,FindFDTInfo,
********************************************************************************************************/
        HANDLE _FileOpenW(acoral_char *DirFileName)
{
    MY_FILE *fp;
    FDT temp;
    HANDLE Rt;
    acoral_u8 i;

    for (Rt = 0; Rt < MAX_OPEN_FILES; Rt++)
    {
        if (FileInfo[Rt].Flags == 0)
        {
            break;
        }
    }
    
    if (Rt < MAX_OPEN_FILES)
    {
        fp = FileInfo + Rt;
        
        /* 获取目录开始簇号和文件名 */
        fp->DirClus = _GetFDTInfo(fp->Name, DirFileName);
        if (fp->DirClus < BAD_CLUS)
        {
            fp->Drive = GetDrive(DirFileName);     /* 获取文件所在逻辑盘号 */

            /* 文件已经以读写方式打开，不能再次以读写方式打开 */
            if (FindOpenFileW(fp->Drive, fp->DirClus, fp->Name) == Not_Open_FILE)
            {
                if (FindFDTInfo(&temp, fp->Drive, fp->DirClus, fp->Name) == RETURN_OK)
                {
                    if ((temp.Attr & ATTR_DIRECTORY) != 0)
                    {
                       return Not_Open_FILE;
                    }
                    if (RemoveFile(DirFileName) != RETURN_OK)   /* 删除文件 */
                    {
                       return Not_Open_FILE;
                    }
                }
                /* 创建文件 */
                for (i = 0; i < 11; i++)
                {
                    temp.Name[i] = fp->Name[i];
                }
                temp.Attr = 0;
                temp.FileSize = 0;

                temp.NTRes = 0;
                temp.CrtTimeTenth = 0;
                temp.CrtTime = 0;
                temp.CrtDate = 0;
                temp.LstAccDate = 0;
                temp.WrtTime = 0;
                temp.WrtDate = 0;

                temp.FstClusLO = 0;
                temp.FstClusHI = 0;
                if (AddFDT(fp->Drive, fp->DirClus, &temp) == RETURN_OK)       /* 增加文件 */
                {
                    /* 设置文件信息 */
                    fp->Flags = FILE_FLAGS_READ | FILE_FLAGS_WRITE;
                    fp->FileSize = 0;
                    fp->FstClus = 0;
                    fp->Clus = 0;
                    fp->Offset = 0;
                    return Rt;
                }
            }
        }
    }
    return Not_Open_FILE;
}

/*********************************************************************************************************
** 函数名称: _FileOpenRW
** 功能描述: 只读写方式打开文件，内部使用
**
** 输　入: DirFileName:用户使用的文件名
**
** 输　出: 文件句柄，Not_Open_FILE为不能打开
**         
** 全局变量: 无
** 调用模块: _FileOpenR,_FileOpenW
********************************************************************************************************/
        HANDLE _FileOpenRW(acoral_char *DirFileName)
{
    HANDLE Rt;
    
    Rt = _FileOpenR(DirFileName);
    if (Rt == Not_Open_FILE)
    {
        Rt = _FileOpenW(DirFileName);
    }
    else
    {
        if (FindOpenFileW(FileInfo[Rt].Drive, FileInfo[Rt].DirClus, FileInfo[Rt].Name) == Not_Open_FILE)
        {
            FileInfo[Rt].Flags |=  (FILE_FLAGS_WRITE | FILE_FLAGS_READ);
        }
        else
        {
            FileInfo[Rt].Flags = 0;
            Rt = Not_Open_FILE;
        }
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: FileOpen
** 功能描述: 以指定方式打开文件
**
** 输　入: DirFileName:用户使用的文件名
**        Type:打开方式
** 输　出: 文件句柄，Not_Open_FILE为不能打开
**         
** 全局变量: 无
** 调用模块: _FileOpenR,_FileOpenW,_FileOpenRW
********************************************************************************************************/
        HANDLE FileOpen(acoral_char *DirFileName, acoral_char *Type)
{
    HANDLE Rt;

    Rt = Not_Open_FILE;    
    strupr(Type);
    strupr(DirFileName);
    if (Type[0] == 'R')
    {
        if (Type[1] == 'W')
        {
            Rt = _FileOpenRW(DirFileName);
        }
        else
        {
            Rt = _FileOpenR(DirFileName);
        }
        return Rt;
    }
    if (Type[0] == 'W')
    {
        if (Type[1] == 'R')
        {
            Rt = _FileOpenRW(DirFileName);
        }
        else
        {
            Rt = _FileOpenW(DirFileName);
        }
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: FileClose
** 功能描述: 关闭指定文件
**
** 输　入: Handle:文件句柄
**
** 输　出: RETURN_OK:成功
**        其它参考fat.h中关于返回值的说明 
** 全局变量: 无
** 调用模块: 无
********************************************************************************************************/
        acoral_u8 FileClose(HANDLE Handle)
{
    acoral_u8 Rt;
    FDT FileFDT;
    MY_FILE *fp;
    
    Rt = PARAMETER_ERR;
    if (Handle >= 0 && Handle < MAX_OPEN_FILES)
    {
        Rt = RETURN_OK;
        fp = FileInfo + Handle;
        if ((fp->Flags & FILE_FLAGS_WRITE) == FILE_FLAGS_WRITE)
        {
            Rt = FindFDTInfo(&FileFDT, fp->Drive, fp->DirClus, fp->Name);
            if (Rt == RETURN_OK)
            if (FileFDT.FileSize < fp->FileSize)
            {
                FileFDT.FileSize = fp->FileSize;
                if (FileFDT.FstClusLO == 0)
                if (FileFDT.FstClusHI == 0)
                {
                    FileFDT.FstClusLO = fp->FstClus & 0xffff;
                    FileFDT.FstClusHI = (fp->FstClus >> 16) & 0xffff;
                }
                ChangeFDT(fp->Drive, fp->DirClus, &FileFDT);
            }
        }
        fp->Flags = 0;
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: FileGetCh
** 功能描述: 从文件读一个字节
**
** 输　入: Ch:返回读到的数据
**        Handle:文件句柄
** 输　出: RETURN_OK:成功
**        其它参考fat.h中关于返回值的说明 
** 全局变量: 无
** 调用模块: 无
********************************************************************************************************/
        acoral_u8 FileGetCh(acoral_u8 *Ch, HANDLE Handle)
{
    acoral_u32 i, j, SecIndex;
    MY_FILE *fp;
    Disk_Info * DiskInfo;
    acoral_u8 Rt;
    acoral_u8 *Buf;
    
    Rt = PARAMETER_ERR;
    fp = FileInfo + Handle;
    if (Handle >= 0 && Handle < MAX_OPEN_FILES)     /* Handle是否有效 */
    if (fp->Flags != 0)                             /* 对应的打开文件信息表是否已使用 */
    {
        Rt = FILE_EOF;
        if (fp->Offset >= fp->FileSize)
        {
            /* 如果读数据超出文件的长度，看一看谁否有别的程序正在写这个文件 */
            /* 如果有，则可能文件的长度有变化，更正之*/
            Handle = FindOpenFileW(fp->Drive, fp->DirClus, fp->Name);
            if (Handle !=  Not_Open_FILE)
            {
                fp->FileSize = FileInfo[Handle].FileSize;
                if (fp->Offset == 0)
                {
                    fp->FstClus = FileInfo[Handle].FstClus;
                    fp->Clus = FileInfo[Handle].Clus;
                }
            }
        }
        Rt = FILE_EOF;
        if (fp->Offset < fp->FileSize)              /* 是否到文件结束 */
        {
            Rt = NOT_FIND_DISK;
                /* 获取文件所在逻辑盘信息 */
            DiskInfo = GetDiskInfo(fp->Drive);
            if (DiskInfo != NULL)
            {
                /* 计算数据所在扇区 */
                j = fp->Offset % (DiskInfo->SecPerClus * DiskInfo->BytsPerSec);
                i = j / DiskInfo->BytsPerSec;
                j = j % DiskInfo->BytsPerSec;
                SecIndex = (fp->Clus - 2) * DiskInfo->SecPerClus + 
                           DiskInfo->DataStartSec + i;
                Rt = SECTOR_READ_ERR;
                /* 打开扇区 */
                Buf = OpenSec(fp->Drive, SecIndex);
                if (Buf != NULL)
                {
                    /* 读取扇区数据 */
                    if (ReadSec(fp->Drive, SecIndex) == RETURN_OK)
                    {
                        /* 读取数据 */
                        *Ch = Buf[j];
                        /* 调整文件指针 */
                        fp->Offset++;
                        j++;
                        if (j >= DiskInfo->BytsPerSec)
                        {
                            i++;
                            if (i >= DiskInfo->SecPerClus)
                            {
                                fp->Clus = FATGetNextClus(fp->Drive, fp->Clus);
                            }
                        }
                        Rt = RETURN_OK;
                    }
                    /* 关闭扇区 */
                    CloseSec(fp->Drive, SecIndex);
                }
            }
        }
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: FileRead
** 功能描述: 读取文件
**
** 输　入: Buf:保存读回的数据
**        Size:要读的字节数
**        Handle:文件句柄
** 输　出: 实际读到的字节数
**         
** 全局变量: 无
** 调用模块: 无
********************************************************************************************************/
 /*       acoral_32 FileRead(void *Buf, acoral_u32 Size, HANDLE Handle)
{
    acoral_u8 *temp;
    acoral_u32 i;
    
    temp = (acoral_u8 *)Buf;
    for (i = 0; i < Size; i++)
    {
        if (FileGetCh(temp, Handle) != RETURN_OK)
        {
            break;
        }
        temp++;
    }
    return i;
}*/
	/***************re-write by gaopan*******************/
	acoral_32 FileRead(void *Buf_out, acoral_u32 Size, HANDLE Handle)
{
	acoral_32 Rt;
	MY_FILE * fp;
	acoral_u32 i,j,k,SecIndex,RealSize,FirstSize;
	Disk_Info *DiskInfo;
	acoral_u8 *SecBuf;
	acoral_u8 *Buf=(acoral_u8 *)Buf_out;

	Rt = -1;
	RealSize=Size;
	fp = FileInfo + Handle;
	if(Buf!=NULL)
	if(Handle >= 0 && Handle < MAX_OPEN_FILES)
	if(fp->Flags != 0 )
	{
		Rt = 0;
		if(fp->Offset+Size >= fp->FileSize)
		{
			Handle = FindOpenFileW(fp->Drive,fp->DirClus,fp->Name);
			if(Handle !=  Not_Open_FILE)
			{
				fp->FileSize = FileInfo[Handle].FileSize;
				if(fp->Offset == 0)
				{
					fp->FstClus = FileInfo[Handle].FstClus;
					fp->Clus = FileInfo[Handle].FstClus;
				}
			}
		}
		if(fp->Offset < fp->FileSize)
		{
			if(fp->Offset+Size >= fp->FileSize)
				RealSize=fp->FileSize - fp->Offset;
			Rt = RealSize;
//			Rt=NOT_FIND_DISK;
			DiskInfo = GetDiskInfo(fp->Drive);
			if(DiskInfo != NULL)
			{
				j=fp->Offset % (DiskInfo->SecPerClus * DiskInfo->BytsPerSec);
				i=j / DiskInfo->BytsPerSec;
				j=j % DiskInfo->BytsPerSec;
				SecIndex = (fp->Clus-2) * DiskInfo->SecPerClus+DiskInfo->DataStartSec+i;
//				Rt = SECTOR_READ_ERR;
				//处理第一个扇区
				SecBuf = OpenSec(fp->Drive,SecIndex);
				if(SecBuf == NULL)
				{
					return -1;
				}
				
				if(ReadSec(fp->Drive,SecIndex) == RETURN_OK)
				{
					FirstSize = ((DiskInfo->BytsPerSec-j)<RealSize) ? (DiskInfo->BytsPerSec-j):RealSize;
					for(k=FirstSize;k>0;k--)
					{
						*Buf++ = SecBuf[j];
						j++;
					}
					fp->Offset = fp->Offset + FirstSize;
					RealSize = RealSize - FirstSize;
				}
				else
					return -1;
				
				CloseSec(fp->Drive,SecIndex);
				if(j>=DiskInfo->BytsPerSec)
				{
					i++;
					if(i>=DiskInfo->SecPerClus)
					{
						i=0;
						fp->Clus = FATGetNextClus(fp->Drive,fp->Clus);
					}
				}

				//处理中间扇区
				for(;RealSize >= DiskInfo->BytsPerSec;RealSize -= DiskInfo->BytsPerSec)
				{
					SecIndex=(fp->Clus-2) * DiskInfo->SecPerClus+DiskInfo->DataStartSec+i;
					if((SecBuf = OpenSec(fp->Drive,SecIndex))==NULL)
						return -1;
					if(ReadSec(fp->Drive,SecIndex) != RETURN_OK)
						return -1;
					for(k=0;k<DiskInfo->BytsPerSec;k+=4)
					{
						Buf[0] = SecBuf[k];
						Buf[1] = SecBuf[k+1];
						Buf[2] = SecBuf[k+2];
						Buf[3] = SecBuf[k+3];
						Buf += 4;
					}
					CloseSec(fp->Drive,SecIndex);
					fp->Offset = fp->Offset+DiskInfo->BytsPerSec;
					i++;
					if(i>=DiskInfo->SecPerClus)
					{
						i=0;
						fp->Clus = FATGetNextClus(fp->Drive,fp->Clus);
					}
					
				}

				//处理最后一个扇区
				if(RealSize>0)
				{
					SecIndex=(fp->Clus-2) * DiskInfo->SecPerClus+DiskInfo->DataStartSec+i;
					if((SecBuf = OpenSec(fp->Drive,SecIndex))==NULL)
						return -1;
					if(ReadSec(fp->Drive,SecIndex) != RETURN_OK)
						return -1;
					for(k=0;k<RealSize;k++)
						*Buf++ = SecBuf[k];
					CloseSec(fp->Drive,SecIndex);
					fp->Offset = fp->Offset+RealSize;					
				}
			}
			else
				return -1;
		}
	}
	return Rt;
}
/*********************************************************************************************************
** 函数名称: FilePutCh
** 功能描述: 写一个字节到文件
**
** 输　入: Ch:要写的数据
**        Handle:文件句柄
** 输　出: RETURN_OK:成功
**        其它参考fat.h中关于返回值的说明 
** 全局变量: 无
** 调用模块: 无
********************************************************************************************************/
        acoral_u8 FilePutCh(acoral_u8 Ch, HANDLE Handle)
{
    acoral_u32 i, j, SecIndex;
    MY_FILE *fp;
    Disk_Info * DiskInfo;
    acoral_u8 Rt;
    acoral_u8 *Buf;

    Rt = PARAMETER_ERR;
    fp = FileInfo + Handle;
    if (Handle >= 0 && Handle < MAX_OPEN_FILES)      /* Handle是否有效 */
    if (((fp->Flags) & FILE_FLAGS_WRITE) != 0)       /* 对应的打开文件信息表是否已使用 */
    {
        Rt = FILE_EOF;
        if (fp->Offset <= fp->FileSize)
        {
            Rt = NOT_FIND_DISK;
                /* 获取文件所在逻辑盘信息 */
            DiskInfo = GetDiskInfo(fp->Drive);
            if (DiskInfo != NULL)
            {
                
                /* 调整文件指针 */
                j = (fp->Offset) % (DiskInfo->SecPerClus * DiskInfo->BytsPerSec); 
                
                if (j == 0)
                {
                    if (fp->Offset < fp->FileSize)
                    {
                        fp->Clus = FATGetNextClus(fp->Drive, fp->Clus);
                    }
                    else
                    {
                        i = FATAddClus(fp->Drive, fp->Clus);
                        if (i >= BAD_CLUS)
                        {
                            return DISK_FULL;
                        }
                        fp->Clus = i;
                        if (fp->FstClus == EMPTY_CLUS)
                        {
                            fp->FstClus = i;
                        }
                    }
                }
                
                /* 计算数据所在扇区 */
                i = j / DiskInfo->BytsPerSec;
                j = j % DiskInfo->BytsPerSec;
                SecIndex = (fp->Clus - 2) * DiskInfo->SecPerClus + DiskInfo->DataStartSec + i;
                Rt = SECTOR_READ_ERR;
                
                /* 调整打开文件信息表 */
                fp->Offset++;
                if (fp->Offset > fp->FileSize)
                {
                    fp->FileSize = fp->Offset;
                }

                /* 打开扇区 */
                Buf = OpenSec(fp->Drive, SecIndex);
                if (Buf != NULL)
                {
                    /* 写扇区数据 */
                    if (ReadSec(fp->Drive, SecIndex) == RETURN_OK)
                    {
                        /* 存储数据 */
                        Buf[j] = Ch;
                        WriteSec(fp->Drive, SecIndex);
                        Rt = RETURN_OK;
                    }
                    CloseSec(fp->Drive, SecIndex);
                }
            }
        }
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: FileWrite
** 功能描述: 写文件
**
** 输　入: Buf:要写的数据
**        Size:要写的字节数
**        Handle:文件句柄
** 输　出: 实际写的字节数
**         
** 全局变量: 无
** 调用模块: 无
********************************************************************************************************/
/*        acoral_32 FileWrite(void *Buf, acoral_u32 Size, HANDLE Handle)
{
    acoral_u8 *temp;
    acoral_u32 i;
    
    temp = (acoral_u8 *)Buf;
    for (i = 0; i < Size; i++)
    {
        if (FilePutCh(*temp, Handle) != RETURN_OK)
        {
            break;
        }
        temp++;
    }
    return i;
}*/
	acoral_32 FileWrite(const void *Buf_in,acoral_u32 Size,HANDLE Handle)
{
	acoral_u8 *Buf=(acoral_u8 *)Buf_in;
	MY_FILE *fp;
	acoral_u8 *SecBuf=NULL;
	Disk_Info *DiskInfo;
	acoral_u32 i,j,k,SecIndex,FirstSize,RealSize;

	fp=FileInfo+Handle;
	RealSize=Size;

	if(Buf_in==NULL||Handle<0||Handle>=MAX_OPEN_FILES)
		return -1;
	if(fp->Offset>fp->FileSize || (((fp->Flags)&FILE_FLAGS_WRITE)==0))
		return -1;
	if(Size==0)
		return 0;

	if((DiskInfo=GetDiskInfo(fp->Drive))==NULL)
		return -1;
	j=fp->Offset%(DiskInfo->SecPerClus * DiskInfo->BytsPerSec);
	if(j==0)
	{
		if(fp->Offset<fp->FileSize)
			fp->Clus=FATGetNextClus(fp->Drive,fp->Clus);
		else
		{
			i=FATAddClus(fp->Drive,fp->Clus);
			if(i>=BAD_CLUS)
				return -1;
			fp->Clus = i;
			if(fp->FstClus==EMPTY_CLUS)
				fp->FstClus=i;
		}
	}
	i=j/DiskInfo->BytsPerSec;
	j=j%DiskInfo->BytsPerSec;
	SecIndex=(fp->Clus-2)*DiskInfo->SecPerClus+DiskInfo->DataStartSec+i;
	//处理第一个扇区
	SecBuf = (acoral_u8 *)OpenSec(fp->Drive,SecIndex);
	if(SecBuf == NULL)
		return -1;
	if(ReadSec(fp->Drive,SecIndex) != RETURN_OK)
		return -1;
	FirstSize=(DiskInfo->BytsPerSec-j)<Size?(DiskInfo->BytsPerSec-j):Size;
	for(k=0;k<FirstSize;k++)
	{
		SecBuf[j]=*Buf++;
		j++;
	}
	WriteSec(fp->Drive,SecIndex);
	CloseSec(fp->Drive,SecIndex);
	RealSize=RealSize-FirstSize;
	fp->Offset=fp->Offset+FirstSize;
	if(fp->Offset>fp->FileSize)
		fp->FileSize=fp->Offset;
	//处理中间的扇区
	for(;RealSize>=DiskInfo->BytsPerSec;RealSize-=DiskInfo->BytsPerSec)
	{
		i++;
		if(i>=DiskInfo->SecPerClus)
		{
			i=0;
			if(fp->Offset<fp->FileSize)
				fp->Clus=FATGetNextClus(fp->Drive,fp->Clus);
			else
			{
				k=FATAddClus(fp->Drive,fp->Clus);
				if(k>=BAD_CLUS)
					return (Size-RealSize);
				fp->Clus=k;
			}
		}		
		SecIndex=(fp->Clus-2)*DiskInfo->SecPerClus+DiskInfo->DataStartSec+i;
		if((SecBuf=OpenSec(fp->Drive,SecIndex))==NULL)
			return (Size-RealSize);
		if((ReadSec(fp->Drive,SecIndex))!=RETURN_OK)
			return (Size-RealSize);
		for(k=0;k<DiskInfo->BytsPerSec;k+=4)
		{
			SecBuf[k]=Buf[0];
			SecBuf[k+1]=Buf[1];
			SecBuf[k+2]=Buf[2];
			SecBuf[k+3]=Buf[3];
			Buf+=4;
				
		}
		WriteSec(fp->Drive,SecIndex);
		CloseSec(fp->Drive,SecIndex);
		fp->Offset=fp->Offset+DiskInfo->BytsPerSec;
		if(fp->Offset>fp->FileSize)
			fp->FileSize=fp->Offset;
	}

	//处理最后的一个扇区
	if(RealSize>0)
	{
		i++;
		if(i>=DiskInfo->SecPerClus)
		{
			i=0;
			if(fp->Offset<fp->FileSize)
				fp->Clus=FATGetNextClus(fp->Drive,fp->Clus);
			else
			{
				k=FATAddClus(fp->Drive,fp->Clus);
				if(k>=BAD_CLUS)
					return (Size-RealSize);
				fp->Clus=k;
			}
		}		
		SecIndex=(fp->Clus-2)*DiskInfo->SecPerClus+DiskInfo->DataStartSec+i;
		if((SecBuf=OpenSec(fp->Drive,SecIndex))==NULL)
			return (Size-RealSize);
		if((ReadSec(fp->Drive,SecIndex))!=RETURN_OK)
			return (Size-RealSize);
		for(k=0;k<RealSize;k++)
		{
			SecBuf[k]=*Buf++;
				
		}
		WriteSec(fp->Drive,SecIndex);
		CloseSec(fp->Drive,SecIndex);
		fp->Offset=fp->Offset+RealSize;
		RealSize=0;
		if(fp->Offset>fp->FileSize)
			fp->FileSize=fp->Offset;		
	}
	return (Size-RealSize);
}


/*********************************************************************************************************
** 函数名称: FileCloseAll
** 功能描述: 关闭所有打开的文件
**
** 输　入: 无
**
** 输　出: 无
**         
** 全局变量: FileInfo
** 调用模块: AllCacheWriteBack
********************************************************************************************************/
        void FileCloseAll(void)
{
    acoral_u32 i;

    for (i = 0; i < MAX_OPEN_FILES; i++)
    {
        FileClose(i);
    }
    AllCacheWriteBack();
}

/*********************************************************************************************************
** 函数名称: FileEof
** 功能描述: 判断文件是否到读\写到文件尾
**
** 输　入: Handle:文件句柄
**
** 输　出: 0:否
**        1:是 
** 全局变量: FileInfo
** 调用模块: 无
********************************************************************************************************/
        acoral_u8 FileEof(HANDLE Handle)
{
    if (Handle < MAX_OPEN_FILES)
    if (FileInfo[Handle].Offset < FileInfo[Handle].FileSize)
    {
        return FALSE;
    }
    return TRUE;
}

/*********************************************************************************************************
** 函数名称: FileSeek
** 功能描述: 移动文件读\写位置
**
** 输　入: Handle:文件句柄
**        offset:移动偏移量
**        Whence:移动模式SEEK_SET:从文件头计算SEEK_CUR:从当前位置计算SEEK_END:从文件尾计算
** 输　出: 无
**         
** 全局变量: 无
** 调用模块: 无
********************************************************************************************************/
        acoral_u32 FileSeek(HANDLE Handle, acoral_32 offset, acoral_u8 Whence)
{
    acoral_u32 Rt;
    acoral_u32 i, Clus;
    MY_FILE *fp;
    Disk_Info * DiskInfo;
    
    Rt = PARAMETER_ERR;
    fp = FileInfo + Handle;
    if (Handle >= 0 && Handle < MAX_OPEN_FILES)     /* Handle是否有效 */
    if (fp->Flags  != 0)                            /* 对应的打开文件信息表是否已使用 */
    {
        Rt = RETURN_OK;
        switch (Whence)
        {
            case SEEK_END:                          /* 从文件尾计算 */
                fp->Offset = fp->FileSize + offset;
//                offset = -offset;
                break;
            case SEEK_SET:
                fp->Offset = offset;
                break;
            case SEEK_CUR:                          /* 从当前位置计算 */
                i = fp->Offset + offset;
                break;
            default:
                Rt = PARAMETER_ERR;
                break;
        }
        if (Rt == RETURN_OK)
        {
            if (fp->Offset > fp->FileSize)
	fp->Offset=fp->FileSize;
           if(fp->Offset<0)
    	fp->Offset=0;
            /* 改变当前簇号 */
            Rt = NOT_FIND_DISK;
            DiskInfo = GetDiskInfo(fp->Drive);
            if (DiskInfo != NULL)
            {
                Rt = RETURN_OK;
                i = fp->Offset / (DiskInfo->BytsPerSec * DiskInfo->SecPerClus);
                Clus = fp->FstClus;
                for (; i != 0; i--)
                {
                    Clus = FATGetNextClus(fp->Drive, Clus);
                    if (Clus >= BAD_CLUS)
                    {
                        Rt = FAT_ERR;
                        break;
                    }
                }
                fp->Clus = Clus;
            }
        }
    }
    if(Rt!=RETURN_OK)
    	return -1;
    else
    	return fp->Offset;
}
/*********************************************************************************************************
**                            FS需使用的函数 
********************************************************************************************************/

 acoral_char  *strupr(acoral_char *Str)					
{    acoral_char *cp;
     cp = Str;
    while (*cp != 0)
    {   if (*cp >= 'a' && *cp <= 'z' )
            *cp -=  'a' - 'A';
       cp++;
    }
    return Str;
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
