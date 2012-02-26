#include "hal_2440_cfg.h"

void nand_init(void);
int nand_read(unsigned char *buf, unsigned long start_addr, int size);

static void nand_wait(void)
{
  int i;
  while (!(NFSTAT & NFSTAT_BUSY))
   for (i=0; i<10; i++);
}


static void nand_reset(){
   int i;

   nand_select();
   NFCMD=NAND_CMD_RESET;
   for(i=0;i<10;i++);  
   nand_wait();  
   nand_deselect();
}


void nand_init(void){
   

    NFCONF=(7<<12)|(7<<8)|(7<<4)|(0<<0);
    NFCONT=(1<<4)|(0<<1)|(1<<0);
    
    nand_reset();
}


struct boot_nand_t{
    int page_size;
    int block_size;
    int bad_block_offset;
};


static int is_bad_block(struct boot_nand_t * nand, unsigned long i)
{
	unsigned char data;
	unsigned long page_num;

	nand_clear_RnB();
	if (nand->page_size == 512) {
		NFCMD = NAND_CMD_READOOB; /* 0x50 */
		NFADDR = nand->bad_block_offset & 0xf;
		NFADDR = (i >> 9) & 0xff;
		NFADDR = (i >> 17) & 0xff;
		NFADDR = (i >> 25) & 0xff;
	} else if (nand->page_size == 2048) {
		page_num = i >> 11; /* addr / 2048 */
		NFCMD = NAND_CMD_READ0;
		NFADDR = nand->bad_block_offset & 0xff;
		NFADDR = (nand->bad_block_offset >> 8) & 0xff;
		NFADDR = page_num & 0xff;
		NFADDR = (page_num >> 8) & 0xff;
		NFADDR = (page_num >> 16) & 0xff;
		NFCMD = NAND_CMD_READSTART;
	} else {
		return -1;
	}
	nand_wait();
	data = (NFDATA & 0xff);
	if (data != 0xff)
		return 1;

	return 0;
}

static int nand_read_page(struct boot_nand_t * nand, unsigned char *buf, unsigned long addr)
{
	unsigned short *ptr16 = (unsigned short *)buf;
	unsigned int i, page_num;

	nand_clear_RnB();

	NFCMD = NAND_CMD_READ0;

	if (nand->page_size == 512) {
		/* Write Address */
		NFADDR = addr & 0xff;
		NFADDR = (addr >> 9) & 0xff;
		NFADDR = (addr >> 17) & 0xff;
		NFADDR = (addr >> 25) & 0xff;
	} else if (nand->page_size == 2048) {
		page_num = addr >> 11; /* addr / 2048 */
		/* Write Address */
		NFADDR = 0;
		NFADDR = 0;
		NFADDR = page_num & 0xff;
		NFADDR = (page_num >> 8) & 0xff;
		NFADDR = (page_num >> 16) & 0xff;
		NFCMD = NAND_CMD_READSTART;
	} else {
		return -1;
	}
	nand_wait();
	for (i = 0; i < (nand->page_size>>1); i++) {
		*ptr16 = NFDATA16;
		ptr16++;
	}

	return nand->page_size;
}


static unsigned short nand_read_id()
{
	unsigned short res = 0;
	NFCMD = NAND_CMD_READID;
	NFADDR = 0;
	res = NFDATA;
	res = (res << 8) | NFDATA;
	return res;
}



int nand_read(unsigned char *buf, unsigned long start_addr, int size)
{

       

	int i, j;
	unsigned short nand_id;
	struct boot_nand_t nand;

	
	nand_select();
	nand_clear_RnB();
	
	for (i = 0; i < 10; i++);

	nand_id = nand_read_id();	

       if (nand_id == 0xec76 ||		/* Samsung K91208 */
           nand_id == 0xad76 ) {	/*Hynix HY27US08121A*/
		nand.page_size = 512;
		nand.block_size = 16 * 1024;
		nand.bad_block_offset = 5;
	} 
        else if (nand_id == 0xecf1 ||	/* Samsung K9F1G08U0B */
		   nand_id == 0xecda ||	/* Samsung K9F2G08U0B */
		   nand_id == 0xecd3 )	{ /* Samsung K9K8G08 */
		nand.page_size = 2048;
		nand.block_size = 128 * 1024;
		nand.bad_block_offset = nand.page_size;
	} 
        else {
		return -1; 
	}

         if ((start_addr & (nand.block_size-1)))
		return -1;	
        
        if(size & (nand.page_size-1)){
             size=(size+nand.page_size-1) & (~(nand.page_size-1));
        }

        if ((size & (nand.page_size-1)))
		return -1;

	for (i=start_addr; i < (start_addr + size);) {

		if ((i & (nand.block_size-1))== 0) {
			if (is_bad_block(&nand, i) || is_bad_block(&nand, i + nand.page_size)) {
				i += nand.block_size;
				size += nand.block_size;
				continue;
			}
		}

		j = nand_read_page(&nand, buf, i);
		i += j;
		buf += j;
	}


	nand_deselect();

	return 0;
}
