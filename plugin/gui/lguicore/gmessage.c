/*
	Copyright (C) 2004-2005 Li Yudong
	Some ideas come from MiniGUI
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

#include "../include/common.h"
#include "../include/blockheap.h"
#include "../include/invalidregion.h"
#include "../include/clipregion.h"
#include "../include/gmessage.h"
#include "../include/hdc.h"
#include "../include/regclass.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/lguiapp.h"


#define QS_SYNCMSG				0x10000000
#define QS_NOTIFYMSG			0x20000000
#define QS_POSTMSG				0x40000000
#define QS_QUITMSG				0x80000000
#define QS_PAINTMSG			0x01000000

#define QS_TIMER				0x0000FFFF
#define QS_EMPTY				0x00000000



extern PWNDCLASSEX WndClassRegTable[26];

static PrivateHeap MsgQueueHeap;

BOOL 
InitMsgQueueHeap()
{
	return HeapCreate(&MsgQueueHeap,SIZE_QMSG_HEAP,sizeof(NtfMsgLink));
}

void 
DestroyMsgQueueHeap()
{
	HeapDestroy(&MsgQueueHeap);
}


BOOL 
InitMsgQueue(
	HWND hWnd
)
{
	PMsgQueue pMsgQueue;
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	
	pMsgQueue = (PMsgQueue)malloc(sizeof(MsgQueue));
	if(!(pMsgQueue))
		return false;
	memset(pMsgQueue,0,sizeof(MsgQueue));
	pMsgQueue->dwState = QS_EMPTY;
	pthread_mutex_init(&pMsgQueue->mutex,NULL);
	sem_init(&pMsgQueue->sem,0,0);
	pWin->pMsgQueue = pMsgQueue;
	return true;
}

void 
DestroyMsgQueue(
	HWND hWnd
)
{
	PWindowsTree pWin;
	PNtfMsgLink pNext,pHead;
	pWin = (PWindowsTree)hWnd;

	pNext = pHead = pWin->pMsgQueue->pHeadNtfMsg;
	while(pHead){
		pNext = pHead->pNext;
		HeapFree(&MsgQueueHeap,(void*)pHead);
		pHead = pNext;
	}

	pNext = pHead = pWin->pMsgQueue->pHeadPntMsg;
	while(pHead){
		pNext = pHead->pNext;
		HeapFree(&MsgQueueHeap,(void*)pHead);
		pHead = pNext;
	}
	sem_destroy(&pWin->pMsgQueue->sem);
	free(pWin->pMsgQueue);
}

PMsgQueue 
GetMsgQueue(
	HWND hWnd
)
{
	PWindowsTree pWin;
	int iType;
	pWin = (PWindowsTree)hWnd;
	iType = GetWinType(hWnd);
	if((iType == WS_DESKTOP)||(iType == WS_MAIN) || (iType == WS_CHILD))
		return pWin->pMsgQueue;
	else
		return pWin->pParent->pMsgQueue;
}

BOOL GUIAPI
GetMessage(
	PMSG pMsg,
	HWND hWnd
)
{
	int slot;
	PNtfMsgLink pHead;
	PMsgQueue pMsgQueue;

	pMsgQueue = GetMsgQueue(hWnd);
	memset(pMsg,0,sizeof(MSG));

loop:
	if(pMsgQueue->dwState & QS_QUITMSG){
		pMsg->hWnd = hWnd;
		pMsg->message =LMSG_QUIT;
		pMsg->wParam = pMsg->lParam = 0;
		ExitWindow(hWnd);
		return 0;
	}
	if (pMsgQueue->dwState & QS_TIMER) {
		pthread_mutex_lock (&pMsgQueue->mutex);
		for (slot = 0; slot < NUM_WIN_TIMERS; slot++) {
			if (LOWORD(pMsgQueue->dwState) & (0x0001 << slot))
				break;
		}
		if (slot == NUM_WIN_TIMERS) {
			pMsgQueue->dwState &= ~QS_TIMER;
			pthread_mutex_unlock (&pMsgQueue->mutex);
		}
		else {//bit of slot is 1
			pMsg->hWnd = pMsgQueue->TimerOwner[slot];
			pMsg->message = LMSG_TIMER;
			pMsg->wParam = pMsgQueue->TimerID[slot];
			pMsg->lParam = 0;

			pMsgQueue->dwState &= ~(0x0001 << slot);
			pthread_mutex_unlock (&pMsgQueue->mutex);
			return 1;
		}
	}
	
	if (pMsgQueue->dwState & QS_SYNCMSG) {
		pthread_mutex_lock (&pMsgQueue->mutex);
		if (pMsgQueue->pHeadSyncMsg) {
			memcpy(pMsg,&pMsgQueue->pHeadSyncMsg->msg,sizeof(MSG));
			pMsg->pData = (void*)(pMsgQueue->pHeadSyncMsg);
			pMsgQueue->pHeadSyncMsg = pMsgQueue->pHeadSyncMsg->pNext;

			pthread_mutex_unlock (&pMsgQueue->mutex);
			return 1;
		}
		else
			pMsgQueue->dwState &= ~QS_SYNCMSG;

		pthread_mutex_unlock (&pMsgQueue->mutex);
	}

	if (pMsgQueue->dwState & QS_NOTIFYMSG) {
		pthread_mutex_lock (&pMsgQueue->mutex);
		if (pMsgQueue->pHeadNtfMsg) {
			pHead = pMsgQueue->pHeadNtfMsg;
			pMsgQueue->pHeadNtfMsg = pHead->pNext;
			memcpy(pMsg,&pHead->msg,sizeof(MSG));
			HeapFree(&MsgQueueHeap,(void*)pHead);
			pthread_mutex_unlock (&pMsgQueue->mutex);
			return 1;
		}
		else{
			pMsgQueue->dwState &= ~QS_NOTIFYMSG;
		}
		pthread_mutex_unlock (&pMsgQueue->mutex);
	}
	if(pMsgQueue->dwState &QS_POSTMSG){
		pthread_mutex_lock (&pMsgQueue->mutex);
		if(pMsgQueue->wndMailBox.iReadPos != pMsgQueue->wndMailBox.iWritePos){
			memcpy(pMsg,
				&pMsgQueue->wndMailBox.msg[pMsgQueue->wndMailBox.iReadPos],
				sizeof(MSG));
			pMsgQueue->wndMailBox.iReadPos ++;
			
			if (pMsgQueue->wndMailBox.iReadPos == SIZE_WND_MAILBOX) 
				pMsgQueue->wndMailBox.iReadPos = 0;
			pthread_mutex_unlock (&pMsgQueue->mutex);
			//printf("%d,%d\n",pMsgQueue->wndMailBox.iReadPos,pMsgQueue->wndMailBox.iWritePos);
			return 1;
		}
		else{
			pMsgQueue->dwState &= ~QS_POSTMSG;
		}
		pthread_mutex_unlock (&pMsgQueue->mutex);
	}

	if (pMsgQueue->dwState & QS_PAINTMSG) {
		pthread_mutex_lock (&pMsgQueue->mutex);
		if (pMsgQueue->pHeadPntMsg) {
			pHead = pMsgQueue->pHeadPntMsg;
			pMsgQueue->pHeadPntMsg = pHead->pNext;
			memcpy(pMsg,&pHead->msg,sizeof(MSG));
			HeapFree(&MsgQueueHeap,(void*)pHead);
			pthread_mutex_unlock (&pMsgQueue->mutex);
			return 1;
		}
		else{
			pMsgQueue->dwState &= ~QS_PAINTMSG;
		}
		pthread_mutex_unlock (&pMsgQueue->mutex);

	}
	sem_wait (&pMsgQueue->sem);
	goto loop;
	return 1;
}


BOOL GUIAPI
PostMessage(
	HWND hWnd,
	int iMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	
	PMsgQueue pMsgQueue;
	PWindowsTree pWin;
	PMSG pMsg;
	int sem_value;
	if(!hWnd)
		return false;
	pWin=(PWindowsTree)hWnd;
	pMsgQueue = GetMsgQueue(hWnd);
	if(!pMsgQueue)
		return false;
	pthread_mutex_lock(&pMsgQueue->mutex);
	if(iMsg == LMSG_QUIT){
		pMsgQueue->dwState |= QS_QUITMSG;
	}
	else{
		if((pMsgQueue->wndMailBox.iWritePos + 1) % SIZE_WND_MAILBOX
			== pMsgQueue->wndMailBox.iReadPos)
			return false;
		
		pMsg = &(pMsgQueue->wndMailBox.msg[pMsgQueue->wndMailBox.iWritePos]);
		pMsg->hWnd = hWnd;
		pMsg->message = iMsg;
		pMsg->wParam = wParam;
		pMsg->lParam = lParam;
		
		pMsgQueue->wndMailBox.iWritePos ++;
		if(pMsgQueue->wndMailBox.iWritePos >= SIZE_WND_MAILBOX)
			pMsgQueue->wndMailBox.iWritePos = 0;
	
		pMsgQueue->dwState |= QS_POSTMSG;
	}

	pthread_mutex_unlock (&pMsgQueue->mutex);
	if(pWin->threadid != pthread_self()){
		sem_getvalue (&pMsgQueue->sem, &sem_value);
		if(sem_value <= 0)
		sem_post(&pMsgQueue->sem);
	}
	return true;
}


BOOL GUIAPI
PostQuitMessage(
	HWND hWnd
)
{
	return PostMessage(hWnd,LMSG_QUIT,(WPARAM)NULL,(LPARAM)NULL);
}


int GUIAPI
SendMessage(
	HWND hWnd,
	int iMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
    WNDPROC WndProc;
	PWindowsTree pWin;
	int iWinType;
	pWin=(PWindowsTree)hWnd;

	iWinType=GetWinType(hWnd);
	if(
		((iWinType == WS_DESKTOP) || (iWinType == WS_MAIN) || (iWinType == WS_CHILD))
		&&
	    (pWin->threadid != pthread_self())
	   )
        return PostSyncMessage (hWnd, iMsg, wParam, lParam);

    WndProc = GetWndProc(hWnd);

    return (*WndProc)(hWnd, iMsg, wParam, lParam);
}


BOOL GUIAPI
SendNotifyMessage(
	HWND hWnd,
	int iMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	PMsgQueue pMsgQueue;
	PNtfMsgLink pLinkNode;
	int sem_value;
		
	pMsgQueue = GetMsgQueue(hWnd);

	if(!pMsgQueue)
		return false;

	pLinkNode = HeapAlloc(&MsgQueueHeap);

	pthread_mutex_lock(&pMsgQueue->mutex);
	pLinkNode->msg.hWnd = hWnd;
	pLinkNode->msg.message = iMsg;
	pLinkNode->msg.wParam = wParam;
	pLinkNode->msg.lParam = lParam;
	pLinkNode->msg.pData = NULL;
	pLinkNode->pNext = NULL;

	if(!pMsgQueue->pHeadNtfMsg){
		pMsgQueue->pHeadNtfMsg = pMsgQueue->pTailNtfMsg = pLinkNode;
	}
	else{
		pMsgQueue->pTailNtfMsg->pNext = pLinkNode;
		pMsgQueue->pTailNtfMsg = pLinkNode;
	}
	pMsgQueue->dwState |= QS_NOTIFYMSG;

	pthread_mutex_unlock (&pMsgQueue->mutex);
	sem_getvalue (&pMsgQueue->sem, &sem_value);
	if (sem_value <= 0)
		sem_post(&pMsgQueue->sem);
	return true;
}

BOOL
SendPaintMessage(
	HWND hWnd
)
{
	PMsgQueue pMsgQueue;
	PNtfMsgLink pLinkNode;
	int sem_value;
	PWindowsTree pWin=(PWindowsTree)hWnd;
	pMsgQueue = GetMsgQueue(hWnd);

	if(!pMsgQueue)
		return false;
	pLinkNode = HeapAlloc(&MsgQueueHeap);
	
	pthread_mutex_lock(&pMsgQueue->mutex);
	pLinkNode->msg.hWnd = hWnd;
	pLinkNode->msg.message = LMSG_PAINT;
	pLinkNode->msg.wParam = (WPARAM)NULL;
	pLinkNode->msg.lParam = (LPARAM)NULL;
	pLinkNode->msg.pData = NULL;
	pLinkNode->pNext = NULL;

	if(!pMsgQueue->pHeadPntMsg){
		pMsgQueue->pHeadPntMsg = pMsgQueue->pTailPntMsg = pLinkNode;
	}
	else{
		pMsgQueue->pTailPntMsg->pNext = pLinkNode;
		pMsgQueue->pTailPntMsg = pLinkNode;
	}

	pMsgQueue->dwState |= QS_PAINTMSG;

	pthread_mutex_unlock (&pMsgQueue->mutex);
	if(pWin->threadid != pthread_self()){
		sem_getvalue (&pMsgQueue->sem, &sem_value);
		if (sem_value <= 0)
			sem_post(&pMsgQueue->sem);
	}
	return true;
}

BOOL GUIAPI
TranslateMessage(
	PMSG pMsg
)
{
	return true;
}


int
PostSyncMessage(
	HWND hWnd,
	int msg,
	WPARAM wParam,
	LPARAM lParam
)
{
	PMsgQueue pMsgQueue;
	SyncMsgLink	syncMsgLink;
	int sem_value;
	if(!(pMsgQueue = GetMsgQueue(hWnd)))
		return -1;
	pthread_mutex_lock(&pMsgQueue->mutex);
	syncMsgLink.msg.hWnd = hWnd;
	syncMsgLink.msg.message = msg;
	syncMsgLink.msg.wParam = wParam;
	syncMsgLink.msg.lParam = lParam;
	syncMsgLink.pNext = NULL;
	sem_init(&syncMsgLink.sem,0,0);
	
	if(pMsgQueue->pHeadSyncMsg == NULL){
		pMsgQueue->pHeadSyncMsg = pMsgQueue->pTailSyncMsg = &syncMsgLink;
	}
	else{
		pMsgQueue->pTailSyncMsg->pNext = &syncMsgLink;
		pMsgQueue->pTailSyncMsg = &syncMsgLink;
	}

    pMsgQueue->dwState |= QS_SYNCMSG;

    pthread_mutex_unlock (&pMsgQueue->mutex);

    // Signal that the msg queue contains one more element for reading
    sem_getvalue (&pMsgQueue->sem, &sem_value);
    if (sem_value <= 0)
        sem_post(&pMsgQueue->sem);

     sem_wait(&syncMsgLink.sem);
     sem_destroy(&syncMsgLink.sem);
      return syncMsgLink.iRetValue;
}


int GUIAPI
DispatchMessage(
	PMSG pMsg
)
{
	WNDPROC WndProc;
	PSyncMsgLink pSyncMsgLink;
	PWindowsTree pWin, pCurWin;
	if(!pMsg->hWnd){
		if(pMsg->pData){
			pSyncMsgLink = (PSyncMsgLink)pMsg->pData;
			pSyncMsgLink->iRetValue = false;
			sem_post(&pSyncMsgLink->sem);
		}
		return 0;
	}

    WndProc = GetWndProc(pMsg->hWnd);
	if(!WndProc)
		return 0;
	pWin=(PWindowsTree)(pMsg->hWnd);
	if(IsChildWin(pMsg->hWnd)){
		pCurWin=(pWin->pParent)->pChildHead;
		while(pCurWin){
			if(pCurWin == pWin)
				break;
			pCurWin = pCurWin->pNext;
		}
		if(!pCurWin){
			if(pMsg->pData){
				pSyncMsgLink = (PSyncMsgLink)pMsg->pData;
				pSyncMsgLink->iRetValue = false;
				sem_post(&pSyncMsgLink->sem);
			}
			return 0;
		}
	}
	else if(IsControl(pMsg->hWnd)){
		pCurWin = (pWin->pParent)->pControlHead;
		while(pCurWin){
			if(pCurWin == pWin)
				break;
			pCurWin = pCurWin->pNext;
		}
		if(!pCurWin){
			if(pMsg->pData){
				pSyncMsgLink = (PSyncMsgLink)pMsg->pData;
				pSyncMsgLink->iRetValue = false;
				sem_post(&pSyncMsgLink->sem);
			}
			return 0;
		}
	}


    if (pMsg->pData){
		pSyncMsgLink = (PSyncMsgLink)pMsg->pData;
		pSyncMsgLink->iRetValue = (*WndProc)
                   (pMsg->hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
        sem_post(&pSyncMsgLink->sem);
        return pSyncMsgLink->iRetValue;
    }

    return (*WndProc)(pMsg->hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
}

BOOL
PostTimerMessage(
	HWND hWndWin,
	HWND hWndOwner,
	int id
)
{
	PMsgQueue pMsgQueue;
	int iCounter;
	int sem_value;
	if(!(pMsgQueue = GetMsgQueue(hWndWin)))
		return false;
	pthread_mutex_lock(&pMsgQueue->mutex);

	for(iCounter=0;iCounter<NUM_WIN_TIMERS;iCounter++){
		if(pMsgQueue->TimerID[iCounter] == id)
			break;
	}
	if(iCounter != NUM_WIN_TIMERS){
		pMsgQueue->dwState |= 0x0001 << iCounter;
	}

	pthread_mutex_unlock (&pMsgQueue->mutex);
	sem_getvalue (&pMsgQueue->sem, &sem_value);
	if (sem_value <= 0){
		sem_post(&pMsgQueue->sem);
	}

	return true;
}

BOOL GUIAPI
NotifyParent(
	HWND hWnd,
	int iMsg
)
{
	PWindowsTree pWin;
	unsigned long iRetMsg;
	pWin = (PWindowsTree)hWnd;
	iRetMsg=iMsg;
	iRetMsg=iRetMsg<<16;
	iRetMsg=iRetMsg | (unsigned long)(pWin->hMenu);

	return SendNotifyMessage((HWND)(pWin->pParent), 
				LMSG_COMMAND,(WPARAM)iRetMsg,(LPARAM)NULL);
}





