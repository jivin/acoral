#include <hal.h>
#include "driver.h"
#include "print.h"
#include "../include/reg.h"

#define INICLK	300000
#define SDCLK	24000000	//PCLK=49.392MHz
#define MMCCLK	15000000	//PCLK=49.392MHz
acoral_32 CMD13(void);    // Send card status
acoral_32 CMD9(void);
void CMD0(void);
acoral_32 Wide=1; // 0:1bit, 1:4bit
acoral_32 MMC=0;  // 0:SD  , 1:MMC
volatile acoral_32 RCA;
acoral_32 SD_card_init(void);
void Set_4bit_bus(void);
void Card_sel_desel(char sel_desel);
void Set_1bit_bus(void);

acoral_u32 SDI_Init(void)
{
    MMC=0;
    RCA=0;
    rGPEUP  = 0xf83f;     // SDCMD, SDDAT[3:0] => PU En.
    rGPECON = 0xaaaaaaaa;	//SDCMD, SDDAT[3:0]
    return SD_card_init();
}

acoral_32 SD_card_init(void)
{
    acoral_u32 i;
    rSDIPRE=PCLK/(INICLK)-1;	// 400KHz
    rSDICON=(0<<4)|1;	// Type A, clk enable
    rSDIFSTA=rSDIFSTA|(1<<16);	//YH 040223 FIFO reset
    rSDIBSIZE=0x200;		// 512byte(128word)
    rSDIDTIMER=0x7fffff;		// Set timeout count

    for(i=0;i<0x1000;i++);    // Wait 74SDCLK for MMC card

    CMD0();
    acoral_printdbg("\nIn idle\n");
	if(Chk_MMC_OCR()) 
	{
		acoral_printdbg("In MMC ready\n");
		MMC=1;
	}else{

		if(Chk_SD_OCR()){
			acoral_printdbg("\nIn SD ready\n");
		}
		else
		{
			acoral_printerr("\nNo Card assertion\n");
			return 0;
		}
	}
    do{
		rSDICARG=0x0;   // CMD2(stuff bit)
		rSDICCON=(0x1<<10)|(0x1<<9)|(0x1<<8)|0x42; //lng_resp, wait_resp, start, CMD2
    }while(!Chk_CMDend(2, 1)); 
    rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    acoral_printdbg("\nEnd id\n");

RECMD3:
    //--Send RCA
    rSDICARG=MMC<<16;	    // CMD3(MMC:Set RCA, SD:Ask RCA-->SBZ)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x43;	// sht_resp, wait_resp, start, CMD3
    //-- Check end of CMD3
    if(!Chk_CMDend(3, 1)) 
	goto RECMD3;
    rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
	
	if(MMC)
	{
		RCA=1;
		rSDIPRE=(PCLK/MMCCLK)-1;	
	}
	else 
    	{
		RCA=( rSDIRSP0 & 0xffff0000 )>>16;
		rSDIPRE=PCLK/(SDCLK)-1;	// Normal clock=25MHz
	}

    //--State(stand-by) check
    if(rSDIRSP0 & 0x1e00!=0x600 )  // CURRENT_STATE check
	goto RECMD3;
    acoral_printdbg("\nIn stand-by\n");
    Card_sel_desel(1);	// Select
	if(!MMC)
		Set_4bit_bus();
	else
		Set_1bit_bus();
    Card_sel_desel(0);	// Select
    return 1;
}

void Card_sel_desel(char sel_desel)
{
    //-- Card select or deselect
    if(sel_desel)
    {
RECMDS7:	
	rSDICARG=RCA<<16;	// CMD7(RCA,stuff bit)
	rSDICCON= (0x1<<9)|(0x1<<8)|0x47;   // sht_resp, wait_resp, start, CMD7

	//-- Check end of CMD7
	if(!Chk_CMDend(7, 1))
	    goto RECMDS7;
	rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

	//--State(transfer) check
	if( rSDIRSP0 & 0x1e00!=0x800 )
	    goto RECMDS7;
    }
    else
    {
RECMDD7:	
	rSDICARG=0<<16;		//CMD7(RCA,stuff bit)
	rSDICCON=(0x1<<8)|0x47;	//no_resp, start, CMD7

	//-- Check end of CMD7
	if(!Chk_CMDend(7, 0))
	    goto RECMDD7;
	rSDICSTA=0x800;	// Clear cmd_end(no rsp)
    }
}

void Rd_Block(acoral_u32 adr,acoral_u32 *buf,acoral_u32 block)
{
    acoral_u32 mode;
    acoral_32 status,rd_cnt;

    rd_cnt=0;    

    Card_sel_desel(1);	// Select
	rSDIFSTA=rSDIFSTA|(1<<16);	// FIFO reset
	rSDIDCON=(2<<22)|(1<<19)|(1<<17)|(Wide<<16)|(1<<14)|(2<<12)|(block<<0);	//YH 040220
		//Word Rx, Rx after cmd, blk, 4bit bus, Rx start, blk num, data start, data transmit mode

    rSDICARG=adr;	// CMD17/18(addr)
	    if(block<2)	// SINGLE_READ
	    {
		do{
			rSDICCON=(0x1<<9)|(0x1<<8)|0x51;    // sht_resp, wait_resp, dat, start, CMD17
		}while(!Chk_CMDend(17, 1));	//-- Check end of CMD17
	    }
	    else	// MULTI_READ
	    {
		do{
			rSDICCON=(0x1<<9)|(0x1<<8)|0x52;    // sht_resp, wait_resp, dat, start, CMD18
		}while(!Chk_CMDend(18, 1));	//-- Check end of CMD18 
	    }

	    rSDICSTA=0xa00;	// Clear cmd_end(with rsp)	    

	    while(rd_cnt<128*block)	// 512*block bytes
	    {
		if((rSDIDSTA&0x20)==0x20) // Check timeout 
		{
		    rSDIDSTA=(0x1<<0x5);  // Clear timeout flag
		}
		status=rSDIFSTA;
		if((status&0x1000)==0x1000)	// Is Rx data?
		{
		    *buf++=rSDIDAT;
		    rd_cnt++;
		}
	    }

    if(!Chk_DATend()) 
		acoral_print("Sdi Read Data Error\n");

	rSDIDCON=rSDIDCON&~(7<<12);		//YH 040220, Clear Data Transfer mode => no operation, Cleata Data Transfer start
	rSDIFSTA=rSDIFSTA&0x200;	//Clear Rx FIFO Last data Ready, YH 040221
    rSDIDSTA=0x10;	// Clear data Tx/Rx end detect

    if(block>1)
    {
		do{
			rSDICARG=0;	    //CMD12(stuff bit)
			rSDICCON=(0x1<<9)|(0x1<<8)|0x4c;//sht_resp, wait_resp, start, CMD12
		}while(!Chk_CMDend(12, 1));
		rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
	}
    Card_sel_desel(0);	// Select
}

void Wt_Block(acoral_u32 adr,acoral_u32 *buf,acoral_u32 block)
{
	acoral_32 status;
	acoral_u32 wt_cnt=0;    
	Card_sel_desel(1);	// Select
//    rSDICON |= rSDICON|(1<<1);	// FIFO reset
	rSDIFSTA=rSDIFSTA|(1<<16);	//YH 040223 FIFO reset
	rSDIDCON=(2<<22)|(1<<20)|(1<<17)|(Wide<<16)|(1<<14)|(3<<12)|(block<<0);	//YH 040220
		    //Word Tx, Tx after rsp, blk, 4bit bus, Tx start, blk num
	rSDICARG=adr;	    // CMD24/25(addr)
	if(block<2)	// SINGLE_WRITE
	    {
		do{
			rSDICCON=(0x1<<9)|(0x1<<8)|0x58;
		}while(!Chk_CMDend(24, 1));	//-- Check end of CMD24
	    }
	    else	// MULTI_WRITE
	    {
			do{
				rSDICCON=(0x1<<9)|(0x1<<8)|0x59;	//sht_resp, wait_resp, dat, start, CMD25
			}while(!Chk_CMDend(25, 1));	//-- Check end of CMD25
	    }

	    rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
	    
	    while(wt_cnt<128*block)
	    {
		status=rSDIFSTA;
		if((status&0x2000)==0x2000) 
		{
		    rSDIDAT=*buf++;
		    wt_cnt++;
			//acoral_print("Block No.=%d, wt_cnt=%d\n",block,wt_cnt);
		}
	    }
    //-- Check end of DATA
	if(!Chk_DATend()) 
		acoral_print("dat error\n");
	rSDIDCON=rSDIDCON&~(7<<12);		//YH 040220, Clear Data Transfer mode => no operation, Cleata Data Transfer start
	rSDIDSTA=0x10;	// Clear data Tx/Rx end

	if(block>1)
	{
		do{
			rSDIDCON=(1<<18)|(1<<17)|(0<<16)|(1<<14)|(1<<12)|(block<<0); 	//YH  040220
			rSDICARG=0x0;	    //CMD12(stuff bit)
			rSDICCON=(0x1<<9)|(0x1<<8)|0x4c;    //sht_resp, wait_resp, start, CMD12
		}while(!Chk_CMDend(12, 1));
		rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
		if(!Chk_BUSYend()) 
			acoral_print("error\n");
		rSDIDSTA=0x08;	//! Should be cleared by writing '1'.
	}
	Card_sel_desel(0);	// Select
}


acoral_32 Chk_CMDend(acoral_32 cmd, acoral_32 be_resp)
//0: Timeout
{
    acoral_32 finish0;

    if(!be_resp)    // No response
    {
    	finish0=rSDICSTA;
		while((finish0&0x800)!=0x800)	// Check cmd end
	    finish0=rSDICSTA;

	rSDICSTA=finish0;// Clear cmd end state

	return 1;
    }
    else	// With response
    {
    	finish0=rSDICSTA;
	while( !( ((finish0&0x200)==0x200) | ((finish0&0x400)==0x400) ))    // Check cmd/rsp end
		finish0=rSDICSTA;

//YH 0624	if(cmd==1 | cmd==9 | cmd==41)	// CRC no check
	if(cmd==1 | cmd==41)	// CRC no check, CMD9 is a long Resp. command.

	{
	    if( (finish0&0xf00) != 0xa00 )  // Check error
	    {
		rSDICSTA=finish0;   // Clear error state

		if(((finish0&0x400)==0x400))
		    return 0;	// Timeout error
    	    }
	    rSDICSTA=finish0;	// Clear cmd & rsp end state
	}
	else	// CRC check
	{
	    if( (finish0&0x1f00) != 0xa00 )	// Check error
	    {
			rSDICSTA=finish0;   // Clear error state
			if(((finish0&0x400)==0x400))
				return 0;	// Timeout error
    	    }
			rSDICSTA=finish0;
	}
	return 1;
    }
}

acoral_32 Chk_DATend(void)
{
    acoral_32 finish;

    finish=rSDIDSTA;
    while( !( ((finish&0x10)==0x10) | ((finish&0x20)==0x20) ))	
	// Chek timeout or data end
	finish=rSDIDSTA;

    if( (finish&0xfc) != 0x10 )
    {
        acoral_print("DATA:finish=0x%x\n", finish);
        rSDIDSTA=0xec;  // Clear error state
        return 0;
    }
    return 1;
}

acoral_32 Chk_BUSYend(void)
{
    acoral_32 finish;

    finish=rSDIDSTA;
    while( !( ((finish&0x08)==0x08) | ((finish&0x20)==0x20) ))
	finish=rSDIDSTA;

    if( (finish&0xfc) != 0x08 )
    {
        acoral_print("DATA:finish=0x%x\n", finish);
        rSDIDSTA=0xf4;  //clear error state
        return 0;
    }
    return 1;
}

void CMD0(void)
{
    //-- Make card idle state 
    rSDICARG=0x0;	    // CMD0(stuff bit)
    rSDICCON=(1<<8)|0x40;   // No_resp, start, CMD0

    //-- Check end of CMD0
    Chk_CMDend(0, 0);
    rSDICSTA=0x800;	    // Clear cmd_end(no rsp)
}

acoral_32 Chk_MMC_OCR(void)
{
    acoral_32 i;


    //-- Negotiate operating condition for MMC, it makes card ready state
    for(i=0;i<300;i++)	//Negotiation time is dependent on CARD Vendors.
    {
		rSDICARG=0xff8000;	    	    //CMD1(SD OCR:2.7V~3.6V)
    	rSDICCON=(0x1<<9)|(0x1<<8)|0x41;    //sht_resp, wait_resp, start, CMD1

		if(Chk_CMDend(1, 1) && (rSDIRSP0>>16)==0x80ff) //YH 0903 [31]:Card Power up status bit (busy)
		{
			rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
			return 1;	// Success
		}
    }
    rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    return 0;		// Fail
}


acoral_32 Chk_SD_OCR(void)
{
    acoral_32 i,j=0;

    //-- Negotiate operating condition for SD, it makes card ready state
    for(i=0;i<500;i++)	//If this time is short, init. can be fail.
    {
    	CMD55();    // Make ACMD

    	rSDICARG=0xff8000;	//ACMD41(SD OCR:2.7V~3.6V)
    	rSDICCON=(0x1<<9)|(0x1<<8)|0x69;//sht_resp, wait_resp, start, ACMD41

	//-- Check end of ACMD41
    	if( Chk_CMDend(41, 1) & rSDIRSP0==0x80ff8000 ) 
	{
	    rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

	    return 1;	// Success	    
	}
    }
    //acoral_print("SDIRSP0=0x%x\n",rSDIRSP0);
    rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    return 0;		// Fail
}

acoral_32 CMD55(void)
{
    //--Make ACMD
    rSDICARG=RCA<<16;			//CMD7(RCA,stuff bit)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x77;	//sht_resp, wait_resp, start, CMD55

    //-- Check end of CMD55
    if(!Chk_CMDend(55, 1)) 
		return 0;

    rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    return 1;
}

acoral_32 CMD13(void)//SEND_STATUS
{
    acoral_32 response0;

    rSDICARG=RCA<<16;			// CMD13(RCA,stuff bit)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x4d;	// sht_resp, wait_resp, start, CMD13

    //-- Check end of CMD13
    if(!Chk_CMDend(13, 1)) 
	return 0;
    //acoral_print("rSDIRSP0=0x%x\n", rSDIRSP0);
    if(rSDIRSP0&0x100)
	//acoral_print("Ready for Data\n");
//    else 
	//acoral_print("Not Ready\n");
    response0=rSDIRSP0;
    response0 &= 0x3c00;
    response0 = response0 >> 9;
    //acoral_print("Current Status=%d\n", response0);
    if(response0==6)
	SDI_Init();

    rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    return 1;
}

acoral_32 CMD9(void)//SEND_CSD
{
    rSDICARG=RCA<<16;				// CMD9(RCA,stuff bit)
    rSDICCON=(0x1<<10)|(0x1<<9)|(0x1<<8)|0x49;	// long_resp, wait_resp, start, CMD9
    acoral_printdbg("\n****CSD register****\n");
    //-- Check end of CMD9
    if(!Chk_CMDend(9, 1)) 
	return 0;
    return 1;
}

void SetBus(void)
{
SET_BUS:
    CMD55();	// Make ACMD
    //-- CMD6 implement
    rSDICARG=Wide<<1;	    //Wide 0: 1bit, 1: 4bit
    rSDICCON=(0x1<<9)|(0x1<<8)|0x46;	//sht_resp, wait_resp, start, CMD55

    if(!Chk_CMDend(6, 1))   // ACMD6
	goto SET_BUS;
    rSDICSTA=0xa00;	    // Clear cmd_end(with rsp)
}

void Set_1bit_bus(void)
{
    Wide=0;
    if(!MMC){
       	SetBus();
    acoral_printdbg("\n****1bit bus****\n");
}
}

void Set_4bit_bus(void)
{
    Wide=1;
    SetBus();
    acoral_printdbg("\n****4bit bus****\n");
}

acoral_dev_ops_t sd_ops;
acoral_32 sd_open(){
	return !SDI_Init();
}

acoral_32 sd_write_block(void *data,acoral_size size,acoral_size offset,acoral_time tm_out){
	acoral_32 ret,sdblock_num;
	acoral_u8 *write_data;
	sdblock_num=size/512;
	write_data=(acoral_u8 *)data;
	for(;sdblock_num>0;sdblock_num--)
	{
		Wt_Block(offset,(acoral_u32 *)write_data,1);
		write_data=write_data+512;
		offset+=512;
	}
	return 0;
}

acoral_32 sd_read_block(void *data,acoral_size size,acoral_size offset,acoral_time tm_out){
	acoral_32 ret,sdblock_num;
	acoral_u8 *write_data;
	sdblock_num=size/512;
	write_data=(acoral_u8 *)data;
	for(;sdblock_num>0;sdblock_num--)
	{
		Rd_Block(offset,(acoral_u32 *)write_data,1);
		write_data=write_data+512;
		offset+=512;
	}
	return 0;
}

void sd_init(){
	sd_ops.write=sd_write_block;
	sd_ops.read=sd_read_block;
	sd_ops.open=sd_open;
	sd_ops.close=NULL;
  	acoral_drv_register(&sd_ops,"disk");
}
