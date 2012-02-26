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

#ifndef _IMEWIN_H
#define _IMEWIN_H

#ifdef __cplusplus
extern "C" {
#endif

//[|----4 ----||------4------|][|-----4-----||----4----|][|----4----||----4----|][|----4----||----4----|]
//[|in method-||-L/R pressed-|][|-sdc/dbc  -||----4----|][|----4----||----4----|][|----4----||----4----|]



#define IMEWIN_LEFT 			0
#define IMEWIN_TOP				SCREEN_HEIGHT-118
#define IMEWIN_WIDTH			239
#define IMEWIN_HEIGHT			16


#define IMEWIN_LIST_WIDTH		160
#define IMEWIN_INPUT_WIDTH	50
#define IMEWIN_BUTTON_WIDTH	12
#define IMEWIN_BORDER			1

#define IMEBMP_BACKGROUND		"/pics/sys/ime/ime_bar.bmp"	//


#define IME_STATUS_INVALID		-1
#define IME_STATUS_MAX			3
#define IME_STATUS_ENGLISH		0
#define IME_STATUS_HANDWRITE	1
#define IME_STATUS_PINYIN		2
//#define IME_STATUS_WUBI		3


#define IME_LEFT_PRESSED		1
#define IME_RIGHT_PRESSED		2

#define IME_STATUS_SBC			0 
#define IME_STATUS_DBC			1 


#define SBC2DBC_TABLE_LENGTH 	64



typedef struct tagSBC_DBC{
	unsigned char sbc;
	unsigned char dbc;
} SBC_DBC;


static SBC_DBC sbc_dbc_table[]={
	{'0','０'},
	{'1','１'},
	{'2','２'},
	{'3','３'},
	{'4','４'},
	{'5','５'},
	{'6','６'},
	{'7','７'},
	{'8','８'},
	{'9','９'},
	{'A','Ａ'},
	{'B','Ｂ'},
	{'C','Ｃ'},
	{'D','Ｄ'},
	{'E','Ｅ'},
	{'F','Ｆ'},
	{'G','Ｇ'},
	{'H','Ｈ'},
	{'I','Ｉ'},
	{'J','Ｊ'},
	{'K','Ｋ'},
	{'L','Ｌ'},
	{'M','Ｍ'},
	{'N','Ｎ'},
	{'O','Ｏ'},
	{'P','Ｐ'},
	{'Q','Ｑ'},
	{'R','Ｒ'},
	{'S','Ｓ'},
	{'T','Ｔ'},
	{'U','Ｕ'},
	{'V','Ｖ'},
	{'W','Ｗ'},
	{'X','Ｘ'},
	{'Y','Ｙ'},
	{'Z','Ｚ'},
	{'!','！'},
	{'@','·'},
	{'#','＃'},
	{'$','￥'},
	{'%','％'},
	{'^','…'},
	{'&','—'},
	{'*','＊'},
	{'(','（'},
	{')','）'},
	{'-','－'},
	{'_','—'},
	{'+','＋'},
	{'=','＝'},
	{'[','［'},
	{']','］'},
	{'{','｛'},
	{'}','｝'},
	{'\\','、'},
	{'|','｜'},
	{',','，'},
	{'<','《'},
	{'.','。'},
	{'>','》'},
	{'/','／'},
	{'?','？'},
	{'`','｀'},
	{'~','～'},
	{':','：'},
	{';','；'}
};


#define FRAME_CHARS		6
#define MAX_HZCHARS		256
#define MAX_INPUTCHARS		6


typedef struct tagImeDataTable{
	int		iTotalInputChars;
	char		pTotalInputChars[MAX_INPUTCHARS];
	int		iTotalHZChars;						//strlen 
	char		pTotalHZChars[MAX_HZCHARS];		
	int		iBeginPos;							// in chinese
	char		pReturnHZChars[FRAME_CHARS*2+1];	//in chinese 
	char		pCurSelected[3];
	char 	pListChars[MAX_HZCHARS];
} ImeDataTable;
typedef ImeDataTable* PImeDataTable;





typedef struct tagLGUIIme{
	int 	iIndex;					//current ime index value 
	char pTaskBarIconPath[256];	//dimension of task bar icon is 18X18
	BOOL (*InitIme)(void);
	void   (*DestroyIme)(void);
	void   (*ResetIme)(void);
		
	BOOL (*CharArrived)(char ch);
	void (*FillBackwardSelection)(void);
	void (*FillForwardSelection)(void);
	struct tagLGUIIme* pNext;
}LGUIIme;
typedef LGUIIme* PLGUIIme;

void 
CreateImeWindow(
	LPRECT lpRect
);


void 
DestroyImeWindow();


BOOL 
RegisterImeWindow();


LRESULT 
ImeWindowProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);


void 
SwitchIme();


void 
SwitchDbc(
	BOOL bValue
);

void 
SetDbc();


void 
SetSbc();

#ifdef __cplusplus

}
#endif


#endif





