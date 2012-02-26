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
#include "../include/ipc.h"
#include "../include/gmessage.h"
#include "../include/regclass.h"
#include "../include/shmem.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/scrollbar.h"
#include "../include/initgui.h"
#include "../include/gtimer.h"


extern PlGUIAppStat  _lGUI_pAppStat;
extern PlGUIAppStat  _lGUI_ServerStat;
//extern WndCaptureMouse _lGUI_wndCaptureMouse;

BOOL CheckGuiSysDir();
BOOL GUIAPI
InitGUIServer()
{
	if(!CheckGuiSysDir()){
		printerror("Err,GUI System Root Dir TLGUI Not Exist!\n");
		return false;
	}
	if(_lGUI_ServerStat!=NULL){
		printerror("GUI Server have been Running!");
		return false;
	}
	RegisterServerControls();
	RegisterClientControls();
	InitMsgQueueHeap();
	InitClipRegionHeap();
	InitInvalidRegionHeap();

	if(!InitGal()){
		printerror("init framebuffer error!");
		return false;
	}
	_lGUI_ServerStat=(PlGUIAppStat)malloc(sizeof(lGUIAppStat));
	if(!_lGUI_ServerStat){
		printerror("alloc memory error!");
		return false;
	}
	memset(_lGUI_ServerStat,0,sizeof(lGUIAppStat));
	PortSetCurStat(_lGUI_ServerStat);

	InitShareMemServer();

	CreateStockObject();

	if(!InitIpcServer())
		return false;
	RegisterTimerRoutine();

	InitMouseServer();
	InitKeyboard();
	return true;
}


BOOL GUIAPI 
InitGUIClient()
{
	PlGUIAppStat _lGUI_Stat;
//	InitMsgQueueHeap();
//	InitClipRegionHeap();
//	InitInvalidRegionHeap();


/*	if(!InitFrameBuffer()){
		printerror("init framebuffer error!");
		return false;
	}*/

	_lGUI_Stat=(PlGUIAppStat)malloc(sizeof(lGUIAppStat));
	if(!_lGUI_Stat){
		printerror("Init Gui Client alloc memory error!");
		return false;
	}
	memset(_lGUI_Stat,0,sizeof(lGUIAppStat));
	_lGUI_Stat->pClipRgn=(PClipRegion)malloc(sizeof(ClipRegion));
	sem_init(&_lGUI_Stat->sem_cs,NULL,0);
	if(!(_lGUI_Stat->pClipRgn))
		return false;
	memset(_lGUI_Stat->pClipRgn,0,sizeof(ClipRegion));

	InitClipRegion(_lGUI_Stat->pClipRgn);
	PortSetCurStat(_lGUI_Stat);

	if(!InitIpcClient())
		return false;

	InitShareMemClient();
	//register signal callback function, used when server want to terminate client 
//	RegSignalCallBack();
	//initial timer function
//	RegisterTimerRoutine();

//	InitMouseClient();

	_lGUI_wndCaptureMouse->pWin=NULL;
	return true;
}


void GUIAPI 
TerminateGUIServer()
{

//	TerminateMouseServer();
//	TerminateKeyboard();
	TerminateIpc();
	DestroyRegWndTable();
	UnInitGal();

	DestroyClipRegionHeap();
	DestroyInvalidRegionHeap();
	DestroyMsgQueueHeap();

	UnInitShareMem();
//	UnInitTimer();
	PortServerExit();
}

BOOL CheckGuiSysDir(){
	FILE_P fd;
   	if((fd=OPEN("tlgui/sys.cfg",O_RDONLY))<0)
		return false;
	CLOSE(fd);
	return true;
}

void GUIAPI 
TerminateGUIClient()
{
	PlGUIAppStat curStat=_lGUI_CurStat;
	EmptyClipRegion(curStat->pClipRgn);
	free(curStat->pClipRgn);

	free(curStat);
//下面在多线程情况下都是公用的了,由server来释放
//	DestroyRegWndTable();

//	DestroyClipRegionHeap();
//	DestroyInvalidRegionHeap();
//	DestroyMsgQueueHeap();
//	UnInitShareMem();
	TerminateIpc();
	PortClientExit();
	//cancel the socket thread will close the socket at first
	//if client close the socket,server will get this socket close event
	//and will process destroy application.
//	UnInitTimer();
}
