#ifndef _APP_H
#define _APP_H
struct AppCfg{
	void (*entry)(void *args);
	char *Name;
	char *IconName;
};

struct DeskAppClass{
	int id;
	char *Name;
	char *IconName;
	struct AppCfg **Apps;
};
#endif
