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

#include "../include/common.h"
#include "../include/blockheap.h"
#include "../include/mouse.h"
#include "../include/keyboard.h"
#include "../include/gal.h"
#include "../include/rect.h"
#include "../include/invalidregion.h"
#include "../include/clipregion.h"
#include "../include/hdc.h"
#include "../include/lguiapp.h"
#include "../include/gmessage.h"
#include "../include/regclass.h"
#include "../include/shmem.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/gtimer.h"
static void /*inline*/ 
InitTimerValueMutex();

static void /*inline*/ 
LockTimerValue();

static void /*inline*/ 
UnLockTimerValue();


static void 
IncTotalTimerNumber();


static void 
DecTotalTimerNumber();



//number of total timer include control and user defined in the program
int iTotalTimer=0;

PTimerLink pTimerHead =NULL;
PTimerLink pTimerTail =NULL;

mutex_t mutex_timer;

//desktop and mainwindow will register the timer routine
//when being created.
void 
RegisterTimerRoutine()
{
	PortTimerAttach(TimerRoutine);
	//initialize
	InitTimerValueMutex();
}

//routine 
void 
TimerRoutine()
{
	PTimerLink pLink;
	PWindowsTree pWin;
	int iWinType;
	pLink = pTimerHead;
	while(pLink){
		if(!pLink->iCurValue){
			iWinType=GetWinType(pLink->hWnd);
			if(iWinType==WS_CONTROL)
				pWin=((PWindowsTree)(pLink->hWnd))->pParent;
			else
				pWin=(PWindowsTree)(pLink->hWnd);
			//send message to message queue
			PostTimerMessage((HWND)pWin,pLink->hWnd,pLink->id);
			pLink->iCurValue = pLink->iInterval;
		}
		else{
			pLink->iCurValue --;
		}
		pLink = pLink->pNext;
	}
}

BOOL GUIAPI SetTimer(HWND hWnd, int id, int speed){
	int iCounter, iSlot;
	PWindowsTree pWin;
	PTimerLink pLink;

	pWin=(PWindowsTree)hWnd;
	if(GetWinType(hWnd)==WS_CONTROL)
		pWin=pWin->pParent;
	//slot is full
	if(pWin->pMsgQueue->TimerMask == 0xff)
		return false;
	//find a empty slot
	for(iCounter=0;iCounter<NUM_WIN_TIMERS;iCounter++){
		if((pWin->pMsgQueue->TimerMask & (0x0001 << iCounter))==0)
			break;
	}
	if(iCounter == NUM_WIN_TIMERS)
		return false;
	iSlot=iCounter;
	//whether ID exists
	for(iCounter=0;iCounter<NUM_WIN_TIMERS;iCounter++){
		if(pWin->pMsgQueue->TimerID[iCounter] == id)
			return false;
	}

	pLink=(PTimerLink)malloc(sizeof(TimerLink));
	memset(pLink,0,sizeof(TimerLink));
	if(!pLink)
		return false;
	if((pTimerHead = pTimerTail) == NULL){
		pTimerHead = pTimerTail = pLink;
	}
	else{
		pLink->pNext = NULL;
		pLink->pPrev = pTimerTail;
		pTimerTail->pNext = pLink;
		pTimerTail = pLink;
	}
	pLink->hWnd = hWnd;
	pLink->id = id;
	pLink->iCurValue = speed;
	pLink->iInterval = speed;

	//added into message queue
	pWin->pMsgQueue->TimerID[iSlot] = id;
	pWin->pMsgQueue->TimerOwner[iSlot] = hWnd;
	pWin->pMsgQueue->TimerMask |= 0x0001<<iSlot;
	//increase the total number
	IncTotalTimerNumber();
	return true;
}

//kill a timer 
BOOL GUIAPI KillTimer(HWND hWnd,int id){
	PWindowsTree pWin;
	PTimerLink pLink;
	int iCounter;

	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return false;

	pLink = pTimerHead;
	while(pLink){
		if((pLink->hWnd == hWnd)&&(pLink->id == id))
			break;
		pLink = pLink->pNext;
	}
	//no found 
	if(!pLink)
		return false;
	//clear the node in the link table
	if(pTimerHead != pTimerTail){
		if(pLink == pTimerHead){
			pLink->pNext->pPrev = NULL;
			pTimerHead = pLink->pNext;
		}
		else if(pLink == pTimerTail){
			pLink->pPrev->pNext = NULL;
			pTimerTail = pLink->pPrev;
		}
		else{
			pLink->pPrev->pNext = pLink->pNext;
			pLink->pNext->pPrev = pLink->pPrev;
		}
	}
	else{
		pTimerHead = pTimerTail = NULL;
	}
	free(pLink);


	if(GetWinType(hWnd)==WS_CONTROL)
		pWin = pWin->pParent;
	for(iCounter=0;iCounter<NUM_WIN_TIMERS;iCounter++){
		if(pWin->pMsgQueue->TimerID[iCounter] == id)
			break;
	}
	if(iCounter == NUM_WIN_TIMERS)
		return false;
	//reset
	pWin->pMsgQueue->TimerMask &= ~ 0x0001<<iCounter;
	pWin->pMsgQueue->TimerID[iCounter]=0;

	DecTotalTimerNumber();

	return true;
}
/**/
//
//reset a timer
BOOL GUIAPI 
ResetTimer(
	HWND hWnd,
	int id, 
	int speed
)
{
	PWindowsTree pWin;
	PTimerLink pLink;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return false;
	pLink=pTimerHead;
	while(pLink){
		if((pLink->hWnd == hWnd) && (pLink->id == id)){
			pLink->iCurValue = speed;
			pLink->iInterval = speed;
			return true;
		}
		pLink = pLink->pNext;
	}
	return true;

}
//start system timer
void GUIAPI 
StartTimer()
{
/*	struct itimerval value;
	value.it_value.tv_sec=0;
	value.it_value.tv_usec=10000;
	value.it_interval=value.it_value;
	setitimer(ITIMER_REAL,&value,NULL);
*/
}

//stop system timer
void GUIAPI 
EndTimer()
{
/*
	struct itimerval value;
	value.it_value.tv_sec=0;
	value.it_value.tv_usec=0;
	value.it_interval=value.it_value;
	setitimer(ITIMER_REAL,&value,NULL);
*/
}

//increase the total timer number
//if the old value is zero, 
//then start system timer
static void IncTotalTimerNumber()
{
	LockTimerValue();
	if(iTotalTimer==0){
		StartTimer();
	}
	iTotalTimer ++;
	UnLockTimerValue();
}

//decrease the total timer number
//if the new value is zero
//then stop system timer
static void DecTotalTimerNumber()
{
	LockTimerValue();
	if(iTotalTimer==1){
		EndTimer();
	}
	iTotalTimer--;
	UnLockTimerValue();
}



static void /*inline*/ 
InitTimerValueMutex()
{
	pthread_mutex_init(&mutex_timer,NULL);
}


static void /*inline*/ 
LockTimerValue()
{
	pthread_mutex_lock(&mutex_timer);
}


static void /*inline*/ 
UnLockTimerValue()
{
	pthread_mutex_unlock(&mutex_timer);
}


void 
UnInitTimer()
{
	PTimerLink pLink;
	EndTimer();
	while(pTimerHead){
		pLink=pTimerHead->pNext;
		free(pTimerHead);
		pTimerHead=pLink;
	}
}

