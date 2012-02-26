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

#ifndef _MOUSE_H_
#define _MOUSE_H_

#ifdef __cplusplus                     
extern "C" {
#endif

typedef  struct tagMOUSEINFO{
	BOOL fHide;				//current status
	int x;					
	int y;					
	BOOL fPaintCompleted;	
	int nWidth;				
	int nHeight;			
	PCOLORREF pCursorBox;	
	PCOLORREF pSaveBox;		
} MOUSEINFO;


void /*inline*/ 
LockMouse();

void /*inline*/ 
UnLockMouse();

BOOL 
WithinCursorBox(
	int x, 
	int y
);

BOOL 
IntersectCursorBox(
	PRECT lpRect
);

void 
ShowMouse();

void 
HideMouse();

BOOL /*inline*/ 
MouseHided();

void 
cleanup_memory(void*);

void *
MouseMainLoop(
	void* para
);
void 
InitMouseServer();

void 
InitMouseClient();

void 
TerminateMouseServer();


void /*inline*/ 
RequestPaint(PRECT lpRect);

void /*inline*/ 
CompletePaint();

void 
PaintTimerOutRoutine(
	int sig_num
);


void /*inline*/ 
SetMousePos();

void /*inline*/ 
GetMousePos();

void /*inline*/ 
SetMouseDimension();

void /*inline*/ 
GetMouseDimension();


#ifdef __cplusplus
}
#endif

#endif
