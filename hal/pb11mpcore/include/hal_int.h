#ifndef HAL_INTR_H
#define HAL_INTR_H
#define HAL_INTR_MIN            0

#define IRQ_TC11MP_GIC_START			32
#define IRQ_PB11MP_GIC_START			64


/*ARM11MPCore 测试芯片中断源(主GIC，共32个)(编号32－63)*/

#define IRQ_TC11MP_AACI		(IRQ_TC11MP_GIC_START + 0)
#define IRQ_TC11MP_TIMER0_1	(IRQ_TC11MP_GIC_START + 1)
#define IRQ_TC11MP_TIMER2_3	(IRQ_TC11MP_GIC_START + 2)
#define IRQ_TC11MP_USB		(IRQ_TC11MP_GIC_START + 3)
#define IRQ_TC11MP_UART0	(IRQ_TC11MP_GIC_START + 4)
#define IRQ_TC11MP_UART1	(IRQ_TC11MP_GIC_START + 5)
#define IRQ_TC11MP_RTC		(IRQ_TC11MP_GIC_START + 6)
#define IRQ_TC11MP_KMI0		(IRQ_TC11MP_GIC_START + 7)
#define IRQ_TC11MP_KMI1		(IRQ_TC11MP_GIC_START + 8)
#define IRQ_TC11MP_ETH		(IRQ_TC11MP_GIC_START + 9)
#define IRQ_TC11MP_PB_IRQ1	(IRQ_TC11MP_GIC_START + 10)		/* main GIC */
#define IRQ_TC11MP_PB_IRQ2	(IRQ_TC11MP_GIC_START + 11)		/* tile GIC */
#define IRQ_TC11MP_PB_FIQ1	(IRQ_TC11MP_GIC_START + 12)		/* main GIC */
#define IRQ_TC11MP_PB_FIQ2	(IRQ_TC11MP_GIC_START + 13)		/* tile GIC */
#define IRQ_TC11MP_MMCI0A	(IRQ_TC11MP_GIC_START + 14)
#define IRQ_TC11MP_MMCI0B	(IRQ_TC11MP_GIC_START + 15)

#define IRQ_TC11MP_PMU_CPU0	(IRQ_TC11MP_GIC_START + 17)
#define IRQ_TC11MP_PMU_CPU1	(IRQ_TC11MP_GIC_START + 18)
#define IRQ_TC11MP_PMU_CPU2	(IRQ_TC11MP_GIC_START + 19)
#define IRQ_TC11MP_PMU_CPU3	(IRQ_TC11MP_GIC_START + 20)
#define IRQ_TC11MP_PMU_SCU0	(IRQ_TC11MP_GIC_START + 21)
#define IRQ_TC11MP_PMU_SCU1	(IRQ_TC11MP_GIC_START + 22)
#define IRQ_TC11MP_PMU_SCU2	(IRQ_TC11MP_GIC_START + 23)
#define IRQ_TC11MP_PMU_SCU3	(IRQ_TC11MP_GIC_START + 24)
#define IRQ_TC11MP_PMU_SCU4	(IRQ_TC11MP_GIC_START + 25)
#define IRQ_TC11MP_PMU_SCU5	(IRQ_TC11MP_GIC_START + 26)
#define IRQ_TC11MP_PMU_SCU6	(IRQ_TC11MP_GIC_START + 27)
#define IRQ_TC11MP_PMU_SCU7	(IRQ_TC11MP_GIC_START + 28)

#define IRQ_TC11MP_L220_EVENT	(IRQ_TC11MP_GIC_START + 29)
#define IRQ_TC11MP_L220_SLAVE	(IRQ_TC11MP_GIC_START + 30)
#define IRQ_TC11MP_L220_DECODE	(IRQ_TC11MP_GIC_START + 31)


/* PB11MPCore GIC中断源(次GIC，共32个)(编号64－95)*/


#define IRQ_PB11MP_WATCHDOG	(IRQ_PB11MP_GIC_START + 0)	/* Watchdog time */
#define IRQ_PB11MP_SOFT		(IRQ_PB11MP_GIC_START + 1)	/* Software interrupt */
#define IRQ_PB11MP_COMMRx	(IRQ_PB11MP_GIC_START + 2)	/* Debug Comm Rx interrupt */
#define IRQ_PB11MP_COMMTx	(IRQ_PB11MP_GIC_START + 3)	/* Debug Comm Tx interrupt */
#define IRQ_PB11MP_GPIO0	(IRQ_PB11MP_GIC_START + 6)	/* GPIO 0 */
#define IRQ_PB11MP_GPIO1	(IRQ_PB11MP_GIC_START + 7)	/* GPIO 1 */
#define IRQ_PB11MP_GPIO2	(IRQ_PB11MP_GIC_START + 8)	/* GPIO 2 */
								/* 9 reserved */
#define IRQ_PB11MP_RTC_GIC1	(IRQ_PB11MP_GIC_START + 10)	/* Real Time Clock */
#define IRQ_PB11MP_SSP		(IRQ_PB11MP_GIC_START + 11)	/* Synchronous Serial Port */
#define IRQ_PB11MP_UART0_GIC1	(IRQ_PB11MP_GIC_START + 12)	/* UART 0 on development chip */
#define IRQ_PB11MP_UART1_GIC1	(IRQ_PB11MP_GIC_START + 13)	/* UART 1 on development chip */
#define IRQ_PB11MP_UART2	(IRQ_PB11MP_GIC_START + 14)	/* UART 2 on development chip */
#define IRQ_PB11MP_UART3	(IRQ_PB11MP_GIC_START + 15)	/* UART 3 on development chip */
#define IRQ_PB11MP_SCI		(IRQ_PB11MP_GIC_START + 16)	/* Smart Card Interface */
#define IRQ_PB11MP_MMCI0A_GIC1	(IRQ_PB11MP_GIC_START + 17)	/* Multimedia Card 0A */
#define IRQ_PB11MP_MMCI0B_GIC1	(IRQ_PB11MP_GIC_START + 18)	/* Multimedia Card 0B */
#define IRQ_PB11MP_AACI_GIC1	(IRQ_PB11MP_GIC_START + 19)	/* Audio Codec */
#define IRQ_PB11MP_KMI0_GIC1	(IRQ_PB11MP_GIC_START + 20)	/* Keyboard/Mouse port 0 */
#define IRQ_PB11MP_KMI1_GIC1	(IRQ_PB11MP_GIC_START + 21)	/* Keyboard/Mouse port 1 */
#define IRQ_PB11MP_CHARLCD	(IRQ_PB11MP_GIC_START + 22)	/* Character LCD */
#define IRQ_PB11MP_CLCD		(IRQ_PB11MP_GIC_START + 23)	/* CLCD controller */
#define IRQ_PB11MP_DMAC		(IRQ_PB11MP_GIC_START + 24)	/* DMA controller */
#define IRQ_PB11MP_PWRFAIL	(IRQ_PB11MP_GIC_START + 25)	/* Power failure */
#define IRQ_PB11MP_PISMO	(IRQ_PB11MP_GIC_START + 26)	/* PISMO interface */
#define IRQ_PB11MP_DoC		(IRQ_PB11MP_GIC_START + 27)	/* Disk on Chip memory controller */
#define IRQ_PB11MP_ETH_GIC1	(IRQ_PB11MP_GIC_START + 28)	/* Ethernet controller */
#define IRQ_PB11MP_USB_GIC1	(IRQ_PB11MP_GIC_START + 29)	/* USB controller */
#define IRQ_PB11MP_TSPEN	(IRQ_PB11MP_GIC_START + 30)	/* Touchscreen pen */
#define IRQ_PB11MP_TSKPAD	(IRQ_PB11MP_GIC_START + 31)	/* Touchscreen keypad */


#define NR_GIC_PB11MP		2    /*GIC个数*/

#define NR_IRQS_PB11MP		(IRQ_TC11MP_GIC_START + 96)
#define NR_IRQS			NR_IRQS_PB11MP
#define MAX_GIC_NR		NR_GIC_PB11MP
#define HAL_INTR_MAX            IRQ_PB11MP_TSKPAD
#define HAL_INTR_NUM            HAL_INTR_MAX+1

struct gic_chip_data{
    acoral_u32   irq_offset;
    acoral_u32  dist_base;
    acoral_u32  cpu_base; 
};

acoral_u32  PRIMASK ;

#define HAL_INTR_INIT()	                hal_intr_init()
#define HAL_INTR_MASK(vector)           hal_intr_mask(vector)
#define HAL_INTR_UNMASK(vector)         hal_intr_unmask(vector)
#define HAL_INTR_ACK(vector)	        hal_intr_ack(vector)

#define HAL_SET_INTR_PRIO_BOUND(prio)   hal_set_intr_prio_bound(prio)
#define HAL_SET_INTR_REAL(irq)          hal_set_intr_real(irq)
#define HAL_SET_INTR_COMM(irq)          hal_set_intr_comm(irq)
#define HAL_SET_INTR_OFFSET(irq,offset) hal_set_intr_offset(irq,offset)
#define HAL_GET_INTR_PRIO(irq)          hal_get_intr_prio(irq)
#define HAL_SET_INTR_PRIO(irq,prio)     hal_set_intr_prio(irq,offset)
#endif

