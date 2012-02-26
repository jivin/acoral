#ifndef __SDCRC_H__
#define __SDCRC_H__
#include<type.h>
extern acoral_u16 SD_GetCRC16(acoral_u8 *pSource,acoral_u16 len);	//得到len个字节的CRC16

extern acoral_u8 SD_GetCmdByte6(acoral_u8 cmd,acoral_u8 *param);	//得到SD命令的第6个字节: CRC7 + 停止位1

extern acoral_u8 SD_GetCRC7(acoral_u8 *pSource,acoral_u16 len);		//得到len个字节的CRC7

#endif
