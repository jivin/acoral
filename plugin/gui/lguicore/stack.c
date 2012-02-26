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
#include "../include/stack.h"


void 
InitStack(
	PPointStack* pStack
)
{
	//*pStack=malloc(sizeof(PointStack));
	//memset(*pStack,0,sizeof(PointStack));
	pStack=NULL;

}

void 
PushData(
	PPointStack* pStack,
	void* pData
)
{
	PPointStack pNode;
	pNode=malloc(sizeof(PointStack));
	memset(pNode,0,sizeof(PointStack));
	pNode->pData=pData;
	if(!*pStack){
		*pStack=pNode;
		(*pStack)->pPrev=NULL;
	}
	else{
		pNode->pPrev=*pStack;
		*pStack=pNode;
	}
}

void*
PeekData(
	PPointStack* pStack
)
{
	PPointStack pNode;
	void* pData;
	if(*pStack){
		pNode=*pStack;
		pData = pNode->pData;
		return pData;
	}
	else
		return NULL;
}

void* 
PopData(
	PPointStack* pStack
)
{
	PPointStack pNode;
	void* pData;
	if(*pStack){
		pNode=*pStack;
		*pStack=(*pStack)->pPrev;
		pData=pNode->pData;
		free(pNode);
		return pData;
	}
	else
		return NULL;
}

void 
DestroyStack(
	PPointStack* pStack
)
{
	PPointStack pNode,pPrev;
	if(!(*pStack))
		return;
	pNode=*pStack;
	while(pNode){
		pPrev=pNode->pPrev;
		free(pNode);
		pNode=pPrev;
	}
}


