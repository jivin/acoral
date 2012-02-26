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


static PrivateHeap InvalidRegionHeap;

BOOL 
InitInvalidRegionHeap()
{
	return HeapCreate(&InvalidRegionHeap,SIZE_IRGN_HEAP,sizeof(InvalidRect));
}

void 
DestroyInvalidRegionHeap()
{
	HeapDestroy(&InvalidRegionHeap);
}


BOOL 
InitInvalidRegion(
	PInvalidRegion pInvalidRegion
)
{
	if(!pInvalidRegion)
		return false;
	memset(pInvalidRegion,0,sizeof(InvalidRegion));
	pInvalidRegion->pHeap = &InvalidRegionHeap;
	return true;
}

BOOL
IsEmptyInvalidRegion(
	PInvalidRegion pInvalidRegion
)
{
	if(pInvalidRegion->pHead)
		return false;
	else
		return true;
}

BOOL
GetBoundInvalidRegion(
	PInvalidRegion pInvalidRegion
)
{
	RECT rect;
	PInvalidRect pListNode;
	if(!pInvalidRegion)
		return  false;
	pListNode = pInvalidRegion->pHead;
	memcpy(&rect,&pListNode->rect,sizeof(RECT));
	pListNode = pListNode->pNext;
	while(pListNode){
		if(pListNode->rect.left < rect.left)
			rect.left = pListNode->rect.left;
		if(pListNode->rect.top < rect.top)
			rect.top = pListNode->rect.top;
		if(pListNode->rect.right > rect.right)
			rect.right = pListNode->rect.right;
		if(pListNode->rect.bottom > rect.bottom)
			rect.bottom = pListNode->rect.bottom;
		pListNode = pListNode->pNext;
	}
	memcpy(&pInvalidRegion->rcBound,&rect,sizeof(RECT));
	return true;
}


BOOL 
EmptyInvalidRegion(
	PInvalidRegion pInvalidRegion
)
{
	PInvalidRect pListNode;
	PInvalidRect pNextNode;

	if(!pInvalidRegion)
		return false;
	pNextNode = pListNode = pInvalidRegion->pHead;
	while(pListNode){
		pNextNode = pListNode->pNext;
		HeapFree(pInvalidRegion->pHeap,(void*)pListNode);
		pListNode = pNextNode;
	}
	pInvalidRegion->pHead = pInvalidRegion->pTail = NULL;
	memset(&pInvalidRegion->rcBound,0,sizeof(RECT));
	return true;
}

BOOL
AddRectInvalidRegion(
	PInvalidRegion pInvalidRegion,
	const PRECT prc,
	BOOL bErase
)
{

	PInvalidRect pNodeList;
	if(!pInvalidRegion)
		return false;
	if(!pInvalidRegion->pHeap)
		return false;
	pNodeList = (PInvalidRect)HeapAlloc(pInvalidRegion->pHeap);
	if(!pNodeList){
		printerror("Invalid Malloc ERROR\n");		
		return false;
	}
	memcpy(&pNodeList->rect,prc,sizeof(RECT));
	pNodeList->bErase = bErase;
	pNodeList->pNext = NULL;
	if(!pInvalidRegion->pHead){
		pInvalidRegion->pHead = pInvalidRegion->pTail = pNodeList;
		memcpy(&pInvalidRegion->rcBound,prc,sizeof(RECT));
	}
	else{
		pInvalidRegion->pTail->pNext = pNodeList;
		pInvalidRegion->pTail = pNodeList;
		GetBoundInvalidRegion(pInvalidRegion);
	}
	return true;
}





























