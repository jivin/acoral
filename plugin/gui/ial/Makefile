O_TARGET=libial.o
obj-y=mouse_ial.o kb_ial.o kb_dummy.o mouse_dummy.o
obj-$(CFG_X86_EMU_SINGLE)+=mouse_qvfb.o  kb_qvfb.o
obj-$(CFG_X86_EMU_CMP)+=mouse_qvfb.o  kb_qvfb.o
obj-$(CFG_ARM_S3C2440)+=mouse_ts.o  kb_2440.o
include $(TOPDIR)/rule.mk
