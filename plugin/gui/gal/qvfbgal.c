#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>

#include "../include/common.h"
#include "../include/rect.h"
#include "../include/gal.h"

#define QT_VFB_MOUSE_PIPE "/tmp/.qtvfb_mouse-%d"
#define QT_VFB_KEYBOARD_PIPE "/tmp/.qtvfb_keyboard-%d"
static BOOL Init();
static void  UnInit();
static void  
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

static void  
PutRectRaw(
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

static void  
GetBoxRaw(
	int x,
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
);

static void  
PutBoxRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
);

static void  
PutBoxWithTrCorRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData, 
	COLORREF crColor
);

static void  
EraseRectRaw(
	PRECT lpRc,
	COLORREF color
);


struct GalOps QvfbGalOps={
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

struct GalDev QvfbGalDev={
	&QvfbGalOps,
	0,
	0,
	0
};


struct QVFbHeader
{
    int width;
    int height;
    int depth;
    int linestep;
    int dataoffset;
    RECT update;
    unsigned char dirty;
    int numcols;
    unsigned int clut[256];
};


unsigned char* shmrgn;
static int iFrameWidth;
static int iFrameHeight;
static unsigned char* pFrameBuffer;
static int iLineSize;
static int iBufferSize;
static int iBytesPerPixel = 0;
static int iBytesDataType = 0;
struct QVFbHeader *hdr;
RECT rect;
static void Update();

static BOOL Init(void)
{
    char file [50];
    int display;
    key_t key;
    int shmid;
    display = 0;

    sprintf (file,50,QT_VFB_MOUSE_PIPE, display);
    key = ftok (file, 'b');

    shmid = shmget (key, 0, 0);
    if (shmid != -1)
        shmrgn = (unsigned char *)shmat (shmid, 0, 0);
    else {
        printf("Error Init Qvfb\n");
        return false;
    }
    
    hdr = (struct QVFbHeader *) shmrgn;

    pFrameBuffer = (char *)hdr + hdr->dataoffset;
	if(pFrameBuffer==NULL){
		printerror("Qvfb Buffer Error\n");
		return false;
	}
    iLineSize = hdr->linestep;
	iBytesPerPixel = hdr->depth/8;
	iFrameWidth=hdr->width;
	iFrameHeight=hdr->height;
	if (iBytesPerPixel == 3)
		iBytesDataType = 4;
	else
		iBytesDataType = iBytesPerPixel;
	QvfbGalDev.ScreenHeight=iFrameHeight;
	QvfbGalDev.ScreenWidth=iFrameWidth;
	QvfbGalDev.iBytesDataType=iBytesDataType;
	rect.left=0;
	rect.right=iFrameWidth;
	rect.top=0;
	rect.bottom=iFrameHeight;
    return true;
}

static void Update(){
    memcpy(&hdr->update,&rect,sizeof(RECT));
    hdr->dirty = 1;
}

static void 
UnInit(){
    shmdt (hdr);
}

static void  
PutPixelRaw(
	int x,
	int y, 
	COLORREF color
)
{
	unsigned char* pDest;
	if(y>=iFrameHeight||x>=iFrameWidth)
		return;
	pDest = pFrameBuffer + iLineSize * y + (x * iBytesPerPixel);

	if(iBytesPerPixel == 3){
		*pDest = B(color);
		*(pDest+1) = G(color);
		*(pDest+2) = R(color);
	}
	else	
		*((PCOLORREF)pDest)=color;

	Update();
}

static COLORREF /*inline*/ 
GetPixelRaw(
	int x,
	int y
)
{
	unsigned char* pDest;
	COLORREF crColor;
	pDest = pFrameBuffer + iLineSize * y + (x * iBytesPerPixel);

	if(iBytesPerPixel == 3)
		crColor = 0xff000000 | RGB(*(pDest+2), *(pDest+1), *pDest);
	else
		crColor = *((PCOLORREF)pDest);
	
	return crColor;
}

static void  
PutRectRaw(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData
){
	int x,y;
	unsigned char* pCurPointer;
	unsigned char* pDest;
	int iOrgWidth;
	int iSourceLeftMarge,iTargetLeftMarge;

	iOrgWidth=(lpOrgRc->right-lpOrgRc->left+1)* iBytesDataType;
	iSourceLeftMarge=(lpOutRc->left-lpOrgRc->left)*iBytesDataType;
	iTargetLeftMarge=lpOutRc->left*iBytesPerPixel;

	if(iBytesPerPixel == 3){
		for(y = lpOutRc->top; y <= lpOutRc->bottom; y++){
			pCurPointer=(acoral_u8*)pData + iOrgWidth*(y-lpOrgRc->top) + iSourceLeftMarge;
			pDest=pFrameBuffer + iLineSize*y + iTargetLeftMarge;

			for(x = lpOutRc->left; x <= lpOutRc->right; x++){
				*pDest = B(*pCurPointer);
				*(pDest+1) = G(*pCurPointer);
				*(pDest+2) = R(*pCurPointer);
				pDest +=3;
				pCurPointer += iBytesDataType;
			}
		}
	}
	else{
		for(y=lpOutRc->top;y<=lpOutRc->bottom;y++){
			pCurPointer=(acoral_u8*)pData + iOrgWidth*(y-lpOrgRc->top) + iSourceLeftMarge;
			pDest=pFrameBuffer + iLineSize*y + iTargetLeftMarge;
			memcpy(pDest,pCurPointer,(lpOutRc->right-lpOutRc->left+1)*iBytesPerPixel);
		}
	}
	Update();

}

static void  
PutRectWithTrCorRaw(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData, 
	COLORREF crTrColor
)
{
	int x,y;
	unsigned char* pCurPointer;
	unsigned char* pDest;
	int iOrgWidth;

	iOrgWidth=(lpOrgRc->right-lpOrgRc->left+1)* iBytesDataType;

	if(iBytesPerPixel == 3){
		for(y=lpOutRc->top;y<=lpOutRc->bottom;y++){
			for(x=lpOutRc->left;x<=lpOutRc->right;x++){
				pCurPointer=(acoral_u8*)pData + iOrgWidth*(y-lpOrgRc->top)+(x-lpOrgRc->left)*iBytesDataType;
				if(*((PCOLORREF)pCurPointer)!=crTrColor){
					
					pDest = pFrameBuffer + iLineSize*y + x* iBytesPerPixel;
					
					*pDest = B(*pCurPointer);
					*(pDest+1) = G(*pCurPointer);
					*(pDest+2) = R(*pCurPointer);
					
				}
			}
		}

	}
	else{
		for(y=lpOutRc->top;y<=lpOutRc->bottom;y++){
			for(x=lpOutRc->left;x<=lpOutRc->right;x++){
				pCurPointer=(acoral_u8*)pData + iOrgWidth*(y-lpOrgRc->top)+(x-lpOrgRc->left)*iBytesDataType;
				if(*((PCOLORREF)pCurPointer)!=crTrColor){
					pDest=pFrameBuffer + iLineSize*y + x* iBytesDataType;
					memcpy(pDest,pCurPointer,iBytesDataType);
				}
			}
		}

	}
	Update();
}


static void  
PutLineRaw(
	int xPos,
	int yPos, 
	int iLen, 
	void* pData
)
{

	int i;
	unsigned char* pDest;
	PCOLORREF pSource;

	pSource = (PCOLORREF)pData;

	pDest=pFrameBuffer + iLineSize*yPos + xPos * iBytesPerPixel;
	if(iBytesPerPixel == 3)
		for(i=0;i<iLen;i++){
			*pDest = B(*pSource);
			*(pDest+1) = G(*pSource);
			*(pDest+2) = R(*pSource);

			pDest +=3;
			pSource += 1;
			
		}
	else
		memcpy(pDest,pData,iLen*iBytesPerPixel);

	Update();
}


static void  
GetLineRaw(
	int xPos, 
	int yPos, 
	int iLen, 
	void* pData
)
{
	int i;
	unsigned char* pSrc;
	PCOLORREF pTarget;
	
	pSrc=pFrameBuffer + iLineSize*yPos + xPos * iBytesPerPixel;
	
	if(iBytesPerPixel == 3){
		for(i=0;i<iLen;i++){
			*pTarget = 0xff000000 | RGB(*(pSrc+2), *(pSrc+1),*pSrc);
			pTarget +=1;
			pSrc += iBytesPerPixel;
		}
	}
	else
		memcpy(pData,pSrc,iLen*iBytesPerPixel);
	Update();
}

static void  
EraseRectRaw(
	PRECT lpRc,
	COLORREF color
)
{
	int x,y;
	int iLeftMarge;

	iLeftMarge=lpRc->left;
	for(y=lpRc->top;y<=lpRc->bottom;y++)
		for(x=lpRc->left;x<=lpRc->right;x++)
			PutPixelRaw(x,y,color);

	Update();
}

static void  
GetBoxRaw(
	int x,
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
)
{
	int i,j;
	for(i=0; (i<nHeight)&&(y+i<SCREEN_HEIGHT); i++){
		for(j=0;(j<nWidth)&&(x+j<SCREEN_WIDTH); j++){
			*(pData + i*nWidth + j) = GetPixelRaw(x+j,y+i);
		}
	}
	Update();
}

static void  
PutBoxRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
)
{
	int i,j;
	for(i=0; (i<nHeight) && (y + i < SCREEN_HEIGHT); i++){
		for(j=0; (j<nWidth) && (x + j < SCREEN_WIDTH);j++){
			PutPixelRaw(x+j,y+i,*(pData + i*nWidth + j));
		}
	}
}

static void  
PutBoxWithTrCorRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData, 
	COLORREF crColor
)
{
	int i,j;
	COLORREF crPointColor;
	for(i=0; (i<nHeight) && (y + i < SCREEN_HEIGHT); i++){
		for(j=0; (j<nWidth) && (x + j < SCREEN_WIDTH);j++){
			crPointColor = *(pData + i*nWidth + j);
			if(crPointColor!=crColor){
				PutPixelRaw(x+j,y+i,crPointColor);
			}
		}
	}
	Update();
}

