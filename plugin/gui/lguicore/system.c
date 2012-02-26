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
#include "../include/regclass.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/system.h"

//return system parameter
int GUIAPI 
GetSystemMetrics(
	int nIndex
)
{
	switch (nIndex){
	case SM_CXVSCROLL://height of scrollbar
		return VSCROLL_WIDTH;
	case SM_CYVSCROLL: //Height of arrow of scrollbar
		return VSCROLL_ARROW_HEIGHT;
	case SM_CYHSCROLL://Width of scrollbar
		return HSCROLL_WIDTH;
	case SM_CXHSCROLL: //Width of arrow of scrollbar
		return HSCROLL_ARROW_HEIGHT;
	default:
		return 0;
	}
	return 0;
}


//system default font width
int GUIAPI 
GetSysCharWidth()
{
	HGDIOBJ hFont;
	int nWidth;
	hFont = GetStockObject(SYS_DEFAULT_FONT);
	nWidth = ((FONT*)hFont)->FontLibHeader.iAscWidth;
	DeleteObject(hFont);
	return nWidth;
}
//system default font width
int GUIAPI 
GetSysCCharWidth()
{
	HGDIOBJ hFont;
	int nWidth;
	hFont = GetStockObject(SYS_DEFAULT_FONT);
	nWidth = ((FONT*)hFont)->FontLibHeader.iChnWidth;
	DeleteObject(hFont);
	return nWidth;
}

//get system default char height
int GUIAPI GetSysCharHeight()
{
	HGDIOBJ hFont;
	int nHeight;
	hFont = GetStockObject(SYS_DEFAULT_FONT);
	nHeight = ((FONT*)hFont)->FontLibHeader.iChnHeight;
	DeleteObject(hFont);
	return nHeight;
}







