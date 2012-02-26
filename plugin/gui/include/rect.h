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

#ifndef _RECT_H_
#define _RECT_H_

#ifdef __cplusplus                     
extern "C" {
#endif


BOOL GUIAPI
SetRect(
	LPRECT lprc, // pointer to structure with rectangle to set
	int xLeft,   // left side
	int yTop,    // top side
	int xRight,  // right side
	int yBottom  // bottom side
);

BOOL GUIAPI
SetRectEmpty(
	LPRECT lprc   // pointer to structure with rectangle set to empty
);

BOOL GUIAPI
IsRectEmpty(
	const RECT *lprc   // pointer to structure with rectangle
);

BOOL GUIAPI
CopyRect(
	LPRECT lprcDst,      // pointer to structure for destination rectangle
	const RECT *lprcSrc  // pointer to structure for source rectangle
);

BOOL GUIAPI
EqualRect(
	const RECT *lprc1,  // pointer to structure with first rectangle
	const RECT *lprc2   // pointer to structure with second rectangle
);

BOOL GUIAPI
NormalizeRect(
	LPRECT lprc
);

BOOL GUIAPI
IntersectRect(
	LPRECT lprcDst,        // pointer to structure for intersection
	const RECT *lprcSrc1,  // pointer to structure with first rectangle
	const RECT *lprcSrc2   // pointer to structure with second rectangle
);

BOOL GUIAPI
IsIntersect(
	const RECT *lprc1,  // pointer to structure with first rectangle
	const RECT *lprc2   // pointer to structure with second rectangle
);

BOOL GUIAPI
OffsetRect(
	LPRECT lprc,  // pointer to structure with rectangle
	int dx,       // horizontal offset
	int dy        // vertical offset
);

BOOL GUIAPI
InflateRect(
	LPRECT lprc,  // pointer to rectangle
	int dx,       // amount to increase or decrease width
	int dy        // amount to increase or decrease height
);

BOOL GUIAPI
PtInRect(
	const RECT *lprc,  // pointer to structure with rectangle
	int x,
	int y
);




#ifdef __cplusplus
}
#endif 

#endif

