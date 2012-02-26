/*
	Copyright (C) 2004-2005 Li Yudong
	Some ideas come from MiniGUI
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


static PrivateHeap ClipRegionHeap;

BOOL 
InitClipRegionHeap()
{
	return HeapCreate(&ClipRegionHeap,SIZE_CRGN_HEAP,sizeof(ClipRect));
}

void 
DestroyClipRegionHeap()
{
	HeapDestroy(&ClipRegionHeap);
}

BOOL
InitClipRegion(
	PClipRegion pClipRegion
)
{
	if(!pClipRegion)
		return false;
	memset(pClipRegion,0,sizeof(ClipRegion));
	pClipRegion->pHeap = &ClipRegionHeap;
	return true;
}

BOOL 
IsEmptyClipRegion(
	const PClipRegion pClipRegion)
{
	if(!pClipRegion->pHead)
		return true;
	else
		return false;
}

BOOL
GetBoundClipRegion(
	PClipRegion pClipRegion
)
{
	RECT rect;
	PClipRect pListNode, pTemp;
	if(!pClipRegion)
		return false;
	
	pListNode = pClipRegion->pHead;
	if(!pListNode)
		return true;
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
	memcpy(&pClipRegion->rcBound,&rect,sizeof(RECT));
	
    //cleanup null rect node in the clipregion link table
	pListNode = pClipRegion->pHead;
	while(pListNode->pNext){
		if(IsRectEmpty(&pListNode->pNext->rect)){
			pTemp = pListNode->pNext;
			pListNode->pNext = pTemp->pNext;
			HeapFree(pClipRegion->pHeap,(void*)pTemp);
		}
		else
			pListNode = pListNode->pNext;
	}
	pClipRegion->pTail = pListNode;
	if(IsRectEmpty(&pClipRegion->pHead->rect)){
		pTemp = pClipRegion->pHead;
		pClipRegion->pHead = pTemp->pNext;
		HeapFree(pClipRegion->pHeap,(void*)pTemp);
	}
	return true;
}
		
 
	
//make the clipregion clean by delete the empty node
BOOL 
CleanupClipRegion(
	PClipRegion pClipRegion
)
{
	PClipRect pListNode;
	PClipRect pPrevNode;
	pPrevNode = pListNode = pClipRegion->pHead;
	if(!pClipRegion)
		return false;
	while(pListNode){
		if(IsRectEmpty(&pListNode->rect)){
			if(pListNode == pClipRegion->pHead){
				if(pClipRegion->pHead == pClipRegion->pTail){//only one node
					pClipRegion->pHead = pClipRegion->pTail = NULL;
				}
				else{//not only one node 
					pClipRegion->pHead = pListNode->pNext;
				}
			}
			else if(pListNode == pClipRegion->pTail){
				pClipRegion->pTail = pPrevNode;
			}
			else{
				pPrevNode->pNext = pListNode->pNext;
			}

			HeapFree(pClipRegion->pHeap, (void*)pListNode);
		}
		pPrevNode = pListNode;
		pListNode = pListNode->pNext;
	}
	return true;
}


// make clip region empty
BOOL 
EmptyClipRegion(
	PClipRegion pClipRegion
)
{
	PClipRect pListNode;
	PClipRect pNextNode;

	if(!pClipRegion)
		return false;
	pNextNode = pListNode = pClipRegion->pHead;
	while(pListNode){
		pNextNode = pListNode->pNext;
		HeapFree(pClipRegion->pHeap,(void*)pListNode);
		pListNode = pNextNode;
	}
	pClipRegion->pHead = pClipRegion->pTail = NULL;
	memset(&pClipRegion->rcBound,0,sizeof(RECT));
	return true;
}


//set initial rectangle to clip region
BOOL
SetInitRectClipRegion(
	PClipRegion pClipRegion,
	const PRECT prc
)
{
	PClipRect pNodeList;
	if(pClipRegion->pHead)//if not empty clipregion
		if(!EmptyClipRegion(pClipRegion))
			return false;
	pNodeList = (PClipRect)HeapAlloc(pClipRegion->pHeap);
	if(!pNodeList)
		return false;
	memcpy(&pNodeList->rect,prc,sizeof(RECT));
	pNodeList->pNext = NULL;
	pClipRegion->pHead = pClipRegion->pTail = pNodeList;
	memcpy(&pClipRegion->rcBound,prc,sizeof(RECT));
	return true;
}

BOOL
CopyClipRegion(
	PClipRegion pDst,
	PClipRegion	pSrc
)
{
	PClipRect pSrcNode,pDstNode;
	if(pDst->pHead)
		EmptyClipRegion(pDst);
	if(!pSrc->pHead)
		return false;
	if(!pDst->pHeap)
		return false;
	pDst->pHead = (PClipRect)HeapAlloc(pSrc->pHeap);
	pDstNode = pDst->pHead;
	pSrcNode = pSrc->pHead;
	memcpy(&pDst->pHead->rect,&pSrc->pHead->rect,sizeof(RECT));
	pSrcNode = pSrcNode->pNext;
	while(pSrcNode){
		pDstNode->pNext = (PClipRect)HeapAlloc(pSrc->pHeap);
		memcpy(&pDstNode->pNext->rect,&pSrcNode->rect,sizeof(RECT));
		pDstNode = pDstNode->pNext;
		pSrcNode = pSrcNode->pNext;
	}
	pDstNode->pNext = NULL;
	pDst->pTail = pDstNode;

	memcpy(&pDst->rcBound,&pSrc->rcBound,sizeof(RECT));
	return true;
}



//Add a rect to clip region
//no recalculate boundary of clip region
BOOL
AddRectClipRegion(
	PClipRegion pClipRegion,
	const PRECT prc
)
{

	PClipRect pNodeList;
	if(!pClipRegion)
		return false;
	if(!pClipRegion->pHeap)
		return false;
	pNodeList = (PClipRect)HeapAlloc(pClipRegion->pHeap);
	memcpy(&pNodeList->rect,prc,sizeof(RECT));
	pNodeList->pNext = NULL;
	if(!pClipRegion->pHead){
		pClipRegion->pHead = pClipRegion->pTail = pNodeList;
		memcpy(&pClipRegion->rcBound,prc,sizeof(RECT));
	}
	else{
		pClipRegion->pTail->pNext = pNodeList;
		pClipRegion->pTail = pNodeList;
	}
	return true;
}

//every rect of clip region will be clipped by the prc
BOOL
IntersectClipRegion(
	PClipRegion pClipRegion, 
	const PRECT prc
)
{
	PClipRect pNodeList;
	NormalizeRect(prc);
	pNodeList = pClipRegion->pHead;
	while(pNodeList){
		if(!IntersectRect(&pNodeList->rect,&pNodeList->rect,prc))
			SetRectEmpty(&pNodeList->rect);
		pNodeList = pNodeList->pNext;
	}
	GetBoundClipRegion(pClipRegion);
	return true;
}

BOOL
SubtractClipRegion(
	PClipRegion pClipRegion, 
	const PRECT prc
)
{
//	RECT rcDest;
	int i,iCount;
	RECT rcNew[4];
	RECT rcClip;
	RECT rcThis;
	PClipRect pNodeList, pInsertPos,pNewNode;
	if(!pClipRegion)
		return false;
	if(!pClipRegion->pHeap)
		return false;
	memcpy(&rcClip,prc,sizeof(RECT));
	NormalizeRect(&rcClip);
	InflateRect(&rcClip,1,1);
	pNodeList = pClipRegion->pHead;
	if(!IsIntersect(&rcClip, &pClipRegion->rcBound))
		return true;
	while(pNodeList){
		if(!IsIntersect(&pNodeList->rect,&rcClip)){
			pNodeList = pNodeList->pNext;
			continue;
		}
		memcpy(&rcThis,&pNodeList->rect,sizeof(RECT));
		iCount = 0;
		if(rcClip.top >= rcThis.top){
			memcpy(&rcNew[iCount],&rcThis,sizeof(RECT));
			rcNew[iCount].bottom = rcClip.top;
			rcThis.top = rcClip.top;
			iCount ++;
		}

		if(rcClip.bottom <= rcThis.bottom){
			memcpy(&rcNew[iCount],&rcThis,sizeof(RECT));
			rcNew[iCount].top = rcClip.bottom;
			rcThis.bottom = rcClip.bottom;
			iCount ++;
		}

		if(rcClip.left >= rcThis.left){
			memcpy(&rcNew[iCount],&rcThis,sizeof(RECT));
			rcNew[iCount].right = rcClip.left;
			iCount ++;
		}

		if(prc->right <= rcThis.right){
			memcpy(&rcNew[iCount],&rcThis,sizeof(RECT));
			rcNew[iCount].left = rcClip.right;
			iCount ++;
		}
		if(!iCount){
			SetRectEmpty(&pNodeList->rect);
		}
		else{
			pInsertPos = pNodeList->pNext;
			memcpy(&pNodeList->rect,&rcNew[0],sizeof(RECT));
			for(i=1;i<iCount;i++){
				pNewNode = (PClipRect)HeapAlloc(pClipRegion->pHeap);
				memcpy(&pNewNode->rect,&rcNew[i],sizeof(RECT));
				pNodeList->pNext = pNewNode;
				pNodeList = pNodeList->pNext;
			}
			pNodeList->pNext = pInsertPos;
		}
		pNodeList = pNodeList->pNext;
	}
	GetBoundClipRegion(pClipRegion);
	return true;
}

//get intersect between a clipregion and a invalidate region
BOOL  
ClipRgnIntersectInvRgn(
	PClipRegion pRstRegion, 
	const PClipRegion pClipRegion, 
	const PInvalidRegion pInvalidRegion
)
{
	RECT rc;
	PClipRect pClipRect,pNewNode;
	PInvalidRect pInvalidRect;

	if(!pClipRegion)
		return false;
	if(!pInvalidRegion)
		return false;
	if(!pRstRegion)
		return false;
	if(!pRstRegion->pHeap)
		return false;
	if(!IsIntersect(&pClipRegion->rcBound,&pInvalidRegion->rcBound))
		return true;
	pClipRect = pClipRegion->pHead;
	pInvalidRect = pInvalidRegion->pHead;
	while(pClipRect){
		while(pInvalidRect){
			if(IntersectRect(&rc,&(pClipRect->rect),&(pInvalidRect->rect))){
				pNewNode = (PClipRect)HeapAlloc(pRstRegion->pHeap);
				pNewNode->pNext = NULL;
				memcpy(&pNewNode->rect,&rc,sizeof(RECT));
				if(!pRstRegion->pHead){
					pRstRegion->pHead = pRstRegion->pTail = pNewNode;
				}
				else{
					pRstRegion->pTail->pNext = pNewNode;
					pRstRegion->pTail = pNewNode;
				}
			}
			pInvalidRect = pInvalidRect->pNext;
		}
		pInvalidRect = pInvalidRegion->pHead;
		pClipRect = pClipRect->pNext;
	}
	
	GetBoundClipRegion(pRstRegion);
	return true;
}


