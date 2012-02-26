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

#ifndef _MOUSE_IAL_H_
#define _MOUSE_IAL_H_

#ifdef __cplusplus                     
extern "C" {
#endif

#define MOUSE_EVENT_LBUTTON_DOWN		1
#define MOUSE_EVENT_LBUTTON_UP			2
#define MOUSE_EVENT_RBUTTON_DOWN		3	
#define MOUSE_EVENT_RBUTTON_UP			4
#define MOUSE_EVENT_MOVE				5
#define MOUSE_EVENT_RESERVED			6

typedef struct tagMOUSEDEVICE {
        int     (*Open)(void);
        void    (*Close)(void);
        int     (*Read)(int *x,int *y,int* event);
} MOUSEDEVICE;

void 
InstallMouseDevice();


int 
OpenMouse();


void 
CloseMouse();


int 
ReadMouse(
	int* x, 
	int* y, 
	int* event
);

#ifdef __cplusplus
}
#endif 

#endif 
