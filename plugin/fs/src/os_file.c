#define IN_OSFILE
#include "../include/fs.h"
#include "../include/os_file.h"
#include <mutex.h>
#include <mem.h>
acoral_evt_t *fs_mutex;

acoral_32 acoral_unlink(const acoral_char *pathname)
{
	acoral_u32 ret;
	acoral_u8  fs_ret,len;
	acoral_char *path;
	len=acoral_str_len(pathname);
	if(path=(acoral_char *)acoral_malloc2(len+1))
	{
		acoral_str_cpy(path,pathname);
		path[len]=0;
	}
	else
		return -1;
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
	{
		acoral_free2(path);
		return -1;
	}
	fs_ret=RemoveFile(path);
	acoral_mutex_post(fs_mutex);
	acoral_free2(path);
	if(fs_ret!=RETURN_OK)
		return -1;
	return 0;
}


acoral_32 acoral_open(const acoral_char *pathname,acoral_32 oflag)
{
	acoral_u32 ret;
	acoral_32 fd;
	acoral_u8 len;
	acoral_char flag[3];
	acoral_char *pflag=flag;
	acoral_char *path;
	len=acoral_str_len(pathname);
	if(path=(acoral_char *)acoral_malloc2(len+1))
	{
		acoral_str_cpy(path,pathname);
		path[len]=0;
	}
	else
		return -1;
		
	if(oflag&O_RDONLY)
		*pflag++='r';
	if(oflag&O_WRONLY)
		*pflag++='w';
	*pflag='\0';
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
	{
		acoral_free2(path);
		return -1;
	}
	fd=FileOpen(path,flag);
	acoral_mutex_post(fs_mutex);
	acoral_free2(path);
	return fd;
}

acoral_32 acoral_close(acoral_32 fd)
{
	acoral_u32 ret;
	acoral_u8 fs_ret;
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
		return -1;
	fs_ret=FileClose(fd);
	AllCacheWriteBack();
	acoral_mutex_post(fs_mutex);
	if(fs_ret!=RETURN_OK)
		return -1;
	return 0;
}

acoral_32 acoral_read(acoral_32 fd,void *buf,acoral_u32 nbytes)
{
	acoral_u32 ret;
	acoral_32  fs_ret;
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
		return -1;
	fs_ret=FileRead(buf,nbytes,fd);
	acoral_mutex_post(fs_mutex);
	return fs_ret;
}

acoral_32 acoral_write(acoral_32 fd,const void *buf,acoral_u32 nbytes)
{
	acoral_u32 ret,fs_ret;
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
		return -1;
	fs_ret=FileWrite(buf,nbytes,fd);
	acoral_mutex_post(fs_mutex);
	return fs_ret;
}

acoral_32 acoral_closeall(void)
{
	acoral_u32 ret;
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
		return -1;
	FileCloseAll();
	acoral_mutex_post(fs_mutex);
	return 0;
}

acoral_u32 acoral_lseek(acoral_32 fd,acoral_32 offset,acoral_u8 whence)
{
	acoral_u32 ret,fs_ret;
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
		return -1;
	fs_ret=FileSeek(fd,offset,whence);
	acoral_mutex_post(fs_mutex);
	return fs_ret;
}

acoral_32 acoral_mkdir(const acoral_char *pathname,acoral_u8 mode)
{
	acoral_u32 ret;
	acoral_u8 fs_ret,len;
	acoral_char *path;
	len=acoral_str_len(pathname);
	if(path=(acoral_char *)acoral_malloc2(len+1))
	{
		acoral_str_cpy(path,pathname);
		path[len]=0;
	}
	else
		return -1;
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
	{
		acoral_free2(path);
		return -1;
	}
	fs_ret=MakeDir(path);
	acoral_mutex_post(fs_mutex);
	acoral_free2(path);
	if(fs_ret!=RETURN_OK)
		return -1;
	return 0;	
}

acoral_32 acoral_rmdir(const acoral_char *pathname)
{
	acoral_u32 ret;
	acoral_u8 fs_ret,len;
	acoral_char *path;
	len=acoral_str_len(pathname);
	if(path=(acoral_char *)acoral_malloc2(len+1))
	{
		acoral_str_cpy(path,pathname);
		path[len]=0;
	}
	else
		return -1;
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
	{
		acoral_free2(path);
		return -1;
	}
	fs_ret=RemoveDir(path);
	acoral_mutex_post(fs_mutex);
	acoral_free2(path);
	if(fs_ret!=RETURN_OK)
		return -1;
	return 0;
}

acoral_32 acoral_chdir(const acoral_char *pathname)
{
	acoral_u32 ret;
	acoral_u8 fs_ret,len;
	acoral_char *path;
	len=acoral_str_len(pathname);
	if(path=(acoral_char *)acoral_malloc2(len+1))
	{
		acoral_str_cpy(path,pathname);
		path[len]=0;
	}
	else
		return -1;
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
	{
		acoral_free2(path);
		return -1;
	}
	fs_ret=ChangeDir(path);
	acoral_mutex_post(fs_mutex);
	acoral_free2(path);
	if(fs_ret!=RETURN_OK)
		return -1;
	return 0;
}

void acoral_flush(void)
{
	acoral_u32 ret;
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
		return;
	AllCacheWriteBack();
	acoral_mutex_post(fs_mutex);
	return;
}

acoral_32 acoral_mount(acoral_char *source,const char *target,
					   const acoral_char *filesystemtype,acoral_u32 mountflags,
					   const void *data)
{
	acoral_u32 ret;
	acoral_32 fs_ret;
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
		return -1;
	fs_ret=AddFileDriver(FsCommand,source,GetDrive(target));
	if(fs_ret==-1)
	{
		acoral_mutex_post(fs_mutex);
		return -1;
	}
	acoral_mutex_post(fs_mutex);
	return 0;
}

acoral_32 acoral_umount(const char *target)
{
	acoral_u32 ret;
	ret=acoral_mutex_pend(fs_mutex,0);
	if(ret!=MUTEX_SUCCED)
		return -1;
	RemoveFileDriver(GetDrive(target));
	acoral_mutex_post(fs_mutex);
	return 0;
}

void acoral_fs_init(void)
{
    DiskInit();
    FileInit();	

    fs_mutex=acoral_mutex_create(0,NULL);
    acoral_mount("disk","A:",NULL,0,NULL);
}
