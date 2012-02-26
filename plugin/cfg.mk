ifeq ($(CFG_PLUGIN_GUI),y)
PLUGIN_CPPFLAGS+=-I$(TOPDIR)/plugin/gui/include
endif

ifeq ($(CFG_PLUGIN_NET),y)
PLUGIN_CPPFLAGS+=-I$(TOPDIR)/plugin/net/include\
		-I$(TOPDIR)/plugin/net/arch/include \
		-I$(TOPDIR)/plugin/net/include \
		-I$(TOPDIR)/plugin/net/include/ipv4
endif

ifeq ($(CFG_PLUGIN_FS),y)
PLUGIN_CPPFLAGS+=-I$(TOPDIR)/plugin/fs/include
endif

