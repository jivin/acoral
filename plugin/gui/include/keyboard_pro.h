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

#ifndef _KEYBOARD_PRO_H
#define _KEYBOARD_PRO_H

#ifdef __cplusplus
extern "C" {
#endif


/************************** Key Scan code ***********************************/
#define SCANCODE_ESCAPE			0
#define SCANCODE_WAVE				1
#define SCANCODE_1					2
#define SCANCODE_2					3
#define SCANCODE_3					4
#define SCANCODE_4					5
#define SCANCODE_5					6
#define SCANCODE_6					7
#define SCANCODE_7					8
#define SCANCODE_8					9
#define SCANCODE_9					10
#define SCANCODE_0					11
#define SCANCODE_MINUS				12	// -
#define SCANCODE_EQUAL				13	// =
#define SCANCODE_BACKSPACE		14	


#define SCANCODE_TAB				15
#define SCANCODE_Q					16
#define SCANCODE_W					17
#define SCANCODE_E					18
#define SCANCODE_R					19
#define SCANCODE_T					20
#define SCANCODE_Y					21
#define SCANCODE_U					22
#define SCANCODE_I					23
#define SCANCODE_O					24
#define SCANCODE_P					25
#define SCANCODE_BRACKET_LEFT		26	// (
#define SCANCODE_BRACKET_RIGHT	27	// )
#define SCANCODE_BACKSLASH		28	//"\"

#define SCANCODE_CAPSLOCK			29
#define SCANCODE_A					30
#define SCANCODE_S					31
#define SCANCODE_D					32
#define SCANCODE_F					33
#define SCANCODE_G					34
#define SCANCODE_H					35
#define SCANCODE_J					36
#define SCANCODE_K					37
#define SCANCODE_L					38
#define SCANCODE_SEMICOLON		39	// ;
#define SCANCODE_APOSTROPHE		40	// '
#define SCANCODE_ENTER				41	// cr


#define SCANCODE_LEFTSHIFT			42
#define SCANCODE_Z					43
#define SCANCODE_X					44
#define SCANCODE_C					45
#define SCANCODE_V					46
#define SCANCODE_B					47
#define SCANCODE_N					48
#define SCANCODE_M					49
#define SCANCODE_COMMA			50	//  ,
#define SCANCODE_PERIOD			51	//  .
#define SCANCODE_SLASH				52	//  /	
#define SCANCODE_RIGHTSHIFT		53

#define SCANCODE_LEFTCONTROL		54
#define SCANCODE_LEFTALT			55
#define SCANCODE_SPACE				56
#define SCANCODE_RIGHTALT			57
#define SCANCODE_RIGHTCONTROL		58

#define SCANCODE_HOME				59			        
#define SCANCODE_END				60
#define SCANCODE_INS				61
#define SCANCODE_DEL				62
#define SCANCODE_PAGEUP			63
#define SCANCODE_PAGEDOWN		64
#define SCANCODE_LEFT				65
#define SCANCODE_RIGHT				66
#define SCANCODE_UP				67
#define SCANCODE_DOWN				68

#define SCANCODE_EXTPAD			69

#define SCANCODE_SLEEP				70


#define NR_KEYNUM			70

//=============================================



typedef struct tagKBRetValue{
	int iScanCode;
	BOOL bRetChar;	
	char NormalValue;
	char ShiftValue;
} KBRetValue;

typedef struct tagLGUIKBDefine{
	BOOL bShift;
	BOOL bCapsLock;
	BOOL bCtrl;
	BOOL bAlt;
	KBRetValue kbRetValue[NR_KEYNUM];
} LGUIKBDefine;
typedef LGUIKBDefine* PLGUIKBDefine ;


void
SetLGUIKBShift(
	BOOL bValue
);


void
SetLGUIKBCapsLock(
	BOOL bValue
);


void
SetLGUIKBCtrl(
	BOOL bValue
);


void 
SetLGUIKBAlt(
	BOOL bValue
);


BOOL
IsLGUIKBShift();


BOOL
IsLGUIKBCapsLock();


BOOL 
IsLGUIKBCtrl();


BOOL
IsLGUIKBAlt();


void
InitKBRetValue();


BOOL
SendKBMessage(
	int iScanCode
);

#ifdef __cplusplus
}
#endif


#endif





