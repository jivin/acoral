#define IN_DIR
#include "../include/fs.h"

acoral_u8 CurrentDrive;

acoral_u32 GetDirClusIndex(acoral_char *Path);

/*********************************************************************************************************
** 函数名称: StrToFDTName
** 功能描述: 用户文件\目录名转换为系统名
**
** 输　入: Str:用户名称
**        
** 输　出: 驱动器号
**
** 全局变量: 无
** 调用模块: 无
********************************************************************************************************/
        acoral_u8 StrToFDTName(acoral_char *FDTName, acoral_char *Str)
{
    acoral_u8 i, temp;
    
    /* 文件\目录名是否有效 */
    if (Str[0] == 0x20 || Str[0] == 0)
    {
        return FILE_NAME_ERR; 
    }

    for (i = 11; i != 0; i--)
    {
        *FDTName++ = 0x20;
    }
    FDTName -= 11;
    

    /* 是否为"." */
    if (Str[0] == '.')
    if (Str[1] == 0 || Str[1] == '\\' || Str[1] == '/')
    {
        FDTName[0] = '.';
        return RETURN_OK;
    }

    /* 是否为".." */
    if (Str[0] == '.')
    if (Str[1] == '.' )
    if (Str[2] == 0 || Str[2] == '\\' || Str[2] == '/')
    {
        FDTName[0] = '.';
        FDTName[1] = '.';
        return RETURN_OK;
    }
    
    /* 获取主文件/目录名 */
    for (i = 8; i != 0; i--)
    {
        temp = *Str;
        if (temp == 0 || temp == '\\' || temp == '/')
        {
            break;
        }
        Str++;
        if (temp == '.')
        {
            break;
        }
        *FDTName++ = temp;
    }
    FDTName += i;
    
    /* 获取文件\目录扩展名 */
    for (i = 3; i != 0; i--)
    {
        temp = *Str;
        if (temp == 0 || temp == '\\' || temp == '/')
        {
            break;
        }
        Str++;
        *FDTName++ = temp;
    }
    return RETURN_OK;
}

/*********************************************************************************************************
** 函数名称: _GetFDTInfo
** 功能描述: 获取FDT所在的目录的开始簇号及系统内名称，内部使用
**
** 输　入: DirFileName:用户使用的FDT名（包括路径）
**         FDTName:用于返回系统使用的FDT名（不包括路径）
** 输　出: FDT所在的目录的开始簇号，BAD_CLUS表示找不到路径
**         
** 全局变量: 无
** 调用模块: GetDirClusIndex,StrToFDTName
********************************************************************************************************/
        acoral_u32 _GetFDTInfo(acoral_char *FDTName, acoral_char *DirFileName)
{
    acoral_u32 Rt;
    acoral_char *temp;
    acoral_u8 i;

    /* 获取字符串结束位置 */
    temp = DirFileName;
    while (*temp != 0)
    {
         temp++;
    }
    temp--;
    if (*temp == '\\' || *temp == '/')  /* 最后字符为\不是有效文件/目录名 */
    {
        return BAD_CLUS;
    }

    
    /* 获取目录开始簇号 */
    Rt = BAD_CLUS;
    while (1)
    {
        if (*temp == '\\' || *temp == '/' || *temp == ':')
        {
            /* 找到目录分割符号'\' 或 */
            /* 找到逻辑盘分割符号':'，表明是指定逻辑盘当前目录 */
            temp++;
            i = *temp;
            *temp = 0;
            Rt = GetDirClusIndex(DirFileName);
            *temp = i;
            break;
        }
        if (temp == DirFileName)
        {
            /* 只有文件\目录名，表明是当前逻辑盘当前目录 */
            Rt = GetDirClusIndex(".");
            break;
        }
        temp--;
    }
    /* 获取系统内文件\目录名 */
    if (StrToFDTName(FDTName, temp) != RETURN_OK)
    {
        Rt = BAD_CLUS;
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: GetDrive
** 功能描述: 获取指定目录的驱动器
**
** 输　入: Path:路径名
**        
** 输　出: 驱动器号
**
** 全局变量: 无
** 调用模块: 无
********************************************************************************************************/
        acoral_u8 GetDrive(const acoral_char *Path)
{
    acoral_u8 Drive;

    Drive = CurrentDrive;
    if (Path != NULL)
    {
//        strupr(Path);                           /* 变为大写 */
        if (Path[1] == ':')
        {
        	if(Path[0]>='A' && Path[0]<='Z')
            	Drive = Path[0] - 'A';
            else
            	Drive = Path[0] - 'a';
            	
        }
    }
    return Drive;
}

/*********************************************************************************************************
** 函数名称: ChangeDrive
** 功能描述: 改变当前逻辑盘
**
** 输　入: Drive:逻辑盘符字符串
**        
** 输　出: RETURN_OK:成功
**        NOT_FIND_DISK:逻辑盘不存在
**        PARAMETER_ERR:非法参数
** 全局变量: 无
** 调用模块: strupr,GetDiskInfo
********************************************************************************************************/
        acoral_u8 ChangeDrive(acoral_char *Drive)
{
    acoral_u8 Rt;
    Disk_Info *Disk;
    
    Rt = PARAMETER_ERR;
    strupr(Drive);                           /* 变为大写 */
    if (Drive[1] == ':')
    {
        Rt = NOT_FIND_DISK;
        Disk = GetDiskInfo(Drive[0] - 'A');
        if (Disk != NULL)
        {
            CurrentDrive = Drive[0] - 'A';
            Rt = RETURN_OK;
        }
    }
    return Rt;    
}

/*********************************************************************************************************
** 函数名称: GetDirClusIndex
** 功能描述: 获取指定目录开始簇号
**
** 输　入: Path:路径名
**        
** 输　出: 开始簇号，EMPTY_CLUS：为根目录
**
** 全局变量: 无
** 调用模块: strupr,GetDiskInfo,FindFDTInfo
********************************************************************************************************/
        acoral_u32 GetDirClusIndex(acoral_char *Path)
{
    acoral_char DirName[12];
    acoral_u8 Drive;
    acoral_u32 Rt;
    FDT temp;
    Disk_Info *Disk;
    
    Rt = BAD_CLUS;
    if (Path != NULL)
    {
        strupr(Path);                           /* 变为大写 */
        Drive = GetDrive(Path);                 /* 获取路基盘符 */
        if (Path[1] == ':')
        {
            Path += 2;
        }
        Disk = GetDiskInfo(Drive);              /* 获取逻辑盘信息 */
        if (Disk != NULL)
        {
            Rt = 0;
            if (Disk->FATType == FAT32)         /* FAT32 根目录 */
            {
                Rt = Disk->RootDirTable;
            }
            if (Path[0] != '\\' || Path[0] != '/')                /* 不是目录分隔符号，表明起点是当前路径 */
            {
                Rt = Disk->PathClusIndex;
            }
            else
            {
                Path++;
            }
            if (Path[0] == '.')             /* '\.'表明起点是当前路径 */
            {
                Rt = Disk->PathClusIndex;
                if (Path[1] == 0 || Path[1] == '\\' || Path[1] == '/')
                {
                    Path++;
                }
            }
            if (Path[0] == '\\' || Path[0] == '/')
            {
                Path++;
            }
            
            DirName[11] = 0;
            while (Path[0] != 0)
            {
                /* 获取子目录名 */
                StrToFDTName(DirName , Path);

                /* 子目录名开始簇号 */
                if (DirName[0] == 0x20)
                {
                    Rt = BAD_CLUS;
                    break;
                }
                /* 获取FDT信息 */
                if (FindFDTInfo(&temp, Drive, Rt, DirName) != RETURN_OK)
                {
                    Rt = BAD_CLUS;
                    break;
                }
                /* FDT是否是目录 */
                if ((temp.Attr & ATTR_DIRECTORY) == 0)
                {
                    Rt = BAD_CLUS;
                    break;
                }
                Rt = temp.FstClusLO + ((acoral_u32)(temp.FstClusHI) << 16);
                /* 字符串到下一个目录 */
                while (1)
                {
                    if (*Path == '\\' || *Path == '/')
                    {
                        Path++;
                        break;
                    }
                    if (*Path == 0)
                    {
                        break;
                    }
                    Path++;
                }
            }
        }
        if (Disk->FATType == FAT32)
        if (Rt != BAD_CLUS)
        if (Rt == Disk->RootDirTable)
        {
            Rt = 0;
        }
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: MakeDir
** 功能描述: 建立目录
**
** 输　入: Path:路径名
**
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: strupr,ClearClus,AddFDT
********************************************************************************************************/
        acoral_u8 MakeDir(acoral_char *Path)
{
    acoral_u32 ClusIndex, Temp1;
    acoral_u8 Drive, Rt;
    FDT temp;
    
    strupr(Path);                       /* 变为大写 */

    ClusIndex = _GetFDTInfo(temp.Name, Path);
    if (ClusIndex == BAD_CLUS)
    {
        return PATH_NOT_FIND;
    }

    Drive = GetDrive(Path);
    
    /* FDT是否存在 */
    Rt = FDTIsLie(Drive, ClusIndex, temp.Name);
    if (Rt != NOT_FIND_FDT)
    {
        return Rt;
    }
    
    /* 不存在 */
    Temp1 = FATAddClus(Drive, 0);               /* 获取目录所需磁盘空间 */
    if ((Temp1 <= EMPTY_CLUS_1) || (Temp1 >= BAD_CLUS))
    {
        /* 没有空闲空间 */
        return  DISK_FULL;
    }

    ClearClus(Drive, Temp1);                /* 清空簇 */
        
        /* 设置FDT属性 */
    temp.Attr = ATTR_DIRECTORY;             
    temp.FileSize = 0;

    temp.NTRes = 0;
    temp.CrtTimeTenth = 0;
    temp.CrtTime = 0;
    temp.CrtDate = 0;
    temp.LstAccDate = 0;
    temp.WrtTime = 0;
    temp.WrtDate = 0;

    temp.FstClusLO = Temp1 & 0xffff;
    temp.FstClusHI = Temp1 / 0x10000;

    Rt = AddFDT(Drive, ClusIndex, &temp);       /* 增加目录项 */
    if (Rt == RETURN_OK)
    {
        /* 建立'.'目录 */
        temp.Name[0] = '.';
        temp.Name[1] = 0x20;
        temp.Name[2] = 0x20;
        temp.Name[3] = 0x20;
        temp.Name[4] = 0x20;
        temp.Name[5] = 0x20;
        temp.Name[6] = 0x20;
        temp.Name[7] = 0x20;
        temp.Name[8] = 0x20;
        temp.Name[9] = 0x20;
        temp.Name[10] = 0x20;
        
        AddFDT(Drive, Temp1, &temp);

        /* 建立'..'目录 */
        temp.Name[1] = '.';
        
        temp.FstClusLO = ClusIndex & 0xffff;
        temp.FstClusHI = ClusIndex / 0x10000;
        Rt = AddFDT(Drive, Temp1, &temp);
    }
    else
    {
        FATDelClusChain(Drive, Temp1);
    }
    
    return Rt;
}

/*********************************************************************************************************
** 函数名称: RemoveDir
** 功能描述: 删除目录
**
** 输　入: Path:路径名
**
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: strupr,DelFDT
********************************************************************************************************/
        acoral_u8 RemoveDir(acoral_char *Path)
{
    acoral_u32 ClusIndex, ClusIndex1;
    acoral_u8 Drive, Rt;
    acoral_char DirName[12];
    FDT temp;
    
    strupr(Path);                       /* 变为大写 */

    ClusIndex = _GetFDTInfo(DirName, Path);
    if (ClusIndex == BAD_CLUS)
    {
        return PATH_NOT_FIND;
    }

    Drive = GetDrive(Path);
    
    /* 获取FDT其信息 */
    Rt = FindFDTInfo(&temp, Drive, ClusIndex, DirName);
    if (Rt == RETURN_OK)
    {
        /* 是否是目录 */
        if ((temp.Attr & ATTR_DIRECTORY) != 0)
        {
            /* 是 */
            ClusIndex1 = temp.FstClusLO + ((acoral_u32)(temp.FstClusHI) << 16);
            /* 是否是空目录 */
            Rt = DirIsEmpty(Drive, ClusIndex1);
            if (Rt == DIR_EMPTY)
            {
                /* 是，删除 */
                FATDelClusChain(Drive, ClusIndex1);
                Rt = DelFDT(Drive, ClusIndex, DirName);
            }
        }
        else
        {
            return PATH_NOT_FIND;
        }
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: ChangeDir
** 功能描述: 改变当前目录
**
** 输　入: Path:路径名
**
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: GetDirClusIndex,GetDiskInfo
********************************************************************************************************/
        acoral_u8 ChangeDir(acoral_char *Path)
{
    acoral_u32 ClusIndex;
    acoral_u8 Rt, Drive;
    Disk_Info *Disk;

    Rt = PATH_NOT_FIND;
    ClusIndex = GetDirClusIndex(Path);
    if (ClusIndex != BAD_CLUS)
    {
        Drive = GetDrive(Path);
        Disk = GetDiskInfo(Drive);
        Rt = NOT_FIND_DISK;
        if (Disk != NULL)
        {
            Rt = RETURN_OK;
            Disk->PathClusIndex = ClusIndex;
        }
    }
    return Rt;
}
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
