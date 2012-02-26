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

#ifndef _SLEDIT_H
#define _SLEDIT_H

#ifdef __cplusplus
extern "C" {
#endif

#define LEN_EDITBOX_BUFFER       1024

typedef struct tagEDITBOXDATA
{
    DWORD   dwStatus;         // dwStatus of box
    int     iBufferLen;      // length of buffer
    int     iDataLen;        // data end position

    int     iEditPos;        // current edit position
    int     iCaretPos;       // caret offset in box
    int     iStartPos;       // start display position

    char     passwdChar;     // password character

    int     iLeftMargin;     // left margin
    int     iTopMargin;      // top margin
    int     iRightMargin;    // right margin
    int     iBottomMargin;   // bottom margin

    char    buffer[LEN_EDITBOX_BUFFER];
                            // buffer
}SLEditData;
typedef SLEditData* PSLEditData;


BOOL 
RegisterSLEditControl();

LRESULT 
SLEditCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);


#ifdef __cplusplus
}
#endif 


#endif 
