ifeq ($(CFG_ARM_S3C2440),y)
BOARD=s3c2440
endif
ifeq ($(CFG_ARM_S3C2410),y)
BOARD=s3c2410
endif
ifeq ($(CFG_ARM_S3C44B0),y)
BOARD=s3c44b0
endif
ifeq ($(CFG_ARM_STM3210),y)
BOARD=stm3210
endif
ifeq ($(CFG_ARM_PB11MPCORE),y)
BOARD=pb11mpcore
endif
ifeq ($(CFG_ARM_LPC2131),y)
BOARD=lpc2131
endif
ifeq ($(CFG_ARM_LPC2200),y)
BOARD=lpc2200
endif

ifeq ($(CFG_X86_EMU_SINGLE),y)
BOARD=emu_single
endif
ifeq ($(CFG_X86_EMU_CMP),y)
BOARD=emu_cmp
endif
ifeq ($(CFG_BRD_TEMPLATE),y)
BOARD=template
endif
BOARD?=err
