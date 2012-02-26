O_TARGET := plugin.o
obj-y=src/src.o
subdir-y=src 
obj-$(CFG_PLUGIN_GUI)+=gui/gui.o 
obj-$(CFG_PLUGIN_NET)+=net/net.o 
obj-$(CFG_PLUGIN_FS)+=fs/fs.o 
subdir-$(CFG_PLUGIN_GUI)+=gui
subdir-$(CFG_PLUGIN_NET)+=net
subdir-$(CFG_PLUGIN_FS)+=fs
include $(TOPDIR)/rule.mk
