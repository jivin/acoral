/*
;*****************************************************************************************************
;*    							Copyright (c) 2006 by JiaoJinXing. 
;*									All rights reserved.
;*
;*---- 文件信息 --------------------------------------------------------------------------------------
;*文   件   名 : ethernetif.c
;*创   建   人 : 焦进星
;*描        述 : 以太网接口文件
;*
;*---- 历史版本信息 ----------------------------------------------------------------------------------
;* 日　		期 : 2009年 5 月 17 日
;* 创	建	人 : 焦进星
;* 描　		述 : 建立版本 V1.0.0
;*
;*---- 联系方式 --------------------------------------------------------------------------------------
;*	
;* Email 	   : wyoujtg@163.com
;* Web 		   : http://user.qzone.qq.com/376637405
;*		    
;*****************************************************************************************************
;*/

#include "driver.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/lwmem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "netif/etharp.h"
//#include "netif/ppp_oe.h"

#include <net_drv.h>

/* Define those to better describe your network interface. */
#define IFNAME0   'e'
#define IFNAME1   'n'
static acoral_id net_dev_id=ACORAL_DEV_ERR_ID;

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/* Forward declarations. */
void ethernetif_input(void *arg);

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init(struct netif *netif)
{
	/* set MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;

	/* set MAC hardware address */
	/* MAC地址 */
	netif->hwaddr[0] = 0x00;
	netif->hwaddr[1] = 0x11;
	netif->hwaddr[2] = 0x22;
	netif->hwaddr[3] = 0x33;
	netif->hwaddr[4] = 0x44;     
	netif->hwaddr[5] = 0x55;

	/* maximum transfer unit */
	/* 最大传输单元 */
	netif->mtu = 1500;
  
	/* device capabilities */
	/* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
 
	/* Do whatever else is needed to initialize interface. */
	/* 初始化enc28j60 */
	
	// pegasus0809
	// hw_init(netif->hwaddr);
		
	net_dev_id = acoral_dev_open("netdev");
	if (net_dev_id == ACORAL_DEV_ERR_ID)
	{
		acoral_printerr("Not Found Net Device\n");
		return;
	}
	acoral_dev_config(net_dev_id, NET_DEV_INIT, netif->hwaddr, 0);
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
#if ETH_PAD_SIZE
	pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
	//acoral_prints("\r\nlowLOutput=sta\r\n");
	//hw_sendPacket(p);
	acoral_dev_write(net_dev_id, p, 0, 0, 0);
	//acoral_prints("\r\nlowLOutput=end\r\n");
#if ETH_PAD_SIZE
	pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
  
	LINK_STATS_INC(link.xmit);

	return ERR_OK;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input(void *arg)
{
	struct eth_hdr *ethhdr;
	struct pbuf *p;
	
	struct netif *netif = (struct netif *)arg;
	
	if(net_dev_id==ACORAL_DEV_ERR_ID)
			return;
	do
	{
		/* move received packet into a new pbuf */
		//p = hw_getPacket(/*netif,*/ &flag);
		
		acoral_dev_read(net_dev_id, &p, 0, 0,0);
		
		/* no packet could be read, silently ignore this */
		if (p == NULL) 
			continue;
		/* points to packet payload, which starts with an Ethernet header */
		ethhdr = p->payload;

		switch (htons(ethhdr->type)) 
		{
			/* IP or ARP packet? */
			case ETHTYPE_IP:
			case ETHTYPE_ARP:
			#if PPPOE_SUPPORT
			/* PPPoE packet? */
			case ETHTYPE_PPPOEDISC:
			case ETHTYPE_PPPOE:
			#endif /* PPPOE_SUPPORT */
			/* full packet send to tcpip_thread to process */
			if (netif->input(p, netif)!=ERR_OK)
			{ 
				
				LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
				pbuf_free(p);
                               // acoral_delay_self(400); 
				p = NULL;
			}
			break;

		    default:
			pbuf_free(p);
			p = NULL;
			break;
		}
	}while(1);
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t 
ethernetif_init(struct netif *netif)
{
	struct ethernetif *ethernetif;


	LWIP_ASSERT("netif != NULL", (netif != NULL));
    
	ethernetif = mem_malloc(sizeof(struct ethernetif));
	if (ethernetif == NULL) 
	{
		LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
		return ERR_MEM;
	}

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

	/*
	 * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
	//下面这句不知为什么,先注释掉
	//NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, ???);

	netif->state = ethernetif;
	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;
	/* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) 
	 */
	netif->output = etharp_output;
	netif->linkoutput = low_level_output;
  
	ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
  
	/* initialize the hardware */
	low_level_init(netif);

	return ERR_OK;
}

/*
;*****************************************************************************************************
;*                            			End Of File
;*****************************************************************************************************
;*/
