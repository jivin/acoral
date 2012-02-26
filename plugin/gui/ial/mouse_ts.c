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
#include <driver.h> 
#include <ts.h> 
acoral_id ts_id;
struct ts_fix_t{
	acoral_fl xfactor;
	acoral_fl yfactor;
	acoral_fl xoffset;
	acoral_fl yoffset;
}ts_fix;
acoral_ts_info ts_info;
POINT v_pt[4]={{40,40},{100,100},{40,140},{20,160}};
POINT v_cpt[2];
POINT p_pt[4];
acoral_u32 event;
acoral_char fix_file[]="/tlgui/fix";
static int  TS_Read(int *,int *,int *);
static int  TS_ReadRaw(int *,int *,int *);
static int 
TS_Open(void)
{
	acoral_u32 i,x,y;
	acoral_32 fd;
	ts_id=acoral_dev_open("ts");
	fd=OPEN(fix_file,O_RDONLY);
	if(fd<0){
		while(1){
			for(i=0;i<4;){
				Gal_PutBoxRaw(v_pt[i].x,v_pt[i].y,4,4,0xf800);		
				TS_ReadRaw(&x,&y,&event);
				if(event==ACORAL_TS_DOWN){
					p_pt[i].x=x;	
					p_pt[i].y=y;	
					Gal_PutBoxRaw(v_pt[i].x-4,v_pt[i].y-4,20,20,0x00);		
					i++;
				}
			}
			ts_fix.xfactor=((acoral_fl)v_pt[1].x-v_pt[0].x)/((acoral_fl)p_pt[1].x-p_pt[0].x);
			ts_fix.xoffset=v_pt[0].x+2-ts_fix.xfactor*p_pt[0].x;
			ts_fix.yfactor=((acoral_fl)v_pt[1].y-v_pt[0].y)/((acoral_fl)p_pt[1].y-p_pt[0].y);
			ts_fix.yoffset=v_pt[0].y+2-ts_fix.yfactor*p_pt[0].y;
			v_cpt[0].x=ts_fix.xfactor*p_pt[2].x+ts_fix.xoffset-2;
			v_cpt[0].y=ts_fix.yfactor*p_pt[2].y+ts_fix.yoffset-2;
			v_cpt[1].x=ts_fix.xfactor*p_pt[3].x+ts_fix.xoffset-2;
			v_cpt[1].y=ts_fix.yfactor*p_pt[3].y+ts_fix.yoffset-2;
			x=v_cpt[0].x>v_pt[2].x?v_cpt[0].x-v_pt[2].x:v_pt[2].x-v_cpt[0].x;
			y=v_cpt[0].y>v_pt[2].y?v_cpt[0].y-v_pt[2].y:v_pt[2].y-v_cpt[0].y;
			acoral_print("%d,%d\n",x,y);
			if(x<3&&y<3)
				break;
		}
		fd=OPEN(fix_file,O_RDWR);	
		if(fd<0)
			return;	
		WRITE(fd,&ts_fix,sizeof(struct ts_fix_t));
		FLUSH();
		CLOSE(fd);
	}
	else{
		READ(fd,&ts_fix,sizeof(struct ts_fix_t));
		CLOSE(fd);
	}
}


static void 
TS_Close(void)
{ 
	acoral_dev_close(ts_id);
}

static int 
TS_Read(
	int *px, 
	int *py, 
	int *pb
)
{
	acoral_dev_read(ts_id,&ts_info,0,0,0);		
	*px=ts_fix.xfactor*ts_info.x+ts_fix.xoffset;
	*py=ts_fix.yfactor*ts_info.y+ts_fix.yoffset;
	if(ts_info.event==ACORAL_TS_DOWN)
		*pb=MOUSE_EVENT_LBUTTON_DOWN;
	else
		*pb=MOUSE_EVENT_LBUTTON_UP;
	return 1;
}

static int 
TS_ReadRaw(
	int *px, 
	int *py, 
	int *pb
)
{
	acoral_dev_read(ts_id,&ts_info,0,0,0);		
	*px=ts_info.x;
	*py=ts_info.y;
	if(ts_info.event==ACORAL_TS_DOWN)
		*pb=MOUSE_EVENT_LBUTTON_DOWN;
	else
		*pb=MOUSE_EVENT_LBUTTON_UP;
	return 1;
}


MOUSEDEVICE mousedev_TS = {
        TS_Open,
        TS_Close,
        TS_Read
};
