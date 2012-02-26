ifeq ($(CFG_BSP),y)
BSP_CPPFLAGS+=-I$(TOPDIR)/bsp/$(BOARD)/include
subdir-y += $(TOPDIR)/bsp
obj-y += $(TOPDIR)/bsp/bsp.o
endif
