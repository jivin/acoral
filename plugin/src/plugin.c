#include<acoral.h>
#include<plugin.h>
void plugin_init(){
#ifdef CFG_PLUGIN_NET
	acoral_net_lwip_init();
#endif
#ifdef CFG_PLUGIN_FS
	acoral_fs_init();
#endif

#ifdef CFG_PLUGIN_GUI
	acoral_gui_init();
#endif
}

