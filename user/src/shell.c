#include<acoral.h>
#include"../include/shell.h"
#include<console.h>
#define BUF_SIZE 128
#define MAX_ARGS_NUM 8
acoral_shell_cmd_t *head_cmd,*tail_cmd;
enum parse_state {
	PS_WHITESPACE,
	PS_TOKEN,
	PS_STRING,
	PS_ESCAPE
};

void parse_args(acoral_char *argstr, acoral_32 *argc_p, acoral_char **argv, acoral_char** resid,enum parse_state *stacked)
{
	acoral_32 argc = 0;
	acoral_char c;
	enum parse_state newState;
	enum parse_state stacked_state=*stacked;
	enum parse_state lastState = PS_WHITESPACE;

	while ((c = *argstr) != 0&&argc<MAX_ARGS_NUM) {

		if (c == ';' && lastState != PS_STRING && lastState != PS_ESCAPE)
			break;

		if (lastState == PS_ESCAPE) {
			newState = stacked_state;
		} else if (lastState == PS_STRING) {
			if (c == '"') {
				newState = PS_WHITESPACE;
				*argstr = 0;
			} else {
				newState = PS_STRING;
			}
		} else if ((c == ' ') || (c == '\t')) {
			*argstr = 0;
			newState = PS_WHITESPACE;
		} else if (c == '"') {
			newState = PS_STRING;
			*argstr++ = 0;
			argv[argc++] = argstr;
		} else if (c == '\\') {
			stacked_state = lastState;
			newState = PS_ESCAPE;
		} else {
			if (lastState == PS_WHITESPACE) {
				argv[argc++] = argstr;
			}
			newState = PS_TOKEN;
		}

		lastState = newState;
		argstr++;
	}

	argv[argc] = NULL;
	if (argc_p != NULL)
		*argc_p = argc;

	if (*argstr == ';') {
		*argstr++ = '\0';
	}
	*resid = argstr;
	*stacked=stacked_state;
}

void add_command(acoral_shell_cmd_t *cmd)
{
	if (head_cmd == NULL) {
		head_cmd = tail_cmd = cmd;
	} else {
		tail_cmd->next = cmd;
		tail_cmd = cmd;
	}
}

acoral_shell_cmd_t *find_cmd(acoral_char *cmdname)
{
	acoral_shell_cmd_t *curr;
	curr =head_cmd;
	while(curr != NULL) {
		if (acoral_str_cmp(curr->name, cmdname) == 0)
			return curr;
		curr = curr->next;
	}
	return NULL;
}

void execmd(acoral_32 argc,acoral_char **argv)
{
	acoral_shell_cmd_t *cmd = find_cmd(argv[0]);
	if (cmd == NULL) {
		acoral_print("Could not found '%s' command\r\n", argv[0]);
		acoral_print("you can type 'help'\r\n"); 
		return;
	}
	cmd->exe(argc, argv);
}


void cmd_exe(acoral_char *buf){
	acoral_32 argc;
	acoral_char *argv[MAX_ARGS_NUM];
	acoral_char *resid;
	enum parse_state stacked_state;
	while (*buf) {
		acoral_memset(argv, 0, sizeof(argv));
		parse_args(buf, &argc, argv, &resid,&stacked_state);
		if (argc > 0)
			execmd(argc, (const acoral_char **)argv);
		buf = resid;
	}	
}

void echo(acoral_char c){
	acoral_putchar(c);
}

void get_cmd(acoral_char *cmd_buf){
	acoral_u32 cur_pos;
	acoral_char c;
	acoral_u8 IsEcho=0;
	acoral_id console_id;
	console_id=acoral_cur_thread->console_id;
	acoral_dev_config(console_id,CONSOLE_ECHO,&IsEcho,0);
	cur_pos=0;
	while(1){
		while(acoral_dev_read(console_id,&c,1,0,0)<1);
		switch (c) {
			case 0x08:
			case 0x06:
			case 0x07:
			case 0x7E:
			case 0x7F:
				if (cur_pos) {
					cur_pos--;
					if(IsEcho){
						echo(0x08);
						echo(' ');
						echo(0x08);
					}
				}
				cmd_buf[cur_pos] = '\0';
				break;
			case '\r':
			case '\n':
			case '\0':
				echo('\r');
				echo('\n');
				cmd_buf[cur_pos]='\0';
				acoral_dev_config(console_id,CONSOLE_BUF_CLEAR,NULL,0);
				return;
			case 'x'-'a'+1:
				cur_pos = 0;
				break;

			default:
				if (cur_pos < BUF_SIZE) {
					cmd_buf[cur_pos] = c;
					if(IsEcho)
						echo(c);
					cur_pos++;
				}
				break;
		}
	}
}

#define SHELL_STACK_SIZE 1024
void acoral_shell_init(){
	acoral_comm_policy_data_t data;
	head_cmd=NULL;
	tail_cmd=NULL;
	cmd_init();
#ifdef CFG_UART_SHELL
	data.cpu=acoral_current_cpu;
	data.prio=ACORAL_TMP_PRIO;
	data.prio_type=ACORAL_ABSOLUTE_PRIO;
	acoral_create_thread_ext(acoral_shell_enter,SHELL_STACK_SIZE,(void *)acoral_cur_thread->console_id,"shell",NULL,ACORAL_SCHED_POLICY_COMM,&data);
#endif
}

void acoral_shell_exit(void *ptr){
	acoral_free(ptr);
	acoral_thread_exit();
}

void acoral_shell_enter(void *args){
	acoral_char *cmd_buf;
	acoral_set_thread_console((acoral_id)args);
	if(acoral_cur_thread->console_id<0){
		acoral_prints("The thread have no Console\n");
		return;
	}
	cmd_buf=acoral_malloc(sizeof(BUF_SIZE));
	while(1){
		acoral_prints("\r\n");
		acoral_prints("aCoral:>");
		get_cmd(cmd_buf);
		if(cmd_buf[0])
			cmd_exe(cmd_buf);
	}
}
