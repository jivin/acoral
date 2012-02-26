#ifndef HAL_BRD_REG_H
#define HAL_BRD_REG_H
#include "hal_brd_cfg.h"
#define  rSCCTRL  (*(volatile acoral_u32 *)(SC_BASE+SC_CTRL))
#define  rTCNT0 (*(volatile acoral_u32 *)(TIMER0_1_BASE+TIMER_CONTROL))
#define  rTLD0 (*(volatile acoral_u32 *)(TIMER0_1_BASE+TIMER_LOAD))
#define  rSCUCTRL  (*(volatile acoral_u32 *)(SCU_BASE+SCU_CTRL))
#define  rSYSFLGSET  (*(volatile acoral_u32 *)(BRD_BASE+SYS_FLAGSSET))
#endif
