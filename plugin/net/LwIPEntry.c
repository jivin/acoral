#include "lwip/err.h"
#include "arch/cc.h"
#include "lwip/netif.h"

struct netif net_if;
void httpd_thread(void *);
err_t tcpip_input(struct pbuf *p, struct netif *inp);
err_t ethernetif_init(struct netif *netif);
void  tcpip_init(void (* initfunc)(void *), void *arg);
void  ethernetif_input(void *arg);

void  my_lwip_init(void)
{  	
	struct ip_addr ipaddr, netmask, gw;
	u8_t bTemp;

	tcpip_init(NULL, NULL);
	IP4_ADDR(&gw, 192,168,1,1);
	IP4_ADDR(&ipaddr, 192,168,1,2);
	IP4_ADDR(&netmask, 255,255,255,0);
	netif_add(&net_if, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);  
	netif_set_default(&net_if);
	netif_set_up(&net_if);	 
}


void acoral_net_lwip_init()
{
	//acoral_prints("\r\nacoral_net_lwip_init\r\n");
	my_lwip_init();
	sys_thread_new("RecvData", ethernetif_input, &net_if, 512, ETH_RECV_TASK_PPIO);
}
