#include"../include/comm.h"
#include"../include/app.h"
extern struct AppCfg NotePad;
extern struct AppCfg Art;
extern struct AppCfg PingPang;
extern struct AppCfg Draw;
struct AppCfg *apps[]={
	&NotePad,
	&Art,
	NULL
};

struct AppCfg *games[]={
	&PingPang,
	&Draw,
	NULL
};

struct DeskAppClass AppClass={
	1004,
	"App",
	"tab_app.bmp",
	apps
};

struct DeskAppClass GameClass={
	1005,
	"Game",
	"tab_gm.bmp",
	games
};

struct DeskAppClass *DeskClass[]={
	&AppClass,
	&GameClass,
	NULL
};
