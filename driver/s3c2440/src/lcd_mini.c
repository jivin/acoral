#include"driver.h"
#include<hal.h>
#include"screen.h"
#include"mem.h"
#include"../include/reg.h"
#define MVAL		(13)
#define MVAL_USED 	(0)			//0=each frame   1=rate by MVAL
#define INVVDEN		(1)			//0=normal       1=inverted
#define BSWP		(0)			//Byte swap control
#define HWSWP		(1)			//Half word swap control
#define PNRMODE		(3)			// 设置为TFT屏
#define BPPMODE		(12)			// 设置为16bpp模式

//TFT_SIZE
#define LCD_XSIZE_TFT 	(240)	
#define LCD_YSIZE_TFT 	(320)

#define SCR_XSIZE_TFT 	(240)
#define SCR_YSIZE_TFT 	(320)

//Timing parameter for 3.5' LCD
#define VBPD 		(12)			//垂直同步信号的后肩
#define VFPD 		(4)			//垂直同步信号的前肩
#define VSPW 		(5)			//垂直同步信号的脉宽

#define HBPD 		(22)			//水平同步信号的后肩
#define HFPD 		(33)			//水平同步信号的前肩
#define HSPW 		(44)			//水平同步信号的脉宽
#define CLKVAL_TFT 	(6) 	

#define HOZVAL_TFT	(LCD_XSIZE_TFT-1)
#define LINEVAL_TFT	(LCD_YSIZE_TFT-1)

#define M5D(n)				((n) & 0x1fffff)	// To get lower 21bits

//volatile acoral_u16 LCD_BUFFER[SCR_YSIZE_TFT][SCR_XSIZE_TFT];
volatile acoral_u16 *LCD_BUFFER;
static acoral_screen_info screen_info;
/**************************************************************
TFT LCD功能模块初始化
**************************************************************/
void Lcd_PowerEnable(int invpwren,int pwren);




/**************************************************************
640×480 TFT LCD功能模块初始化
**************************************************************/
#define LCD_WIDTH 240
#define LCD_HEIGHT 320
#define LCD_PIXCLOCK 4

#define LCD_RIGHT_MARGIN 25
#define LCD_LEFT_MARGIN 10
#define LCD_HSYNC_LEN 5

#define LCD_UPPER_MARGIN 1
#define LCD_LOWER_MARGIN 5
#define LCD_VSYNC_LEN 1
void Lcd_Init(void)
{
    rGPCUP=0xffffffff; // Disable Pull-up register
    rGPCCON=0xaaaa02a8; //Initialize VD[7:0],VM,VFRAME,VLINE,VCLK

    rGPDUP=0xffffffff; // Disable Pull-up register
    rGPDCON=0xaaaaaaaa; //Initialize VD[15:8]


    rLCDCON1 = (LCD_PIXCLOCK << 8) | (3 <<  5) | (12 << 1);
    rLCDCON2 = (LCD_UPPER_MARGIN << 24) | ((LCD_HEIGHT - 1) << 14) | (LCD_LOWER_MARGIN << 6) | (LCD_VSYNC_LEN << 0);
    rLCDCON3 = (LCD_RIGHT_MARGIN << 19) | ((LCD_WIDTH  - 1) <<  8) | (LCD_LEFT_MARGIN << 0);
    rLCDCON4 = (13 <<  8) | (LCD_HSYNC_LEN << 0);
    rLCDCON5   =  ((1<<11) | (1 << 9) | (1 << 8) | (1 << 3) | (1 << 0));

    rLCDSADDR1=(((acoral_u32)LCD_BUFFER>>22)<<21)|M5D((acoral_u32)LCD_BUFFER>>1);

    rLCDSADDR2=M5D( ((acoral_u32)LCD_BUFFER+(SCR_XSIZE_TFT*LCD_YSIZE_TFT*2))>>1 );

    rLCDSADDR3=(((SCR_XSIZE_TFT-LCD_XSIZE_TFT)/1)<<11)|(LCD_XSIZE_TFT/1);

    rLCDINTMSK|=(3); // MASK LCD Sub Interrupt
    rTCONSEL &= (~7) ;     // Disable LPC3480

    rTPAL=0; // Disable Temp Palette
}


/*void Lcd_Init(void)
{
	rGPCUP  = 0x00000000;
	rGPCCON = 0xaaaa02a9; 
	 
	rGPDUP  = 0x00000000;
	rGPDCON=0xaaaaaaaa; //Initialize VD[15:8]

	rLCDCON1=(CLKVAL_TFT<<8)|(MVAL_USED<<7)|(3<<5)|(12<<1)|0;
    	// TFT LCD panel,16bpp TFT,ENVID=off

	rLCDCON2=(VBPD<<24)|(LINEVAL_TFT<<14)|(VFPD<<6)|(VSPW);

	rLCDCON3=(HBPD<<19)|(HOZVAL_TFT<<8)|(HFPD);

	rLCDCON4=(MVAL<<8)|(HSPW);

	rLCDCON5 = (1<<11) | (1<<10) | (1<<9) | (1<<8) | (0<<7) | (0<<6) | (1<<3)  |(BSWP<<1) | (HWSWP);

	rLCDSADDR1=(((acoral_u32)LCD_BUFFER>>22)<<21)|M5D((acoral_u32)LCD_BUFFER>>1);

	rLCDSADDR2=M5D( ((acoral_u32)LCD_BUFFER+(SCR_XSIZE_TFT*LCD_YSIZE_TFT*2))>>1 );

	rLCDSADDR3=(((SCR_XSIZE_TFT-LCD_XSIZE_TFT)/1)<<11)|(LCD_XSIZE_TFT/1);

	rLCDINTMSK|=(3); // MASK LCD Sub Interrupt
	rTCONSEL &= (~7) ;     // Disable LPC3480

	rTPAL=0; // Disable Temp Palette
}*/


/**************************************************************
LCD视频和控制信号输出或者停止，1开启视频输出
**************************************************************/
void Lcd_EnvidOnOff(int onoff)
{
	if(onoff==1)
		rLCDCON1|=1; // ENVID=ON
	else
		rLCDCON1 =rLCDCON1 & 0x3fffe; // ENVID Off
}

/**************************************************************
TFT LCD 电源控制引脚使能
**************************************************************/
void Lcd_PowerEnable(int invpwren,int pwren)
{
	//GPG4 is setted as LCD_PWREN
	rGPGUP=rGPGUP&(~(1<<4))|(1<<4); // Pull-up disable
	rGPGCON=rGPGCON&(~(3<<8))|(3<<8); //GPG4=LCD_PWREN
	rGPGDAT = rGPGDAT | (1<<4) ;
	//invpwren=pwren;
	//Enable LCD POWER ENABLE Function
	rLCDCON5=rLCDCON5&(~(1<<3))|(pwren<<3);   // PWREN
	rLCDCON5=rLCDCON5&(~(1<<5))|(invpwren<<5);   // INVPWREN
}
/**************************************************************
TFT LCD全屏填充特定颜色单元或清屏
**************************************************************/
void Lcd_ClearScr(acoral_u32 c)
{
	acoral_u32 x,y ;
		
    for( y = 0 ; y < SCR_YSIZE_TFT ; y++ )
    {
    	for( x = 0 ; x < SCR_XSIZE_TFT ; x++ )
    	{
			*(LCD_BUFFER+y*LCD_XSIZE_TFT+x) = c ;
    	}
    }
}

void Lcd_TFT_Init(void)
{
	Lcd_Init();
	Lcd_PowerEnable(0, 1);
	Lcd_EnvidOnOff(1);		//turn on vedio
	Lcd_ClearScr( (0x00<<11) | (0x00<<5) | (0x00) );  
}

acoral_dev_ops_t lcd_ops;
acoral_32 lcd_open(){
	LCD_BUFFER=(acoral_u16 *)acoral_malloc(2*LCD_XSIZE_TFT*LCD_YSIZE_TFT);
	Lcd_TFT_Init();
	screen_info.xres=LCD_XSIZE_TFT;
	screen_info.yres=LCD_YSIZE_TFT;
	screen_info.bits_per_pixel=16;
	screen_info.screen_buf=(acoral_char *)LCD_BUFFER;
	return 0;
}

acoral_32 lcd_close(){
	acoral_free(LCD_BUFFER);
	return 0;
}

acoral_32 lcd_config(acoral_u32 cmd,void *data,acoral_time time_out){
	switch (cmd)
	{
		case CMD_SCREEN_INFO:
			*((acoral_screen_info **)data)=&screen_info;
			break;
		default:
			break;	
	}	
	return 0;
}

void lcd_init(){
	lcd_ops.write=NULL;
	lcd_ops.open=lcd_open;
	lcd_ops.close=lcd_close;
	lcd_ops.config=lcd_config;
  	acoral_drv_register(&lcd_ops,"screen");
}
