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
#include "../include/mouse_ts.h"
#include "../include/mouse_ial.h"
#include <kernel.h>
static int 
Dummy_Open(void)
{
	return 0;
}

static void 
Dummy_Close(void)
{ 
}

static int 
Dummy_Read(
	int *px, 
	int *py, 
	int *pb
)
{
	acoral_suspend_self();
	return true;
}


MOUSEDEVICE mousedev_Dummy = {
       Dummy_Open,
       Dummy_Close,
       Dummy_Read
};
