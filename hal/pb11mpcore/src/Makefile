O_TARGET :=src.o
obj-y :=hal_thread_c.o hal_int_c.o hal_timer.o \
		hal_board.o hal_mem_c.o hal_mem_s.o hal_thread_s.o hal_int_s.o
obj-$(CFG_CMP)+=hal_ipi.o hal_cmp_c.o hal_cmp_s.o
include $(TOPDIR)/rule.mk
