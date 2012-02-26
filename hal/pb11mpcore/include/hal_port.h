#ifndef HAL_PORT_H
#define HAL_PORT_H
#include "autocfg.h"

#include "hal_thread.h"
#include "hal_mem.h"
#include "hal_int.h"
#include "hal_timer.h"
#include "hal_board.h"
#include "hal_atomic.h"

#ifdef CFG_CMP
#include"hal_cmp.h"
#include"hal_ipi.h"
#include"hal_spinlock.h"
#endif

#include"hal_brd_reg.h"
#include"hal_brd_cfg.h"
#endif
