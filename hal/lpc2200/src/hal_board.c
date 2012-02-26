#include"../include/hal_brd.h"
#define IniLED_DiskBe()			IO2DIR  |=1<<28;IO2SET=1<<28
#define SetLED_DiskBe()			IO2SET   =1<<28
#define ClrLED_DiskBe()			IO2CLR   =1<<28
void led()
{
IniLED_DiskBe();
ClrLED_DiskBe();
}
void hal_board_init(void){

    BCFG0 = 0x10001460;
    BCFG1 = 0x10000460;
  	 MEMMAP = 0x3;
  	 /* 设置系统各部分时钟 */
/* Set system timers for each component */
    PLLCON = 1;
#if (Fpclk / (Fcclk / 4)) == 1
    VPBDIV = 0;
#endif
#if (Fpclk / (Fcclk / 4)) == 2
    VPBDIV = 2;
#endif
#if (Fpclk / (Fcclk / 4)) == 4
    VPBDIV = 1;
#endif

#if (Fcco / Fcclk) == 2
    PLLCFG = ((Fcclk / Fosc) - 1) | (0 << 5);
#endif
#if (Fcco / Fcclk) == 4
    PLLCFG = ((Fcclk / Fosc) - 1) | (1 << 5);
#endif
#if (Fcco / Fcclk) == 8
    PLLCFG = ((Fcclk / Fosc) - 1) | (2 << 5);
#endif
#if (Fcco / Fcclk) == 16
    PLLCFG = ((Fcclk / Fosc) - 1) | (3 << 5);
#endif
    PLLFEED = 0xaa;
    PLLFEED = 0x55;
    while((PLLSTAT & (1 << 10)) == 0);
    PLLCON = 3;
    PLLFEED = 0xaa;
    PLLFEED = 0x55;
    
    /* 设置存储器加速模块 */
    /* Set memory accelerater module*/
    MAMCR = 0;
#if Fcclk < 20000000
    MAMTIM = 1;
#else
#if Fcclk < 40000000
    MAMTIM = 2;
#else
    MAMTIM = 3;
#endif
#endif
    MAMCR = 2;
}
