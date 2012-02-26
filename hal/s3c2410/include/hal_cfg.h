#ifndef HAL_CFG_H
#define HAL_CFG_H

#define USR26_MODE		0x00
#define FIQ26_MODE		0x01
#define IRQ26_MODE		0x02
#define SVC26_MODE		0x03
#define USR_MODE		0x10
#define FIQ_MODE		0x11
#define IRQ_MODE		0x12
#define SVC_MODE		0x13
#define ABT_MODE		0x17
#define UND_MODE		0x1b
#define SYS_MODE		0x1f
#define MODE_MASK		0x1f
#define NOINT        	0xc0
#define NOIRQ           0x80
#define PSR_T_BIT       0x20
#define PSR_F_BIT       0x40
#define PSR_I_BIT       0x80


#define FCLK 120000000
#define HCLK 60000000
#define PCLK 30000000
#define M_MDIV    0x5c 
#define M_PDIV    0x4
#define M_SDIV    0x0
#define M_DIVN    0x3
#define vMPLLCON		((M_MDIV << 12) | (M_PDIV << 4) | (M_SDIV)) 
#define UART_BAUD_RATE 115200

/* initial values for DRAM */
#define vBWSCON			0x22111110
#define vBANKCON0		0x00000700
#define vBANKCON1		0x00000700
#define vBANKCON2		0x00000700
#define vBANKCON3		0x00000700
#define vBANKCON4		0x00000700
#define vBANKCON5		0x00000700
#define vBANKCON6		0x00018005
#define vBANKCON7		0x00018005
#define vREFRESH		0x008e0459
#define vBANKSIZE		0xb2
#define vMRSRB6			0x30
#define vMRSRB7			0x30


/* Interrupts */
#define INT_CTL_BASE		0x4A000000
#define bINT_CTL(Nb)		(INT_CTL_BASE + (Nb))
/* Offset */
#define oSRCPND			0x00
#define oINTMOD			0x04
#define oINTMSK			0x08
#define oPRIORITY		0x0a
#define oINTPND			0x10
#define oINTOFFSET		0x14
#define oSUBSRCPND		0x18
#define oINTSUBMSK		0x1C

/* Registers */
#define SRCPND			bINT_CTL(oSRCPND)
#define INTMOD			bINT_CTL(oINTMOD)
#define INTMSK			bINT_CTL(oINTMSK)
#define PRIORITY		bINT_CTL(oPRIORITY)
#define INTPND			bINT_CTL(oINTPND)
#define INTOFFSET		bINT_CTL(oINTOFFSET)
#define SUBSRCPND		bINT_CTL(oSUBSRCPND)
#define INTSUBMSK		bINT_CTL(oINTSUBMSK)


/* Memory Controller */
#define MEM_CTL_BASE		0x48000000
#define bMEMCTL(Nb)		(MEM_CTL_BASE + (Nb))
/* Offset */
#define oBWSCON			0x00	/* R/W, Bus width and wait status ctrl reg. */
#define oBANKCON0		0x04	/* R/W, Bank 0 control reg. */
#define oBANKCON1		0x08	/* R/W, Bank 1 control reg. */
#define oBANKCON2		0x0C	/* R/W, Bank 2 control reg. */
#define oBANKCON3		0x10	/* R/W, Bank 3 control reg. */
#define oBANKCON4		0x14	/* R/W, Bank 4 control reg. */
#define oBANKCON5		0x18	/* R/W, Bank 5 control reg. */
#define oBANKCON6		0x1C	/* R/W, Bank 6 control reg. */
#define oBANKCON7		0x20	/* R/W, Bank 7 control reg. */
#define oREFRESH		0x24	/* R/W, SDRAM refresh control register */
#define oBANKSIZE		0x28	/* R/W, Flexible bank size register */
#define oMRSRB6			0x2C	/* R/W, Mode register set register bank 6 */
#define oMRSRB7			0x2C	/* R/W, Mode register set register bank 7 */
/* Registers */
#define BWSCON			bMEMCTL(oBWSCON)
#define BANKCON0		bMEMCTL(oBANKCON0)
#define BANKCON1		bMEMCTL(oBANKCON1)
#define BANKCON2		bMEMCTL(oBANKCON2)
#define BANKCON3		bMEMCTL(oBANKCON3)
#define BANKCON4		bMEMCTL(oBANKCON4)
#define BANKCON5		bMEMCTL(oBANKCON5)
#define BANKCON6		bMEMCTL(oBANKCON6)
#define BANKCON7		bMEMCTL(oBANKCON7)
#define REFRESH			bMEMCTL(oREFRESH)
#define BANKSIZE		bMEMCTL(oBANKSIZE)
#define MRSRB6			bMEMCTL(oMRSRB6)
#define MRSRB7			bMEMCTL(oMRSRB7)
/* Bits */
#define SELF_REFRESH		(1 << 22)

#define DESC_SEC	(0x2|(1<<4))
#define CB		(3<<2)  /*cache_on, write_back*/
#define CNB		(2<<2)  /*cache_on, write_through */
#define NCB     (1<<2)  /*cache_off,WR_BUF on*/
#define NCNB	(0<<2)  /*cache_off,WR_BUF off*/
#define AP_RW	(3<<10) /*supervisor=RW, user=RW*/
#define AP_RO	(2<<10) /*supervisor=RW, user=RO*/

#define DOMAIN_FAULT	(0x0)
#define DOMAIN_CHK	(0x1) 
#define DOMAIN_NOTCHK	(0x3) 
#define DOMAIN0		(0x0<<5)
#define DOMAIN1		(0x1<<5)

#define DOMAIN0_ATTR	(DOMAIN_CHK<<0) 
#define DOMAIN1_ATTR	(DOMAIN_FAULT<<2) 

#define RW_CB		(AP_RW|DOMAIN0|CB|DESC_SEC)
#define RW_CNB		(AP_RW|DOMAIN0|CNB|DESC_SEC)
#define RW_NCNB		(AP_RW|DOMAIN0|NCNB|DESC_SEC)
#define RW_FAULT	(AP_RW|DOMAIN1|NCNB|DESC_SEC)


/* Clock and Power Management */

#define SCLKE		(1 << 19)
#define SCLK1		(1 << 18)
#define SCLK0		(1 << 17)
#define PMST_SMR	(1 << 1)


#define CLK_CTL_BASE		0x4C000000
#define bCLKCTL(Nb)		(CLK_CTL_BASE + (Nb))
/* Offset */
#define oLOCKTIME		0x00	/* R/W, PLL lock time count register */
#define oMPLLCON		0x04	/* R/W, MPLL configuration register */
#define oUPLLCON		0x08	/* R/W, UPLL configuration register */
#define oCLKCON			0x0C	/* R/W, Clock generator control reg. */
#define oCLKSLOW		0x10	/* R/W, Slow clock control register */
#define oCLKDIVN		0x14	/* R/W, Clock divider control */
/* Registers */
#define LOCKTIME		bCLKCTL(oLOCKTIME)
#define MPLLCON			bCLKCTL(oMPLLCON)
#define UPLLCON			bCLKCTL(oUPLLCON)
#define CLKCON			bCLKCTL(oCLKCON)
#define CLKSLOW			bCLKCTL(oCLKSLOW)
#define CLKDIVN			bCLKCTL(oCLKDIVN)
#define CLKCON_POWER           (1<<3)

/* UART */
#define UART_CTL_BASE		0x50000000
#define UART0_CTL_BASE		UART_CTL_BASE
#define UART1_CTL_BASE		UART_CTL_BASE + 0x4000
#define UART2_CTL_BASE		UART_CTL_BASE + 0x8000
#define bUART(x, Nb)		(UART_CTL_BASE + (x)*0x4000 + (Nb))
#define bUARTb(x, Nb)		(UART_CTL_BASE + (x)*0x4000 + (Nb))
/* Offset */
#define oULCON			0x00	/* R/W, UART line control register */
#define oUCON			0x04	/* R/W, UART control register */
#define oUFCON			0x08	/* R/W, UART FIFO control register */
#define oUMCON			0x0C	/* R/W, UART modem control register */
#define oUTRSTAT		0x10	/* R  , UART Tx/Rx status register */
#define oUERSTAT		0x14	/* R  , UART Rx error status register */
#define oUFSTAT			0x18	/* R  , UART FIFO status register */
#define oUMSTAT			0x1C	/* R  , UART Modem status register */
#define oUTXHL			0x20	/*   W, UART transmit(little-end) buffer */
#define oUTXHB			0x23	/*   W, UART transmit(big-end) buffer */
#define oURXHL			0x24	/* R  , UART receive(little-end) buffer */
#define oURXHB			0x27	/* R  , UART receive(big-end) buffer */
#define oUBRDIV			0x28	/* R/W, Baud rate divisor register */
/* Registers */
#define ULCON0			bUART(0, oULCON)
#define UCON0			bUART(0, oUCON)
#define UFCON0			bUART(0, oUFCON)
#define UMCON0			bUART(0, oUMCON)
#define UTRSTAT0		bUART(0, oUTRSTAT)
#define UERSTAT0		bUART(0, oUERSTAT)
#define UFSTAT0			bUART(0, oUFSTAT)
#define UMSTAT0			bUART(0, oUMSTAT)
#define UTXH0			bUARTb(0, oUTXHL)
#define URXH0			bUARTb(0, oURXHL)
#define UBRDIV0			bUART(0, oUBRDIV)
#define ULCON1			bUART(1, oULCON)
#define UCON1			bUART(1, oUCON)
#define UFCON1			bUART(1, oUFCON)
#define UMCON1			bUART(1, oUMCON)
#define UTRSTAT1		bUART(1, oUTRSTAT)
#define UERSTAT1		bUART(1, oUERSTAT)
#define UFSTAT1			bUART(1, oUFSTAT)
#define UMSTAT1			bUART(1, oUMSTAT)
#define UTXH1			bUARTb(1, oUTXHL)
#define URXH1			bUARTb(1, oURXHL)
#define UBRDIV1			bUART(1, oUBRDIV)
#define ULCON2			bUART(2, oULCON)
#define UCON2			bUART(2, oUCON)
#define UFCON2			bUART(2, oUFCON)
#define UMCON2			bUART(2, oUMCON)
#define UTRSTAT2		bUART(2, oUTRSTAT)
#define UERSTAT2		bUART(2, oUERSTAT)
#define UFSTAT2			bUART(2, oUFSTAT)
#define UMSTAT2			bUART(2, oUMSTAT)
#define UTXH2			bUARTb(2, oUTXHL)
#define URXH2			bUARTb(2, oURXHL)
#define UBRDIV2			bUART(2, oUBRDIV)

#define UTRSTAT_TX_EMPTY	(1 << 2)
#define UTRSTAT_RX_READY	(1 << 0)
#define UART_ERR_MASK		0xF 


/* GPIO */
#define GPIO_CTL_BASE		0x56000000
#define bGPIO(p,o)		(GPIO_CTL_BASE + (p) + (o))
/* Offset */
#define oGPIO_CON		0x0	/* R/W, Configures the pins of the port */
#define oGPIO_DAT		0x4	/* R/W,	Data register for port */
#define oGPIO_UP		0x8	/* R/W, Pull-up disable register */
#define oGPIO_RESERVED		0xC	/* R/W, Reserved */
#define oGPIO_A			0x00
#define oGPIO_B			0x10
#define oGPIO_C			0x20
#define oGPIO_D			0x30
#define oGPIO_E			0x40
#define oGPIO_F			0x50
#define oGPIO_G			0x60
#define oGPIO_H			0x70
#define oMISCCR			0x80	/* R/W, Miscellaneous control register */
#define oDCLKCON		0x84	/* R/W, DCLK0/1 control register */
#define oEXTINT0		0x88	/* R/W, External interrupt control reg. 0 */
#define oEXTINT1		0x8C	/* R/W, External interrupt control reg. 1 */
#define oEXTINT2		0x90	/* R/W, External interrupt control reg. 2 */
#define oEINTFLT0		0x94	/* R/W, Reserved */
#define oEINTFLT1		0x98	/* R/W, Reserved */
#define oEINTFLT2		0x9C	/* R/W, External interrupt control reg. 2 */
#define oEINTFLT3		0xA0	/* R/W, External interrupt control reg. 3 */
#define oEINTMASK		0xA4	/* R/W, External interrupt mask register */
#define oEINTPEND		0xA8	/* R/W, External interrupt pending reg. */

/* Registers */
#define GPACON			bGPIO(oGPIO_A, oGPIO_CON)
#define GPADAT			bGPIO(oGPIO_A, oGPIO_DAT)
#define GPBCON			bGPIO(oGPIO_B, oGPIO_CON)
#define GPBDAT			bGPIO(oGPIO_B, oGPIO_DAT)
#define GPBUP			bGPIO(oGPIO_B, oGPIO_UP)
#define GPCCON			bGPIO(oGPIO_C, oGPIO_CON)
#define GPCDAT			bGPIO(oGPIO_C, oGPIO_DAT)
#define GPCUP			bGPIO(oGPIO_C, oGPIO_UP)
#define GPDCON			bGPIO(oGPIO_D, oGPIO_CON)
#define GPDDAT			bGPIO(oGPIO_D, oGPIO_DAT)
#define GPDUP			bGPIO(oGPIO_D, oGPIO_UP)
#define GPECON			bGPIO(oGPIO_E, oGPIO_CON)
#define GPEDAT			bGPIO(oGPIO_E, oGPIO_DAT)
#define GPEUP			bGPIO(oGPIO_E, oGPIO_UP)
#define GPFCON			bGPIO(oGPIO_F, oGPIO_CON)
#define GPFDAT			bGPIO(oGPIO_F, oGPIO_DAT)
#define GPFUP			bGPIO(oGPIO_F, oGPIO_UP)
#define GPGCON			bGPIO(oGPIO_G, oGPIO_CON)
#define GPGDAT			bGPIO(oGPIO_G, oGPIO_DAT)
#define GPGUP			bGPIO(oGPIO_G, oGPIO_UP)
#define GPHCON			bGPIO(oGPIO_H, oGPIO_CON)
#define GPHDAT			bGPIO(oGPIO_H, oGPIO_DAT)
#define GPHUP			bGPIO(oGPIO_H, oGPIO_UP)
#define MISCCR			bGPIO(oMISCCR, 0)
#define DCLKCON			bGPIO(oDCLKCON, 0)
#define EXTINT0			bGPIO(oEXTINT0, 0)
#define EXTINT1			bGPIO(oEXTINT1, 0)
#define EXTINT2			bGPIO(oEXTINT2, 0)
#define EINTFLT0		bGPIO(oEINTFLT0, 0)
#define EINTFLT1		bGPIO(oEINTFLT1, 0)
#define EINTFLT2		bGPIO(oEINTFLT2, 0)
#define EINTFLT3		bGPIO(oEINTFLT3, 0)
#define EINTMASK		bGPIO(oEINTMASK, 0)
#define MISCCR_SDSLEEP   (7<<17)

/*Nand Flash*/
#define __REGb(x)	(*(volatile unsigned char *)(x))
#define __REGw(x)	(*(volatile unsigned short *)(x))
#define __REGi(x)	(*(volatile unsigned int *)(x))

#define REGb(x)	  __REGb(x)
#define REGw(x)	  __REGw(x)
#define REGi(x)	  __REGi(x)

#define NF_BASE		0x4e000000
#define NFCONF		__REGi(NF_BASE + 0x0)
#define NFCONT		__REGi(NF_BASE + 0x4)
#define NFCMD		__REGb(NF_BASE + 0x8)
#define NFADDR		__REGb(NF_BASE + 0xc)
#define NFDATA		__REGb(NF_BASE + 0x10)
#define NFDATA16	__REGw(NF_BASE + 0x10)
#define NFSTAT		__REGb(NF_BASE + 0x20) 
#define NFECC		__REGb(NF_BASE + 0x2c)        
#define NFSTAT_BUSY	(1<<2)
#define nand_select()	  (NFCONT &= ~(1 << 1))
#define nand_deselect()	  (NFCONT |= (1 << 1))
#define nand_clear_RnB()  (NFSTAT |= (1 << 2))

/* Standard NAND flash commands*/
#define NAND_CMD_READ0          0
#define NAND_CMD_READ1          1
#define NAND_CMD_RNDOUT         5
#define NAND_CMD_PAGEPROG       0x10
#define NAND_CMD_READOOB         0x50
#define NAND_CMD_ERASE1         0x60                                        
#define NAND_CMD_STATUS         0x70
#define NAND_CMD_STATUS_MULTI   0x71
#define NAND_CMD_SEQIN          0x80
#define NAND_CMD_RNDIN          0x85
#define NAND_CMD_READID         0x90
#define NAND_CMD_ERASE2         0xd0
#define NAND_CMD_RESET          0xff
#define NAND_CMD_READSTART      0x30
#define NAND_CMD_RNDOUTSTART    0xE0 
#define NAND_CMD_CACHEDPROG     0x15 

#endif
