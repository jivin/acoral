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
#include "../include/kb_ial.h"
#include <kb.h>
#include <driver.h>
static int 
BDKB_Open(void);

static void
BDKB_Close(void);

static int 
BDKB_Read(
	BYTE* btScanCode,
	BYTE* btPressed
);

static acoral_id kb_id;
static acoral_kb_info buf;

KBDEVICE kbdev_2440 = {
        BDKB_Open,
        BDKB_Close,
        BDKB_Read
};

static int 
BDKB_Open(void)
{
	
	kb_id = acoral_dev_open("kb");
	if ( kb_id<0 ){
		printf("Open kdc device error!!!\n\n\n");
		return -1;
	}
	return 0;
}


static void 
BDKB_Close(void)
{
	acoral_dev_close(kb_id);
}


static int 
BDKB_Read(
	BYTE* btScanCode,
	BYTE* btPressed
)
{
	int ret;
	int size;
	if(kb_id<0){
		acoral_suspend_self();
	}
	size = sizeof(acoral_kb_info);
	ret = acoral_dev_read(kb_id,&buf,size,0,0);
	if(ret == size){
		*btScanCode = buf.key_code;
		*btPressed = buf.event;
		return 1;
	}
	else{
		printf("read keyboard error\n");
		return 0;
	}
	return 1;
}
