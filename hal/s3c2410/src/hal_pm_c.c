#include<acoral.h>
#include"../include/hal_cfg.h"
#include"../include/hal_brd.h"
acoral_u32 *regs_save_ptr;
#define SET_ITEM(x) \
	{(x),0}
typedef struct{
	volatile acoral_u32 *reg;
	acoral_u32 val;
}hal_slp_save;
extern HAL_REAL_RESUME(acoral_u32 *regs);
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
static hal_slp_save core_save[] = {
	SET_ITEM(LOCKTIME),
	SET_ITEM(CLKCON),


	SET_ITEM(BWSCON),
	SET_ITEM(BANKCON0),
	SET_ITEM(BANKCON1),
	SET_ITEM(BANKCON2),
	SET_ITEM(BANKCON3),
	SET_ITEM(BANKCON4),
	SET_ITEM(BANKCON5),

	SET_ITEM(CLKDIVN),
	SET_ITEM(MPLLCON),
	SET_ITEM(UPLLCON),
	SET_ITEM(CLKSLOW),
	SET_ITEM(REFRESH),
};

static hal_slp_save irq_save[] = {
	SET_ITEM(EXTINT0),
	SET_ITEM(EXTINT1),
	SET_ITEM(EXTINT2),
	SET_ITEM(EINTFLT0),
	SET_ITEM(EINTFLT1),
	SET_ITEM(EINTFLT2),
	SET_ITEM(EINTFLT3),
	SET_ITEM(EINTMASK),
	SET_ITEM(INTMSK)
};

static hal_slp_save gpio_save[] = {
	SET_ITEM(GPACON),
	SET_ITEM(GPADAT),

	SET_ITEM(GPBCON),
	SET_ITEM(GPBDAT),
	SET_ITEM(GPBUP),

	SET_ITEM(GPCCON),
	SET_ITEM(GPCDAT),
	SET_ITEM(GPCUP),

	SET_ITEM(GPDCON),
	SET_ITEM(GPDDAT),
	SET_ITEM(GPDUP),

	SET_ITEM(GPECON),
	SET_ITEM(GPEDAT),
	SET_ITEM(GPEUP),

	SET_ITEM(GPFCON),
	SET_ITEM(GPFDAT),
	SET_ITEM(GPFUP),

	SET_ITEM(GPGCON),
	SET_ITEM(GPGDAT),
	SET_ITEM(GPGUP),

	SET_ITEM(GPHCON),
	SET_ITEM(GPHDAT),
	SET_ITEM(GPHUP),

	SET_ITEM(DCLKCON),
};

static hal_slp_save uart_save[] = {
	SET_ITEM(ULCON0),
	SET_ITEM(UCON0),
	SET_ITEM(UFCON0),
	SET_ITEM(UMCON0),
	SET_ITEM(UBRDIV0),
	SET_ITEM(ULCON1),
	SET_ITEM(UCON1),
	SET_ITEM(UFCON1),
	SET_ITEM(UMCON1),
	SET_ITEM(UBRDIV1)
};

void hal_reg_array_save(hal_slp_save *ptr,acoral_u32 count){
	 for (; count > 0; count--, ptr++) {
		ptr->val = *(ptr->reg);
	}
}
void hal_reg_array_restore(hal_slp_save *ptr,acoral_u32 count){
	 for (; count > 0; count--, ptr++) {
		*(ptr->reg)=ptr->val;
	}
}


acoral_u32 hal_susp_prepare(){
	return 0;
}

acoral_u32 hal_susp_enter(){
	acoral_u32 regs[16];
	acoral_sr cpu_sr;
	HAL_ENTER_CRITICAL();
	regs_save_ptr=regs;
	rGSTATUS3=(acoral_u32)HAL_REAL_RESUME;
	hal_reg_array_save(core_save, ARRAY_SIZE(core_save));
	hal_reg_array_save(gpio_save, ARRAY_SIZE(gpio_save));
	hal_reg_array_save(irq_save, ARRAY_SIZE(irq_save));
	hal_reg_array_save(uart_save, ARRAY_SIZE(uart_save));
	rGPGCON=0x22;//key1,key2允许唤醒
	rINTMSK=0xffffffffL;
	rEINTMSK=0xffffffffL;
	rEINTPND=rEINTPND;
	acoral_prints("Enter Sleep\n");
	rCLKCON=0x0;
	flush_cache();//将cache内容写回mem,防止掉电的时候及恢复的时候丢失
	HAL_REAL_SLEEP(regs);
	int i=rEINTPND;
	hal_reg_array_restore(uart_save, ARRAY_SIZE(uart_save));
	hal_reg_array_restore(core_save, ARRAY_SIZE(core_save));
	hal_reg_array_restore(irq_save, ARRAY_SIZE(irq_save));
	hal_reg_array_restore(gpio_save, ARRAY_SIZE(gpio_save));
	acoral_prints("Wake up from Sleep\n");
	acoral_print("0x%x\n",rINTPND);
	acoral_print("0x%x\n",i);
	acoral_print("0x%x\n",rINTOFFSET);
	HAL_EXIT_CRITICAL();
}

acoral_u32 hal_susp_finish(){
	return 0;
}
