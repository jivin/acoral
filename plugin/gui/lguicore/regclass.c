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
#include "../include/hdc.h"
#include "../include/rect.h"
#include "../include/gmessage.h"
#include "../include/blockheap.h"
#include "../include/invalidregion.h"
#include "../include/clipregion.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/regclass.h"
#include "../include/button.h"
#include "../include/deskicon.h"
#include "../include/scrollbar.h"
#include "../include/tabcontrol.h"
#include "../include/listbox.h"
#include "../include/picturebox.h"
#include "../include/static.h"
#include "../include/idlecontrol.h"
#include "../include/imewin.h"
#include "../include/keyboard_pro.h"
#include "../include/skbwin.h"
#include "../include/sledit.h"
#include "../include/mledit.h"
#include "../include/tabcontrol.h"
#include "../include/taskbar.h"
#include "../include/startmenu.h"




static PWNDCLASSEX _lGUI_pHeadRegClass;
static PWNDCLASSEX _lGUI_pTailRegClass;

BOOL GUIAPI
RegisterClass(
	WNDCLASSEX* lpwcx
)
{
	PWNDCLASSEX pNewNode;
	PWNDCLASSEX pWndClass;
	int i=0;
	char lpszClassName [MAXLEN_CLASSNAME + 1];
	if(!lpwcx) 
		return false;
	if(!(lpwcx->lpszClassName))
		return false;
	if(strlen(lpwcx->lpszClassName)>MAXLEN_CLASSNAME)  
		return false;
	strcpy(lpszClassName,lpwcx->lpszClassName);
	while(lpszClassName[i]){
		lpszClassName[i]=toupper(lpszClassName[i]);
		i++;
	}
	pNewNode = (PWNDCLASSEX)malloc(sizeof(WNDCLASSEX));
	if(!pNewNode)
		return false;
	memcpy(pNewNode,lpwcx,sizeof(WNDCLASSEX));
	pNewNode->lpszClassName = malloc(MAXLEN_CLASSNAME);
	if(!pNewNode->lpszClassName)
		return false;
	strcpy(pNewNode->lpszClassName,lpszClassName);
	pNewNode->lpszMenuName = malloc(MAXLEN_MENUNAME);
	if(!pNewNode->lpszMenuName)
		return false;
	if(lpwcx->lpszMenuName)
		strcpy(pNewNode->lpszMenuName,lpwcx->lpszMenuName);
	if(_lGUI_pHeadRegClass){
		//search the link table to confirm the class doesn't exist in the table
		pWndClass = _lGUI_pHeadRegClass;
		while(pWndClass){
			if(strcmp(lpszClassName,pWndClass->lpszClassName)==0){
				acoral_printdbg("Regclass Same Class\n");
				free(pNewNode->lpszClassName);
				free(pNewNode->lpszMenuName);
				free(lpwcx->hbrBackground);
				free(pNewNode);
				return false;
			}
			pWndClass = pWndClass->pNext;
		}
		_lGUI_pTailRegClass->pNext = pNewNode;
		_lGUI_pTailRegClass = pNewNode;
		_lGUI_pTailRegClass->pNext = NULL;
	}
	else{
		_lGUI_pHeadRegClass = _lGUI_pTailRegClass = pNewNode;
		_lGUI_pTailRegClass->pNext = NULL;
	}
	return true;
}



//get register class by class name
PWNDCLASSEX GUIAPI
GetRegClass(
	char* lpszClassName
)
{
	int i=0;
	PWNDCLASSEX pWndClass;
	char szName[MAXLEN_CLASSNAME + 1];
	if(!_lGUI_pHeadRegClass)
		return NULL;
	if(strlen(lpszClassName)>MAXLEN_CLASSNAME)
		return NULL;
	strcpy(szName,lpszClassName);
	while(szName[i]){
		szName[i]=toupper(szName[i]);
		i++;
	}
	pWndClass = _lGUI_pHeadRegClass;
	while(pWndClass){
		if(strcmp(pWndClass->lpszClassName,szName)==0)
			return pWndClass;
		pWndClass = pWndClass->pNext;
	}
	return NULL;
}




void 
DestroyRegWndTable()
{
	PWNDCLASSEX pWndClass, pNextWndClass;
	pWndClass = pNextWndClass = _lGUI_pHeadRegClass;
	while(pWndClass){
		pNextWndClass = pWndClass->pNext;
		free(pWndClass);
		pWndClass = pNextWndClass;
	}

}


WNDPROC
GetWndProc(
	HWND hWnd
)
{
	PWNDCLASSEX pWndClass;
	PWindowsTree pWin;
	char lpszClassName [MAXLEN_CLASSNAME + 1];
	int i=0;
	pWin=(PWindowsTree)hWnd;

	if(!pWin->lpszClassName)
		return NULL;
	if(strlen(pWin->lpszClassName)>MAXLEN_CLASSNAME)
		return NULL;
	strcpy(lpszClassName,pWin->lpszClassName);
	
	while(lpszClassName[i]){
		lpszClassName[i]=toupper(lpszClassName[i]);
		i++;
	}
	pWndClass = _lGUI_pHeadRegClass;
	while(pWndClass){
		if(strcmp(pWndClass->lpszClassName,lpszClassName)==0) break;
		pWndClass=pWndClass->pNext;
	}
	if(!pWndClass)
		return NULL;
	return pWndClass->lpfnWndProc;

}


void 
RegisterServerControls()
{
	//button
	RegisterButtonControl();
	//scrollbar control
	RegisterScrollBarControl();	
	//desktop icon
	RegisterDeskIconControl();
	//static textbox control
	RegisterStaticControl();
	//picturebox control
	RegisterPictureBoxControl();
	//listbox control
	RegisterListBoxControl();
	//idle control
	RegisterIdleControl();
	//softkey window
	RegisterSkbWindow();
	//ime window
	RegisterImeWindow();
	//tabcontrol
	RegisterTabControl();
	//taskbar
	RegisterTaskBar();
	//start menu
	RegisterStartMenuControl();

}


void 
RegisterClientControls()
{
	//button
	RegisterButtonControl();
	//scrollbar control
	RegisterScrollBarControl();
	//listbox control
	RegisterListBoxControl();
	//picturebox control
	RegisterPictureBoxControl();
	//static textbox control
	RegisterStaticControl();
	//single text box
	RegisterSLEditControl();
	//multi text box
	RegisterMLEditControl ();
	//tabcontrol
	RegisterTabControl();

}





