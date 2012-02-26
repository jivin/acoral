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

#include <linux/fb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <termios.h>
#include <stdio.h>
#include "../include/common.h"
#include "../include/mouse_ps2.h"
#include "../include/mouse_ial.h"

static int mouse_fd;
MOUSEDEVICE mousedev_PS2 = {
        PS2_Open,
        PS2_Close,
        PS2_Read
};

PS2MOUSEINFO ps2MouseInfo={0,0,BP_NOBUTTON_DOWN};

int 
PS2_Open(void)
{
	/* Make sure the mouse is enabled and in a sane state */
	unsigned char initdata_ps2[] =
	{ PS2_DEFAULT, PS2_SCALE11, PS2_ENABLE };
	//mouse_fd = open (PS2_DEV_FILE, RDONLY | O_NOCTTY | O_NONBLOCK);
	mouse_fd = open (PS2_DEV_FILE, RDONLY | O_NOCTTY);
	if (mouse_fd < 0)
		return -1;
	write(mouse_fd, initdata_ps2, sizeof(initdata_ps2));
	return mouse_fd;
}


void 
PS2_Close(void)
{
	if (mouse_fd > 0)
		close(mouse_fd);
	mouse_fd = -1;
}


int 
PS2_Read(
	int *x, 
	int *y, 
	int *event
)
{
	int newx,newy;
	static unsigned char buf[3];
	static int nbytes=0;
	int n;
	int buttons[4] = { 0, 4, 1, 5};

	int dx, dy, dz, bp;
	
	while((n = read(mouse_fd, &buf[nbytes], 3 - nbytes))) {
		if(n < 0) {
			if ((errno == EINTR) || (errno == EAGAIN))
				return 0;
			else return -1;
		}
		nbytes += n;
		if(nbytes == 3) {
		if ((buf[0] & 0xc0) != 0) {
			buf[0] = buf[1];
			buf[1] = buf[2];
			nbytes = 2;
			return -1;
		}
		bp = buttons[(buf[0] & 0x07)];
		dx = (buf[0] & 0x10) ? buf[1] - 256 : buf[1];
		dy = (buf[0] & 0x20) ? -(buf[2] - 256) : -buf[2];
		dz = 0;
		nbytes = 0;

		newx = ps2MouseInfo.x + dx;
		newy = ps2MouseInfo.y + dy;
		
		if((newx>=0)&&(newx<SCREEN_WIDTH)&&dx)
			ps2MouseInfo.x = newx;
		if((newy>=0)&&(newy<SCREEN_HEIGHT)&&dy)
			ps2MouseInfo.y = newy;
	
				


		*x = ps2MouseInfo.x;
		*y = ps2MouseInfo.y;
		if((ps2MouseInfo.nLastBP==BP_NOBUTTON_DOWN)&&(bp==BP_LBUTTON_DOWN)){
			//mousedown			
			*event = MOUSE_EVENT_LBUTTON_DOWN;
		}
		else if((ps2MouseInfo.nLastBP==BP_LBUTTON_DOWN)&&(bp==BP_NOBUTTON_DOWN)){
			//mouseup
			*event = MOUSE_EVENT_LBUTTON_UP;
		}
		else if((ps2MouseInfo.nLastBP==BP_LBUTTON_DOWN)&&(bp==BP_LBUTTON_DOWN)){
			//mousemove
			*event = MOUSE_EVENT_MOVE;
		}
		else
			*event = MOUSE_EVENT_RESERVED;
		ps2MouseInfo.x = *x;
		ps2MouseInfo.y = *y;
		ps2MouseInfo.nLastBP = bp;
		
		return 1;
		}
	}
	return 0;
}
