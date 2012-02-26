/*
	Copyright (C) 2004-2005 Li Yudong
*/
/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _MSG {     // msg 
	HWND	hWnd;     
	UINT	message; 
	WPARAM	wParam; 
	LPARAM	lParam; 
	void*	pData;
} MSG;
typedef MSG* PMSG;



typedef struct _NtfMsgLink
{
	MSG		msg;
	struct _NtfMsgLink* pNext;
} NtfMsgLink;
typedef NtfMsgLink* PNtfMsgLink;


typedef struct _SyncMsgLink
{
	MSG		msg;
	int		iRetValue;
	sem_t	sem;
	struct _SyncMsgLink* pNext;
} SyncMsgLink;

typedef SyncMsgLink* PSyncMsgLink;


typedef struct _WndMailBox
{
	MSG			msg[SIZE_WND_MAILBOX];
	int			iReadPos;
	int			iWritePos;
} WndMailBox;
typedef WndMailBox* PWndMailBox;



typedef struct _MsgQueue
{
	mutex_t 	mutex;       
	sem_t			sem;  

	DWORD			dwState;              
         
	PSyncMsgLink		pHeadSyncMsg;
	PSyncMsgLink		pTailSyncMsg;

	PNtfMsgLink		pHeadNtfMsg;
	PNtfMsgLink		pTailNtfMsg;

	PNtfMsgLink		pHeadPntMsg;
	PNtfMsgLink		pTailPntMsg;

	WndMailBox		wndMailBox;

    	HWND			TimerOwner[NUM_WIN_TIMERS];        
    	int				TimerID[NUM_WIN_TIMERS];            
    	WORD			TimerMask;						
} 	MsgQueue;
typedef MsgQueue* 	PMsgQueue;







BOOL 
InitMsgQueueHeap();

void 
DestroyMsgQueueHeap();


BOOL 
InitMsgQueue(
	HWND hWnd
);

void 
DestroyMsgQueue(
	HWND hWnd
);

PMsgQueue 
GetMsgQueue(
	HWND hWnd
);

BOOL GUIAPI
GetMessage(
	PMSG pMsg,
	HWND hWnd
);


BOOL GUIAPI
PostMessage(
	HWND hWnd,
	int iMsg,
	WPARAM wParam,
	LPARAM lParam
);

BOOL GUIAPI
PostQuitMessage(
	HWND hWnd
);

int GUIAPI
SendMessage(
	HWND hWnd,
	int iMsg,
	WPARAM wParam,
	LPARAM lParam
);


BOOL GUIAPI
SendNotifyMessage(
	HWND hWnd,
	int iMsg,
	WPARAM wParam,
	LPARAM lParam
);

BOOL
SendPaintMessage(
	HWND hWnd
);

BOOL GUIAPI
TranslateMessage(
	PMSG pMsg
);

int
PostSyncMessage(
	HWND hWnd,
	int msg,
	WPARAM wParam,
	LPARAM lParam
);

int GUIAPI
DispatchMessage(
	PMSG pMsg
);

BOOL
PostTimerMessage(
	HWND hWndWin,
	HWND hWndOwner,
	int id
);

BOOL GUIAPI
NotifyParent(
	HWND hWnd,
	int iMsg
);




#ifdef __cplusplus
}
#endif 

#endif



