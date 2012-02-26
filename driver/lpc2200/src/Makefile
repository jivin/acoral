O_TARGET := src.o
obj-y :=brd_driver.o 
obj-$(CFG_DRV_CONSOLE)+=console.o 
obj-$(CFG_DRV_SDCARD)+=lpc2200_sd.o sdcrc.o sdcmd.o sddriver.o sdhal.o
include $(TOPDIR)/rule.mk
