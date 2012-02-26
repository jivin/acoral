#include <autocfg.h>
#include "acoral.h"
#include "fs.h"
#include "os_file.h"
#include <mutex.h>
#include "../include/shell.h"
extern acoral_evt_t *fs_mutex;
acoral_char CurrentDirName[12];
void cd_dir(acoral_32 argc,acoral_char **argv)
{
	acoral_u32 ret;
	acoral_u8 fs_ret;
	acoral_u32 len;
	acoral_char *path;
	if(argc!=2)
	{
		acoral_print("Usage ls <Pathname>\n");
		return ;
	}
	path=argv[1];
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
		return;
	fs_ret=ChangeDir(path);
	acoral_mutex_post(fs_mutex);
	if(fs_ret!=RETURN_OK)
		return ;
	len=acoral_str_len(path);
	path+=len;
	while((len--)!=0 && *path!='/' && *path!='\\')
		path--;
	acoral_str_ncpy(CurrentDirName,path,11);
	CurrentDirName[11]='\0';
	acoral_print("%s>",path);
	return ;	
}

acoral_shell_cmd_t cd_cmd={
	"cd",
	cd_dir,
	"Change current dir",
	NULL
};

void print(FDT *Rt,acoral_u8 detail)
{
	acoral_char *name,*pbuf;
	acoral_char buf[38];
	acoral_u8 i;
	static acoral_u8 j=0;
	for(i=0;i<37;i++)
		buf[i]=' ';
	buf[37]='\0';
	pbuf=buf;

	name=Rt->Name;
	for(i=0;i<8;i++)
	{
		if(*name!=0x20)
			*pbuf++=*name++;
		else
			break;
	}
	name=Rt->Name+8;
	if(*name!=0x20)
	{
		*pbuf++='.';
		for(i=0;i<3;i++)
		{
			if(*name!=0x20)
				*pbuf++=*name++;
			else
				break;
		}
	}
	if(detail)
	{
		pbuf=buf+13;
		acoral_sprint(pbuf,16,"%10dBytes",Rt->FileSize);
		pbuf[15]=' ';
		pbuf+=18;
		name=(Rt->Attr&0x10)?"dir":"file";
		acoral_sprint(pbuf,5,"%s",name);
		acoral_print("%s\n",buf);
	}
	else
	{
		acoral_print("%s\n",buf);

	}
}

void ls_dir(acoral_32 argc,acoral_char **argv)
{
	acoral_u32 ret;
	acoral_u32 len;
	acoral_u8 temp;
	acoral_u32 i;
	FDT Rt;
	acoral_u8 Drive;
	acoral_u32 ClusIndex;
	acoral_u8 flag;
	Drive=CurrentDrive;
	ClusIndex=DiskInfo[Drive].PathClusIndex;
	flag=0;
	if(argc==2)
	{
		if(argv[1][0]=='-' && argv[1][1]=='a')
			flag=1;
	}

	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
		return;
	
	i=0;
	while(1)
	{
		temp=GetFDTInfo(&Rt,Drive,ClusIndex,i);
		if(temp!=RETURN_OK)
		{
			break;
		}
		if(Rt.Name[0]==EMPTY_FDT)
		{
			break;
		}
		if(Rt.Name[0]==DEL_FDT)
		{
			i++;
			continue;
		}
		print(&Rt,flag);
		i++;
	}
	
	acoral_mutex_post(fs_mutex);
	acoral_print("\n%s>",CurrentDirName);
	return ;	
}

acoral_shell_cmd_t ls_cmd={
	"ls",
	ls_dir,
	"List file/dir in current dir",
	NULL
};

void mk_dir(acoral_32 argc,acoral_char **argv)
{
	if(argc!=2)
	{
		acoral_print("Usage mkdir <Pathname>!\n");
		return ;
	}
	acoral_mkdir(argv[1],0);
	acoral_flush();
}
acoral_shell_cmd_t mkdir_cmd={
	"mkdir",
	mk_dir,
	"make a dir in current dir",
	NULL
};

void rm_dir(acoral_32 argc,acoral_char **argv)
{
	acoral_32 ret;
	if(argc!=2)
	{
		acoral_print("Usage rmdir <Pathname>!\n");
		return ;
	}
	ret=acoral_rmdir(argv[1]);
	if(ret==-1)
	{
		acoral_print("delete dir:%s error!\n%s must be empty!\n",argv[1],argv[1]);
		return ;
	}
	acoral_flush();
}
acoral_shell_cmd_t rmdir_cmd={
	"rmdir",
	rm_dir,
	"delete a dir in current dir",
	NULL
};

static acoral_char tmp[128];
void copy_file(acoral_32 argc,acoral_char **argv)
{
	acoral_32 fd,fd1;
	int ret=0,i;
	if(argc!=3)
	{
		acoral_print("Usage create <srcname> <targetname>!\n");
		return ;
	}
	fd=acoral_open(argv[1],O_RDONLY);
	fd1=acoral_open(argv[2],O_RDWR);
	if(fd==-1||fd1==-1)
	{
		acoral_print("copy file:%s error!\n",argv[1]);
		return ;
	}
	
	do{
		i=acoral_read(fd,tmp,sizeof(tmp));
		acoral_write(fd1,tmp,i);
		ret+=i;
	}while(i>0);
	acoral_close(fd);
	acoral_close(fd1);
}

acoral_shell_cmd_t copy_cmd={
	"cp",
	copy_file,
	"copy a file in current dir",
	NULL
};


void create_file(acoral_32 argc,acoral_char **argv)
{
	acoral_32 fd;
	if(argc!=2)
	{
		acoral_print("Usage create <Filename>!\n");
		return ;
	}
	fd=acoral_open(argv[1],O_RDWR);
	if(fd==-1)
	{
		acoral_print("create file:%s error!\n",argv[1]);
		return ;
	}
	acoral_write(fd,tmp,sizeof(tmp));
	acoral_close(fd);
	acoral_flush();	
}
acoral_shell_cmd_t create_cmd={
	"create",
	create_file,
	"create a file in current dir",
	NULL
};

void rm_file(acoral_32 argc,acoral_char **argv)
{
	if(argc!=2)
	{
		acoral_print("Usage rm <Filename>!\n");
		return ;
	}
	acoral_unlink(argv[1]);
	acoral_flush();
	return;
}

acoral_shell_cmd_t rm_cmd={
	"rm",
	rm_file,
	"delete a file in current dir",
	NULL
};

void fs_cmd_init(){
	add_command(&cd_cmd);
	add_command(&ls_cmd);
	add_command(&mkdir_cmd);
	add_command(&rmdir_cmd);
	add_command(&create_cmd);
	add_command(&rm_cmd);
	add_command(&copy_cmd);
}
