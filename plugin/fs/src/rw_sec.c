#define IN_RWSEC
#include "../include/fs.h"
#include <mem.h>
//Disk_cache DiskCache[MAX_DISK_CACHES];      /* 磁盘cache */
Disk_cache* HashTable[CACHE_HASH_SIZE]; //hash表，用磁盘扇区号取模来索引
Disk_cache* FreeList;  //空闲链表头，表头为最近最久未使用的缓冲区
Disk_cache* FreeListTail;  //空闲链表尾，为刚刚使用的缓冲区
acoral_u8 CacheNum;  //当前缓冲区的数目

/*********************************************************************************************************
** 函数名称: CacheInit
** 功能描述: 初始化磁盘cache
**
** 输　入: 无
**
** 输　出: 无
**         
** 全局变量: DiskCache
** 调用模块: 无
********************************************************************************************************/
        void CacheInit(void)
{
    acoral_u16 i;
    for(i=0;i<CACHE_HASH_SIZE;i++)
    	HashTable[i]=NULL;
    FreeList=NULL;
    FreeListTail = NULL;
    CacheNum = 0;
}


/*********************************************************************************************************
** 函数名称: CloseSec
** 功能描述: 解锁指定cache（允许切换出内存）
**
** 输　入: Drive：逻辑驱动器号
**        Index：缓冲的扇区号
** 输　出: 无
**         
** 全局变量: DiskCache
** 调用模块: 无
********************************************************************************************************/
        void CloseSec(acoral_u8 Drive, acoral_u32 Index)
{
    Drive = Drive;
    Index = Index;
}

/*********************************************************************************************************
** 函数名称: CacheWriteBack2
** 功能描述: 把指定扇区写回逻辑盘
**
** 输　入: Index：cache索引
**
** 输　出: 无
**         
** 全局变量: DiskCache
** 调用模块: GetDiskInfo
********************************************************************************************************/
        void CacheWriteBack2(Disk_cache* pcache)
{
    Disk_RW_Parameter Pa;
    Disk_Info * Disk;

    Disk = GetDiskInfo(pcache->Drive);     // 获取逻辑盘信息
    if (Disk != NULL)
    if (Disk->DiakCommand != NULL)
    {
        pcache->Flag &= ~CACHE_WRITED;     // cache 不再需要回写
        /* 传递参数 */
        Pa.Drive = pcache->Drive;               
        Pa.SectorIndex = pcache->SecIndex;
        Pa.RsvdForLow = Disk->RsvdForLow;
        Pa.Buf = pcache->buf;
        
        Disk->DiakCommand(DISK_WRITE_SECTOR, &Pa);  /* 调用底层驱动写扇区 */ 
    }
}

/*********************************************************************************************************
** 函数名称: CacheWriteBack
** 功能描述: 把指定cache写回逻辑盘
**
** 输　入: Drive：逻辑驱动器号
**        Index：缓冲的扇区号
** 输　出: 无
**         
** 全局变量: DiskCache
** 调用模块: CacheWriteBack2
********************************************************************************************************/
       void CacheWriteBack(acoral_u8 Drive, acoral_u32 Index)
{
    Disk_cache* pcache;
    acoral_u8  hashindex = Index % CACHE_HASH_SIZE;
    for(pcache = HashTable[hashindex]; pcache != NULL; pcache = pcache->NextHash)
    {
    	if(pcache->Drive == Drive)
    	if(pcache->SecIndex == Index)
    	{
    		if((pcache->Flag & CACHE_WRITED) != 0)
    		{
    			CacheWriteBack2(pcache);
    		}
    		break;
    	}
    }
}

/*********************************************************************************************************
** 函数名称: AllCacheWriteBack
** 功能描述: 把所有已改变的扇区写回逻辑盘
**
** 输　入: 无
**
** 输　出: 无
**         
** 全局变量: DiskCache
** 调用模块: CacheWriteBack2
********************************************************************************************************/
       void AllCacheWriteBack(void)
{
    Disk_cache* pcache;

    for (pcache = FreeList; pcache != NULL; pcache = pcache->NextFreeList)
    {
        if ((pcache->Flag & CACHE_WRITED) != 0)
        {
            CacheWriteBack2(pcache);
        }
    }
}

/*********************************************************************************************************
** 函数名称: GetCache
** 功能描述: 获取一个cache
**
** 输　入: 无
**
** 输　出: cache索引
**         
** 全局变量: DiskCache
** 调用模块: 无
********************************************************************************************************/
        Disk_cache* GetCache(void)
{
	Disk_cache* pcache;
	
	//小于最大缓冲区大小，则分配新缓冲区
	if(CacheNum < MAX_DISK_CACHES)
	{
		pcache = (Disk_cache *)acoral_malloc(sizeof(Disk_cache));
		if(pcache == NULL)
			return NULL;
		pcache->NextHash=NULL;
		pcache->FrontHash=NULL;
		pcache->NextFreeList=NULL;
		CacheNum++;
		if(FreeList == NULL)
		{
			FreeList = FreeListTail = pcache;
		}
		else //新分配的缓冲区插入空闲链表的末尾
		{
			FreeListTail->NextFreeList=pcache;
			FreeListTail=pcache;
		}
	}
	//否则，摘掉空闲缓冲区链表的头结点，并插入到尾节点，并从对应的hash表里删除
	else
	{
		if(FreeList != FreeListTail)
		{
			pcache=FreeList;
			FreeList=FreeList->NextFreeList;
			FreeListTail->NextFreeList=pcache;
			FreeListTail=pcache;
			FreeListTail->NextFreeList=NULL;
		}
		
		if(pcache->FrontHash == NULL)
		{
			if(pcache->NextHash != NULL)
				pcache->NextHash->FrontHash=NULL;
			HashTable[pcache->SecIndex % CACHE_HASH_SIZE] = pcache->NextHash;
		}
		else
		{
			if(pcache->NextHash != NULL)
				pcache->NextHash->FrontHash = pcache->FrontHash;
			pcache->FrontHash->NextHash = pcache->NextHash;
		}
		if(pcache->Drive != EMPTY_DRIVE)
		if((pcache->Flag & CACHE_WRITED) != 0)
		{
			CacheWriteBack2(pcache);
		}
	}
	return pcache;
}

/*********************************************************************************************************
** 函数名称: OpenSec
** 功能描述: 为逻辑盘上的一个扇区打开一个cache并锁定
**
** 输　入: Drive：磁盘号
**        Index：扇区号
** 输　出: 指向指定扇区数据的指针
**         
** 全局变量: DiskCache
** 调用模块: 无
********************************************************************************************************/
        acoral_u8 *OpenSec(acoral_u8 Drive, acoral_u32 Index)
{
    Disk_cache *pcache;
    acoral_u8 *Rt;
    Disk_Info *Disk;
   	acoral_u8 hashindex;
   	acoral_u16 i;
   	hashindex = Index % CACHE_HASH_SIZE;

    /* 看扇区是否已经缓存 */
    Rt = NULL;
    for (pcache = HashTable[hashindex]; pcache != NULL; pcache = pcache->NextHash)
    {
        if (pcache->Drive == Drive )
        if (pcache->SecIndex == Index)
        {
            Rt = pcache->buf;
            break;
        }
    }
    
    /* Rt == NULL则未缓存 */
    if (Rt == NULL)
    {
        Disk = GetDiskInfo(Drive);
        if (Disk != NULL)
        if (Disk->SecPerDisk > Index)
        {
            pcache = GetCache();                     /* 获取cache */
            if (pcache != NULL)
            {
                /* 初始化cache  */
                pcache->Drive = Drive;     
                pcache->Flag = 0;
                pcache->SecIndex = Index;
                if(HashTable[hashindex] == NULL)
                {
                	pcache->FrontHash = NULL;
                	pcache->NextHash = NULL;
                	HashTable[hashindex] = pcache;
                }
                else
                {
                	pcache->FrontHash = NULL;
                	pcache->NextHash = HashTable[hashindex];
                	HashTable[hashindex]->FrontHash = pcache;
                	HashTable[hashindex] = pcache;
                }
                Rt = pcache->buf;
                for (i = 0; i < Disk->BytsPerSec; i++)
                {
                    Rt[i] = 0;
                }
            }
        }
    }
    return Rt;
}

acoral_u8 ReadSuperSec(acoral_u8 Drive,acoral_u8 *buf)
{
	Disk_RW_Parameter Pa;
	Disk_Info *Disk;
        Disk = GetDiskInfo(Drive);    
	Pa.Drive=Drive;
	Pa.Buf=buf;
        if(Disk->DiakCommand(DISK_READ_SUPER_SECTOR, &Pa) == DISK_READ_OK)
        {
                return RETURN_OK;
	}
	return SECTOR_READ_ERR;
}

/*********************************************************************************************************
** 函数名称: ReadSec
** 功能描述: 从逻辑盘读扇区
**
** 输　入: Disk：逻辑盘信息
**        Index：扇区号
** 输　出: TRUE:成功
**         FALSE:失败
** 全局变量: DiskCache
** 调用模块: GetDiskInfo
********************************************************************************************************/
        acoral_u8 ReadSec(acoral_u8 Drive, acoral_u32 Index)
{
    Disk_cache *pcache;
    Disk_RW_Parameter Pa;
    Disk_Info *Disk;
    acoral_u8 Rt;
    acoral_u8 hashindex = Index % CACHE_HASH_SIZE;
    
    for (pcache = HashTable[hashindex]; pcache != NULL; pcache = pcache->NextHash)
    {
        if (pcache->Drive == Drive)
        if (pcache->SecIndex == Index)
        {
            if ((pcache->Flag & CACHE_READED) != 0)
            {
                return RETURN_OK;
            }
            break;
        }
    }
    Rt = SECTOR_NOT_IN_CACHE;
    if (pcache != NULL)
    {
        Disk = GetDiskInfo(Drive);    

        pcache->Flag |= CACHE_READED;

        /* 从磁盘读取扇区数据 */
        Pa.Drive = Drive;
        Pa.SectorIndex = Index;
        Pa.RsvdForLow = Disk->RsvdForLow;
        Pa.Buf = pcache->buf;
        Rt = NOT_FIND_DISK;
        if (Disk->DiakCommand != NULL)
        {

            Rt = SECTOR_READ_ERR;
            if (Disk->DiakCommand(DISK_READ_SECTOR, &Pa) == DISK_READ_OK)
            {
                return RETURN_OK;
            }
        }
    }
    return Rt;
}

/*********************************************************************************************************
** 函数名称: WriteSec
** 功能描述: 说明指定逻辑盘的指定一个扇区被改写
**
** 输　入: Disk：逻辑盘信息
**        Index：扇区号
** 输　出: 无
**         
** 全局变量: DiskCache
** 调用模块: 无
********************************************************************************************************/
        void WriteSec(acoral_u8 Drive, acoral_u32 Index)
{
    Disk_cache *pcache;
    acoral_u8 hashindex = Index % CACHE_HASH_SIZE;
    
    for (pcache = HashTable[hashindex]; pcache != NULL; pcache = pcache->NextHash)
    {
        if (pcache->Drive == Drive)
        if (pcache->SecIndex == Index)
        {
            pcache->Flag |= (CACHE_WRITED | CACHE_READED);
            break;
        }
    }
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
