TOPDIR=$(shell pwd)
ACORAL_INCLUDE_DIR=-I$(TOPDIR)/include -I$(TOPDIR)/kernel/include \
	-I$(TOPDIR)/hal/include \
	-I$(TOPDIR)/hal/$(BOARD)/include \
       	-I$(TOPDIR)/lib/include\
       	-I$(TOPDIR)/driver/include\
       	-I$(TOPDIR)/plugin/include 
CONFIG_SHELL:= $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	else if [ -x /bin/bash ]; then echo /bin/bash; \
	else echo sh; fi ; fi)


CLEAN_FILES = \
	acoral.elf \
	acoral.bin \
	acoral.nm \
	acoral.map\
	acoral.api\
	hal.api \
	acoral.t\
	acoral.d \
	acoral.x

all: 	do-it-all
ifeq (.config,$(wildcard .config))
include .config
-include ./hal/cfg.mk
-include ./hal/$(BOARD)/cfg.mk
include ./plugin/cfg.mk
include ./bsp/cfg.mk
do-it-all: 	acoral	
else
do-it-all:	choose
endif

#
# Include the make variables (CC, etc...)
#

CROSS_COMPILE   =

AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
CPP             = $(CC) -E
AR              = $(CROSS_COMPILE)ar
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump
HOSTCC     = gcc
MAKEFILES= $(TOPDIR)/.config
export TOPDIR CROSS_COMPILE AS LD CC ARCH BOARD HOSTCC\
	CPP AR NM STRIP OBJCOPY OBJDUMP MAKE MAKEFILES

#
# CC flags
#

CPPFLAGS =$(ACORAL_INCLUDE_DIR) $(PLUGIN_CPPFLAGS) $(BSP_CPPFLAGS)
CFLAGS = $(CPPFLAGS) -Wall -Wstrict-prototypes -g -O1 -fomit-frame-pointer
AFLAGS = $(CPPFLAGS)
export CPPFLAGS CFLAGS AFLAGS

CLIBS =-L$(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc $(HAL_CLIBS)

LINKFLAGS =-Bstatic -Thal/$(BOARD)/acoral.lds $(HAL_LINKFLAGS)
OBJCOPYFLAGS = -R .comment -R .stab -R .stabstr


CORE_FILES=hal/$(BOARD)/src/start.o acoral.o

O_TARGET:=acoral.o
obj-y :=hal/hal.o kernel/kernel.o lib/lib.o\
	plugin/plugin.o user/user.o
obj-$(CFG_DRIVER)+=driver/driver.o
obj-$(CFG_TEST)+=test/test.o
obj-$(CFG_BSP)+=bsp/bsp.o
subdir-y=hal kernel lib plugin user
subdir-$(CFG_DRIVER)+=driver
subdir-$(CFG_BSP)+=bsp
subdir-$(CFG_TEST)+=test
choose:
	tools/scripts/buildTool tools/configs/
	
acoral:include/autocfg.h first_rule $(CORE_FILES)
	@echo "###System Configure###"
	@echo   ARCH=$(ARCH)
	@echo   BOARD=$(BOARD)
	@echo "######################"
	$(LD) -v  \
		$(CORE_FILES) $(HAL_EXT_FILES)\
		-o acoral.elf $(CLIBS) $(LINKFLAGS)
	$(NM) -v -l acoral.elf > acoral.map
	$(OBJCOPY) -O binary -S acoral.elf acoral.bin $(OBJCOPYFLAGS)
	$(OBJDUMP) -S acoral.elf > acoral.d 
	$(OBJDUMP) -t acoral.elf > acoral.t 
	$(OBJDUMP) -x acoral.elf > acoral.x 
	cat acoral.t |grep ".text"|grep "acoral"|sort>acoral.api
	cat acoral.t |grep ".text"|grep "hal"|sort>hal.api

oldconfig:
	$(CONFIG_SHELL) tools/scripts/Configure -d cfg.in

config: 
	$(CONFIG_SHELL) tools/scripts/Configure cfg.in

menuconfig:
	$(MAKE) -C tools/scripts/lxdialog all
	$(CONFIG_SHELL) tools/scripts/Menuconfig cfg.in

%_cfg: tools/configs/%_cfg
	@echo "Select Config $@"
	@cp $< .config 
	@tools/scripts/AutoConfigure $<

fs:
	cd tools/fs;./mkfs ../..
	chmod 777 acoral.disk

guifs:fs
	mount -o loop acoral.disk /mnt
	cp tools/fs/gui/* /mnt -rf	
	umount /mnt

clean:
	find . \( -name '*.o' -o -name core -o -name ".*.flags" \) -type f -print \
	| grep -v lxdialog/ | xargs rm -f
	rm -f $(CLEAN_FILES) $(hal_CLEAN_FILES)

distclean: clean
	rm -f $(DISTCLEAN_FILES)
include rule.mk
