/*
 * dm9000 Ethernet
 */ 
#ifndef __DM9000_H 
#define __DM9000_H 

#include "hal_2440_c.h"
#include "acoral.h"
#include "arch/cc.h"
#define DM9000_ID		0x90000A46

/* types for dm9000a
 */
//typedef acoral_u8    u8_t;
//typedef acoral_8     s8_t;
//typedef acoral_u16   u16_t;
//typedef acoral_16    s16_t;
//typedef acoral_u32   u32_t;
//typedef acoral_32    s32_t;

/* although the registers are 16 bit, they are 32-bit aligned.
 */

#define DM9000_NCR             0x00
#define DM9000_NSR             0x01
#define DM9000_TCR             0x02
#define DM9000_TSR1            0x03
#define DM9000_TSR2            0x04
#define DM9000_RCR             0x05
#define DM9000_RSR             0x06
#define DM9000_ROCR            0x07
#define DM9000_BPTR            0x08
#define DM9000_FCTR            0x09
#define DM9000_FCR             0x0A
#define DM9000_EPCR            0x0B
#define DM9000_EPAR            0x0C
#define DM9000_EPDRL           0x0D
#define DM9000_EPDRH           0x0E
#define DM9000_WCR             0x0F

#define DM9000_PAR             0x10
#define DM9000_MAR             0x16

#define DM9000_GPCR			   0x1e
#define DM9000_GPR			   0x1f
#define DM9000_TRPAL           0x22
#define DM9000_TRPAH           0x23
#define DM9000_RWPAL           0x24
#define DM9000_RWPAH           0x25

#define DM9000_VIDL            0x28
#define DM9000_VIDH            0x29
#define DM9000_PIDL            0x2A
#define DM9000_PIDH            0x2B

#define DM9000_CHIPR           0x2C
#define DM9000_SMCR            0x2F

#define DM9000_MRCMDX          0xF0
#define DM9000_MRCMD           0xF2
#define DM9000_MRRL            0xF4
#define DM9000_MRRH            0xF5
#define DM9000_MWCMDX          0xF6
#define DM9000_MWCMD           0xF8
#define DM9000_MWRL            0xFA
#define DM9000_MWRH            0xFB
#define DM9000_TXPLL           0xFC
#define DM9000_TXPLH           0xFD
#define DM9000_ISR             0xFE
#define DM9000_IMR             0xFF


/*
typedef struct eth_hdr 			//以太网头部结构，为了以后使用方便
{
	uint8	d_mac[6];   		//目的地址
	uint8	s_mac[6];   		//源地址
	uint16	type;    			//协议类型
}ETH_HDR;
typedef struct _arp_hdr 		//以太网头部+ARP首部结构
{
	ETH_HDR ethhdr;				//以太网首部
	uint16	hwtype;     		//硬件类型(1表示传输的是以太网MAC地址)
	uint16	protocol;			//协议类型(0x0800表示传输的是IP地址)
	uint8	hwlen;				//硬件地址长度(6)
	uint8	protolen;			//协议地址长度(4)
	uint16	opcode; 			//操作(1表示ARP请求,2表示ARP应答)
	uint8	smac[6];			//发送端MAC地址
	uint8	sipaddr[4];			//发送端IP地址
	uint8	dmac[6];			//目的端MAC地址
	uint8	dipaddr[4];			//目的端IP地址
}ARP_HDR;

typedef struct ip_hdr			//以太网头部+IP首部结构
{
	ETH_HDR ethhdr;    			//以太网首部
	uint8	vhl;      			//4位版本号4位首部长度(0x45)
	uint8	tos;				//服务类型(0)
	uint16	len;				//整个IP数据报总字节长度
	uint16	ipid;           	//IP标识
	uint16	ipoffset;     		//3位标识13位偏移
	uint8 	ttl;             	//生存时间(32或64)
	uint8	proto;         		//协议(1表示ICMP,2表示IGMP,6表示TCP,17表示UDP)
	uint16 	ipchksum;    		//首部校验和
	uint8 	srcipaddr[4];    	//源IP
	uint8	destipaddr[4];   	//目的IP
}IP_HDR;
*/

 
void         hw_init(u8_t* mac_addr);
void         hw_sendPacket(struct pbuf *p);
struct pbuf* hw_getPacket (/*struct netif *netif,*/ u32_t* flag);


#endif /* _DM9000X_H_ */
