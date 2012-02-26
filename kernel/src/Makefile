O_TARGET := src.o
obj-y :=core.o policy.o sched.o thread.o comm_thrd.o resource.o int.o timer.o \
	mem.o spinlock.o event.o mutex.o
obj-$(CFG_MEM_BUDDY)+=buddy.o
obj-$(CFG_MEM_SLATE)+=slate.o
obj-$(CFG_MEM2)+=malloc.o
obj-$(CFG_EVT_MBOX)+=mbox.o
obj-$(CFG_EVT_SEM)+=sem.o
obj-$(CFG_MSG)+=message.o
obj-$(CFG_CMP)+=ipi.o cmp.o
obj-$(CFG_THRD_RM)+=rm_thrd.o
obj-$(CFG_THRD_PERIOD)+=period_thrd.o
obj-$(CFG_THRD_SLICE)+=slice_thrd.o
obj-$(CFG_THRD_POSIX)+=posix_thrd.o
obj-$(CFG_STAT)+=stat.o
obj-$(CFG_SOFT_DELAY)+=softdelay.o
obj-$(CFG_PM)+=pm.o
include $(TOPDIR)/rule.mk
