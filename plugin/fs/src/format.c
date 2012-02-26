#define IN_FORMAT
#include "../include/fs.h"

const acoral_u8 Root[512] = {
       0xEB,0x3E,0x90, 'Z', 'L', 'G', '1', '.', '0', ' ',
        ' ',0x00,0x02,0x01,0x01,0x00,0x02,0xE0,0x00,0x40,
       0x0B,0xF0,0x09,0x00,0x12,0x00,0x02,0x00,0x00,0x00,
       0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x29,0x64,
       0x2A,0xD5,0x18, 'N', 'O', ' ', 'N', 'A', 'M', 'E',
        ' ', ' ', ' ', ' ', 'F', 'A', 'T', ' ', ' ', ' ',
        ' ', ' ',0xF1,0x7D,0xFA,0x33,0xC9,0x8E,0xD1,0xBC,
       0xFC,0x7B,0x16,0x07,0xBD,0x78,0x00,0xC5,0x76,0x00,
       0x1E,0x56,0x16,0x55,0xBF,0x22,0x05,0x89,0x7E,0x00,
       0x89,0x4E,0x02,0xB1,0x0B,0xFC,0xF3,0xA4,0x06,0x1F,
       0xBD,0x00,0x7C,0xC6,0x45,0xFE,0x0F,0x8B,0x46,0x18,
       0x88,0x45,0xF9,0xFB,0x38,0x66,0x24,0x7C,0x04,0xCD,
       0x13,0x72,0x3C,0x8A,0x46,0x10,0x98,0xF7,0x66,0x16,
       0x03,0x46,0x1C,0x13,0x56,0x1E,0x03,0x46,0x0E,0x13,
       0xD1,0x50,0x52,0x89,0x46,0xFC,0x89,0x56,0xFE,0xB8,
       0x20,0x00,0x8B,0x76,0x11,0xF7,0xE6,0x8B,0x5E,0x0B,
       0x03,0xC3,0x48,0xF7,0xF3,0x01,0x46,0xFC,0x11,0x4E,
       0xFE,0x5A,0x58,0xBB,0x00,0x07,0x8B,0xFB,0xB1,0x01,
       0xE8,0x94,0x00,0x72,0x47,0x38,0x2D,0x74,0x19,0xB1,
       0x0B,0x56,0x8B,0x76,0x3E,0xF3,0xA6,0x5E,0x74,0x4A,
       0x4E,0x74,0x0B,0x03,0xF9,0x83,0xC7,0x15,0x3B,0xFB,
       0x72,0xE5,0xEB,0xD7,0x2B,0xC9,0xB8,0xD8,0x7D,0x87,
       0x46,0x3E,0x3C,0xD8,0x75,0x99,0xBE,0x80,0x7D,0xAC,
       0x98,0x03,0xF0,0xAC,0x84,0xC0,0x74,0x17,0x3C,0xFF,
       0x74,0x09,0xB4,0x0E,0xBB,0x07,0x00,0xCD,0x10,0xEB,
       0xEE,0xBE,0x83,0x7D,0xEB,0xE5,0xBE,0x81,0x7D,0xEB,
       0xE0,0x33,0xC0,0xCD,0x16,0x5E,0x1F,0x8F,0x04,0x8F,
       0x44,0x02,0xCD,0x19,0xBE,0x82,0x7D,0x8B,0x7D,0x0F,
       0x83,0xFF,0x02,0x72,0xC8,0x8B,0xC7,0x48,0x48,0x8A,
       0x4E,0x0D,0xF7,0xE1,0x03,0x46,0xFC,0x13,0x56,0xFE,
       0xBB,0x00,0x07,0x53,0xB1,0x04,0xE8,0x16,0x00,0x5B,
       0x72,0xC8,0x81,0x3F,0x4D,0x5A,0x75,0xA7,0x81,0xBF,
       0x00,0x02,0x42,0x4A,0x75,0x9F,0xEA,0x00,0x02,0x70,
       0x00,0x50,0x52,0x51,0x91,0x92,0x33,0xD2,0xF7,0x76,
       0x18,0x91,0xF7,0x76,0x18,0x42,0x87,0xCA,0xF7,0x76,
       0x1A,0x8A,0xF2,0x8A,0x56,0x24,0x8A,0xE8,0xD0,0xCC,
       0xD0,0xCC,0x0A,0xCC,0xB8,0x01,0x02,0xCD,0x13,0x59,
       0x5A,0x58,0x72,0x09,0x40,0x75,0x01,0x42,0x03,0x5E,
       0x0B,0xE2,0xCC,0xC3,0x03,0x18,0x01,0x27,0x0D,0x0A,
       0x49,0x6E,0x76,0x61,0x6C,0x69,0x64,0x20,0x73,0x79,
       0x73,0x74,0x65,0x6D,0x20,0x64,0x69,0x73,0x6B,0xFF,
       0x0D,0x0A,0x44,0x69,0x73,0x6B,0x20,0x49,0x2F,0x4F,
       0x20,0x65,0x72,0x72,0x6F,0x72,0xFF,0x0D,0x0A,0x52,
       0x65,0x70,0x6C,0x61,0x63,0x65,0x20,0x74,0x68,0x65,
       0x20,0x64,0x69,0x73,0x6B,0x2C,0x20,0x61,0x6E,0x64,
       0x20,0x74,0x68,0x65,0x6E,0x20,0x70,0x72,0x65,0x73,
       0x73,0x20,0x61,0x6E,0x79,0x20,0x6B,0x65,0x79,0x0D,
       0x0A,0x00,0x49,0x4F,0x20,0x20,0x20,0x20,0x20,0x20,
       0x53,0x59,0x53,0x4D,0x53,0x44,0x4F,0x53,0x20,0x20,
       0x20,0x53,0x59,0x53,0x80,0x01,0x00,0x57,0x49,0x4E,
       0x42,0x4F,0x4F,0x54,0x20,0x53,0x59,0x53,0x00,0x00,
       0x55,0xAA
       };

static acoral_u8 Buf[4098];
/*********************************************************************************************************
** 函数名称: Format
** 功能描述: 初始化文件指针系统
**
** 输　入: Drive:驱动器号
**        SecPerDisk:逻辑盘包含扇区数
**        BytsPerSec:扇区包含字节数目
** 输　出: 无
**         
** 输　出: TRUE:成功
**        FALSE：失败 
**
** 全局变量: Buf
** 调用模块: 无
********************************************************************************************************/
        acoral_u8 Format(acoral_u8 Drive, acoral_u32 SecPerDisk, acoral_u16 BytsPerSec)
{
    const acoral_u8 *cp1;
    acoral_u8 *cp2;
    acoral_u16 i, j, temp;
    acoral_u32 temp1, k;
    Disk_RW_Parameter parameter;
    Disk_Info *Disk;

    Disk = GetDiskInfo(Drive);
    if (Disk != NULL)
    {
        cp2 = Buf;
        for (i = 0; i < BytsPerSec; i++)
        {
            *cp2++ = 0;
        }
        
        cp1 = Root;
        cp2 = Buf;
        for (i = 0; i < 512; i++)
        {
            *cp2++ = *cp1++;
        }
            /* 每扇区字节数 */
        Buf[11] = BytsPerSec & 0xff;
        Buf[12] = (BytsPerSec >> 8) & 0xff;
            /* 每簇扇区数 */
        Buf[13] = 1;
            /* 保留扇区数 */
        Buf[14] = 1;
        Buf[15] = 0;
            /* FAT表个数 */
        Buf[16] = 1;

        Buf[19] = SecPerDisk & 0xff;
        Buf[20] = (SecPerDisk >> 8) & 0xff;
        if (SecPerDisk >= 0x10000)
        {
            Buf[19] = 0;
            Buf[20] = 0;

            Buf[32] = SecPerDisk & 0xff;
            Buf[33] = (SecPerDisk >> 8) & 0xff;
            Buf[34] = (SecPerDisk >> 16) & 0xff;
            Buf[35] = (SecPerDisk >> 17) & 0xff;
        }

        temp = (4086 * 3 / 2 + BytsPerSec - 1) / BytsPerSec;
        i = (0xe0 * 32 + BytsPerSec - 1) / BytsPerSec;
        temp1 = SecPerDisk - temp - i;
        if (temp1 < 4085)               /* FAT12 */
        {
            temp = ((SecPerDisk - i - 1)  * 3 / 2 + BytsPerSec - 1) / BytsPerSec;

            /* 根目录FDT项数 */
            i = (i * BytsPerSec) / 32;
            Buf[17] = i & 0xff;
            Buf[18] = (i >> 8) & 0xff;

            /* FAT表占用扇区数 */
            Buf[22] = temp & 0xff;
            Buf[23] = (temp >> 8) & 0xff;

            parameter.Drive = Drive;
            parameter.SectorIndex = 0;
            parameter.RsvdForLow = Disk->RsvdForLow;
            parameter.Buf = Buf;
            Disk->DiakCommand(DISK_WRITE_SECTOR, &parameter);

/* 初始化 FAT和根目录 */
            cp2 = Buf + 3;
            Buf[0] = 0xf0;
            Buf[1] = 0xff;
            Buf[2] = 0xff;
            for (j = 3; j < BytsPerSec; j++)
            {
                *cp2++ = 0;
            }
            for (j = 0; j < temp; j++)
            {
                parameter.SectorIndex = j + 1;
                Disk->DiakCommand(DISK_WRITE_SECTOR, &parameter);
                Buf[0] = 0;
                Buf[1] = 0;
                Buf[2] = 0;
            }
            i = (i * 32) / BytsPerSec;
            temp++;
            for (j = 0; j < i; j++)
            {
                parameter.SectorIndex = j + temp;
                Disk->DiakCommand(DISK_WRITE_SECTOR, &parameter);
            }
        }
        else
        {
            temp = (4086 * 2 + BytsPerSec - 1) / BytsPerSec;
            i = (512 * 32 + BytsPerSec - 1) / BytsPerSec;
            temp1 = SecPerDisk - temp - i;
            if (temp1 < 65525)          /* FAT16 */
            {
                temp = ((SecPerDisk - i - 1) * 2 + BytsPerSec - 1) / BytsPerSec;
                /* 根目录FDT项数 */
                i = (i * BytsPerSec) / 32;
                Buf[17] = i & 0xff;
                Buf[18] = (i >> 8) & 0xff;

                /* FAT表占用扇区数 */
                Buf[22] = temp & 0xff;
                Buf[23] = (temp >> 8) & 0xff;
                
                parameter.Drive = Drive;
                parameter.SectorIndex = 0;
                parameter.RsvdForLow = Disk->RsvdForLow;
                parameter.Buf = Buf;
                Disk->DiakCommand(DISK_WRITE_SECTOR, &parameter);
                
/* 初始化 FAT和根目录 */
                cp2 = Buf + 4;
                Buf[0] = 0xf0;
                Buf[1] = 0xff;
                Buf[2] = 0xff;
                Buf[3] = 0xff;
                for (j = 4; j < BytsPerSec; j++)
                {
                    *cp2++ = 0;
                }
                for (j = 0; j < temp; j++)
                {
                    parameter.SectorIndex = j + 1;
                    Disk->DiakCommand(DISK_WRITE_SECTOR, &parameter);
                    Buf[0] = 0;
                    Buf[1] = 0;
                    Buf[2] = 0;
                    Buf[3] = 0;
                }
                i = (i * 32) / BytsPerSec;
                temp++;
                for (j = 0; j < i; j++)
                {
                    parameter.SectorIndex = j + temp;
                    Disk->DiakCommand(DISK_WRITE_SECTOR, &parameter);
                }
                    
            }
            else                        /* FAT32 */
            {
                /* 根目录FDT项数 */
                Buf[17] = 0;
                Buf[18] = 0;

                /* FAT表占用扇区数 */
                Buf[22] = 0;
                Buf[23] = 0;
                    
                temp1 = (SecPerDisk * 4 + BytsPerSec - 1) / BytsPerSec;
                Buf[36] = temp1 & 0xff;
                Buf[37] = (temp1 >> 8) & 0xff;
                Buf[38] = (temp1 >> 16) & 0xff;
                Buf[39] = (temp1 >> 24) & 0xff;

                /* FAT32 RootDirTable为开始簇号 */
                Buf[44] = 2;
                Buf[45] = 0;
                Buf[46] = 0;
                Buf[47] = 0;


                Buf[40] = 0;
                Buf[41] = 0;
                Buf[42] = 0;
                Buf[43] = 0;

                Buf[48] = 1;
                Buf[49] = 0;

                Buf[50] = 0;        /* 6? */
                Buf[51] = 0;

                Buf[52] = 0;
                Buf[53] = 0;

                Buf[82] = 'F';
                Buf[83] = 'A';
                Buf[84] = 'T';
                Buf[85] = '3';
                Buf[86] = '2';
                Buf[87] = ' ';
                Buf[88] = ' ';
                Buf[89] = ' ';

                parameter.Drive = Drive;
                parameter.SectorIndex = 0;
                parameter.RsvdForLow = Disk->RsvdForLow;
                parameter.Buf = Buf;
                Disk->DiakCommand(DISK_WRITE_SECTOR, &parameter);

/* 初始化 FAT和根目录T */
                cp2 = Buf + 8;
                Buf[0] = 0xf0;
                Buf[1] = 0xff;
                Buf[2] = 0xff;
                Buf[3] = 0xff;
                Buf[4] = 0xff;
                Buf[5] = 0xff;
                Buf[6] = 0xff;
                Buf[7] = 0xff;
                for (j = 8; j < BytsPerSec; j++)
                {
                    *cp2++ = 0;
                }
                temp += 2;
                parameter.SectorIndex = 1;
                Disk->DiakCommand(DISK_WRITE_SECTOR, &parameter);
                Buf[0] = 0;
                Buf[1] = 0;
                Buf[2] = 0;
                Buf[3] = 0;
                Buf[4] = 0;
                Buf[5] = 0;
                Buf[6] = 0;
                Buf[7] = 0;
                for (k = 1; k < temp1; k++)
                {
                    parameter.SectorIndex = k + 1;
                    Disk->DiakCommand(DISK_WRITE_SECTOR, &parameter);
                }
            }
        }
        return TRUE;
    }
    return FALSE;
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
