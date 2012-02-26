#include<kernel.h>
#include"../include/shell.h"
#include"console.h"
void malloc_scan(acoral_32 argc,acoral_char **argv){
	acoral_mem_scan();
}

acoral_shell_cmd_t mem_cmd={
	"memscan",
	malloc_scan,
	"View the first Level Memory Managment Info",
	NULL
};

#ifdef CFG_MEM2
void malloc_scan2(acoral_32 argc,acoral_char **argv){
	acoral_mem_scan2();
}

acoral_shell_cmd_t mem2_cmd={
	"memscan2",
	malloc_scan2,
	"View the seconde Level Memory Managment Info",
	NULL
};
#endif

extern acoral_shell_cmd_t *head_cmd;
void help(acoral_32 argc,acoral_char **argv){
	acoral_shell_cmd_t *curr;
	curr =head_cmd;
	while(curr != NULL) {
		acoral_print("%s  %s\n",curr->name,curr->comment);
		curr = curr->next;
	}
}

acoral_shell_cmd_t help_cmd={
	"help",
	help,
	"View all Shell Command info",
	NULL
};

void exit(acoral_32 argc,acoral_char **argv){
	acoral_dev_config(acoral_cur_thread->console_id,CONSOLE_EXIT,NULL,0);
	acoral_shell_exit((void *)argv[0]);
}

acoral_shell_cmd_t exit_cmd={
	"exit",
	exit,
	"Exit Shell",
	NULL
};
#ifdef CFG_PM
void acoral_suspend();
acoral_shell_cmd_t suspend_cmd={
	"suspend",
	acoral_suspend,
	"suspend system",
	NULL
};
#endif

extern acoral_shell_cmd_t dt_cmd;
extern fs_cmd_init();
void cmd_init(){
	add_command(&mem_cmd);
#ifdef CFG_MEM2
	add_command(&mem2_cmd);
#endif
	add_command(&dt_cmd);
#ifdef CFG_PM
	add_command(&suspend_cmd);
#endif
	add_command(&exit_cmd);
	add_command(&help_cmd);
#ifdef CFG_SHELL_FS_CMD
	fs_cmd_init();
#endif

}
