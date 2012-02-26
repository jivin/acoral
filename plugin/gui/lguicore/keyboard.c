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
#include "../include/invalidregion.h"
#include "../include/clipregion.h"
#include "../include/gmessage.h"
#include "../include/hdc.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/lguiapp.h"
#include "../include/keyboard.h"

thread_t thread_kb;


void 
InitKeyboard()
{
	InitLGUIKBDefine();
	InstallKBDevice();
	OpenKB();
	//create keyboard thread
	pthread_create(&thread_kb,NULL,
				KeyboardMainLoop,NULL);

}

void* 
KeyboardMainLoop(
	void* para
)
{
	BYTE btScanCode;
	BYTE btPressed;
	while(1){
		if(ReadKB(&btScanCode,&btPressed)){
			if(btPressed == 1)
				SendKBMessage((int)btScanCode);
			//printf("%d\n", btScanCode);
		}
	}
}

