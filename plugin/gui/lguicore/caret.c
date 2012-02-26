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
#include "../include/blockheap.h"
#include "../include/rect.h"
#include "../include/invalidregion.h"
#include "../include/clipregion.h"
#include "../include/gmessage.h"
#include "../include/hdc.h"
#include "../include/gdi.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"


#define MIN_BLINK_TIME      100     // ms


extern int Gal_iBytesDataType;

BOOL GUIAPI 
CreateCaret (
	HWND hWnd, 
	PBITMAP pBitmap, 
	int nWidth, 
	int nHeight
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	if (!pWin->pCaretInfo) {
		if (!(pWin->pCaretInfo = (PCARETINFO)malloc (sizeof (CARETINFO))))
			return false;
		memset(pWin->pCaretInfo,0,sizeof(CARETINFO));

		pWin->pCaretInfo->pBitmap = pBitmap;
		if (pBitmap) {
			nWidth  = pBitmap->bmWidth;
			nHeight = pBitmap->bmHeight;
		}
		pWin->pCaretInfo->nWidth  = nWidth;
		pWin->pCaretInfo->nHeight = nHeight;
		pWin->pCaretInfo->nEffWidth  = nWidth;
		pWin->pCaretInfo->nEffHeight = nHeight;
		//printf("%d,%d\n",nWidth,nHeight);
		pWin->pCaretInfo->nBytesNr = nWidth * nHeight * Gal_iBytesDataType;
		pWin->pCaretInfo->pNormal = malloc (pWin->pCaretInfo->nBytesNr);
		pWin->pCaretInfo->pXored  = malloc (pWin->pCaretInfo->nBytesNr);
		memset(pWin->pCaretInfo->pNormal,0,pWin->pCaretInfo->nBytesNr);
		memset(pWin->pCaretInfo->pXored ,0,pWin->pCaretInfo->nBytesNr);

		if (pWin->pCaretInfo->pNormal == NULL ||
			pWin->pCaretInfo->pXored == NULL) {
			free (pWin->pCaretInfo);
			pWin->pCaretInfo = NULL;
			return false;
		}

		pWin->pCaretInfo->x = pWin->pCaretInfo->y = 0;

		pWin->pCaretInfo->fBlink  = false;
		pWin->pCaretInfo->fShow   = false;

		pWin->pCaretInfo->hOwner  = hWnd;
		pWin->pCaretInfo->uTime   = 500;
	}
	return true;
}

BOOL GUIAPI 
DestroyCaret(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	if (!pWin->pCaretInfo)
		return false;
	free(pWin->pCaretInfo->pNormal);
	free(pWin->pCaretInfo->pXored);
	free(pWin->pCaretInfo);

	return true;
}

UINT GUIAPI 
GetCaretBlinkTime(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	if (!pWin->pCaretInfo)
		return 0;

	return pWin->pCaretInfo->uTime;
}


BOOL GUIAPI 
SetCaretBlinkTime(
	HWND hWnd, 
	UINT uTime
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	if (!pWin->pCaretInfo)
		return false;

	if (uTime < MIN_BLINK_TIME)
		uTime = MIN_BLINK_TIME;

	pWin->pCaretInfo->uTime = uTime;
	return true;
}


BOOL 
BlinkCaret(
	HWND hWnd
)
{
	PWindowsTree pWin;
	HDC hDC;
	pWin = (PWindowsTree)hWnd;

	if (!pWin->pCaretInfo)
		return false;

	if (!pWin->pCaretInfo->fBlink)
		return false;

	hDC = GetDC (hWnd);

	if (!pWin->pCaretInfo->fShow) {
		// show caret
		PutSavedBoxOnDC (hDC,
		                pWin->pCaretInfo->x, pWin->pCaretInfo->y,
		                pWin->pCaretInfo->nEffWidth,
		                pWin->pCaretInfo->nEffHeight,
		                (PCOLORREF)pWin->pCaretInfo->pXored);

		pWin->pCaretInfo->fShow = true;
	}
	else {
		// hide caret
		PutSavedBoxOnDC (hDC,
		                pWin->pCaretInfo->x, pWin->pCaretInfo->y,
		                pWin->pCaretInfo->nEffWidth,
		                pWin->pCaretInfo->nEffHeight,
		                (PCOLORREF)pWin->pCaretInfo->pNormal);

		pWin->pCaretInfo->fShow = false;
	}

	ReleaseDC (hWnd, hDC);

	return true;
}
//show caret
BOOL GUIAPI 
ShowCaret(
	HWND hWnd
)
{
	PWindowsTree pWin;
	HDC hDC;

	pWin = (PWindowsTree)hWnd;

	if (!pWin->pCaretInfo)
		return false;

	if (pWin->pCaretInfo->fBlink)
		return false;

	pWin->pCaretInfo->fBlink = true;
	GetCaretBitmaps (pWin->pCaretInfo);

	if (!pWin->pCaretInfo->fShow) {
		// show caret immediately
		hDC = GetDC (hWnd);
		PutSavedBoxOnDC (hDC,
		                pWin->pCaretInfo->x, pWin->pCaretInfo->y,
		                pWin->pCaretInfo->nEffWidth,
		                pWin->pCaretInfo->nEffHeight,
		                (PCOLORREF)pWin->pCaretInfo->pXored);

		ReleaseDC (hWnd, hDC);
		pWin->pCaretInfo->fShow = true;
	}

	return true;
}
BOOL GUIAPI 
HideCaret(
	HWND hWnd
)
{
	PWindowsTree pWin;
	HDC hDC;
	pWin = (PWindowsTree)hWnd;

	if (!pWin->pCaretInfo)
		return false;

	if (!pWin->pCaretInfo->fBlink)
		return false;

	pWin->pCaretInfo->fBlink = false;
	if (pWin->pCaretInfo->fShow) {
		pWin->pCaretInfo->fShow = false;
		// hide caret immediately
		hDC = GetDC(hWnd);
		PutSavedBoxOnDC (hDC,
		                pWin->pCaretInfo->x, pWin->pCaretInfo->y,
		                pWin->pCaretInfo->nEffWidth,
		                pWin->pCaretInfo->nEffHeight,
		                (PCOLORREF)pWin->pCaretInfo->pNormal);
		ReleaseDC(hWnd,hDC);
	}
	return true;
}

void 
GetCaretBitmaps(
	PCARETINFO pCaretInfo
)
{
	int i;
	int sx, sy;
	BYTE* normal;
	BYTE* bitmap;
	BYTE* xored;

	// convert to screen coordinates
	sx = pCaretInfo->x;
	sy = pCaretInfo->y;
	ClientToScreen (pCaretInfo->hOwner, &sx, &sy);

	// save normal bitmap first.
	Gal_GetBox(sx,sy,pCaretInfo->nEffWidth,pCaretInfo->nEffHeight,
					(PCOLORREF)pCaretInfo->pNormal);

	// generate XOR bitmap.
	if (pCaretInfo->pBitmap) {

		normal = pCaretInfo->pNormal;
		bitmap = pCaretInfo->pBitmap->bmBits;
		xored  = pCaretInfo->pXored;

		for (i = 0; i < pCaretInfo->nBytesNr; i++)
			xored[i] = normal[i] ^ bitmap[i];
	}
	else{
		normal = pCaretInfo->pNormal;
		xored  = pCaretInfo->pXored;

		for (i = 0; i < pCaretInfo->nBytesNr; i++)
			xored[i] = normal[i] ^ 0xFF;
	}
}


BOOL GUIAPI
SetCaretPos(
	HWND hWnd, 
	int x, 
	int y
)
{
	PWindowsTree pWin;
	HDC hDC;
	pWin = (PWindowsTree)hWnd;

	if (!pWin->pCaretInfo)
		return false;

	if (pWin->pCaretInfo->fBlink) { /// set to blink
		if (pWin->pCaretInfo->fShow) {
			// hide caret first
			hDC = GetDC (hWnd);
			PutSavedBoxOnDC (hDC,
			            pWin->pCaretInfo->x, pWin->pCaretInfo->y,
			            pWin->pCaretInfo->nEffWidth,
			            pWin->pCaretInfo->nEffHeight,
			            (PCOLORREF)pWin->pCaretInfo->pNormal);
			// then update position
			pWin->pCaretInfo->x = x;
			pWin->pCaretInfo->y = y;

			// save normal bitmap first
			GetCaretBitmaps (pWin->pCaretInfo);

			// show caret again
			PutSavedBoxOnDC (hDC,
			            pWin->pCaretInfo->x, pWin->pCaretInfo->y,
			            pWin->pCaretInfo->nEffWidth,
			            pWin->pCaretInfo->nEffHeight,
			            (PCOLORREF)pWin->pCaretInfo->pXored);

			ReleaseDC (hWnd, hDC);
		}
		else {
			// update position
			pWin->pCaretInfo->x = x;
			pWin->pCaretInfo->y = y;

			// save normal bitmap first
			GetCaretBitmaps (pWin->pCaretInfo);

			// show caret
			hDC = GetDC (hWnd);
			PutSavedBoxOnDC (hDC,
			            pWin->pCaretInfo->x, pWin->pCaretInfo->y,
			            pWin->pCaretInfo->nEffWidth,
			            pWin->pCaretInfo->nEffHeight,
			            (PCOLORREF)pWin->pCaretInfo->pXored);

			ReleaseDC (hWnd, hDC);

			pWin->pCaretInfo->fShow = true;
		}
	}
	else {
		// update position
		pWin->pCaretInfo->x = x;
		pWin->pCaretInfo->y = y;
	}
	return true;
}

BOOL GUIAPI 
ChangeCaretSize(
	HWND hWnd, 
	int newWidth, 
	int newHeight
)
{
	PWindowsTree pWin;
	HDC hDC;
	pWin = (PWindowsTree)hWnd;

	if (!pWin->pCaretInfo)
		return false;

	if (newWidth == pWin->pCaretInfo->nEffWidth
	        && newHeight == pWin->pCaretInfo->nEffHeight)
		return true;
	    
	if (newWidth > pWin->pCaretInfo->nWidth
	        || newHeight > pWin->pCaretInfo->nHeight
	        || newWidth <= 0 || newHeight <= 0)
		return false;

	if (pWin->pCaretInfo->fBlink) {
		if (pWin->pCaretInfo->fShow) {
			// hide caret first
			hDC = GetDC (hWnd);
			PutSavedBoxOnDC (hDC,
			            pWin->pCaretInfo->x, pWin->pCaretInfo->y,
			            pWin->pCaretInfo->nEffWidth,
			            pWin->pCaretInfo->nEffHeight,
			            (PCOLORREF)pWin->pCaretInfo->pNormal);

			// then update size info
			pWin->pCaretInfo->nEffWidth = newWidth;
			pWin->pCaretInfo->nEffHeight = newHeight;
			pWin->pCaretInfo->nBytesNr = newWidth*newHeight*Gal_iBytesDataType;

			// save normal bitmap first
			GetCaretBitmaps (pWin->pCaretInfo);

			// show caret again
			PutSavedBoxOnDC (hDC,
			            pWin->pCaretInfo->x, pWin->pCaretInfo->y,
			            pWin->pCaretInfo->nEffWidth,
			            pWin->pCaretInfo->nEffHeight,
			            (PCOLORREF)pWin->pCaretInfo->pXored);

			ReleaseDC (hWnd, hDC);
		}
		else {
			// then update size info
			pWin->pCaretInfo->nEffWidth = newWidth;
			pWin->pCaretInfo->nEffHeight = newHeight;
			pWin->pCaretInfo->nBytesNr = newWidth*newHeight*Gal_iBytesDataType;

			// save normal bitmap first
			GetCaretBitmaps (pWin->pCaretInfo);

			// show caret
			hDC = GetDC (hWnd);
			PutSavedBoxOnDC (hDC,
			            pWin->pCaretInfo->x, pWin->pCaretInfo->y,
			            pWin->pCaretInfo->nEffWidth,
			            pWin->pCaretInfo->nEffHeight,
			            (PCOLORREF)pWin->pCaretInfo->pXored);

			ReleaseDC (hWnd, hDC);

			pWin->pCaretInfo->fShow = true;
		}
	}
	else {
		// update size info
		pWin->pCaretInfo->nEffWidth  = newWidth;
		pWin->pCaretInfo->nEffHeight = newHeight;
		pWin->pCaretInfo->nBytesNr = newWidth*newHeight*Gal_iBytesDataType;
	}

	return true;
}
// Get Caret Position
BOOL GUIAPI 
GetCaretPos(
	HWND hWnd, 
	PPOINT pPt
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	if (!pWin->pCaretInfo)
		return false;

	pPt->x = pWin->pCaretInfo->x;
	pPt->y = pWin->pCaretInfo->y;

	return true;
}

