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

#include <linux/fb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <termios.h>
#include <stdio.h>
#include "../include/common.h"
#include "../include/keyboard_pro.h"
#include "../include/kb_ial.h"
static int 
PCKB_Open(void);


static void 
PCKB_Close(void);


static int 
PCKB_Read(
	BYTE* btScanCode,
	BYTE* btPressed
);
static int kb_fd;

//a table map keyboard return string to a scancode

typedef struct tagKBMapTable{
	char buffer[10];
	char shift[10];
	int iScanCode;
} KBMapTable;
typedef struct KBMapTable* PKBMapTable;

KBMapTable kbMapTable[NR_KEYNUM];


static void
InitKBMapTable();


static int
GetScanCode(
	char* buffer, 
	int iLen
);



KBDEVICE kbdev_PC = {
        PCKB_Open,
        PCKB_Close,
        PCKB_Read
};

static int 
PCKB_Open(void)
{
	struct termios old_flags,new_flags;
	
	kb_fd =fileno(stdin);	

	if ( kb_fd<0 ){
		printf("Open device kdc error!!!\n\n\n");
		return -1;
	}
	
	tcgetattr(kb_fd,&old_flags);
	new_flags = old_flags;
	new_flags.c_lflag &= ~(ICANON | ECHO | ECHOE );
	tcsetattr(kb_fd,TCSAFLUSH, &new_flags);
	
	InitKBMapTable();
	return 0;	
}


static void 
PCKB_Close(void)
{
	close(kb_fd);
}


static int 
PCKB_Read(
	BYTE* btScanCode,
	BYTE* btPressed
)
{
	char buffer[10];
	int readByte;
	memset(buffer, 0, 10);
	readByte = read(kb_fd,buffer,10);
	//printf("%s\n",buffer+1);
	*btScanCode = (BYTE)GetScanCode(buffer, readByte);
	*btPressed = 1;

	return 1;
}



static int
GetScanCode(
	char* buffer, 
	int iLen
)
{
	int i;
	for(i=0; i<NR_KEYNUM; i++){
		if(strncmp(kbMapTable[i].buffer, buffer, iLen)==0)
			return kbMapTable[i].iScanCode;
		if(strncmp(kbMapTable[i].shift, buffer, iLen)==0)
			return kbMapTable[i].iScanCode;
	}
	return -1;
	
}


	
static void
InitKBMapTable()
{
	memset(&kbMapTable,0,sizeof(KBMapTable)*NR_KEYNUM);
	

	kbMapTable[0].buffer[0] = 27;
	kbMapTable[0].shift[0] = 27;
	kbMapTable[0].iScanCode = SCANCODE_ESCAPE;
	
	kbMapTable[1].buffer[0] = 96;
	kbMapTable[1].shift[0] = 126;
	kbMapTable[1].iScanCode = SCANCODE_WAVE;
	
	kbMapTable[2].buffer[0] = 49;
	kbMapTable[2].shift[0] = 33;	
	kbMapTable[2].iScanCode = SCANCODE_1;
	
	kbMapTable[3].buffer[0] = 50;
	kbMapTable[3].shift[0] = 64;
	kbMapTable[3].iScanCode = SCANCODE_2;
	
	kbMapTable[4].buffer[0] = 51;
	kbMapTable[4].shift[0] = 35;
	kbMapTable[4].iScanCode = SCANCODE_3;
	
	kbMapTable[5].buffer[0] = 52;
	kbMapTable[5].shift[0] = 36;
	kbMapTable[5].iScanCode = SCANCODE_4;
	
	kbMapTable[6].buffer[0] = 53;
	kbMapTable[6].shift[0] = 37;
	kbMapTable[6].iScanCode = SCANCODE_5;
	
	kbMapTable[7].buffer[0] = 54;
	kbMapTable[7].shift[0] = 94;
	kbMapTable[7].iScanCode = SCANCODE_6;
	
	kbMapTable[8].buffer[0] = 55;
	kbMapTable[8].shift[0] = 38;
	kbMapTable[8].iScanCode = SCANCODE_7;
	
	kbMapTable[9].buffer[0] = 56;
	kbMapTable[9].shift[0] = 42;
	kbMapTable[9].iScanCode = SCANCODE_8;
	
	kbMapTable[10].buffer[0] = 57;
	kbMapTable[10].shift[0] = 40;
	kbMapTable[10].iScanCode = SCANCODE_9;
	
	kbMapTable[11].buffer[0] = 48;
	kbMapTable[11].shift[0] = 41;
	kbMapTable[11].iScanCode = SCANCODE_0;
	
	kbMapTable[12].buffer[0] = 45;
	kbMapTable[12].shift[0] = 95;
	kbMapTable[12].iScanCode = SCANCODE_MINUS;
	
	kbMapTable[13].buffer[0] = 61;
	kbMapTable[13].shift[0] = 43;
	kbMapTable[13].iScanCode = SCANCODE_EQUAL;
	
	kbMapTable[14].buffer[0] = 127;
	kbMapTable[14].shift[0] = 127;
	kbMapTable[14].iScanCode = SCANCODE_BACKSPACE;
	
	
	kbMapTable[15].buffer[0] = 9;
	kbMapTable[15].shift[0] = 9;
	kbMapTable[15].iScanCode = SCANCODE_TAB;
	
	kbMapTable[16].buffer[0] = 113;
	kbMapTable[16].shift[0] = 81;
	kbMapTable[16].iScanCode = SCANCODE_Q;
	
	kbMapTable[17].buffer[0] = 119;;
	kbMapTable[17].shift[0] = 87;
	kbMapTable[17].iScanCode = SCANCODE_W;
	
	kbMapTable[18].buffer[0] = 101;
	kbMapTable[18].shift[0] = 69;
	kbMapTable[18].iScanCode = SCANCODE_E;
	
	kbMapTable[19].buffer[0] = 114;
	kbMapTable[19].shift[0] = 82;
	kbMapTable[19].iScanCode = SCANCODE_R;
	
	kbMapTable[20].buffer[0] = 116;
	kbMapTable[20].shift[0] = 84;
	kbMapTable[20].iScanCode = SCANCODE_T;
	
	kbMapTable[21].buffer[0] = 121;
	kbMapTable[21].shift[0] = 89;
	kbMapTable[21].iScanCode = SCANCODE_Y;
	
	kbMapTable[22].buffer[0] = 117;
	kbMapTable[22].shift[0] = 85;
	kbMapTable[22].iScanCode = SCANCODE_U;
	
	kbMapTable[23].buffer[0] = 105;
	kbMapTable[23].shift[0] = 73;
	kbMapTable[23].iScanCode = SCANCODE_I;
	
	kbMapTable[24].buffer[0] = 111;
	kbMapTable[24].shift[0] = 79;
	kbMapTable[24].iScanCode = SCANCODE_O;
	
	kbMapTable[25].buffer[0] = 112;
	kbMapTable[25].shift[0] = 80;
	kbMapTable[25].iScanCode = SCANCODE_P;
	
	kbMapTable[26].buffer[0] = 91;
	kbMapTable[26].shift[0] = 123;
	kbMapTable[26].iScanCode = SCANCODE_BRACKET_LEFT;
	
	kbMapTable[27].buffer[0] = 93;
	kbMapTable[27].shift[0] = 125;
	kbMapTable[27].iScanCode = SCANCODE_BRACKET_RIGHT;
	
	kbMapTable[28].buffer[0] = 92;
	kbMapTable[28].shift[0] = 124;
	kbMapTable[28].iScanCode = SCANCODE_BACKSLASH;
	
	
	
//	kbMapTable[29].buffer[0] = 27;
//	kbMapTable[29].shift[0] = 27;
//	kbMapTable[29].iScanCode = SCANCODE_CAPSLOCK;
	
	kbMapTable[30].buffer[0] = 97;
	kbMapTable[30].shift[0] = 65;
	kbMapTable[30].iScanCode = SCANCODE_A;
	
	kbMapTable[31].buffer[0] = 115;
	kbMapTable[31].shift[0] = 83;
	kbMapTable[31].iScanCode = SCANCODE_S;
	
	kbMapTable[32].buffer[0] = 100;
	kbMapTable[32].shift[0] = 68;
	kbMapTable[32].iScanCode = SCANCODE_D;
	
	kbMapTable[33].buffer[0] = 102;
	kbMapTable[33].buffer[0] = 70;
	kbMapTable[33].iScanCode = SCANCODE_F;
	
	kbMapTable[34].buffer[0] = 103;
	kbMapTable[34].shift[0] = 71;
	kbMapTable[34].iScanCode = SCANCODE_G;
	
	kbMapTable[35].buffer[0] = 104;
	kbMapTable[35].shift[0] = 72;
	kbMapTable[35].iScanCode = SCANCODE_H;
	
	kbMapTable[36].buffer[0] = 106;
	kbMapTable[36].shift[0] = 74;
	kbMapTable[36].iScanCode = SCANCODE_J;
	
	kbMapTable[37].buffer[0] = 107;
	kbMapTable[37].shift[0] = 75;
	kbMapTable[37].iScanCode = SCANCODE_K;
	
	kbMapTable[38].buffer[0] = 108;
	kbMapTable[38].shift[0] = 76;
	kbMapTable[38].iScanCode = SCANCODE_L;
	
	kbMapTable[39].buffer[0] = 59;
	kbMapTable[39].shift[0] = 58;
	kbMapTable[39].iScanCode = SCANCODE_SEMICOLON;
	
	kbMapTable[40].buffer[0] = 39;
	kbMapTable[40].shift[0] = 34;
	kbMapTable[40].iScanCode = SCANCODE_APOSTROPHE;
	
	kbMapTable[41].buffer[0] = 10;
	kbMapTable[41].shift[0] = 10;
	kbMapTable[41].iScanCode = SCANCODE_ENTER;
	
	
	

	
//	kbMapTable[42].buffer[0] = 27;
//	kbMapTable[42].shift[0] = 27;
//	kbMapTable[42].iScanCode = SCANCODE_LEFTSHIFT;
	
	kbMapTable[43].buffer[0] = 122;
	kbMapTable[43].shift[0] = 90;
	kbMapTable[43].iScanCode = SCANCODE_Z;
	
	kbMapTable[44].buffer[0] = 120;
	kbMapTable[44].shift[0] = 88;
	kbMapTable[44].iScanCode = SCANCODE_X;
	
	kbMapTable[45].buffer[0] = 99;
	kbMapTable[45].shift[0] = 67;
	kbMapTable[45].iScanCode = SCANCODE_C;
	
	kbMapTable[46].buffer[0] = 118;
	kbMapTable[46].shift[0] = 86;
	kbMapTable[46].iScanCode = SCANCODE_V;
	
	kbMapTable[47].buffer[0] = 98;
	kbMapTable[47].shift[0] = 66;
	kbMapTable[47].iScanCode = SCANCODE_B;
	
	kbMapTable[48].buffer[0] = 110;
	kbMapTable[48].shift[0] = 78;
	kbMapTable[48].iScanCode = SCANCODE_N;
	
	kbMapTable[49].buffer[0] = 109;
	kbMapTable[49].shift[0] = 77;
	kbMapTable[49].iScanCode = SCANCODE_M;
	
	kbMapTable[50].buffer[0] = 44;
	kbMapTable[50].shift[0] = 60;
	kbMapTable[50].iScanCode = SCANCODE_COMMA;
	
	kbMapTable[51].buffer[0] = 46;
	kbMapTable[51].shift[0] = 62;
	kbMapTable[51].iScanCode = SCANCODE_PERIOD;
	
	kbMapTable[52].buffer[0] = 47;
	kbMapTable[52].shift[0] = 63;
	kbMapTable[52].iScanCode = SCANCODE_SLASH;
	
//	kbMapTable[53].buffer[0] = 27;
//	kbMapTable[53].shift[0] = 27;
//	kbMapTable[53].iScanCode = SCANCODE_RIGHTSHIFT;
	


	
//	kbMapTable[54].buffer[0] = 27;
//	kbMapTable[54].shift[0] = 27;
//	kbMapTable[54].iScanCode = SCANCODE_LEFTCONTROL;
	
//	kbMapTable[55].buffer[0] = 27;
//	kbMapTable[55].shift[0] = 27;
//	kbMapTable[55].iScanCode = SCANCODE_LEFTALT;
	
	kbMapTable[56].buffer[0] = 32;
	kbMapTable[56].shift[0] = 32;
	kbMapTable[56].iScanCode = SCANCODE_SPACE;
	
//	kbMapTable[57].buffer[0] = 27;
//	kbMapTable[57].shift[0] = 27;
//	kbMapTable[57].iScanCode = SCANCODE_RIGHTALT;
	
//	kbMapTable[58].buffer[0] = 27;
//	kbMapTable[58].shift[0] = 27;
//	kbMapTable[58].iScanCode = SCANCODE_RIGHTCONTROL;
	
	
	
	kbMapTable[59].buffer[0] = 27;
	strcpy(&(kbMapTable[59].buffer[1]),"[1~");
	kbMapTable[59].iScanCode = SCANCODE_HOME;
	
	kbMapTable[60].buffer[0] = 27;
	strcpy(&(kbMapTable[60].buffer[1]),"[4~");
	kbMapTable[60].iScanCode = SCANCODE_END;
	
	kbMapTable[61].buffer[0] = 27;
	strcpy(&(kbMapTable[61].buffer[1]),"[2~");
	kbMapTable[61].iScanCode = SCANCODE_INS;
	
	kbMapTable[62].buffer[0] = 27;
	strcpy(&(kbMapTable[62].buffer[1]),"[3~");
	kbMapTable[62].iScanCode = SCANCODE_DEL;
	
	kbMapTable[63].buffer[0] = 27;
	strcpy(&(kbMapTable[63].buffer[1]),"[5~");
	kbMapTable[63].iScanCode = SCANCODE_PAGEUP;
	
	kbMapTable[64].buffer[0] = 27;
	strcpy(&(kbMapTable[64].buffer[1]),"[6~");
	kbMapTable[64].iScanCode = SCANCODE_PAGEDOWN;
	
	kbMapTable[65].buffer[0] = 27;
	strcpy(&(kbMapTable[65].buffer[1]),"[D");
	kbMapTable[65].iScanCode = SCANCODE_LEFT;
	
	kbMapTable[66].buffer[0] = 27;
	strcpy(&(kbMapTable[66].buffer[1]),"[C");
	kbMapTable[66].iScanCode = SCANCODE_RIGHT;
	
	kbMapTable[67].buffer[0] = 27;
	strcpy(&(kbMapTable[67].buffer[1]),"[A");	
	kbMapTable[67].iScanCode = SCANCODE_UP;
	
	kbMapTable[68].buffer[0] = 27;
	strcpy(&(kbMapTable[68].buffer[1]),"[B");	
	kbMapTable[68].iScanCode = SCANCODE_DOWN;
	
	
	
}


