#ifndef HAL_BRD_CFG_H
#define HAL_BRD_CFG_H
#define MODE_MASK        0x1F
#define MODE_USER        0x10
#define MODE_FIQ         0x11
#define MODE_IRQ         0x12
#define MODE_SVC         0x13
#define MODE_ABORT       0x17
#define MODE_UNDEF       0x1B
#define MODE_SYSTEM      0x1F
#define PSR_T_BIT        0x20
#define PSR_F_BIT        0x40
#define PSR_I_BIT        0x80
#define NOINT            0xc0

/*MMU Controller*/
#define    CTRL_M_BIT       (1 << 0)
#define    CTRL_C_BIT       (1 << 2)
#define    CTRL_B_BIT       (1 << 7)
#define    CTRL_I_BIT       (1 << 12)

/*peripher*/
#define    BRD_BASE         0x10000000
#define    BRD_ID           0x00
#define    BRD_LED          0x08
#define    BRD_OSC0         0x0C
#define    BRD_LOCK         0x20
#define    BRD_CFGDATA2     0x2C        /* Only used on PB\AB926*/
#define    BRD_FLAGS        0x30
#define    BRD_BOOTCS       0x58
#define    BRD_MISC         0x60
#define    BRD_IOSEL        0x70
#define    BRD_PLD_CTRL2    0x78
#define    BRD_BUSID        0x80
#define    BRD_PROCID1      0x84
#define    BRD_PROCID2      0x88

/* System controller definitions*/
#define    SC_BASE          0x10001000
#define    SC_CTRL          0x00
#define    SC_REMAP_STAT    0x200
#define    SC_REMAP         0x100
#define    SC_CPU_BASE      0x1F000000
#define    SC_CPU_CONFIG    0x4


/* Static memory controller*/
#define    SSMC_BASE                    0x100E1000

/* Static memory configuation definitions for SSMC*/
#define    SMC_DIRECT_CMD               0x10
#define    SMC_SET_CYCLES               0x14
#define    SMC_SET_OPMODE               0x18

/* Dynamic memory controller*/
#define    DMC_BASE         0x100E0000  

/* Real Time clock definitions*/
#define    RTC_BASE         0x10017000  

/* General UART definitions*/
#define    UART0_BASE       0x10009000  

#define UART_PL01x_DR                   0x00	 /*  Data read or written from the interface. */
#define UART_PL01x_RSR                  0x04	 /*  Receive status register (Read). */
#define UART_PL01x_ECR                  0x04	 /*  Error clear register (Write). */
#define UART_PL01x_FR                   0x18	 /*  Flag register (Read only). */
#define    UART_PL011_IBRD  0x24
#define    UART_PL011_FBRD  0x28
#define    UART_PL011_LCRH  0x2C
#define    UART_PL011_CR    0x30
#define UART_CLOCK           24000000



#define    NOR_FLASH        0x40000000
#define    NOR_FLASH_EXP    0x44000000
#define    SSRAM_BASE       0x48000000
#define    EXP_FLASH        0x50000000
#define    SDRAM_BASE       0x70000000

/* Misc definitions*/
#define    BRD_UNLOCK_KEY   0xA05F

/*Static memory controller register definitions used on PB1176*/
#define    SSMC_STATICCS0_IDCYR               0x00
#define    SSMC_STATICCS0_WSTRDR              0x04
#define    SSMC_STATICCS0_WSTWRR              0x08
#define    SSMC_STATICCS0_WSTOENR             0x0C
#define    SSMC_STATICCS0_WSTWENR             0x10
#define    SSMC_STATICCS0_CR                  0x14
#define    SSMC_STATICCS0_SR                  0x18
#define    SSMC_STATICCS0_WSTBRDR             0x1C
#define    SSMC_STATICCS1_IDCYR               0x20
#define    SSMC_STATICCS1_WSTRDR              0x24
#define    SSMC_STATICCS1_WSTWRR              0x28
#define    SSMC_STATICCS1_WSTOENR             0x2C
#define    SSMC_STATICCS1_WSTWENR             0x30
#define    SSMC_STATICCS1_CR                  0x34
#define    SSMC_STATICCS1_SR                  0x38
#define    SSMC_STATICCS1_WSTBRDR             0x3C
#define    SSMC_STATICCS2_IDCYR               0x40
#define    SSMC_STATICCS2_WSTRDR              0x44
#define    SSMC_STATICCS2_WSTWRR              0x48
#define    SSMC_STATICCS2_WSTOENR             0x4C
#define    SSMC_STATICCS2_WSTWENR             0x50
#define    SSMC_STATICCS2_CR                  0x54
#define    SSMC_STATICCS2_SR                  0x58
#define    SSMC_STATICCS2_WSTBRDR             0x5C
#define    SSMC_STATICCS3_IDCYR               0x60
#define    SSMC_STATICCS3_WSTRDR              0x64
#define    SSMC_STATICCS3_WSTWRR              0x68
#define    SSMC_STATICCS3_WSTOENR             0x6C
#define    SSMC_STATICCS3_WSTWENR             0x70
#define    SSMC_STATICCS3_CR                  0x74
#define    SSMC_STATICCS3_SR                  0x78
#define    SSMC_STATICCS3_WSTBRDR             0x7C
#define    SSMC_STATICCS4_IDCYR               0x80
#define    SSMC_STATICCS4_WSTRDR              0x84
#define    SSMC_STATICCS4_WSTWRR              0x88
#define    SSMC_STATICCS4_WSTOENR             0x8C
#define    SSMC_STATICCS4_WSTWENR             0x90
#define    SSMC_STATICCS4_CR                  0x94
#define    SSMC_STATICCS4_SR                  0x98
#define    SSMC_STATICCS4_WSTBRDR             0x9C
#define    SSMC_STATICCS5_IDCYR               0xA0
#define    SSMC_STATICCS5_WSTRDR              0xA4
#define    SSMC_STATICCS5_WSTWRR              0xA8
#define    SSMC_STATICCS5_WSTOENR             0xAC
#define    SSMC_STATICCS5_WSTWENR             0xB0
#define    SSMC_STATICCS5_CR                  0xB4
#define    SSMC_STATICCS5_SR                  0xB8
#define    SSMC_STATICCS5_WSTBRDR             0xBC
#define    SSMC_STATICCS6_IDCYR               0xC0
#define    SSMC_STATICCS6_WSTRDR              0xC4
#define    SSMC_STATICCS6_WSTWRR              0xC8
#define    SSMC_STATICCS6_WSTOENR             0xCC
#define    SSMC_STATICCS6_WSTWENR             0xD0
#define    SSMC_STATICCS6_CR                  0xD4
#define    SSMC_STATICCS6_SR                  0xD8
#define    SSMC_STATICCS6_WSTBRDR             0xDC
#define    SSMC_STATICCS7_IDCYR               0xE0
#define    SSMC_STATICCS7_WSTRDR              0xE4
#define    SSMC_STATICCS7_WSTWRR              0xE8
#define    SSMC_STATICCS7_WSTOENR             0xEC
#define    SSMC_STATICCS7_WSTWENR             0xF0
#define    SSMC_STATICCS7_CR                  0xF4
#define    SSMC_STATICCS7_SR                  0xF8
#define    SSMC_STATICCS7_WSTBRDR             0xFC

/*dyamic memory controller register*/
#define   DMC_STATUS               0x00        /* Status register*/
#define   DMC_COMMAND              0x04        /* Command register*/
#define   DMC_DIRECT_CMD           0x08        /* Direct Command register*/
#define   DMC_MEMORY_CONFIG        0x0C        /* Memory Configuration register*/
#define   DMC_REFRESH_PRD          0x10        /* Refresh Period register*/
#define   DMC_CAS_LATENCY          0x14        /* CAS Latency Register*/
#define   DMC_T_DQSS               0x18        /* t_dqss register*/
#define   DMC_T_MRD                0x1C        /* t_mrd register*/
#define   DMC_T_RAS                0x20        /* t_ras register*/
#define   DMC_T_RC                 0x24        /* t_rc register*/
#define   DMC_T_RCD                0x28        /* t_rcd register*/
#define   DMC_T_RFC                0x2C        /* t_rfc register*/
#define   DMC_T_RP                 0x30        /* t_rp register*/
#define   DMC_T_RRD                0x34        /* t_rrd register*/
#define   DMC_T_WR                 0x38        /* t_wr register*/
#define   DMC_T_WTR                0x3C        /* t_wtr register*/
#define   DMC_T_XP                 0x40        /* t_xp register*/
#define   DMC_T_XSR                0x44        /* t_xsr register*/
#define   DMC_T_ESR                0x48        /* t_esr register*/
#define   DMC_ID_0_CFG             0x100       /* id_cfg registers*/
#define   DMC_CHIP_0_CFG           0x200       /* chip_cfg 0 registers*/
#define   DMC_CHIP_1_CFG           0x204       /* chip_cfg 1 registers*/
#define   DMC_PLL                  0x3000

/*time_register*/
#define TIMER0_1_BASE   0x10011000
#define TIMER2_3_BASE   0x10012000
#define TIMER_LOAD              0x00
#define TIMER_VALUE             0x04
#define TIMER_CONTROL           0x08
#define TIMER_INTCLR            0x0C
#define TIMER_RIS               0x10
#define TIMER_MIS               0x14
#define TIMER_BGLOAD            0x18

#define TIMER_ENABLED           0x80
#define TIMER_INT_ENABLED       0x20
#define TIMER_32BIT_OPERATION   0x02    /* Thirty two bit counter mode*/
#define MPCORE_CPU_INTERFACE    0x1F000100
#define CONTROL_REGISTER        0x0
#define PRIORITY_MASK_REGISTER  0x4
#define TIMER_MODE_PERIODIC     0x40    /* Counter operates continously reloading from the load register on reaching zero*/
/*
 * SCU registers
 */
#define SCU_BASE		0x1F000000	/* IRQ, Test chip */
#define SCU_CTRL		0x00
#define SCU_CONFIG		0x04
#define SCU_CPU_STATUS		0x08
#define SCU_INVALIDATE		0x0c
#define SCU_FPGA_REVISION	0x10
/*
* SYS_FLAGS
*/
#define SYS_FLAGSSET         0x30

/*
 * Testchip peripheral and fpga gic regions
 */
#define REALVIEW_TC11MP_GIC_CPU_BASE		0x1F000100	/* Test chip interrupt controller CPU interface */
#define REALVIEW_TC11MP_GIC_DIST_BASE		0x1F001000	/* Test chip interrupt controller distributor */

#define REALVIEW_PB11MP_GIC_CPU_BASE		0x1E000000	/* Generic interrupt controller CPU interface */

#define REALVIEW_PB11MP_GIC_DIST_BASE		0x1E001000	/* Generic interrupt controller distributor */

/*寄存器地址偏移*/
#define GIC_CPU_CTRL			0x0
#define GIC_CPU_PRIMASK			0x4
#define GIC_CPU_BINPOINT		0x8
#define GIC_CPU_INTACK			0xc
#define GIC_CPU_EOI			0x10
#define GIC_CPU_RUNNINGPRI		0x14
#define GIC_CPU_HIGHPRI			0x18

#define GIC_DIST_CTRL			0x0
#define GIC_DIST_CTR			0x4
#define GIC_DIST_ENABLE_SET		0x100
#define GIC_DIST_ENABLE_CLEAR		0x180
#define GIC_DIST_PENDING_SET		0x200
#define GIC_DIST_PENDING_CLEAR		0x280
#define GIC_DIST_ACTIVE_BIT		0x300
#define GIC_DIST_PRI			0x400
#define GIC_DIST_TARGET			0x800
#define GIC_DIST_CONFIG			0xc00
#define GIC_DIST_SOFTINT		0xf00

/* ------------------------------------------------------------------------
 *  RealView Registers
 * ------------------------------------------------------------------------
 * 
 */
#define REALVIEW_PB11MP_SYS_PLD_CTRL1		0x74
#define REALVIEW_SYS_ID_OFFSET               0x00
#define REALVIEW_SYS_SW_OFFSET               0x04
#define REALVIEW_SYS_LED_OFFSET              0x08
#define REALVIEW_SYS_OSC0_OFFSET             0x0C

#define REALVIEW_SYS_OSC1_OFFSET             0x10
#define REALVIEW_SYS_OSC2_OFFSET             0x14
#define REALVIEW_SYS_OSC3_OFFSET             0x18
#define REALVIEW_SYS_OSC4_OFFSET             0x1C	/* OSC1 for RealView/AB */

#define REALVIEW_SYS_LOCK_OFFSET             0x20
#define REALVIEW_SYS_100HZ_OFFSET            0x24
#define REALVIEW_SYS_CFGDATA1_OFFSET         0x28
#define REALVIEW_SYS_CFGDATA2_OFFSET         0x2C
#define REALVIEW_SYS_FLAGS_OFFSET            0x30
#define REALVIEW_SYS_FLAGSSET_OFFSET         0x30
#define REALVIEW_SYS_FLAGSCLR_OFFSET         0x34
#define REALVIEW_SYS_NVFLAGS_OFFSET          0x38
#define REALVIEW_SYS_NVFLAGSSET_OFFSET       0x38
#define REALVIEW_SYS_NVFLAGSCLR_OFFSET       0x3C
#define REALVIEW_SYS_RESETCTL_OFFSET         0x40
#define REALVIEW_SYS_PCICTL_OFFSET           0x44
#define REALVIEW_SYS_MCI_OFFSET              0x48
#define REALVIEW_SYS_FLASH_OFFSET            0x4C
#define REALVIEW_SYS_CLCD_OFFSET             0x50
#define REALVIEW_SYS_CLCDSER_OFFSET          0x54
#define REALVIEW_SYS_BOOTCS_OFFSET           0x58
#define REALVIEW_SYS_24MHz_OFFSET            0x5C
#define REALVIEW_SYS_MISC_OFFSET             0x60
#define REALVIEW_SYS_IOSEL_OFFSET            0x70
#define REALVIEW_SYS_PROCID_OFFSET           0x84
#define REALVIEW_SYS_TEST_OSC0_OFFSET        0xC0
#define REALVIEW_SYS_TEST_OSC1_OFFSET        0xC4
#define REALVIEW_SYS_TEST_OSC2_OFFSET        0xC8
#define REALVIEW_SYS_TEST_OSC3_OFFSET        0xCC
#define REALVIEW_SYS_TEST_OSC4_OFFSET        0xD0

#define REALVIEW_SYS_BASE                    0x10000000
#define REALVIEW_SYS_ID                      (REALVIEW_SYS_BASE + REALVIEW_SYS_ID_OFFSET)
#define REALVIEW_SYS_SW                      (REALVIEW_SYS_BASE + REALVIEW_SYS_SW_OFFSET)
#define REALVIEW_SYS_LED                     (REALVIEW_SYS_BASE + REALVIEW_SYS_LED_OFFSET)
#define REALVIEW_SYS_OSC0                    (REALVIEW_SYS_BASE + REALVIEW_SYS_OSC0_OFFSET)
#define REALVIEW_SYS_OSC1                    (REALVIEW_SYS_BASE + REALVIEW_SYS_OSC1_OFFSET)

#define REALVIEW_SYS_LOCK                    (REALVIEW_SYS_BASE + REALVIEW_SYS_LOCK_OFFSET)
#define REALVIEW_SYS_100HZ                   (REALVIEW_SYS_BASE + REALVIEW_SYS_100HZ_OFFSET)
#define REALVIEW_SYS_CFGDATA1                (REALVIEW_SYS_BASE + REALVIEW_SYS_CFGDATA1_OFFSET)
#define REALVIEW_SYS_CFGDATA2                (REALVIEW_SYS_BASE + REALVIEW_SYS_CFGDATA2_OFFSET)
#define REALVIEW_SYS_FLAGS                   (REALVIEW_SYS_BASE + REALVIEW_SYS_FLAGS_OFFSET)
#define REALVIEW_SYS_FLAGSSET                (REALVIEW_SYS_BASE + REALVIEW_SYS_FLAGSSET_OFFSET)
#define REALVIEW_SYS_FLAGSCLR                (REALVIEW_SYS_BASE + REALVIEW_SYS_FLAGSCLR_OFFSET)
#define REALVIEW_SYS_NVFLAGS                 (REALVIEW_SYS_BASE + REALVIEW_SYS_NVFLAGS_OFFSET)
#define REALVIEW_SYS_NVFLAGSSET              (REALVIEW_SYS_BASE + REALVIEW_SYS_NVFLAGSSET_OFFSET)
#define REALVIEW_SYS_NVFLAGSCLR              (REALVIEW_SYS_BASE + REALVIEW_SYS_NVFLAGSCLR_OFFSET)
#define REALVIEW_SYS_RESETCTL                (REALVIEW_SYS_BASE + REALVIEW_SYS_RESETCTL_OFFSET)
#define REALVIEW_SYS_PCICTL                  (REALVIEW_SYS_BASE + REALVIEW_SYS_PCICTL_OFFSET)
#define REALVIEW_SYS_MCI                     (REALVIEW_SYS_BASE + REALVIEW_SYS_MCI_OFFSET)
#define REALVIEW_SYS_FLASH                   (REALVIEW_SYS_BASE + REALVIEW_SYS_FLASH_OFFSET)
#define REALVIEW_SYS_CLCD                    (REALVIEW_SYS_BASE + REALVIEW_SYS_CLCD_OFFSET)
#define REALVIEW_SYS_CLCDSER                 (REALVIEW_SYS_BASE + REALVIEW_SYS_CLCDSER_OFFSET)
#define REALVIEW_SYS_BOOTCS                  (REALVIEW_SYS_BASE + REALVIEW_SYS_BOOTCS_OFFSET)
#define REALVIEW_SYS_24MHz                   (REALVIEW_SYS_BASE + REALVIEW_SYS_24MHz_OFFSET)
#define REALVIEW_SYS_MISC                    (REALVIEW_SYS_BASE + REALVIEW_SYS_MISC_OFFSET)
#define REALVIEW_SYS_IOSEL                   (REALVIEW_SYS_BASE + REALVIEW_SYS_IOSEL_OFFSET)
#define REALVIEW_SYS_PROCID                  (REALVIEW_SYS_BASE + REALVIEW_SYS_PROCID_OFFSET)
#define REALVIEW_SYS_TEST_OSC0               (REALVIEW_SYS_BASE + REALVIEW_SYS_TEST_OSC0_OFFSET)
#define REALVIEW_SYS_TEST_OSC1               (REALVIEW_SYS_BASE + REALVIEW_SYS_TEST_OSC1_OFFSET)
#define REALVIEW_SYS_TEST_OSC2               (REALVIEW_SYS_BASE + REALVIEW_SYS_TEST_OSC2_OFFSET)
#define REALVIEW_SYS_TEST_OSC3               (REALVIEW_SYS_BASE + REALVIEW_SYS_TEST_OSC3_OFFSET)
#define REALVIEW_SYS_TEST_OSC4             (REALVIEW_SYS_BASE + REALVIEW_SYS_TEST_OSC4_OFFSET)

#endif
