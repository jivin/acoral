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
#include "../include/kb_ial.h"
KBDEVICE* kbdev;

extern KBDEVICE  kbdev_PC;//keyboard of pc
extern KBDEVICE  kbdev_2440;//keyboard of pc
extern KBDEVICE  kbdev_Qvfb;//keyboard of pc
extern KBDEVICE  kbdev_Dummy;//keyboard of pc

void 
InstallKBDevice()
{
	kbdev=&kbdev_Dummy;

#ifdef CFG_X86_EMU_CMP
//	kbdev = &kbdev_Qvfb;
#endif

#ifdef CFG_X86_EMU_SINGLE
//	kbdev = &kbdev_Qvfb;
#endif

#ifdef CFG_ARM_S3C2440
	kbdev = &kbdev_2440;
#endif

}


int 
OpenKB()
{
	return kbdev->Open();
}


void 
CloseKB()
{
	kbdev->Close();
}


int 
ReadKB(
	BYTE* btScanCode,
	BYTE* btPressed
)
{
	return kbdev->Read(btScanCode,btPressed);
}

