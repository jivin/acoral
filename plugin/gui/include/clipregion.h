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

#ifndef _CLIPREGION_H_
#define _CLIPREGION_H_
#include <resource.h>

#ifdef __cplusplus                     
extern "C" {
#endif


typedef struct _ClipRect
{
    RECT	rect;
    struct _ClipRect* pNext;
} ClipRect;
typedef ClipRect* PClipRect;

// Clip Region
typedef struct _ClipRegion
{
	RECT            rcBound;     // bound rect of clip region
	PClipRect       pHead;       // clip rect list head
	PClipRect       pTail;       // clip rect list tail
	PPrivateHeap    pHeap;        // heap of clip rect
} ClipRegion;
typedef ClipRegion* PClipRegion;



BOOL 
InitClipRegionHeap();

void 
DestroyClipRegionHeap();

BOOL
InitClipRegion(
	PClipRegion pClipRegion
);

BOOL 
IsEmptyClipRegion(
	const PClipRegion pClipRegion
);

BOOL
GetBoundClipRegion(
	PClipRegion pClipRegion
);

//to make the clipregion clean by delete the empty node
BOOL 
CleanupClipRegion(
	PClipRegion pClipRegion
);

//	to make clip region empty
BOOL 
EmptyClipRegion(
	PClipRegion pClipRegion
);

//set initial rectangle to clip region
BOOL
SetInitRectClipRegion(
	PClipRegion pClipRegion,
	const PRECT prc
);

BOOL
CopyClipRegion(
	PClipRegion pDst,
	PClipRegion	pSrc
);


//Add a rect to clip region
//no recalculate boundary of clip region
BOOL
AddRectClipRegion(
	PClipRegion pClipRegion,
	const PRECT prc
);

//every rect of clip region will be clipped by the prc
BOOL
IntersectClipRegion(
	PClipRegion pClipRegion, 
	const PRECT prc
);

BOOL
SubtractClipRegion(
	PClipRegion pClipRegion, 
	const PRECT prc
);

//get intersect between a clipregion and a invalidate region
BOOL  
ClipRgnIntersectInvRgn(
	PClipRegion pRstRegion, 
	const PClipRegion pClipRegion, 
	const PInvalidRegion pInvalidRegion
);


#ifdef __cplusplus
}
#endif 

#endif
