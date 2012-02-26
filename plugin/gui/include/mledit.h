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

#ifndef _MLEDIT_H
#define _MLEDIT_H

#ifdef __cplusplus
extern "C" {
#endif

#define LEN_MLEDIT_LINEBUFFER	1024
#define MAX_WRAP_NUM			100

#define MLEDIT_LEFTMARGIN	1
#define MLEDIT_RIGHTMARGIN	1
#define MLEDIT_TOPMARGIN	1
#define MLEDIT_BOTTOMMARGIN	1

#define MLEDIT_CHARSPACE	0
#define MLEDIT_LINESPACE	2


#define EST_FOCUSED     0x00000001L
#define EST_REPLACE     0x00000002L
#define EST_READONLY    0x00000004L

//Autowrap is preset attrib and can't be changed 


typedef struct tagLineData{
	int iLineNumber;	//line number, include wrap lines
	char pString[LEN_MLEDIT_LINEBUFFER + 1];
	int iWrapLineNumber;					//total wrap lines in current line
	int WrapStartPos[MAX_WRAP_NUM];
	struct tagLineData* pPrev;
	struct tagLineData* pNext;
} LineData;
typedef LineData* PLineData;

typedef struct tagMLEditData{
	DWORD dwStatus;

	int iBufferLimit;		//Buffer limit of a line 

	int iLeftMargin;
	int iTopMargin;
	int iRightMargin;
	int iBottomMargin;

	int iCharSpace;			//space between chars
	int iLineSpace;			//space between lines

	int iLineHeight;			//line height
	int iCharWidth;			//width of an English character width


	char* pEditPos;			//edit position is a pointer
	
	int iDispPos;				//in autowrap status Horizon display position is unnecessary
	PLineData pDispPointer;	//first display line pointer

	int iDispLines;			//lines can be displayed in the current box 
	int iDispChars;			//characters can be displayed in one line, up limit to wrap

	int iCaretXPos;
	int iCaretYPos;

	int iTotalLen;
	int iTotalLines;			//total actual lines, include autowrapped lines
	
	PLineData pHead;
	PLineData pTail;

} MLEditData;
typedef MLEditData* PMLEditData;


BOOL RegisterMLEditControl ();

LRESULT MLEditCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);



#ifdef __cplusplus
}
#endif


#endif
