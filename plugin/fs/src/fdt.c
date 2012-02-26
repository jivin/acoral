#define IN_FDT
#include "../include/fs.h"

/*********************************************************************************************************
** 函数名称: ClearClus
** 功能描述: 将指定簇所有数据清零
**
** 输　入: Disk：逻辑盘信息
**        Index：簇号
** 输　出: RETURN_OK:成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: GetDiskInfo,OpenSec,WriteSec,CloseSec
********************************************************************************************************/
        acoral_u8 ClearClus(acoral_u8 Drive, acoral_u32 Index)
{
    acoral_u8 i, temp;
    acoral_u8 *Buf;
    acoral_u32 SecIndex;
    acoral_u16 j;
    Disk_Info *Disk;

    Disk = GetDiskInfo(Drive);
    if (Disk != NULL)
    {
        if (Index < Disk->ClusPerData)
        {
            temp = Disk->SecPerClus;
            Index -= 2;                     /* 2：保留簇数 */
            SecIndex = Disk->DataStartSec + Index * temp;
            for (i = 0; i < temp; i++)
            {
                Buf=OpenSec(Drive, SecIndex);
                if(Buf!=NULL)  //扇区可能已经在缓冲区，所以要清零
                {
                	for(j=0;j<Disk->BytsPerSec;j++)
                		Buf[j]=0;
                }
                WriteSec(Drive, SecIndex);
                CloseSec(Drive, SecIndex);
                SecIndex++;
            }
            return RETURN_OK;
        }
        else
        {
            return CLUSTER_NOT_IN_DISK;
        }
    }
    else
    {
        return NOT_FIND_DISK;
    }
}

/*********************************************************************************************************
** 函数名称: ReadFDTInfo
** 功能描述: 读取FDT信息
**
** 输　入: Rt：存储返回信息的指针
**        Drive：驱动器号
**        SecIndex：扇区号
**        ByteIndex：偏移量
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: OpenSec,ReadSec,CloseSec
********************************************************************************************************/
        acoral_u8  ReadFDTInfo(FDT *Rt, acoral_u8 Drive, acoral_u32 SecIndex, acoral_u16 ByteIndex)
{
    acoral_u8 *Buf;
    acoral_u8 temp;
    
    Buf = OpenSec(Drive, SecIndex);
    temp = NOT_EMPTY_CACHE;
    if (Buf != NULL)
    {

        temp = ReadSec(Drive, SecIndex);
        if (temp == RETURN_OK)
        {
            Buf = Buf + ByteIndex;
            Rt->Name[0] = *Buf++;
            Rt->Name[1] = *Buf++;
            Rt->Name[2] = *Buf++;
            Rt->Name[3] = *Buf++;
            Rt->Name[4] = *Buf++;
            Rt->Name[5] = *Buf++;
            Rt->Name[6] = *Buf++;
            Rt->Name[7] = *Buf++;
            Rt->Name[8] = *Buf++;
            Rt->Name[9] = *Buf++;
            Rt->Name[10] = *Buf++;
            Rt->Attr = *Buf++;
            Rt->NTRes = *Buf++;
            Rt->CrtTimeTenth = *Buf++;
            Rt->CrtTime = Buf[0] | (Buf[1] << 8);
            Rt->CrtDate = Buf[2] | (Buf[3] << 8);
            Rt->LstAccDate = Buf[4] | (Buf[5] << 8);
            Rt->FstClusHI = Buf[6] | (Buf[7] << 8);
            Rt->WrtTime = Buf[8] | (Buf[9] << 8);
            Rt->WrtDate = Buf[10] | (Buf[11] << 8);
            Rt->FstClusLO = Buf[12] | (Buf[13] << 8);
            Rt->FileSize = Buf[14]  | (Buf[15] << 8);
            Rt->FileSize |= (((acoral_u32)Buf[16] << 16) | ((acoral_u32)Buf[17] << 24));
            temp = RETURN_OK;
        }
        CloseSec(Drive, SecIndex);
    }
    return temp;
}

/*********************************************************************************************************
** 函数名称: WriteFDTInfo
** 功能描述: 写FDT信息
**
** 输　入:
**        Drive：驱动器号
**        SecIndex：扇区号
**        ByteIndex：偏移量
**        FDT *FDTData:数据
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: OpenSec,ReadSec,CloseSec
********************************************************************************************************/
        acoral_u8  WriteFDTInfo(acoral_u8 Drive, acoral_u32 SecIndex, acoral_u16 ByteIndex, FDT *FDTData)
{
    acoral_u8 *Buf;
    acoral_u8 temp;
    
    temp = NOT_EMPTY_CACHE;
    Buf = OpenSec(Drive, SecIndex);
    if (Buf != NULL)
    {
        temp = ReadSec(Drive, SecIndex);
        if (temp == RETURN_OK)
        {
            Buf = Buf + ByteIndex;
            *Buf++ = FDTData->Name[0];
            *Buf++ = FDTData->Name[1];
            *Buf++ = FDTData->Name[2];
            *Buf++ = FDTData->Name[3];
            *Buf++ = FDTData->Name[4];
            *Buf++ = FDTData->Name[5];
            *Buf++ = FDTData->Name[6];
            *Buf++ = FDTData->Name[7];
            *Buf++ = FDTData->Name[8];
            *Buf++ = FDTData->Name[9];
            *Buf++ = FDTData->Name[10];
            *Buf++ = FDTData->Attr;
            *Buf++ = FDTData->NTRes;
            *Buf++ = FDTData->CrtTimeTenth;
            *Buf++ = FDTData->CrtTime;
            *Buf++ = (FDTData->CrtTime) >> 8;
            *Buf++ = FDTData->CrtDate;
            *Buf++ = (FDTData->CrtDate) >> 8;
            *Buf++ = FDTData->LstAccDate;
            *Buf++ = (FDTData->LstAccDate) >> 8;
            *Buf++ = FDTData->FstClusHI;
            *Buf++ = (FDTData->FstClusHI) >> 8;
            *Buf++ = FDTData->WrtTime;
            *Buf++ = (FDTData->WrtTime) >> 8;
            *Buf++ = FDTData->WrtDate;
            *Buf++ = (FDTData->WrtDate) >> 8;
            *Buf++ = FDTData->FstClusLO;
            *Buf++ = (FDTData->FstClusLO) >> 8;
            *Buf++ = (FDTData->FileSize);
            *Buf++ = (FDTData->FileSize) >> 8;
            *Buf++ = (FDTData->FileSize) >> 16;
            *Buf = (FDTData->FileSize) >> 24;
            WriteSec(Drive, SecIndex);
        }
        CloseSec(Drive, SecIndex);
    }
    return temp;
}

/*********************************************************************************************************
** 函数名称: GetRootFDTInfo
** 功能描述: 获取根目录指定文件（目录）信息
**
** 输　入: Rt：存储返回信息的指针
**        Drive：驱动器号
**        Index：文件（目录）在FDT中的位置
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: GetDiskInfo,GetFDTInfo,ReadFDTInfo
********************************************************************************************************/
        acoral_u8 GetRootFDTInfo(FDT *Rt, acoral_u8 Drive, acoral_u32 Index)
{
    Disk_Info * Disk;
    acoral_u16 ByteIndex;
    acoral_u32 SecIndex;
    acoral_u8 temp;
    
    temp = NOT_FIND_DISK;
    Disk = GetDiskInfo(Drive);
    if (Disk != NULL)
    {
        temp = NOT_FAT_DISK;
        Index = Index * 32;        /* 32:sizeof(FDT ) */
        if (Disk->FATType == FAT12 || Disk->FATType == FAT16)
        {
            temp = FDT_OVER;
            if (Index < (Disk->RootSecCnt * Disk->BytsPerSec))
            {
                ByteIndex = Index % Disk->BytsPerSec;
                SecIndex = Index / Disk->BytsPerSec + Disk->RootDirTable;
                temp = ReadFDTInfo(Rt, Drive, SecIndex, ByteIndex);
            }
        }
    }
    return temp;
}

/*********************************************************************************************************
** 函数名称: GetFDTInfo
** 功能描述: 获取指定目录指定文件（目录）信息
**
** 输　入: Rt：存储返回信息的指针
**        Drive：驱动器号
**        ClusIndex：目录首簇号
**        Index：文件（目录）在FDT中的位置
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: GetDiskInfo,ReadFDTInfo
********************************************************************************************************/
        acoral_u8 GetFDTInfo(FDT *Rt,acoral_u8 Drive, acoral_u32 ClusIndex, acoral_u32 Index)
{
    Disk_Info * Disk;
    acoral_u16 ByteIndex;
    acoral_u32 SecIndex, i;

    Disk = GetDiskInfo(Drive);
    if (Disk == NULL)
    {
        return NOT_FIND_DISK;
    }

    if (ClusIndex == EMPTY_CLUS)
    {
        if (Disk->FATType == FAT32)
        {
            ClusIndex = Disk->RootDirTable;
        }
        else
        {
            return GetRootFDTInfo(Rt, Drive, Index);
        }
    }

    if (Disk->FATType == FAT12 ||
        Disk->FATType == FAT16 ||
        Disk->FATType == FAT32)
    {
        Index = Index * 32;
        ByteIndex = Index % Disk->BytsPerSec;
        SecIndex = Index / Disk->BytsPerSec;
        /* 计算目录项所在扇区 */
        i = Disk->SecPerClus;
        while(SecIndex >= i)
        {
            ClusIndex = FATGetNextClus(Drive, ClusIndex);
            if (ClusIndex <= EMPTY_CLUS_1 ||
                ClusIndex >= BAD_CLUS) 
            {
                return FDT_OVER;
            }
            SecIndex -= i;
        }
        SecIndex = (ClusIndex - 2) * i + SecIndex + Disk->DataStartSec;
        return ReadFDTInfo(Rt, Drive, SecIndex, ByteIndex);
    }
    return NOT_FAT_DISK;
}

/*********************************************************************************************************
** 函数名称: SetRootFDTInfo
** 功能描述: 设置根目录指定文件（目录）信息
**
** 输　入: FDTData：要写入的信息
**        Drive：驱动器号
**        Index：文件（目录）在FDT中的位置
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: GetDiskInfo,SetFDTInfo,WriteFDTInfo
********************************************************************************************************/
        acoral_u8 SetRootFDTInfo(acoral_u8 Drive, acoral_u32 Index, FDT *FDTData)
{
    Disk_Info * Disk;
    acoral_u16 ByteIndex;
    acoral_u32 SecIndex;
    acoral_u8 Rt;
    
    Rt = NOT_FIND_DISK;
    Disk = GetDiskInfo(Drive);
    if (Disk != NULL)
    {
        Index = Index * 32;        /* 32:sizeof(FDT ) */
        if (Disk->FATType == FAT12 || Disk->FATType == FAT16)
        {
            Rt = FDT_OVER;
            if (Index < (Disk->RootSecCnt * Disk->BytsPerSec))
            {
                ByteIndex = Index % Disk->BytsPerSec;
                SecIndex = Index / Disk->BytsPerSec + Disk->RootDirTable;
                Rt = WriteFDTInfo(Drive, SecIndex, ByteIndex, FDTData);
            }
        }
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: SetFDTInfo
** 功能描述: 设置指定目录指定文件（目录）信息
**
** 输　入: FDTData：要写入的信息
**        Drive：驱动器号
**        ClusIndex：目录首簇号
**        Index：文件（目录）在FDT中的位置
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: GetDiskInfo,WriteFDTInfo
********************************************************************************************************/
        acoral_u8 SetFDTInfo(acoral_u8 Drive, acoral_u32 ClusIndex, acoral_u32 Index, FDT *FDTData)
{
    Disk_Info * Disk;
    acoral_u16 ByteIndex;
    acoral_u32 SecIndex;
    acoral_u8 i;
    
    Disk = GetDiskInfo(Drive);
    if (Disk == NULL)
    {
        return NOT_FIND_DISK;
    }

    if (ClusIndex == EMPTY_CLUS)
    {
        if (Disk->FATType == FAT32)
        {
            ClusIndex = Disk->RootDirTable;
        }
        else
        {
            return SetRootFDTInfo(Drive, Index, FDTData);
        }
    }

    if (Disk->FATType == FAT12 ||
        Disk->FATType == FAT16 ||
        Disk->FATType == FAT32)
    {
        Index = Index * 32;
        ByteIndex = Index % Disk->BytsPerSec;
        SecIndex = Index / Disk->BytsPerSec;
        /* 计算目录项所在扇区 */
        i = Disk->SecPerClus;
        while(SecIndex >= i)
        {
            ClusIndex = FATGetNextClus(Drive, ClusIndex);
            if (ClusIndex <= EMPTY_CLUS_1 ||
                ClusIndex >= BAD_CLUS) 
            {
                return FDT_OVER;
            }
            SecIndex -= i;
        }
        SecIndex = (ClusIndex - 2) * i + SecIndex + Disk->DataStartSec;

        return WriteFDTInfo(Drive, SecIndex, ByteIndex, FDTData);
    }
    return NOT_FAT_DISK;
}

/*********************************************************************************************************
** 函数名称: FindFDTInfo
** 功能描述: 在指定目录查找指定文件（目录）
**
** 输　入: Rt：存储返回信息的指针
**        Drive：驱动器号
**        ClusIndex：目录首簇号
**        FileName：文件（目录）名
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: GetDiskInfo,GetFDTInfo
********************************************************************************************************/
        acoral_u8 FindFDTInfo(FDT *Rt,acoral_u8 Drive, acoral_u32 ClusIndex, acoral_char FileName[])
{
    acoral_u32 i;
    acoral_u8 temp;
    

    i = 0;
    if (FileName[0] == DEL_FDT)
    {
        FileName[0] = ESC_FDT;
    }
    while (1)
    {
        temp = GetFDTInfo(Rt, Drive, ClusIndex, i);
        if (temp != RETURN_OK)
        {
            break;
        }

        if (Rt->Name[0] == EMPTY_FDT)
        {
            temp = NOT_FIND_FDT;
            break;
        }
        if ((Rt->Attr & ATTR_VOLUME_ID) == 0)
        if (FileName[0] == Rt->Name[0])
        if (FileName[1] == Rt->Name[1])
        if (FileName[2] == Rt->Name[2])
        if (FileName[3] == Rt->Name[3])
        if (FileName[4] == Rt->Name[4])
        if (FileName[5] == Rt->Name[5])
        if (FileName[6] == Rt->Name[6])
        if (FileName[7] == Rt->Name[7])
        if (FileName[8] == Rt->Name[8])
        if (FileName[9] == Rt->Name[9])
        if (FileName[10] == Rt->Name[10])
        {
            temp = RETURN_OK;
            break;
        }
        i++;
    }
    if (FileName[0] == ESC_FDT)
    {
        FileName[0] = DEL_FDT;
    }
    return temp;
}

/*********************************************************************************************************
** 函数名称: AddFDT
** 功能描述: 在指定目录查增加指定文件（目录）
**
** 输　入: Rt：存储返回信息的指针
**        Drive：驱动器号
**        ClusIndex：目录首簇号
**        FileName：文件（目录）名
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: FindFDTInfo,GetFDTInfo,SetFDTInfo
********************************************************************************************************/
        acoral_u8 AddFDT(acoral_u8 Drive, acoral_u32 ClusIndex, FDT *FDTData)
{
    Disk_Info * Disk;
    acoral_u32 i;
    FDT TempFDT;
    acoral_u8 temp;

    Disk = GetDiskInfo(Drive);
    if (Disk == NULL)
    {
        return NOT_FIND_DISK;
    }

    if (ClusIndex == EMPTY_CLUS)
    if (Disk->FATType == FAT32)
    {
        ClusIndex = Disk->RootDirTable;
    }

    temp = FindFDTInfo(&TempFDT, Drive, ClusIndex, FDTData->Name);
    if (temp == RETURN_OK)
    {
        return FDT_EXISTS;
    }
    if (temp != NOT_FIND_FDT)
    if (temp != FDT_OVER)
    {
        return temp;
    }

    if (FDTData->Name[0] == DEL_FDT)
    {
        FDTData->Name[0] = ESC_FDT;
    }

    i = 0;
    temp = RETURN_OK;
    while (temp == RETURN_OK)
    {
        temp = GetFDTInfo(&TempFDT, Drive, ClusIndex, i);
        if (temp == RETURN_OK)
        {
            if (TempFDT.Name[0] == DEL_FDT || TempFDT.Name[0] == EMPTY_FDT)
            {
                temp = SetFDTInfo(Drive, ClusIndex, i, FDTData);
                break;
            }
        }
        i++;
    }
    if (temp == FDT_OVER)
    if (ClusIndex != EMPTY_CLUS)
    {
        i = FATAddClus(Drive, ClusIndex);
        temp = DISK_FULL;
        if (i != BAD_CLUS)
        {
            ClearClus(Drive, i); 
            temp = SetFDTInfo(Drive, i, 0, FDTData);
        }
    }
    if (FDTData->Name[0] == ESC_FDT)
    {
        FDTData->Name[0] = DEL_FDT;
    }
    return temp;
}

/*********************************************************************************************************
** 函数名称: DelFDT
** 功能描述: 在指定目录删除指定文件（目录）
**
** 输　入: Rt：存储返回信息的指针
**        Drive：驱动器号
**        ClusIndex：目录首簇号
**        FileName：文件（目录）名
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: GetDiskInfo,GetFDTInfo,SetFDTInfo
********************************************************************************************************/
        acoral_u8 DelFDT(acoral_u8 Drive, acoral_u32 ClusIndex, acoral_char FileName[])
{
    acoral_u32 i;
    FDT TempFDT;
    acoral_u8 temp;
    
    i = 0;
    if (FileName[0] == DEL_FDT)
    {
        FileName[0] = ESC_FDT;
    }
        
    while (1)
    {
        temp = GetFDTInfo(&TempFDT, Drive, ClusIndex, i);
        if (temp != RETURN_OK)
        {
            break;
        }
            
        if (TempFDT.Name[0] == EMPTY_FDT)
        {
            temp = NOT_FIND_FDT;
            break;
        }
        if ((TempFDT.Attr & ATTR_VOLUME_ID) == 0)
        if (FileName[0] == TempFDT.Name[0])
        if (FileName[1] == TempFDT.Name[1])
        if (FileName[2] == TempFDT.Name[2])
        if (FileName[3] == TempFDT.Name[3])
        if (FileName[4] == TempFDT.Name[4])
        if (FileName[5] == TempFDT.Name[5])
        if (FileName[6] == TempFDT.Name[6])
        if (FileName[7] == TempFDT.Name[7])
        if (FileName[8] == TempFDT.Name[8])
        if (FileName[9] == TempFDT.Name[9])
        if (FileName[10] == TempFDT.Name[10])
        {
            TempFDT.Name[0] = DEL_FDT;
            temp = SetFDTInfo(Drive, ClusIndex, i, &TempFDT);
            break;
        }
        i++;
    }
    if (FileName[0] == ESC_FDT)
    {
        FileName[0] = DEL_FDT;
    }
    return temp;
}

/*********************************************************************************************************
** 函数名称: ChangeFDT
** 功能描述: 改变指定目录指定文件（目录）的属性
**
** 输　入: Rt：存储返回信息的指针
**        Drive：驱动器号
**        ClusIndex：目录首簇号
**        FileName：文件（目录）名
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: GetDiskInfo,GetFDTInfo,SetFDTInfo
********************************************************************************************************/
        acoral_u8 ChangeFDT(acoral_u8 Drive, acoral_u32 ClusIndex, FDT *FDTData)
{
    acoral_u32 i;
    acoral_u8 temp;
    FDT TempFDT;

    i = 0;
    if (FDTData->Name[0] == DEL_FDT)
    {
        FDTData->Name[0] = ESC_FDT;
    }
    while (1)
    {
        temp = GetFDTInfo(&TempFDT, Drive, ClusIndex, i);
        if (temp != RETURN_OK)
        {
            break;
        }
            
        if (TempFDT.Name[0] == EMPTY_FDT)
        {
            temp = NOT_FIND_FDT;
            break;
        }
        if ((TempFDT.Attr & ATTR_VOLUME_ID) == 0)
        if (FDTData->Name[0] == TempFDT.Name[0])
        if (FDTData->Name[1] == TempFDT.Name[1])
        if (FDTData->Name[2] == TempFDT.Name[2])
        if (FDTData->Name[3] == TempFDT.Name[3])
        if (FDTData->Name[4] == TempFDT.Name[4])
        if (FDTData->Name[5] == TempFDT.Name[5])
        if (FDTData->Name[6] == TempFDT.Name[6])
        if (FDTData->Name[7] == TempFDT.Name[7])
        if (FDTData->Name[8] == TempFDT.Name[8])
        if (FDTData->Name[9] == TempFDT.Name[9])
        if (FDTData->Name[10] == TempFDT.Name[10])
        {
            temp = SetFDTInfo(Drive, ClusIndex, i, FDTData);
            break;
        }
        i++;
    }
    if (FDTData->Name[0] == ESC_FDT)
    {
        FDTData->Name[0] = DEL_FDT;
    }
    return temp;
}

/*********************************************************************************************************
** 函数名称: DirIsEmpty
** 功能描述: 
**
** 输　入: Drive：驱动器号
**        ClusIndex：目录首簇号
** 输　出: DIR_EMPTY：空
**        DIR_NOT_EMPTY：不空
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: GetFDTInfo
********************************************************************************************************/
        acoral_u8 DirIsEmpty(acoral_u8 Drive, acoral_u32 ClusIndex)
{
    acoral_u32 i;
    FDT TempFDT;
    acoral_u8 temp;
    Disk_Info * Disk;

    
    if (ClusIndex == EMPTY_CLUS)
    {
        return DIR_NOT_EMPTY;
    }
/* 判断是否是fat32 的根目录 */
    Disk = GetDiskInfo(Drive);
    if (Disk == NULL)
    {
        return NOT_FIND_DISK;
    }
    if (Disk->FATType == FAT32)
    if (ClusIndex == Disk->RootDirTable )
    {
        return DIR_NOT_EMPTY;
    }
    
    i = 2;
    while (1)
    {
        temp = GetFDTInfo(&TempFDT, Drive, ClusIndex, i);
        if (temp != RETURN_OK)
        {
           return temp; 
        }
    
        if ((TempFDT.Attr & ATTR_VOLUME_ID) == 0)
        if (TempFDT.Name[0] != DEL_FDT)
        {
            break;
        }
        i++;
    }
    if (TempFDT.Name[0] == EMPTY_FDT)
    {
        return DIR_EMPTY;
    }
    else
    {
        return DIR_NOT_EMPTY;
    }
}

/*********************************************************************************************************
** 函数名称: FDTIsLie
** 功能描述: 在指定目录查看指定文件（目录）是否存在
**
** 输　入: Drive：驱动器号
**        ClusIndex：目录首簇号
**        FileName：文件（目录）名
** 输　出: RETURN_OK：成功
**        其它参考fat.h中关于返回值的说明
** 全局变量: 无
** 调用模块: GetDiskInfo,GetFDTInfo
********************************************************************************************************/
        acoral_u8 FDTIsLie(acoral_u8 Drive, acoral_u32 ClusIndex, acoral_char FileName[])
{
    acoral_u32 i;
    acoral_u8 temp;
    FDT temp1;
    
    i = 0;
    if (FileName[0] == DEL_FDT)
    {
        FileName[0] = ESC_FDT;
    }
    while (1)
    {
        temp = GetFDTInfo(&temp1, Drive, ClusIndex, i);
        if (temp == FDT_OVER)
        {
            temp = NOT_FIND_FDT;
            break;
        }

        if (temp != RETURN_OK)
        {
            break;
        }

        if (temp1.Name[0] == EMPTY_FDT)
        {
            temp = NOT_FIND_FDT;
            break;
        }
        if ((temp1.Attr & ATTR_VOLUME_ID) == 0)
        if (FileName[0] == temp1.Name[0])
        if (FileName[1] == temp1.Name[1])
        if (FileName[2] == temp1.Name[2])
        if (FileName[3] == temp1.Name[3])
        if (FileName[4] == temp1.Name[4])
        if (FileName[5] == temp1.Name[5])
        if (FileName[6] == temp1.Name[6])
        if (FileName[7] == temp1.Name[7])
        if (FileName[8] == temp1.Name[8])
        if (FileName[9] == temp1.Name[9])
        if (FileName[10] == temp1.Name[10])
        {
            temp = FDT_EXISTS;
        }
        i++;
    }
    if (FileName[0] == ESC_FDT)
    {
        FileName[0] = DEL_FDT;
    }
    return temp;
}
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
