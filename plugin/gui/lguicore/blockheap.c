/*
	Copyright (C) 2004-2005 Li Yudong
	The idea comes from MiniGUI
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


BOOL 
HeapCreate(
	PPrivateHeap pHeap,
	unsigned long BlockNumber,
	unsigned long BlockSize
)
{
	pHeap->num_per_pool=100;
	pHeap->size=BlockSize;
//	pHeap->type=ACORAL_RES_GUI;
	pHeap->max_pools=BlockNumber/100;
	pHeap->name="GuiHeap";
	pHeap->api=NULL;
	acoral_obj_pool_init(pHeap);
	return true;
}

void*
HeapAlloc(
	PPrivateHeap pHeap
)
{
	return (void *)acoral_get_obj(pHeap);
}

void 
HeapFree(
	PPrivateHeap pHeap,
	void* pData
)
{
	acoral_sr cpu_sr;
	HAL_ENTER_CRITICAL();
	acoral_free_obj((acoral_res_t *)pData);
	HAL_EXIT_CRITICAL();
}





void HeapDestroy(PPrivateHeap pHeap)
{
}


