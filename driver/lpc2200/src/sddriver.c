#include "../include/sdhal.h"
#include "../include/sdcrc.h"
#include "../include/sdcmd.h"
#include "../include/sdconfig.h"
#include "../include/sddriver.h"
/* SD卡信息结构体变量 the information structure variable of SD Card */
sd_struct sds;			

/* 超时时间单位表(单位:0.000000001ns) timeout unit table */
const acoral_u32 time_unit[8] = {1000000000,100000000,10000000,
							 1000000,100000,10000,1000,100};

/* 超时时间表 timeout value table */							 
const acoral_u8 time_value[16] = {0,10,12,13,15,20,25,30,
                              35,40,45,50,55,60,70,80};
 
/* 超时时间因数表 timeout factor table */                              
const acoral_u8 r2w_fator[8] = {1,2,4,8,16,32,64,128};                           
    
	/*       
	***************************************************************************************************
		
	     用户API函数:  读物理块大小、初始化,读,写,擦 SD卡  User API Function: Initialize,read,write,erase SD Card 
				
	***************************************************************************************************
	*/

acoral_u32 SDGetBlockSize(void)
{
	return SD_BLOCKSIZE;
}

				
/*******************************************************************************************************************
** 函数名称: acoral_u8 SD_Initialize()				Name:	  acoral_u8 SD_Initialize()
** 功能描述: 初始化SD/MMC卡						Function: initialize SD/MMC card
** 输　  入: 无									Input:	  NULL
** 输    出: 0:   成功    >0:  错误码			Output:	  0:  right			>0:  error code
********************************************************************************************************************/
acoral_u8 SD_Initialize(void)
{
	acoral_u8 recbuf[4],ret;
	
     
    SD_HardWareInit();					    		/* 初始化读写SD卡的硬件条件 Initialize the hardware that access SD Card */
    
    if (SD_ChkCard() != 1)							/* 检查卡是否插入 check weather card is inserted */
    {
    	ret = SD_ERR_NO_CARD;   
    	goto SD_ERR;
    }
         
    SPI_CS_Assert();								/* 1. 置CS为低 assert CS */  
	SD_SPIDelay(25);								/* 2. 至少延时 74 clock delay more than 74 clock */
    SPI_CS_Deassert();								/* 3. 置CS为高 dessert CS */
    SD_SPIDelay(2);									/* 4. 延时2(8 clock) delay 2(8 clock) */
    
    ret = SD_ResetSD();								/* 5. 发出CMDO命令复位SD卡 send CMD0 command to reset sd card */
    if (ret != SD_NO_ERR)
        goto SD_ERR;									
 	
 	ret = SD_ActiveInit();							/* 6. 激活卡进入初始化过程. active card initialize process */
 	if (ret != SD_NO_ERR)
 		goto SD_ERR;
        
   	ret = SD_ReadOCR(4, recbuf);  					/* 7. 读OCR寄存器,查询卡支持的电压值 read OCR register,get the supported voltage */
    if (ret != SD_NO_ERR)
        goto SD_ERR;
    
    if ((recbuf[1] & MSK_OCR_33) != MSK_OCR_33)
    { 
        ret = SD_ERR_VOL_NOTSUSP;					/* 不支持3.3V,返回错误码  not support 3.3V,return error code */
    	goto SD_ERR;
    }
    
    SPI_ClkToMax();									/* 8. 设置SPI时钟到最大值 set SPI clock to maximum */
        
#if SD_CRC_EN        
    ret = SD_EnableCRC(1);							/* 使能CRC校验 enable CRC check */
	if (ret != SD_NO_ERR)  
	  	goto SD_ERR;
#endif

    ret = SD_SetBlockLen(SD_BLOCKSIZE);				/* 9. 设置块的长度: 512Bytes Set the block length: 512Bytes */
    if (ret != SD_NO_ERR)  
        goto SD_ERR;
        
    ret = SD_GetCardInfo();							/* 10. 读CSD寄存器,获取SD卡信息 read CSD register, get the information of SD card */    
	if (ret != SD_NO_ERR)
		goto SD_ERR;

	 		
	return SD_NO_ERR;								/* 初始化成功 initialize sucessfully */

SD_ERR:	
	 	
	return ret;
}

/********************************************************************************************************************
** 函数名称: acoral_u8 SD_ReadBlock()					Name:	  acoral_u8 SD_ReadBlock()
** 功能描述: 从SD/MMC卡中读一个块					Function: read a single block from SD/MMC card
** 输　  入: acoral_u32 blockaddr: 块地址				Input:    acoral_u32 blockaddr: address of block
			 acoral_u8 *recbuf   : 接收缓冲区,长度512Bytes	 	  acoral_u8 *recbuf   : the buffer of receive,length is 512Bytes
** 输    出: 0:   成功    >0:  错误码				Output:	  0:  right			>0:  error code
*********************************************************************************************************************/
acoral_u8 SD_ReadBlock(acoral_u32 blockaddr, acoral_u8 *recbuf)
{
	acoral_u8 ret; 
	
	 									/* 向OS申请访问卡信号量 request semaphore acessed SD/MMC to OS */
    if (SD_ChkCard() != 1)							
    {
    	 	
    	return SD_ERR_NO_CARD;   					/* 卡没完全插入卡中 card is not inserted entirely */
	}
		
	if (blockaddr > sds.block_num)	
	{
		 	
		return SD_ERR_OVER_CARDRANGE;				/* 操作超出卡容量范围 operate over the card range */
	}
	
 	ret = SD_ReadSingleBlock(blockaddr);			/* 读单块命令 read single blocks command */						
 	if (ret != SD_NO_ERR)
 	{
 		 									
 		return ret;
 	}
 	
  	ret = SD_ReadBlockData(SD_BLOCKSIZE, recbuf);	/* 读出数据 read data from sd card */	
	 										/* 归还访问卡信号量 return semaphore acessed SD/MMC to OS */
	
	return ret;
}


/********************************************************************************************************************
** 函数名称: acoral_u8 SD_WriteBlock()					Name:	  acoral_u8 SD_WriteBlock()
** 功能描述: 向SD/MMC卡中写入一个块					Function: write a block to SD/MMC card
** 输　  入: acoral_u32 blockaddr: 块地址				Input: 	  acoral_u32 blockaddr: address of block
			 acoral_u8 *sendbuf  : 发送缓冲区,长度512Bytes	  	  acoral_u8 *sendbuf  : the buffer of send,length is 512Bytes
** 输    出: 0:   成功    >0:  错误码				Output:	  0:  right			>0:  error code
*********************************************************************************************************************/
acoral_u8 SD_WriteBlock(acoral_u32 blockaddr, acoral_u8 *sendbuf)
{
	acoral_u8 ret,tmp[2];
	
	 												/* 向OS申请访问卡信号量 request semaphore acessed SD/MMC to OS */
    if (SD_ChkCard() != 1)							
    {
    	 	
    	return SD_ERR_NO_CARD;   								/* 卡没完全插入卡中 card is not inserted entirely */
	}	
	
	if (blockaddr > sds.block_num)	
	{
		 	
		return SD_ERR_OVER_CARDRANGE;							/* 操作超出卡容量范围 operate over the card range */
	}
	
	if (SD_ChkCardWP() == 1)
	{
		 
		return SD_ERR_WRITE_PROTECT;							/* 卡有写保护 */
	}
	
	ret = SD_WriteSingleBlock(blockaddr);						/* 写单块命令 write single block */
	if (ret != SD_NO_ERR)
	{	
		 
		return ret;
	}
	
	ret = SD_WriteBlockData(0, SD_BLOCKSIZE, sendbuf);			/* 写入数据 write data */
 	if (ret == SD_NO_ERR)										/* 读Card Status寄存器, 检查写入是否成功 */
 	{															/* read Card Status register to check write wheather sucessfully */
 		ret = SD_ReadCard_Status(2, tmp);
 		if (ret != SD_NO_ERR)
 		{
 			 
 			return ret;											/* 读寄存器失败 read register fail */
		}

 		if((tmp[0] != 0) || (tmp[1] != 0))
 		{
 			 
			ret = SD_ERR_WRITE_BLK; 			     			/* 响应指示写失败 response indicate write fail */
 		}
 	}
  
     
 	return ret;													/* 返回写入结果 return the result of writing */									
}

	/*****************************************************************
	
	    		下面为子程序
	 
	*****************************************************************/
/*******************************************************************************************************************
** 函数名称: acoral_u8 SD_GetCardInfo()				Name:	  acoral_u8 SD_GetCardInfo()
** 功能描述: 获得SD/MMC卡的信息					Function: get the information of SD/MMC card
** 输　  入: acoral_u8 cardtype: 卡类型				Input:    acoral_u8 cardtype: card type	
** 输    出: 0:   成功    >0:  错误码			Output:	  0:  right			>0:  error code
*******************************************************************************************************************/
acoral_u8 SD_GetCardInfo()
{
	acoral_u32 tmp;
	acoral_u8 csdbuf[16],ret;

	ret = SD_ReadCSD(16, csdbuf);	 								    		/* 读CSD寄存器    read CSD register */
	if (ret != SD_NO_ERR)	
		return ret;	
		
	SD_CalTimeout(csdbuf);														/* 计算超时时间值 calculate timeout value */
		
	/* 计算块的最大长度  */														/* calculate the size of a sector */
	sds.block_len = 1 << (csdbuf[READ_BL_LEN_POS] & READ_BL_LEN_MSK);  			/* (2 ^ READ_BL_LEN) */
	
	/* 计算卡中块的个数 */														/* calculate the sector numbers of the SD Card */
	sds.block_num = ((csdbuf[C_SIZE_POS1] & C_SIZE_MSK1) << 10) +
	      			 (csdbuf[C_SIZE_POS2] << 2) +
	 	 			((csdbuf[C_SIZE_POS3] & C_SIZE_MSK3) >> 6) + 1;				/* (C_SIZE + 1)*/
		 	  															
	tmp = ((csdbuf[C_SIZE_MULT_POS1] & C_SIZE_MULT_MSK1) << 1) +   
	      ((csdbuf[C_SIZE_MULT_POS2] & C_SIZE_MULT_MSK2) >> 7) + 2;				/* (C_SIZE_MULT + 2) */
    	
    /* 获得卡中块的数量 */														/* get the block numbers in card */
	sds.block_num = sds.block_num * (1 << tmp);									/* (C_SIZE + 1) * 2 ^ (C_SIZE_MULT + 2) */
																			
	/* 计算擦除的单位(单位: 块) */	
	if (sds.card_type == CARDTYPE_MMC)
	{					    
		tmp  = ((csdbuf[ERASE_GRP_SIZE_POS] & ERASE_GRP_SIZE_MSK) >> 2) + 1;  	/* (ERASE_GRP_SIZE + 1)  */ 
		
		/* (ERASE_GRP_SIZE + 1) * (ERASE_GRP_MULTI + 1) */
		tmp *= ((csdbuf[ERASE_GRP_MULTI_POS1] & ERASE_GRP_MULTI_MSK1) << 3) +
		       ((csdbuf[ERASE_GRP_MULTI_POS2] & ERASE_GRP_MULTI_MSK2) >> 5) + 1;	
	}
	else																		/*calculate the size of sector */
		tmp = ((csdbuf[SECTOR_SIZE_POS1] & SECTOR_SIZE_MSK1) << 1) +  			
	          ((csdbuf[SECTOR_SIZE_POS2] & SECTOR_SIZE_MSK2) >> 7) + 1;			/* SD: SECTOR_SIZE */
	
	sds.erase_unit = tmp;	    												/* 擦除单位(块) */ 
	
	return SD_NO_ERR;															/* 返回执行成功 return perform sucessfully */
}

/*******************************************************************************************************************
** 函数名称: acoral_u8 SD_CalTimeout()				Name:	  acoral_u8 SD_CalTimeout()
** 功能描述: 计算读/写/擦超时时间				Function: calculate timeout of reading,writing,erasing
** 输　  入: acoral_u8 *csdbuf : CSD寄存器内容		Input: 	  acoral_u8 *csdbuf : CSD register content
** 输    出: 0:   成功    >0:  错误码			Output:	  0:  right			>0:  error code
*******************************************************************************************************************/
acoral_u8 SD_CalTimeout(acoral_u8 *csdbuf)
{
	acoral_u32 tmp;
	acoral_u8 time_u,time_v,fator;
	
	sds.timeout_read = READ_TIMEOUT_100MS;								/* 默认读超时为100ms */
	sds.timeout_write = WRITE_TIMEOUT_250MS;							/* 默认写超时为250ms */
	sds.timeout_erase = WRITE_TIMEOUT_250MS;
		
	time_u = (csdbuf[TAAC_POS] & TAAC_MSK);								/* 读超时时间单位 read timeout unit */
	time_v = (csdbuf[TAAC_POS] & NSAC_MSK) >> 3;						/* 读超时时间值   read timeout value */
	fator = (csdbuf[R2WFACTOR_POS] & R2WFACTOR_MSK) >> 2;				/* 读超时时间因数 read timeout factor */
	
	if(time_v == 0)	return SD_ERR_CARD_PARAM;							/* 卡参数有错误 card parameter is error */
	
	tmp = SPI_CLOCK * time_value[time_v] / 10 / time_unit[time_u];		/* TACC * f (单位 unit: clock) */
	tmp = tmp + csdbuf[NSAC_POS] * 100;									/* TACC * f + NSAC * 100 (单位 unit: clock) */
	
	/* 计算得到的超时值 the timeout value of being calculated */
	sds.timeout_read = tmp;
	sds.timeout_write = tmp * r2w_fator[fator];							/* (TACC * f + NSAC * 100) * R2WFACTOR (单位 unit:clock)*/
	
	if (sds.card_type == CARDTYPE_SD)
	{
		sds.timeout_read  = sds.timeout_read * 100 / 8;					/* 实际值为计算值的100倍 */
		sds.timeout_write = sds.timeout_write * 100 / 8;
		if (sds.timeout_read > READ_TIMEOUT_100MS)						/* 取计算值与默认值中的最小值 */
			sds.timeout_read = READ_TIMEOUT_100MS;
		
		if (sds.timeout_write > WRITE_TIMEOUT_250MS)
			sds.timeout_write = WRITE_TIMEOUT_250MS;
	}
	else
	{
		sds.timeout_read  = sds.timeout_read * 10 / 8;					/* 实际值为计算值的10倍 */
		sds.timeout_write = sds.timeout_write * 10 / 8;
	}
	
	sds.timeout_erase = sds.timeout_write;	
	return SD_NO_ERR;	
}

/*******************************************************************************************************************
** 函数名称: acoral_u8 SD_ActiveInit()				Name:	  acoral_u8 SD_ActiveInit()
** 功能描述: 激活卡,并获得卡型					Function: active card, and get the card type 
** 输　  入: 无								 	Input:    NULL
** 输    出: 0:   成功    >0:  错误码			Output:	  0:  right			>0:  error code
** 函数说明: 该命令不断重复发送到SD卡，直到响应R1的Bit0(Idle)位为0，表示SD卡内部初始化处理完成。
		     当响应的Idle位为0时，SD卡就完全进入SPI模式了。当然重复发送命令CMD1是有次数限制的，
		     最大次数为宏定义SD_IDLE_WAIT_MAX.
*******************************************************************************************************************/
acoral_u8 SD_ActiveInit(void)
{
	acoral_u8 param[4] ,resp[5],ret;
	acoral_u32 i = 0;
	acoral_memset(param,0,4); 
 	do 
    {														/* 发出CMD1, 查询卡的状态, send CMD1 to poll card status */
        ret = SD_SendCmd(CMD1, param, CMD1_R, resp);
        if (ret != SD_NO_ERR)
       		return ret;
        i ++;
    }while (((resp[0] & MSK_IDLE) == MSK_IDLE) && (i <= SD_IDLE_WAIT_MAX));
    														/* 如果响应R1的最低位Idle位为1,则继续循环 */
    														/* if response R1 Idle bit is 1,continue recycle */    
    if (i >= SD_IDLE_WAIT_MAX)
        return SD_ERR_TIMEOUT_WAITIDLE;						/* 超时,返回错误 time out,return error */	
	
    ret = SD_SendCmd(CMD55, param, CMD55_R, resp);
    if (ret != SD_NO_ERR)
       	return ret;
       	
    ret = SD_SendCmd(ACMD41, param, ACMD41_R, resp);		/* 激活内部初始化命令 active card to initialize process internal */
    if (ret != SD_NO_ERR)	
    	return SD_ERR_UNKNOWN_CARD;
    
    if ((resp[0] & 0xFE) == 0)
      	sds.card_type = CARDTYPE_SD;						/* 是SD卡 the card is SD card */	
    else	
    	sds.card_type = CARDTYPE_MMC;						/* 是MMC卡 the card is MMC card */

	return SD_NO_ERR;
}









