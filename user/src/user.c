#include"acoral.h"
#include"../include/shell.h"
void user_main()
{
#ifdef CFG_TELNET_SHELL
	user_telnetd();
#endif
#ifdef CFG_WEB_SERVER
	user_httpd();
#endif
//	acoral_create_thread(dummy,256,NULL,NULL,20,0);
}
