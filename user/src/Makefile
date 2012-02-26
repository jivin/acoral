O_TARGET := src.o
obj-y:=user.o
obj-$(CFG_SHELL)+=shell.o
obj-$(CFG_SHELL)+=cmd.o thread_display.o
obj-$(CFG_TELNET_SHELL)+=telnet.o
obj-$(CFG_SHELL_FS_CMD)+=fs_shellcmd.o
obj-$(CFG_WEB_SERVER)+=httpd.o
include $(TOPDIR)/rule.mk
