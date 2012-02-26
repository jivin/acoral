#ifndef GUI_PORT_H
#define GUI_PORT_H
#include"kernel.h"
#include"lib.h"
#include"screen.h"
#include"driver.h"
#include<hal.h>
#define _lGUI_pWindowsTree (((win_data *)acoral_cur_thread->data)->curWin)
#define _lGUI_CurStat (((win_data *)acoral_cur_thread->data)->curStat)
#define PortSetCurWin(win) (((win_data *)acoral_cur_thread->data)->curWin=(win))
//#define _lGUI_wndCaptureMouse (PortGetCapture())
void PortClientExit();

#define MOUSEFILE	"/resource/msenew.bmp"

#include"os_file.h"
#define pEnvStr     "/tlgui" 
#define FONT_16_16_SPEC "/resource/ft1616.ft"
#define FONT_14_14_SPEC "/resource/ft1414.ft"
#define FONT_12_12_SPEC "/resource/ft1212.ft"

//typedef acoral_id thread_t;
#define thread_t acoral_id
#define pthread_create(id,attr,route,args)  acoral_posix_create(id,attr,route,args)
#define pthread_exit(type) 
#define pthread_join(pthread_id,atr) 
#define pthread_cancel(pthread_id) 
#define pthread_testcancel(id) 
#define pthread_setcanceltype(type,old_cancel_type)
#define pthread_cleanup_push(id,atr)
#define pthread_self() (acoral_cur_thread->res.id)

typedef acoral_evt_t mutex_t;
#define pthread_mutex_lock(mutex) acoral_mutex_pend(mutex,0)
#define pthread_mutex_unlock(mutex) acoral_mutex_post(mutex)
#define pthread_mutex_init(mutex,attr) acoral_mutex_init(mutex,0)


typedef acoral_evt_t sem_t;
#define sem_init(sem,share,count) acoral_sem_init(sem,count)
#define sem_post(sem) acoral_sem_post(sem)
#define sem_wait(sem) acoral_sem_pend(sem,0)
#define sem_getvalue(sem,value) ((*value)=acoral_sem_getnum(sem))
#define sem_destroy(sem) //局部变量sem,所以要先destory,要不返回时数据失效，而有些还在使用他

typedef acoral_pool_ctrl_t  PrivateHeap;
#define bmalloc(size) acoral_malloc(size)
#define bfree(size) acoral_free(size)

#define malloc(size) acoral_malloc2(size)
#define free(size) acoral_free2(size)

#define memcpy(des,source,size) acoral_memcpy(des,source,size)
#define memset(des,source,size) acoral_memset(des,source,size)

#define strlen(str) acoral_str_len(str)
#define strcpy(to,from) acoral_str_cpy(to,from)
#define strcmp(a,b) acoral_str_cmp(a,b)
#define strncasecmp(to,from,size) acoral_str_nicmp(to,from,size)
#define strncpy(to,from,size) acoral_str_ncpy(to,from,size)
#define strcat(to,from) acoral_str_cat(to,from)
#define strdup(str) NULL
#define strncat(to,from,size) acoral_str_ncat(to,from,size)
#define toupper(str) acoral_toupper(str)

#define printf 		acoral_print
#define printerror 	acoral_printerr
#define sprintf 	acoral_sprint
#define EPRINTF		acoral_print
typedef int FILE_P;
typedef acoral_size size_t;
#define OPEN(path,type) acoral_open(path,type)
#define SEEK(fd,offset,type) acoral_lseek(fd,offset,type)
#define READ(fd,buf,size) acoral_read(fd,buf,size)
#define WRITE(fd,buf,size) acoral_write(fd,buf,size)
#define CLOSE(fd) acoral_close(fd)
#define FLUSH() acoral_flush()
#define RDONLY O_RDONLY 
#define errno 0 
#define EINTR 0 
#define EAGAIN 0 

//math
#define cos(x) 0
#define sin(x) 0
#define sqrt(x) 0
#define PortGetCurWin
//#define OutputTime(hWnd,hDC) 
//#define KillTimer(hWnd,ID_TIMER)
//#define SetTimer(hWnd,id,time)
#endif
