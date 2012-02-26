#ifndef HAL_TIMER_H
#define HAL_TIMER_H
#include"hal_int.h"
#define HAL_TICKS_INTR IRQ_TC11MP_TIMER0_1
#define HAL_TICKS_INIT() hal_ticks_init()
#define HAL_TICKS_DELAY() hal_ticks_delay()
void hal_ticks_init();
#endif
