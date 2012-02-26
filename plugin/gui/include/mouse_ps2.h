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

#ifndef _MOUSE_PS2_H_
#define _MOUSE_PS2_H_

#ifdef __cplusplus                     
extern "C" {
#endif

#define SCALE           3       /* default scaling factor for acceleration */
#define THRESH          5       /* default threshhold for acceleration */
#define PS2_SCALE11     230     /* Set 1:1 scale factor */
#define PS2_SCALE21     231     /* Set 2:1 scale factor */
#define PS2_SETRES      232     /* Set resolution */
#define PS2_GETSCALE    233     /* Get scale factor */
#define PS2_SETSTREAM   234     /* Set stream mode */
#define PS2_SETSAMPLE   243     /* Set sample rate */
#define PS2_ENABLE      244     /* Enable PS/2 device */
#define PS2_DISABLE     245     /* Disable PS/2 device */
#define PS2_DEFAULT     246     /* Set default settings */
#define PS2_RESET       255     /* Reset PS/2 device */




#define BP_LBUTTON_DOWN		4
#define BP_RBUTTON_DOWN		1
#define BP_LRBUTTON_DOWN	5
#define BP_NOBUTTON_DOWN	0


#define PS2_DEV_FILE    "/dev/psaux"

typedef struct tagPS2MOUSEINFO{
	int x;
	int y;
	int nLastBP;
} PS2MOUSEINFO;


int 
PS2_Open(void);


void
PS2_Close(void);


int 
PS2_Read(
	int *x, 
	int *y, 
	int *event
);

#ifdef __cplusplus
}
#endif 

#endif 




