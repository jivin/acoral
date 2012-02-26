#ifndef SHELL_H
#define SHELL_H
typedef struct{
	acoral_char *name;
	void (*exe)(int argc,const acoral_char **);
	acoral_char *comment;
	struct acoral_shell_cmd *next;
}acoral_shell_cmd_t;
void add_command(acoral_shell_cmd_t *cmd);
void acoral_shell_enter(void *args);
#endif
