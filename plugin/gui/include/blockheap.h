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

#ifndef _BLOCKHEAP_H
#define _BLOCKHEAP_H

#ifdef __cplusplus                     
extern "C" {
#endif

#include "../include/common.h"
#include "../include/blockheap.h"

#define HEAP_BLOCK_FREE			0x00000000
#define HEAP_BLOCK_USED			0x00000001
#define HEAP_BLOCK_OVERFLOW		0x00000002
typedef PrivateHeap* PPrivateHeap;


BOOL 
HeapCreate(
	PPrivateHeap pHeap,
	unsigned long BlockNumber,
	unsigned long BlockSize
);

void*
HeapAlloc(
	PPrivateHeap pHeap
);

void 
HeapFree(
	PPrivateHeap pHeap,
	void* pData
);

void HeapDestroy(
	 PPrivateHeap pHeap
);

#ifdef __cplusplus
}
#endif 


#endif 
