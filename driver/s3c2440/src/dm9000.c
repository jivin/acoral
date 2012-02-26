
#include "acoral.h"
#include <hal.h>
#include "driver.h"

#include <lwip/pbuf.h>
#include <lwip/netif.h>
#include <lwip/stats.h>
#include <net_drv.h>
#include "../include/dm9000.h"
#include "../include/reg.h"

#define DM_ADD (*((volatile unsigned short *) 0x20000300))
#define DM_CMD (*((volatile unsigned short *) 0x20000304))

#define Printf acoral_print
#define	ClearPending(bit) {\
			rSRCPND = bit;\
			rINTPND = bit;\
			rINTPND;\
		}

void OSEINT7ISR();
acoral_dev_ops_t    hw_dm9k_ops;
static acoral_evt_t dm9k_recv_sem;
static acoral_u32   flag = 0;

/*===========================
 *
 *  时延函数
 */
void udelay(u32_t t)
{
   u32_t i;
   for(;t>0;t--)
   {
     for(i=0;i<100;i++){}
   }
}


/*===========================
 *
 *   中断处理函数
 */
void Eint7_ISR(acoral_vector vector)
{
	
	rEINTMSK |=(1<<vector);
	
	//ethernetif_input(&dm9000);
	acoral_sem_post(&dm9k_recv_sem);
/*	acoral_prints("\n=== isr_exit ===\n");
	len = receivepacket(Buffer);
	Printf("the packet length :%d <--->\r\n",len);
	for(i=0;i<len;i++)
	{
		Printf("%x ",Buffer[i]);
		if((i%5)==4) Printf("\r\n");
	}
*/
    
	rEINTPND |= 0x1<<7;
	ClearPending(0x1<<4);
	

}
/*==========================
 *
 * s3c2440 中断相关寄存器初始化
 */
void IOSetInit(void)
{
	acoral_intr_set_type(IRQ_EINT7,ACORAL_EXPERT_INTR);
	acoral_intr_attach(IRQ_EINT7,Eint7_ISR);
	
	rGPFCON  = (rGPFCON & (~(0x03<<14))) | (0x02<<14);	       
	rEXTINT0 = (rEXTINT0 & (~(0x07<<28))) | (0x01<<28);
	rEINTMSK = rEINTMSK & (~(0x01<<7));         
	
	ClearPending((0x1<<4));
	rINTMSK  = rINTMSK & (~(0x1<<4));
}
/*======================
 *
 *  寄存器写数据
 */
void dm9000_reg_write(u16_t reg, u16_t data)
{  
//	udelay(20);
	DM_ADD = reg;
//	udelay(20);
	DM_CMD = data;
	
}
/*======================
 *
 *  寄存器读数据 8bit
 */
u8_t dm9000_reg_read(u16_t reg)
{
  //  udelay(20);
    DM_ADD = reg;
  //  udelay(20);
    return DM_CMD;
}
/*=====================
 *
 *  寄存器读数据 16bit
 */
u16_t dm9000_reg_read16(u16_t reg)
{
  //  udelay(20);
    DM_ADD = reg;
  //  udelay(20);
    return DM_CMD;
}
/*================================
 *
 *  dm9000 网卡初始化
 */
void hw_init(u8_t * mac_addr)
{
	u32_t i;
	IOSetInit();  
	
	dm9000_reg_write(DM9000_GPCR, 0x01);
	dm9000_reg_write(DM9000_GPR,  0x00);
	udelay(5000);					
	
	dm9000_reg_write(DM9000_NCR,  0x03);
	udelay(3000);			
	dm9000_reg_write(DM9000_NCR,  0x00);
	dm9000_reg_write(DM9000_NCR,  0x03);
	udelay(3000);
	dm9000_reg_write(DM9000_NCR,  0x00);
	
	
	dm9000_reg_write(DM9000_NSR,  0x2c);
	dm9000_reg_write(DM9000_ISR,  0xbf);
	
    dm9000_reg_write(DM9000_RCR,  0x39);
    dm9000_reg_write(DM9000_TCR,  0x00);
    dm9000_reg_write(DM9000_BPTR, 0x3f);
    dm9000_reg_write(DM9000_FCTR, 0x38);
    dm9000_reg_write(DM9000_FCR,  0xff);
    dm9000_reg_write(DM9000_SMCR, 0x00);	
    
	for(i=0; i<6; i++)
		dm9000_reg_write(DM9000_PAR + i, mac_addr[i]);
	
	dm9000_reg_write(DM9000_NSR,  0x2c);
	dm9000_reg_write(DM9000_ISR,  0x3f);
	dm9000_reg_write(DM9000_IMR, 0x81);
	udelay(10000);
	acoral_prints("\r\nDM9000 init");
}

/*================================
 *
 *  数据发送函数
 */
void   hw_sendPacket(struct pbuf *p)
{
	struct pbuf *q;
	u32_t len;
	u32_t i;
	u16_t tmp;
	u8_t* dat;
	u8_t data;;

	
	dm9000_reg_write(DM9000_IMR,0x80);
	
	
	//if (p->tot_len > 1514)
	//	return;

/**/
	// 写帧长
	len = p->tot_len;
	dm9000_reg_write(DM9000_TXPLH, (len>>8) & 0x0ff);
	dm9000_reg_write(DM9000_TXPLL, len & 0x0ff);
	
	// 发送数据
	DM_ADD = DM9000_MWCMD;
	for(q = p; q != NULL; q = q->next)
	{
		dat = (u8_t*) q->payload;
		len = q->len;
		for(i=0; i < (len); i+=2)
		{
			udelay(20);
			tmp = 0x0;
			tmp |= *dat;
			dat++;
			tmp |= *dat<< 8;
			dat ++;
			DM_CMD = tmp;
		}
	}

	//发送到网络
	dm9000_reg_write(DM9000_TCR, 0x01);	
	//等待发送完毕
	while(1)
	{
		udelay(1000);
		data = dm9000_reg_read(DM9000_TCR);//DM9000_NSR
		if((data&0x01) == 0x00) break;
	}
	dm9000_reg_write(DM9000_NSR, 0x2c);	

	/*
		Printf("*********  send  DATA*********\r\n");
        Printf("len:   %d", p->tot_len);
		for (q = p; q!=NULL; q = q->next)
		{
			len = q->len;
			Printf("the packed len: %d <--->\r\n",len);
			dat = (u8_t*)q->payload;
			for(i=0;i<len;i++)
			{
				Printf("%4x ",dat[i]);
				if((i%6)==5) Printf("\r\n");
			}
			Printf("\r\n<*<*<*>*>*>\r\n");

		}
		Printf("\r\n+++++++++++++++++++++\r\n");
	*/		

    dm9000_reg_write(DM9000_IMR, 0x81);	

}


/*================================
 *
 *  数据接收函数
 */
struct pbuf* hw_getPacket (/*struct netif *netif,*/ u32_t* flag)
{
	struct pbuf *p, *q;
	u16_t i,tmp,status,len,plen;
	u8_t  ready;
	u32_t st;
	u8_t* dat;
	
	// test for mem alloc. by pegasus
	//static int num_pbuf_alloc = 0;	
	
	ready = 0;						
	status = 0;					
	len = 0; 				
	*flag = 0;
	
 	if(dm9000_reg_read(DM9000_ISR) & 0x01)          
    {
        dm9000_reg_write(DM9000_ISR, 0x01);	
    }


	ready = dm9000_reg_read16(DM9000_MRCMDX); 
    ready = dm9000_reg_read16(DM9000_MRCMDX);
 
 	if ( (ready&0x0ff) > 0x01)
 	{
 		dm9000_reg_write(DM9000_RCR,  0x39);	//接收控制
 		dm9000_reg_write(DM9000_ISR,  0x3f);	//清除所有中断标志位
 		dm9000_reg_write(DM9000_IMR, 0x81);		//中断使能
 		*flag = 0;
 		return NULL;
 	}
 	
 	if ((ready & 0x001) == 0x00)
 	{
 		*flag = 0;
 		return NULL;
 	}


    DM_ADD=DM9000_MRCMD;
    status = DM_CMD;
    len = DM_CMD;
    if((len < 1522) || (len > 0x40))
    {
    	#if ETH_PAD_SIZE
       		pbuf_header(p, -ETH_PAD_SIZE);
       	#endif
       	
       	//alocate a pbuf chain of pbufs from the pool
       	p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
       	
       	if (p == NULL)
       	{
       	    LINK_STATS_INC(link.memerr);
    		LINK_STATS_INC(link.drop);
    		
			/* dump the err data */
			for(i=0; i < len; i+=2) // 16 bit mode
   		 	{
    			udelay(20);
       			tmp = DM_CMD;
    		}
    		
       		*flag = 1;
       		return p;
       	}
       	// test for mem alloc. by pegasus
       	//acoral_print("\nnum_pbuf_alloc == %d \n", ++num_pbuf_alloc);
       	
       	for (q = p; q != NULL; q = q->next)
       	{
       		dat = (u8_t*)q->payload;
       		plen= q->len;
       		
        	for(i=0; i < plen; i+=2) // 16 bit mode
        	{
           		udelay(20);
            	tmp = DM_CMD;
            	*dat++ = tmp & 0x0ff;
            	*dat++ = (tmp >> 8) & 0x0ff;
			}
		}
		
		/*
		Printf("********* receive ************\r\n");
		
		for (q = p; q!=NULL; q = q->next)
		{
			dat = (u8_t*)q->payload;
			plen= q->len;
			for(i=0;i<plen;i++)
			{
				Printf("%4x ",dat[i]);
				if((i%6)==5) Printf("\r\n");
			}
			Printf("\r\n<*<*<*>*>*>\r\n");

		}
		Printf("\r\n+++++++++++++++++++++\r\n");
		*/
				
		#if ETH_PAD_SIZE
			pbuf_header(p, ETH_PAD_SIZE);
		#endif

		LINK_STATS_INC(link.recv);
		*flag = 1;
		return p;
	}
	
	/* dump the err data */
	for(i=0; i < len; i+=2) // 16 bit mode
    {
    	udelay(20);
       	tmp = DM_CMD;
    }
	*flag = 1;
    return NULL;
}


//=========================
//=========================
//=========================

/**/


acoral_32 hw_dm9k_config(acoral_u32 cmd,void *data,acoral_time tm_out)
{
	if (cmd == NET_DEV_INIT)
	{
		hw_init((u8_t*)data);
	}
	acoral_sem_init(&dm9k_recv_sem, 0);
	return 0;
}

acoral_32 hw_dm9k_read (void *data,acoral_size size,acoral_size offset,acoral_time tm_out)
{
	if (flag == 0){
		 rEINTMSK &=~(1<<7);
		 acoral_sem_pend(&dm9k_recv_sem, 0);
	}
	*(void **)data = (void*)hw_getPacket (/*struct netif *netif,*/ &flag);
	
	return 0;
}

acoral_32 hw_dm9k_write(void *data,acoral_size size,acoral_size offset,acoral_time tm_out)
{
	hw_sendPacket((struct pbuf *)data);
	return 0;
}

void dm9000_init(){
	//acoral_prints("\r\ndm9kinit\r\n");
	hw_dm9k_ops.write  = hw_dm9k_write;
	hw_dm9k_ops.read   = hw_dm9k_read;
	hw_dm9k_ops.config = hw_dm9k_config;
	hw_dm9k_ops.open   = NULL;
	hw_dm9k_ops.close  = NULL;
  	acoral_drv_register(&hw_dm9k_ops,"netdev");
}
