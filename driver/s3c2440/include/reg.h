#ifndef REG_H
#define REG_H
#define PCLK 50000000

/* INTERRUPT*/
#define IRQ_EINT0		0
#define IRQ_EINT1		1
#define IRQ_EINT2		2
#define IRQ_EINT3		3

#define IRQ_EINT4		4
#define IRQ_EINT5		5
#define IRQ_EINT6		6
#define IRQ_EINT7		7

#define IRQ_EINT8		8
#define IRQ_EINT9		9
#define IRQ_EINT10		10
#define IRQ_EINT11		11
#define IRQ_EINT12		12
#define IRQ_EINT13		13
#define IRQ_EINT14		14
#define IRQ_EINT15		15
#define IRQ_EINT16		16
#define IRQ_EINT17		17
#define IRQ_EINT18		18
#define IRQ_EINT19		19
#define IRQ_EINT20		20
#define IRQ_EINT21		21
#define IRQ_EINT22		22
#define IRQ_EINT23	 	23

#define IRQ_CAM			24			/* Added for 2440.*/
#define IRQ_BAT_FLT  	25
#define IRQ_TICK		26
#define IRQ_WDT_AC97	27   /*Changed to IRQ_WDT_AC97 for 2440A*/
#define IRQ_TIMER0	 	28               /*10*/
#define IRQ_TIMER1	 	29
#define IRQ_TIMER2		30
#define IRQ_TIMER3		31
#define IRQ_TIMER4		32
#define IRQ_UART2		33
#define IRQ_LCD			34
#define IRQ_DMA0		35
#define IRQ_DMA1		36
#define IRQ_DMA2		37
#define IRQ_DMA3		38
#define IRQ_SDI			39
#define IRQ_SPI0		40
#define IRQ_UART1		41
#define IRQ_NFCON		42		/* Added for 2440.*/
#define IRQ_USBD		43
#define IRQ_USBH		44
#define IRQ_IIC		    45
#define IRQ_UART0		46
#define IRQ_SPI1		47
#define IRQ_RTC 		48
#define IRQ_ADC	       	49


#define rSRCPND     (*(volatile acoral_u32 *)0x4a000000)	/*Interrupt request status*/
#define rINTMOD     (*(volatile acoral_u32 *)0x4a000004)	/*Interrupt mode control*/
#define rINTMSK     (*(volatile acoral_u32 *)0x4a000008)	/*Interrupt mask control*/
#define rPRIORITY   (*(volatile acoral_u32 *)0x4a00000c)	/*IRQ priority control*/
#define rINTPND     (*(volatile acoral_u32 *)0x4a000010)	/*Interrupt request status*/
#define rINTOFFSET  (*(volatile acoral_u32 *)0x4a000014)	/*Interruot request source offset*/
#define rSUBSRCPND  (*(volatile acoral_u32 *)0x4a000018)	/*Sub source pending*/
#define rINTSUBMSK  (*(volatile acoral_u32 *)0x4a00001c)	/*Interrupt sub mask*/

/* UART*/
#define rULCON0     (*(volatile acoral_u32 *)0x50000000)	/*UART 0 Line control*/
#define rUCON0      (*(volatile acoral_u32 *)0x50000004)	/*UART 0 Control*/
#define rUFCON0     (*(volatile acoral_u32 *)0x50000008)	/*UART 0 FIFO control*/
#define rUMCON0     (*(volatile acoral_u32 *)0x5000000c)	/*UART 0 Modem control*/
#define rUTRSTAT0   (*(volatile acoral_u32 *)0x50000010)	/*UART 0 Tx/Rx status*/
#define rUERSTAT0   (*(volatile acoral_u32 *)0x50000014)	/*UART 0 Rx error status*/
#define rUFSTAT0    (*(volatile acoral_u32 *)0x50000018)	/*UART 0 FIFO status*/
#define rUMSTAT0    (*(volatile acoral_u32 *)0x5000001c)	/*UART 0 Modem status*/
#define rUBRDIV0    (*(volatile acoral_u32 *)0x50000028)	/*UART 0 Baud rate divisor*/

#define rULCON1     (*(volatile acoral_u32 *)0x50004000)	/*UART 1 Line control*/
#define rUCON1      (*(volatile acoral_u32 *)0x50004004)	/*UART 1 Control*/
#define rUFCON1     (*(volatile acoral_u32 *)0x50004008)	/*UART 1 FIFO control*/
#define rUMCON1     (*(volatile acoral_u32 *)0x5000400c)	/*UART 1 Modem control*/
#define rUTRSTAT1   (*(volatile acoral_u32 *)0x50004010)	/*UART 1 Tx/Rx status*/
#define rUERSTAT1   (*(volatile acoral_u32 *)0x50004014)	/*UART 1 Rx error status*/
#define rUFSTAT1    (*(volatile acoral_u32 *)0x50004018)	/*UART 1 FIFO status*/
#define rUMSTAT1    (*(volatile acoral_u32 *)0x5000401c)	/*UART 1 Modem status*/
#define rUBRDIV1    (*(volatile acoral_u32 *)0x50004028)	/*UART 1 Baud rate divisor*/
#define rULCON2     (*(volatile acoral_u32 *)0x50008000)	/*UART 2 Line control*/
#define rUCON2      (*(volatile acoral_u32 *)0x50008004)	/*UART 2 Control*/
#define rUFCON2     (*(volatile acoral_u32 *)0x50008008)	/*UART 2 FIFO control*/
#define rUMCON2     (*(volatile acoral_u32 *)0x5000800c)	/*UART 2 Modem control*/
#define rUTRSTAT2   (*(volatile acoral_u32 *)0x50008010)	/*UART 2 Tx/Rx status*/
#define rUERSTAT2   (*(volatile acoral_u32 *)0x50008014)	/*UART 2 Rx error status*/
#define rUFSTAT2    (*(volatile acoral_u32 *)0x50008018)	/*UART 2 FIFO status*/
#define rUMSTAT2    (*(volatile acoral_u32 *)0x5000801c)	/*UART 2 Modem status*/
#define rUBRDIV2    (*(volatile acoral_u32 *)0x50008028)	/*UART 2 Baud rate divisor*/

#ifdef __BIG_ENDIAN
#define WrUTXH0(ch) (*(volatile acoral_u8 *)0x50000023)=(acoral_u8)(ch)
#define RdURXH0()   (*(volatile acoral_u8 *)0x50000027)
#define WrUTXH1(ch) (*(volatile acoral_u8 *)0x50004023)=(acoral_u8)(ch)
#define RdURXH1()   (*(volatile acoral_u8 *)0x50004027)
#define WrUTXH2(ch) (*(volatile acoral_u8 *)0x50008023)=(acoral_u8)(ch)
#define RdURXH2()   (*(volatile acoral_u8 *)0x50008027)

#define UTXH0       (0x50000020+3)  /*Byte_access address by DMA*/
#define URXH0       (0x50000024+3)
#define UTXH1       (0x50004020+3)
#define URXH1       (0x50004024+3)
#define UTXH2       (0x50008020+3)
#define URXH2       (0x50008024+3)

#else /*Little Endian*/
#define rUTXH0 (*(volatile acoral_u8 *)0x50000020)	/*UART 0 Transmission Hold*/
#define rURXH0 (*(volatile acoral_u8 *)0x50000024)	/*UART 0 Receive buffer*/
#define rUTXH1 (*(volatile acoral_u8 *)0x50004020)	/*UART 1 Transmission Hold*/
#define rURXH1 (*(volatile acoral_u8 *)0x50004024)	/*UART 1 Receive buffer*/
#define rUTXH2 (*(volatile acoral_u8 *)0x50008020)	/*UART 2 Transmission Hold*/
#define rURXH2 (*(volatile acoral_u8 *)0x50008024)	/*UART 2 Receive buffer*/

#define WrUTXH0(ch) (*(volatile acoral_u8 *)0x50000020)=(acoral_u8)(ch)
#define RdURXH0()   (*(volatile acoral_u8 *)0x50000024)
#define WrUTXH1(ch) (*(volatile acoral_u8 *)0x50004020)=(acoral_u8)(ch)
#define RdURXH1()   (*(volatile acoral_u8 *)0x50004024)
#define WrUTXH2(ch) (*(volatile acoral_u8 *)0x50008020)=(acoral_u8)(ch)
#define RdURXH2()   (*(volatile acoral_u8 *)0x50008024)
#endif



/* I/O PORT */
#define rGPACON    (*(volatile acoral_u32 *)0x56000000)	/*Port A control*/
#define rGPADAT    (*(volatile acoral_u32 *)0x56000004)	/*Port A data*/

#define rGPBCON    (*(volatile acoral_u32 *)0x56000010)	/*Port B control*/
#define rGPBDAT    (*(volatile acoral_u32 *)0x56000014)	/*Port B data*/
#define rGPBUP     (*(volatile acoral_u32 *)0x56000018)	/*Pull-up control B*/

#define rGPCCON    (*(volatile acoral_u32 *)0x56000020)	/*Port C control*/
#define rGPCDAT    (*(volatile acoral_u32 *)0x56000024)	/*Port C data*/
#define rGPCUP     (*(volatile acoral_u32 *)0x56000028)	/*Pull-up control C*/

#define rGPDCON    (*(volatile acoral_u32 *)0x56000030)	/*Port D control*/
#define rGPDDAT    (*(volatile acoral_u32 *)0x56000034)	/*Port D data*/
#define rGPDUP     (*(volatile acoral_u32 *)0x56000038)	/*Pull-up control D*/

#define rGPECON    (*(volatile acoral_u32 *)0x56000040)	/*Port E control*/
#define rGPEDAT    (*(volatile acoral_u32 *)0x56000044)	/*Port E data*/
#define rGPEUP     (*(volatile acoral_u32 *)0x56000048)	/*Pull-up control E*/

#define rGPFCON    (*(volatile acoral_u32 *)0x56000050)	/*Port F control*/
#define rGPFDAT    (*(volatile acoral_u32 *)0x56000054)	/*Port F data*/
#define rGPFUP     (*(volatile acoral_u32 *)0x56000058)	/*Pull-up control F*/

#define rGPGCON    (*(volatile acoral_u32 *)0x56000060)	/*Port G control*/
#define rGPGDAT    (*(volatile acoral_u32 *)0x56000064)	/*Port G data*/
#define rGPGUP     (*(volatile acoral_u32 *)0x56000068)	/*Pull-up control G*/

#define rGPHCON    (*(volatile acoral_u32 *)0x56000070)	/*Port H control*/
#define rGPHDAT    (*(volatile acoral_u32 *)0x56000074)	/*Port H data*/
#define rGPHUP     (*(volatile acoral_u32 *)0x56000078)	/*Pull-up control H*/

#define rGPJCON    (*(volatile acoral_u32 *)0x560000d0)	/*Port J control*/
#define rGPJDAT    (*(volatile acoral_u32 *)0x560000d4)	/*Port J data*/
#define rGPJUP     (*(volatile acoral_u32 *)0x560000d8)	/*Pull-up control J*/

#define rMISCCR    (*(volatile acoral_u32 *)0x56000080)	/*Miscellaneous control*/
#define rDCLKCON   (*(volatile acoral_u32 *)0x56000084)	/*DCLK0/1 control*/
#define rEXTINT0   (*(volatile acoral_u32 *)0x56000088)	/*External interrupt control register 0*/
#define rEXTINT1   (*(volatile acoral_u32 *)0x5600008c)	/*External interrupt control register 1*/
#define rEXTINT2   (*(volatile acoral_u32 *)0x56000090)	/*External interrupt control register 2*/
#define rEINTFLT0  (*(volatile acoral_u32 *)0x56000094)	/*Reserved*/
#define rEINTFLT1  (*(volatile acoral_u32 *)0x56000098)	/*Reserved*/
#define rEINTFLT2  (*(volatile acoral_u32 *)0x5600009c)	/*External interrupt filter control register 2*/
#define rEINTFLT3  (*(volatile acoral_u32 *)0x560000a0)	/*External interrupt filter control register 3*/
#define rEINTMSK  (*(volatile acoral_u32 *)0x560000a4)	/*External interrupt mask*/
#define rEINTPND  (*(volatile acoral_u32 *)0x560000a8)	/*External interrupt pending*/
#define rGSTATUS0  (*(volatile acoral_u32 *)0x560000ac)	/*External pin status*/
#define rGSTATUS1  (*(volatile acoral_u32 *)0x560000b0)	/*Chip ID(0x32440000)*/
#define rGSTATUS2  (*(volatile acoral_u32 *)0x560000b4)	/*Reset type*/
#define rGSTATUS3  (*(volatile acoral_u32 *)0x560000b8)	/*Saved data0(32-bit) before entering POWER_OFF mode */
#define rGSTATUS4  (*(volatile acoral_u32 *)0x560000bc)	/*Saved data0(32-bit) before entering POWER_OFF mode */

// LCD CONTROLLER
#define rLCDCON1    (*(volatile unsigned *)0x4d000000)	//LCD control 1
#define rLCDCON2    (*(volatile unsigned *)0x4d000004)	//LCD control 2
#define rLCDCON3    (*(volatile unsigned *)0x4d000008)	//LCD control 3
#define rLCDCON4    (*(volatile unsigned *)0x4d00000c)	//LCD control 4
#define rLCDCON5    (*(volatile unsigned *)0x4d000010)	//LCD control 5
#define rLCDSADDR1  (*(volatile unsigned *)0x4d000014)	//STN/TFT Frame buffer start address 1
#define rLCDSADDR2  (*(volatile unsigned *)0x4d000018)	//STN/TFT Frame buffer start address 2
#define rLCDSADDR3  (*(volatile unsigned *)0x4d00001c)	//STN/TFT Virtual screen address set
#define rREDLUT     (*(volatile unsigned *)0x4d000020)	//STN Red lookup table
#define rGREENLUT   (*(volatile unsigned *)0x4d000024)	//STN Green lookup table 
#define rBLUELUT    (*(volatile unsigned *)0x4d000028)	//STN Blue lookup table
#define rDITHMODE   (*(volatile unsigned *)0x4d00004c)	//STN Dithering mode
#define rTPAL       (*(volatile unsigned *)0x4d000050)	//TFT Temporary palette
#define rLCDINTPND  (*(volatile unsigned *)0x4d000054)	//LCD Interrupt pending
#define rLCDSRCPND  (*(volatile unsigned *)0x4d000058)	//LCD Interrupt source
#define rLCDINTMSK  (*(volatile unsigned *)0x4d00005c)	//LCD Interrupt mask
#define rTCONSEL     (*(volatile unsigned *)0x4d000060)	//LPC3600 Control --- edited by junon
#define PALETTE     0x4d000400						//Palette start address

/* SD Interface */
#define rSDICON     (*(volatile unsigned *)0x5a000000)	//SDI control
#define rSDIPRE     (*(volatile unsigned *)0x5a000004)	//SDI baud rate prescaler
#define rSDICARG    (*(volatile unsigned *)0x5a000008)	//SDI command argument
#define rSDICCON    (*(volatile unsigned *)0x5a00000c)	//SDI command control
#define rSDICSTA    (*(volatile unsigned *)0x5a000010)	//SDI command status
#define rSDIRSP0    (*(volatile unsigned *)0x5a000014)	//SDI response 0
#define rSDIRSP1    (*(volatile unsigned *)0x5a000018)	//SDI response 1
#define rSDIRSP2    (*(volatile unsigned *)0x5a00001c)	//SDI response 2
#define rSDIRSP3    (*(volatile unsigned *)0x5a000020)	//SDI response 3
#define rSDIDTIMER  (*(volatile unsigned *)0x5a000024)	//SDI data/busy timer
#define rSDIBSIZE   (*(volatile unsigned *)0x5a000028)	//SDI block size
#define rSDIDCON    (*(volatile unsigned *)0x5a00002c)	//SDI data control
#define rSDIDCNT    (*(volatile unsigned *)0x5a000030)	//SDI data remain counter
#define rSDIDSTA    (*(volatile unsigned *)0x5a000034)	//SDI data status
#define rSDIFSTA    (*(volatile unsigned *)0x5a000038)	//SDI FIFO status
#define rSDIIMSK    (*(volatile unsigned *)0x5a00003c)	//SDI interrupt mask. edited for 2440A
#define rSDIDAT    (*(volatile unsigned *)0x5a000040)	//SDI data 
#define SDIDAT     0x5a000040  
#define BIT_SDI			(0x1<<21)
#define BIT_DMA0		(0x1<<17)

/*Touch Screen */
#define rADCCON    (*(volatile unsigned *)0x58000000)	//ADC control
#define rADCTSC    (*(volatile unsigned *)0x58000004)	//ADC touch screen control
#define rADCDLY    (*(volatile unsigned *)0x58000008)	//ADC start or Interval Delay
#define rADCDAT0   (*(volatile unsigned *)0x5800000c)	//ADC conversion data 0
#define rADCDAT1   (*(volatile unsigned *)0x58000010)	//ADC conversion data 1
#define rADCUPDN   (*(volatile unsigned *)0x58000014)	//Stylus Up/Down interrupt status
#define BIT_ADC			(0x1<<31)
#define BIT_SUB_TC		(0x1<<9)
#define BIT_SUB_ADC		(0x1<<10)


#endif
