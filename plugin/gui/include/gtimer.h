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

#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef __cplusplus                     
extern "C" {
#endif
#define GROUTETIME 10
typedef struct tagTimerLink{
	int id;
	int iInterval;
	int iCurValue;
	HWND hWnd;
	struct tagTimerLink* pNext;
	struct tagTimerLink* pPrev;
}TimerLink;

typedef TimerLink* PTimerLink;

void 
RegisterTimerRoutine();

void 
TimerRoutine();

BOOL  GUIAPI 
SetTimer(
	HWND hWnd,
	int id,
	int speed
);

BOOL GUIAPI 
KillTimer(
	HWND hWnd,
	int id
);

BOOL GUIAPI 
ResetTimer(
	HWND hWnd,
	int id, 
	int speed
);


void GUIAPI 
StartTimer();

void GUIAPI 
EndTimer();

void 
UnInitTimer();



#ifdef __cplusplus
}
#endif 

#endif
