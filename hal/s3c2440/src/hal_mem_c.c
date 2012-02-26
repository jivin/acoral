#include<type.h>
#include"hal_2440_cfg.h"
extern acoral_u32 MMU_base[];
extern acoral_u32 __ENTRY[];
static void hal_mmu_init(void);

void hal_mem_init(){
  	hal_mmu_init();
}

void flush_cache(){
    acoral_32 i,j;
    MMU_DisableDCache();
    MMU_DisableICache();
    /*If write-back is used,the DCache should be cleared.*/
    for(i=0;i<64;i++)
    	for(j=0;j<8;j++)
    	    MMU_CleanInvalidateDCacheIndex((i<<26)|(j<<5));
    MMU_InvalidateICache();

}
static void hal_mmu_init(void)
{
    /*========================== IMPORTANT NOTE =========================*/
    /*The current stack and code area can't be re-mapped in this routine.*/
    /*If you want memory map mapped freely, your own sophiscated MMU*/
    /*initialization code is needed.*/
    /*===================================================================*/

    flush_cache(); 
    MMU_DisableMMU();
    MMU_InvalidateTLB();
    /*hal_mmu_setmtt(acoral_32 vaddrStart,acoral_32 vaddrEnd,acoral_32 paddrStart,acoral_32 attr)*/
    /*hal_mmu_setmtt(0x00000000,0x07f00000,0x00000000,RW_CNB);  bank0*/
    hal_mmu_setmtt(0x00000000,0x03f00000,(acoral_32)__ENTRY,RW_CB);  /*bank0*/
    hal_mmu_setmtt(0x04000000,0x07f00000,0,RW_NCNB);  			/*bank0*/
    hal_mmu_setmtt(0x08000000,0x0ff00000,0x08000000,RW_CNB);  /*bank1*/
    hal_mmu_setmtt(0x10000000,0x17f00000,0x10000000,RW_NCNB); /*bank2*/
    hal_mmu_setmtt(0x18000000,0x1ff00000,0x18000000,RW_NCNB); /*bank3*/
    /*hal_mmu_setmtt(0x20000000,0x27f00000,0x20000000,RW_CB); bank4*/
    hal_mmu_setmtt(0x20000000,0x27f00000,0x20000000,RW_NCNB); /*bank4 for STRATA Flash*/
    hal_mmu_setmtt(0x28000000,0x2ff00000,0x28000000,RW_NCNB); /*bank5*/
    /*30f00000->30100000, 31000000->30200000*/
    hal_mmu_setmtt(0x30000000,0x30100000,0x30000000,RW_CNB);	  /*bank6-1*/
    hal_mmu_setmtt(0x30200000,0x33e00000,0x30200000,RW_CNB); /*bank6-2*/
    /**/
    hal_mmu_setmtt(0x33f00000,0x33f00000,0x33f00000,RW_NCNB);   /*bank6-3*/
    hal_mmu_setmtt(0x38000000,0x3ff00000,0x38000000,RW_NCNB); /*bank7*/

    hal_mmu_setmtt(0x40000000,0x47f00000,0x40000000,RW_NCNB); /*SFR*/
    hal_mmu_setmtt(0x48000000,0x5af00000,0x48000000,RW_NCNB); /*SFR*/
    hal_mmu_setmtt(0x5b000000,0x5b000000,0x5b000000,RW_NCNB); /*SFR*/
    hal_mmu_setmtt(0x5b100000,0xfff00000,0x5b100000,RW_FAULT);/*not used*/


    MMU_SetTTBase(&MMU_base);
    MMU_SetDomain(0x55555550|DOMAIN1_ATTR|DOMAIN0_ATTR);
    	/*DOMAIN1: no_access, DOMAIN0,2~15=client(AP is checked)*/
    MMU_SetProcessId(0x0);
    MMU_EnableAlignFault();
    MMU_EnableMMU();
    MMU_EnableICache();
    MMU_EnableDCache(); /*DCache should be turned on after MMU is turned on.*/
}


void hal_mmu_setmtt(acoral_32 vaddrStart,acoral_32 vaddrEnd,acoral_32 paddrStart,acoral_32 attr)
{
    volatile acoral_u32 *pTT;
    volatile acoral_32 i,nSec;
    pTT=MMU_base+(vaddrStart>>20);
    nSec=(vaddrEnd>>20)-(vaddrStart>>20);
    for(i=0;i<=nSec;i++)*pTT++=attr |(((paddrStart>>20)+i)<<20);
}
