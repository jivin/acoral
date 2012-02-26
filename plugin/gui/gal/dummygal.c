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
#include "../include/mouse.h"
#include "../include/rect.h"
#include "../include/gal.h"
static BOOL Init();
static void UnInit();
static void /*inline*/ 
PutPixelRaw(
	int x,
	int y, 
	COLORREF color
);


static COLORREF /*inline*/ 
GetPixelRaw(
	int x,
	int y
);

static void /*inline*/ 
PutLineRaw(
	int xPos,
	int yPos, 
	int iLen, 
	void* pData
);

static void 
GetLineRaw(
	int xPos, 
	int yPos, 
	int iLen, 
	void* pData
);

static void PutRectRaw(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData
);

static void 
PutRectWithTrCorRaw(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData, 
	COLORREF crTrColor
);

/*inline*/
static void 
GetBoxRaw(
	int x,
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
);

/*inline*/
static  void 
PutBoxRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
);

static  void 
PutBoxWithTrCorRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData, 
	COLORREF crColor
);

static  void 
EraseRectRaw(
	PRECT lpRc,
	COLORREF color
);

struct GalOps DummyGalOps={
	Init,
	UnInit,
	PutPixelRaw,
	GetPixelRaw,
	PutLineRaw,
	GetLineRaw,
	PutRectRaw,
	NULL,//GetRectRaw,
	PutRectWithTrCorRaw,
	PutBoxRaw,
	GetBoxRaw,
	PutBoxWithTrCorRaw,
	EraseRectRaw
};
struct GalDev DummyGalDev={
	&DummyGalOps,
	320,
	240,
	3
};

static BOOL 
Init(){
	return true;
}

static  void 
UnInit(){

}

static  void 
PutPixelRaw(
	int x,
	int y, 
	COLORREF color
)
{
}

static COLORREF /*inline*/ 
GetPixelRaw(
	int x,
	int y
)
{
	return 0;
}

static  void 
PutRectRaw(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData
){

}

static  void 
PutRectWithTrCorRaw(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData, 
	COLORREF crTrColor
)
{
}


static  void 
PutLineRaw(
	int xPos,
	int yPos, 
	int iLen, 
	void* pData
)
{

}


static  void 
GetLineRaw(
	int xPos, 
	int yPos, 
	int iLen, 
	void* pData
)
{
}

static  void 
EraseRectRaw(
	PRECT lpRc,
	COLORREF color
)
{
}

static  void 
GetBoxRaw(
	int x,
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
)
{
}

static  void 
PutBoxRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
)
{
}

static  void 
PutBoxWithTrCorRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData, 
	COLORREF crColor
)
{
}

