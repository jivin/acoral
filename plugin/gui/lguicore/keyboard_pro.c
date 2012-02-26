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
#include "../include/bmp.h"
#include "../include/regclass.h"
#include "../include/hdc.h"
#include "../include/keyboard_pro.h"
#include "../include/skbwin.h"
#include "../include/gdi.h"
#include "../include/rect.h"
#include "../include/gmessage.h"
#include "../include/blockheap.h"
#include "../include/invalidregion.h"
#include "../include/clipregion.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/text.h"
#include "../include/lguiapp.h"
#include "../include/imewin.h"



static LGUIKBDefine lguiKBDefine;

extern PWindowsTree _lGUI_pImeWindow;
	
static int
GetIndexByScanCode(
	int iScanCode
);
	
void
InitLGUIKBDefine ()
{

	lguiKBDefine.bShift = false;
	lguiKBDefine.bCapsLock = false;
	lguiKBDefine.bCtrl = false;
	lguiKBDefine.bAlt = false;
	
	lguiKBDefine.kbRetValue[0].iScanCode = SCANCODE_ESCAPE;
	lguiKBDefine.kbRetValue[1].iScanCode = SCANCODE_WAVE;
	lguiKBDefine.kbRetValue[2].iScanCode = SCANCODE_1;
	lguiKBDefine.kbRetValue[3].iScanCode = SCANCODE_2;
	lguiKBDefine.kbRetValue[4].iScanCode = SCANCODE_3;
	lguiKBDefine.kbRetValue[5].iScanCode = SCANCODE_4;
	lguiKBDefine.kbRetValue[6].iScanCode = SCANCODE_5;
	lguiKBDefine.kbRetValue[7].iScanCode = SCANCODE_6;
	lguiKBDefine.kbRetValue[8].iScanCode = SCANCODE_7;
	lguiKBDefine.kbRetValue[9].iScanCode = SCANCODE_8;
	lguiKBDefine.kbRetValue[10].iScanCode = SCANCODE_9;
	lguiKBDefine.kbRetValue[11].iScanCode = SCANCODE_0;
	lguiKBDefine.kbRetValue[12].iScanCode = SCANCODE_MINUS;
	lguiKBDefine.kbRetValue[13].iScanCode = SCANCODE_EQUAL;
	lguiKBDefine.kbRetValue[14].iScanCode = SCANCODE_BACKSPACE;


	lguiKBDefine.kbRetValue[15].iScanCode = SCANCODE_TAB;
	lguiKBDefine.kbRetValue[16].iScanCode = SCANCODE_Q;
	lguiKBDefine.kbRetValue[17].iScanCode = SCANCODE_W;
	lguiKBDefine.kbRetValue[18].iScanCode = SCANCODE_E;
	lguiKBDefine.kbRetValue[19].iScanCode = SCANCODE_R;
	lguiKBDefine.kbRetValue[20].iScanCode = SCANCODE_T;
	lguiKBDefine.kbRetValue[21].iScanCode = SCANCODE_Y;
	lguiKBDefine.kbRetValue[22].iScanCode = SCANCODE_U;
	lguiKBDefine.kbRetValue[23].iScanCode = SCANCODE_I;
	lguiKBDefine.kbRetValue[24].iScanCode = SCANCODE_O;
	lguiKBDefine.kbRetValue[25].iScanCode = SCANCODE_P;
	lguiKBDefine.kbRetValue[26].iScanCode = SCANCODE_BRACKET_LEFT;
	lguiKBDefine.kbRetValue[27].iScanCode = SCANCODE_BRACKET_RIGHT;
	lguiKBDefine.kbRetValue[28].iScanCode = SCANCODE_BACKSLASH;


	lguiKBDefine.kbRetValue[29].iScanCode = SCANCODE_CAPSLOCK;
	lguiKBDefine.kbRetValue[30].iScanCode = SCANCODE_A;
	lguiKBDefine.kbRetValue[31].iScanCode = SCANCODE_S;
	lguiKBDefine.kbRetValue[32].iScanCode = SCANCODE_D;
	lguiKBDefine.kbRetValue[33].iScanCode = SCANCODE_F;
	lguiKBDefine.kbRetValue[34].iScanCode = SCANCODE_G;
	lguiKBDefine.kbRetValue[35].iScanCode = SCANCODE_H;
	lguiKBDefine.kbRetValue[36].iScanCode = SCANCODE_J;
	lguiKBDefine.kbRetValue[37].iScanCode = SCANCODE_K;
	lguiKBDefine.kbRetValue[38].iScanCode = SCANCODE_L;
	lguiKBDefine.kbRetValue[39].iScanCode = SCANCODE_SEMICOLON;
	lguiKBDefine.kbRetValue[40].iScanCode = SCANCODE_APOSTROPHE;
	lguiKBDefine.kbRetValue[41].iScanCode = SCANCODE_ENTER;

	lguiKBDefine.kbRetValue[42].iScanCode = SCANCODE_LEFTSHIFT;
	lguiKBDefine.kbRetValue[43].iScanCode = SCANCODE_Z;
	lguiKBDefine.kbRetValue[44].iScanCode = SCANCODE_X;
	lguiKBDefine.kbRetValue[45].iScanCode = SCANCODE_C;
	lguiKBDefine.kbRetValue[46].iScanCode = SCANCODE_V;
	lguiKBDefine.kbRetValue[47].iScanCode = SCANCODE_B;
	lguiKBDefine.kbRetValue[48].iScanCode = SCANCODE_N;
	lguiKBDefine.kbRetValue[49].iScanCode = SCANCODE_M;
	lguiKBDefine.kbRetValue[50].iScanCode = SCANCODE_COMMA;
	lguiKBDefine.kbRetValue[51].iScanCode = SCANCODE_PERIOD;
	lguiKBDefine.kbRetValue[52].iScanCode = SCANCODE_SLASH;
	lguiKBDefine.kbRetValue[53].iScanCode = SCANCODE_RIGHTSHIFT;

	lguiKBDefine.kbRetValue[54].iScanCode = SCANCODE_LEFTCONTROL;
	lguiKBDefine.kbRetValue[55].iScanCode = SCANCODE_LEFTALT;
	lguiKBDefine.kbRetValue[56].iScanCode = SCANCODE_SPACE;
	lguiKBDefine.kbRetValue[57].iScanCode = SCANCODE_RIGHTALT;
	lguiKBDefine.kbRetValue[58].iScanCode = SCANCODE_RIGHTCONTROL;

	lguiKBDefine.kbRetValue[59].iScanCode = SCANCODE_EXTPAD;
	
	lguiKBDefine.kbRetValue[60].iScanCode = SCANCODE_HOME;
	lguiKBDefine.kbRetValue[61].iScanCode = SCANCODE_END;
	lguiKBDefine.kbRetValue[62].iScanCode = SCANCODE_INS;
	lguiKBDefine.kbRetValue[63].iScanCode = SCANCODE_DEL;
	lguiKBDefine.kbRetValue[64].iScanCode = SCANCODE_PAGEUP;
	lguiKBDefine.kbRetValue[65].iScanCode = SCANCODE_PAGEDOWN;
	lguiKBDefine.kbRetValue[66].iScanCode = SCANCODE_LEFT;
	lguiKBDefine.kbRetValue[67].iScanCode = SCANCODE_RIGHT;
	lguiKBDefine.kbRetValue[68].iScanCode = SCANCODE_UP;
	lguiKBDefine.kbRetValue[69].iScanCode = SCANCODE_DOWN;




	lguiKBDefine.kbRetValue[0].bRetChar	= false;

	lguiKBDefine.kbRetValue[1].bRetChar	= true;
	lguiKBDefine.kbRetValue[1].NormalValue	= '`';
	lguiKBDefine.kbRetValue[1].ShiftValue	= '~';

	lguiKBDefine.kbRetValue[2].bRetChar	= true;
	lguiKBDefine.kbRetValue[2].NormalValue	= '1';
	lguiKBDefine.kbRetValue[2].ShiftValue	= '!';

	lguiKBDefine.kbRetValue[3].bRetChar	= true;
	lguiKBDefine.kbRetValue[3].NormalValue	= '2';
	lguiKBDefine.kbRetValue[3].ShiftValue	= '@';

	lguiKBDefine.kbRetValue[4].bRetChar	= true;
	lguiKBDefine.kbRetValue[4].NormalValue	= '3';
	lguiKBDefine.kbRetValue[4].ShiftValue	= '#';

	lguiKBDefine.kbRetValue[5].bRetChar	= true;
	lguiKBDefine.kbRetValue[5].NormalValue	= '4';
	lguiKBDefine.kbRetValue[5].ShiftValue	= '$';

	lguiKBDefine.kbRetValue[6].bRetChar	= true;
	lguiKBDefine.kbRetValue[6].NormalValue	= '5';
	lguiKBDefine.kbRetValue[6].ShiftValue	= '%';

	lguiKBDefine.kbRetValue[7].bRetChar	= true;
	lguiKBDefine.kbRetValue[7].NormalValue	= '6';
	lguiKBDefine.kbRetValue[7].ShiftValue	= '^';

	lguiKBDefine.kbRetValue[8].bRetChar	= true;
	lguiKBDefine.kbRetValue[8].NormalValue	= '7';
	lguiKBDefine.kbRetValue[8].ShiftValue	= '&';

	lguiKBDefine.kbRetValue[9].bRetChar	= true;
	lguiKBDefine.kbRetValue[9].NormalValue	= '8';
	lguiKBDefine.kbRetValue[9].ShiftValue	= '*';

	lguiKBDefine.kbRetValue[10].bRetChar	= true;
	lguiKBDefine.kbRetValue[10].NormalValue	= '9';
	lguiKBDefine.kbRetValue[10].ShiftValue	= '(';

	lguiKBDefine.kbRetValue[11].bRetChar	= true;
	lguiKBDefine.kbRetValue[11].NormalValue	= '0';
	lguiKBDefine.kbRetValue[11].ShiftValue	= ')';

	lguiKBDefine.kbRetValue[12].bRetChar	= true;
	lguiKBDefine.kbRetValue[12].NormalValue	= '-';
	lguiKBDefine.kbRetValue[12].ShiftValue	= '_';

	lguiKBDefine.kbRetValue[13].bRetChar	= true;
	lguiKBDefine.kbRetValue[13].NormalValue	= '=';
	lguiKBDefine.kbRetValue[13].ShiftValue	= '+';

	lguiKBDefine.kbRetValue[14].bRetChar	= true;//backspace  send char 
 	lguiKBDefine.kbRetValue[14].NormalValue	= 8;
	lguiKBDefine.kbRetValue[14].ShiftValue	= 8;
	
	lguiKBDefine.kbRetValue[15].bRetChar	= false;//tab

	lguiKBDefine.kbRetValue[16].bRetChar	= true;
	lguiKBDefine.kbRetValue[16].NormalValue	= 'q';
	lguiKBDefine.kbRetValue[16].ShiftValue	= 'Q';

	lguiKBDefine.kbRetValue[17].bRetChar	= true;
	lguiKBDefine.kbRetValue[17].NormalValue	= 'w';
	lguiKBDefine.kbRetValue[17].ShiftValue	= 'W';

	lguiKBDefine.kbRetValue[18].bRetChar	= true;
	lguiKBDefine.kbRetValue[18].NormalValue	= 'e';
	lguiKBDefine.kbRetValue[18].ShiftValue	= 'E';

	lguiKBDefine.kbRetValue[19].bRetChar	= true;
	lguiKBDefine.kbRetValue[19].NormalValue	= 'r';
	lguiKBDefine.kbRetValue[19].ShiftValue	= 'R';

	lguiKBDefine.kbRetValue[20].bRetChar	= true;
	lguiKBDefine.kbRetValue[20].NormalValue	= 't';
	lguiKBDefine.kbRetValue[20].ShiftValue	= 'T';

	lguiKBDefine.kbRetValue[21].bRetChar	= true;
	lguiKBDefine.kbRetValue[21].NormalValue	= 'y';
	lguiKBDefine.kbRetValue[21].ShiftValue	= 'Y';

	lguiKBDefine.kbRetValue[22].bRetChar	= true;
	lguiKBDefine.kbRetValue[22].NormalValue	= 'u';
	lguiKBDefine.kbRetValue[22].ShiftValue	= 'U';

	lguiKBDefine.kbRetValue[23].bRetChar	= true;
	lguiKBDefine.kbRetValue[23].NormalValue	= 'i';
	lguiKBDefine.kbRetValue[23].ShiftValue	= 'I';

	lguiKBDefine.kbRetValue[24].bRetChar	= true;
	lguiKBDefine.kbRetValue[24].NormalValue	= 'o';
	lguiKBDefine.kbRetValue[24].ShiftValue	= 'O';

	lguiKBDefine.kbRetValue[25].bRetChar	= true;
	lguiKBDefine.kbRetValue[25].NormalValue	= 'p';
	lguiKBDefine.kbRetValue[25].ShiftValue	= 'P';

	lguiKBDefine.kbRetValue[26].bRetChar	= true;
	lguiKBDefine.kbRetValue[26].NormalValue	= '[';
	lguiKBDefine.kbRetValue[26].ShiftValue	= '{';

	lguiKBDefine.kbRetValue[27].bRetChar	= true;
	lguiKBDefine.kbRetValue[27].NormalValue	= ']';
	lguiKBDefine.kbRetValue[27].ShiftValue	= '}';

	lguiKBDefine.kbRetValue[28].bRetChar	= true;
	lguiKBDefine.kbRetValue[28].NormalValue	= '\\';
	lguiKBDefine.kbRetValue[28].ShiftValue	= '|';

	lguiKBDefine.kbRetValue[29].bRetChar	= false;

	lguiKBDefine.kbRetValue[30].bRetChar	= true;
	lguiKBDefine.kbRetValue[30].NormalValue	= 'a';
	lguiKBDefine.kbRetValue[30].ShiftValue	= 'A';

	lguiKBDefine.kbRetValue[31].bRetChar	= true;
	lguiKBDefine.kbRetValue[31].NormalValue	= 's';
	lguiKBDefine.kbRetValue[31].ShiftValue	= 'S';

	lguiKBDefine.kbRetValue[32].bRetChar	= true;
	lguiKBDefine.kbRetValue[32].NormalValue	= 'd';
	lguiKBDefine.kbRetValue[32].ShiftValue	= 'D';

	lguiKBDefine.kbRetValue[33].bRetChar	= true;
	lguiKBDefine.kbRetValue[33].NormalValue	= 'f';
	lguiKBDefine.kbRetValue[33].ShiftValue	= 'F';

	lguiKBDefine.kbRetValue[34].bRetChar	= true;
	lguiKBDefine.kbRetValue[34].NormalValue	= 'g';
	lguiKBDefine.kbRetValue[34].ShiftValue	= 'G';

	lguiKBDefine.kbRetValue[35].bRetChar	= true;
	lguiKBDefine.kbRetValue[35].NormalValue	= 'h';
	lguiKBDefine.kbRetValue[35].ShiftValue	= 'H';

	lguiKBDefine.kbRetValue[36].bRetChar	= true;
	lguiKBDefine.kbRetValue[36].NormalValue	= 'j';
	lguiKBDefine.kbRetValue[36].ShiftValue	= 'J';

	lguiKBDefine.kbRetValue[37].bRetChar	= true;
	lguiKBDefine.kbRetValue[37].NormalValue	= 'k';
	lguiKBDefine.kbRetValue[37].ShiftValue	= 'K';

	lguiKBDefine.kbRetValue[38].bRetChar	= true;
	lguiKBDefine.kbRetValue[38].NormalValue	= 'l';
	lguiKBDefine.kbRetValue[38].ShiftValue	= 'L';

	lguiKBDefine.kbRetValue[39].bRetChar	= true;
	lguiKBDefine.kbRetValue[39].NormalValue	= ';';
	lguiKBDefine.kbRetValue[39].ShiftValue	= ':';

	lguiKBDefine.kbRetValue[40].bRetChar	= true;
	lguiKBDefine.kbRetValue[40].NormalValue	= '\'';
	lguiKBDefine.kbRetValue[40].ShiftValue	= '"';

	lguiKBDefine.kbRetValue[41].bRetChar	= false;

	lguiKBDefine.kbRetValue[42].bRetChar	= false;

	lguiKBDefine.kbRetValue[43].bRetChar	= true;
	lguiKBDefine.kbRetValue[43].NormalValue	= 'z';
	lguiKBDefine.kbRetValue[43].ShiftValue	= 'Z';

	lguiKBDefine.kbRetValue[44].bRetChar	= true;
	lguiKBDefine.kbRetValue[44].NormalValue	= 'x';
	lguiKBDefine.kbRetValue[44].ShiftValue	= 'X';

	lguiKBDefine.kbRetValue[45].bRetChar	= true;
	lguiKBDefine.kbRetValue[45].NormalValue	= 'c';
	lguiKBDefine.kbRetValue[45].ShiftValue	= 'C';

	lguiKBDefine.kbRetValue[46].bRetChar	= true;
	lguiKBDefine.kbRetValue[46].NormalValue	= 'v';
	lguiKBDefine.kbRetValue[46].ShiftValue	= 'V';

	lguiKBDefine.kbRetValue[47].bRetChar	= true;
	lguiKBDefine.kbRetValue[47].NormalValue	= 'b';
	lguiKBDefine.kbRetValue[47].ShiftValue	= 'B';

	lguiKBDefine.kbRetValue[48].bRetChar	= true;
	lguiKBDefine.kbRetValue[48].NormalValue	= 'n';
	lguiKBDefine.kbRetValue[48].ShiftValue	= 'N';

	lguiKBDefine.kbRetValue[49].bRetChar	= true;
	lguiKBDefine.kbRetValue[49].NormalValue	= 'm';
	lguiKBDefine.kbRetValue[49].ShiftValue	= 'M';

	lguiKBDefine.kbRetValue[50].bRetChar	= true;
	lguiKBDefine.kbRetValue[50].NormalValue	= ',';
	lguiKBDefine.kbRetValue[50].ShiftValue	= '<';

	lguiKBDefine.kbRetValue[51].bRetChar	= true;
	lguiKBDefine.kbRetValue[51].NormalValue	= '.';
	lguiKBDefine.kbRetValue[51].ShiftValue	= '>';

	lguiKBDefine.kbRetValue[52].bRetChar	= true;
	lguiKBDefine.kbRetValue[52].NormalValue	= '/';
	lguiKBDefine.kbRetValue[52].ShiftValue	= '?';

	lguiKBDefine.kbRetValue[53].bRetChar	= false;

	lguiKBDefine.kbRetValue[54].bRetChar	= false;

	lguiKBDefine.kbRetValue[55].bRetChar	= false;

	lguiKBDefine.kbRetValue[56].bRetChar	= true;
	lguiKBDefine.kbRetValue[56].NormalValue	= ' ';
	lguiKBDefine.kbRetValue[56].ShiftValue	= ' ';

	lguiKBDefine.kbRetValue[57].bRetChar	= false;
	lguiKBDefine.kbRetValue[58].bRetChar	= false;
	lguiKBDefine.kbRetValue[59].bRetChar	= false;
	lguiKBDefine.kbRetValue[60].bRetChar	= false;
	lguiKBDefine.kbRetValue[61].bRetChar	= false;
	lguiKBDefine.kbRetValue[62].bRetChar	= false;
	lguiKBDefine.kbRetValue[63].bRetChar	= false;
	lguiKBDefine.kbRetValue[64].bRetChar	= false;
	lguiKBDefine.kbRetValue[65].bRetChar	= false;
	lguiKBDefine.kbRetValue[66].bRetChar	= false;
	lguiKBDefine.kbRetValue[67].bRetChar	= false;
	lguiKBDefine.kbRetValue[68].bRetChar	= false;
	lguiKBDefine.kbRetValue[69].bRetChar	= false;
	
}




BOOL
SendKBMessage(
	int iScanCode
)
{
	int iIndex;
	char ch;
	
	iIndex = GetIndexByScanCode(iScanCode);
	if(iIndex == -1)
		return false;

//	if(iScanCode == SCANCODE_SLEEP)
//		system("/usr/local/bin/hpc_apm_suspend");

	//Send Message
	if(lguiKBDefine.kbRetValue[iIndex].bRetChar){
		if(lguiKBDefine.bShift){
			//Send Char Message
			if(lguiKBDefine.bCapsLock){
				ch = lguiKBDefine.kbRetValue[iIndex].ShiftValue;
				if(ch>='A' && ch<='Z'){
					SendMessage((HWND)_lGUI_pImeWindow,LMSG_CHAR,
						(WPARAM)(lguiKBDefine.kbRetValue[iIndex].NormalValue),(LPARAM)NULL);
				}
				else{
					SendMessage((HWND)_lGUI_pImeWindow,LMSG_CHAR,
						(WPARAM)(lguiKBDefine.kbRetValue[iIndex].ShiftValue),(LPARAM)NULL);
				}
			}
			else{
				SendMessage((HWND)_lGUI_pImeWindow,LMSG_CHAR,
					(WPARAM)(lguiKBDefine.kbRetValue[iIndex].ShiftValue),(LPARAM)NULL);
			}
		}
		else{// no shift
			if(lguiKBDefine.bCapsLock){
				ch = lguiKBDefine.kbRetValue[iIndex].NormalValue;
				if(ch>='a' && ch<='z'){
					SendMessage((HWND)_lGUI_pImeWindow,LMSG_CHAR,
						(WPARAM)(lguiKBDefine.kbRetValue[iIndex].ShiftValue),(LPARAM)NULL);
				}
				else{
					SendMessage((HWND)_lGUI_pImeWindow,LMSG_CHAR,
						(WPARAM)(lguiKBDefine.kbRetValue[iIndex].NormalValue),(LPARAM)NULL);
				}
			}
			else{
				SendMessage((HWND)_lGUI_pImeWindow,LMSG_CHAR,
					(WPARAM)(lguiKBDefine.kbRetValue[iIndex].NormalValue),(LPARAM)NULL);
			}
		}
	}
	else{
		//Send Key down message to client application process
		SendKeyDown2Client(lguiKBDefine.kbRetValue[iIndex].iScanCode);
		//SendMessage((HWND)_lGUI_pImeWindow,LMSG_KEYDOWN,
		//	(WPARAM)(lguiKBDefine.kbRetValue[iSubValue].iScanCode),(LPARAM)NULL);
	}
	return 1;
}


static int
GetIndexByScanCode(
	int iScanCode
)
{
	int i;
	for(i=0; i<NR_KEYNUM; i++){
		if(lguiKBDefine.kbRetValue[i].iScanCode == iScanCode)
			return i;
	}
	return -1;
}


void
SetLGUIKBShift(
	BOOL bValue
)
{
	lguiKBDefine.bShift = bValue;
}


void
SetLGUIKBCapsLock(
	BOOL bValue
)
{
	lguiKBDefine.bCapsLock = bValue;
}


void
SetLGUIKBCtrl(
	BOOL bValue
)
{
	lguiKBDefine.bCtrl = bValue;
}


void 
SetLGUIKBAlt(
	BOOL bValue
)
{
	lguiKBDefine.bAlt = bValue;
}


BOOL
IsLGUIKBShift()
{
	return lguiKBDefine.bShift;
}


BOOL
IsLGUIKBCapsLock()
{
	return lguiKBDefine.bCapsLock;
}


BOOL 
IsLGUIKBCtrl()
{
	return lguiKBDefine.bCtrl;
}


BOOL
IsLGUIKBAlt()
{
	return lguiKBDefine.bAlt;
}



