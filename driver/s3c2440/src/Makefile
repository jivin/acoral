O_TARGET := src.o
obj-y :=brd_driver.o 
obj-$(CFG_DRV_CON_UART)+=con_uart.o 
obj-$(CFG_DRV_CON_TELNET)+=con_telnet.o
obj-$(CFG_DRV_LCD_MINI2440)+=lcd_mini.o 
obj-$(CFG_DRV_LCD_TQ2440)+=lcd_tq.o 
obj-$(CFG_DRV_SDI)+=sdi.o 
obj-$(CFG_DRV_TS_MINI2440)+=touch_mini.o
obj-$(CFG_DRV_TS_TQ2440)+=touch_tq.o
obj-$(CFG_DRV_NET_DM9000)+=dm9000.o
include $(TOPDIR)/rule.mk
