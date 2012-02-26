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

#ifndef _LGUIBASE_H_
#define _LGUIBASE_H_
#include<resource.h>
#ifdef __cplusplus
extern "C" {
#endif



//==========================================================================
//									BMP								
//==========================================================================
/*
typedef struct tagBMPFILEHEADER
{
	UINT16 bfType;      	//file type the value is 'BM'
	UINT32 bfSize;      	//size of the file
	UINT16 bfReserved1; 	//reserved 
	UINT16 bfReserved2; 	//reserved
	UINT32 bfOffbits;   	//from start  position of file to the start position of image data
} BMPFILEHEADER;

//bmp image information head
typedef struct tagBMPINFOHEADER
{
	UINT32 biSize;          		//size of this structure ，Windows:0x28; OS/2:0x0C
	UINT32 biWidth;			//image width（Pixel)
	UINT32 biHeight;			//image height（Pixel)
	UINT16 biPlanes;			//color planes，always 1
	UINT16 biBitCount;		//bit per point  1-->2color ；4-->16color；8-->256color；24-->true color；
	UINT32 biCompression;   	//BI_RGB/BI_RLE8/BI_RLE4;
	UINT32 biSizeImage;     	//size of image（in byte）
	UINT32 biXPelsPerMeter; 	//horizon resolution（point per meter）
	UINT32 biYPelsPerMeter; 	//vertical resolution（point per meter）
	UINT32 biClrUsed;		//color number used indeed （if 0,color number will be set by iBitCount）
	UINT32 biClrImportant;  	//coloe number important，（if 0,means all colors in pallette are important)
} BMPINFOHEADER;
*/


//==========================================================================
//									HDC								
//==========================================================================

#define SYS_DEFAULT_FONT	FONT_14_14//default font

//#define GDI_SYS				8 /* pre-defined GDI object  */
//#define GDI_USR				0  /* user defined GDI object */


/* Pen Styles */
#define PS_NULL             	0x00000010
#define PS_SOLID            	0x00000011
#define PS_DASH             	0x00000012	/* -------  */
#define PS_DOT              	0x00000013	/* .......  */
#define PS_DASHDOT         0x00000014	/* _._._._  */
#define PS_DASHDOTDOT	0x00000015	/* _.._.._  */

/* Brush Styles */
#define BS_NULL			0x00000020
#define BS_SOLID			0x00000021
#define BS_HATCHED		0x00000022
#define BS_AGAINST		0x00000023


/* Hatch Styles */
#define HS_HORIZONTAL       0	/* ----- */
#define HS_VERTICAL         1	/* ||||| */
#define HS_FDIAGONAL        2	/* \\\\\ */
#define HS_BDIAGONAL        3	/* ///// */
#define HS_CROSS            4	/* +++++ */
#define HS_DIAGCROSS        5	/* xxxxx */


//System Pre-defined GDI

#define WHITE_PEN			0x00000000
#define LTGRAY_PEN			0x00000001
#define GRAY_PEN			0x00000002
#define DKGRAY_PEN			0x00000003
#define BLACK_PEN			0x00000004
#define NULL_PEN			0x00000005

#define WHITE_BRUSH		0x00000006
#define LTGRAY_BRUSH		0x00000007
#define GRAY_BRUSH			0x00000008
#define DKGRAY_BRUSH		0x00000009
#define BLACK_BRUSH			0x0000000A
#define NULL_BRUSH			0x0000000B

#define FONT_16_16			0x0000000C
#define FONT_14_14			0x0000000D
#define FONT_12_12			0x0000000E

//GDI TYPE
#define GDI_PEN				0x00000000
#define GDI_BRUSH			0x00000001
#define GDI_FONT			0x00000002
#define GDI_BITMAP			0x00000003
#define GDI_RES				0x00000004




//字库结构
typedef struct tagFONTLIBHEADER{
	int iSize;				//size of this struction
	int iAscWidth;		//width of Ascii character 
	int iAscHeight;		//height of Ascii character
	int iAscBytes;		//bytes of a Ascii character used
	int iChnWidth;		//width of chinese character
	int iChnHeight;		//height of chinese character
	int iChnBytes;		//bytes of a chinese character used 
	int iAscOffset;		//offset address of Ascii character(from the end of this structure)
	int iChnSymOffset;	//offset address of chinese symbol (from the end of this structure)
	int iChnOffset;		//offset address of chinese character(from the end of this structure)
} FONTLIBHEADER;
typedef FONTLIBHEADER* PFONTLIBHEADER;



typedef unsigned int GDITYPE;


typedef struct tagPEN{
	GDITYPE		iGdiType;
	int			iPenStyle;		// pen style
	int			iPenWidth;		// pen width
	COLORREF	crPenColor;		// pen color
} PEN;
typedef PEN* PPEN;

typedef struct tagBRUSH{
	GDITYPE		iGdiType;
	int			iBrushStyle;		// brush style
	int			iHatchStyle;		// hatch style
	COLORREF	crBrushColor;	// color value
} BRUSH;
typedef BRUSH* PBRUSH;

typedef struct tagFONT{
	GDITYPE		iGdiType;
	int iFontStyle;				//font type
	int iOffset;					//offset address in the shared memory
	FONTLIBHEADER FontLibHeader;//font library head structure 
} FONT;
typedef FONT* PFONT;

typedef struct tagBITMAP { 
	GDITYPE		iGdiType;		//
	LONG		bmType;		//Specifies the bitmap type. This member must be zero
	LONG		bmWidth;		//Specifies the width, in pixels, of the bitmap
	LONG		bmHeight;		//Specifies the height, in pixels, of the bitmap
	LONG		bmWidthBytes;	//Specifies the number of bytes in each scan line. This value must be divisible by 2
	WORD		bmPlanes;		//Specifies the count of color planes
	WORD		bmBitsPixel;		//Specifies the number of bits required to indicate the color of a pixel
	LPVOID		bmBits;			//Pointer to the location of the bit values for the bitmap
} BITMAP; 
typedef BITMAP* PBITMAP; 


#define DC_TYPE_MEM		0x00000000	//device context type: memory
#define DC_TYPE_WIN		0x00000001	//device context type: window

#define DC_COORDTYPE_WINDOW	0x00000000		//device context coord type: window
#define DC_COORDTYPE_CLIENT	0x00000001		//device contect coord type: client

typedef struct tagDC{
	int				iType;			//type(window/memory)
	int				iCoordType;		//CoordType(Window/Client);
	COLORREF		crTextBkColor;	//text background color
	COLORREF		crTextColor;	//text color
	BOOL			isTextBkTrans;	//the text is transparent ?
	POINT			point;			//current point 

	HPEN			hPen;
	HBRUSH			hBrush;
	HFONT			hFont;
	HBITMAP			hBitmap;
	void*			pData;			//memory device context only 
	HWND			hWnd;			//handle of window
} DC;
typedef DC* HDC;


HDC	GUIAPI 	
GetDC(
	 HWND hWnd
);

HDC GUIAPI
GetWindowDC(
	HWND hWnd
);

int	GUIAPI 	
ReleaseDC(
	HWND hWnd, 
	HDC hDC
);


HDC	GUIAPI
CreateCompatibleDC(
	HDC hDCRefer
);


BOOL GUIAPI 
CreateStockObject();


HGDIOBJ	GUIAPI 
GetStockObject(
	int iStyle
);


HPEN GUIAPI 
CreatePen(
	int iPenStyle, 
	int iWidth, 
	COLORREF crColor
);


HBRUSH GUIAPI 
CreateSolidBrush(
	COLORREF crColor
);


HBRUSH GUIAPI 
CreateHatchBrush(
	int iStyle, 
	COLORREF clrref
);


HBRUSH GUIAPI 
CreateAgainstBrush();


HBITMAP GUIAPI 
CreateBitmap(
	char* pFile
);

BOOL GUIAPI 
SelectObject(
	HDC hDC,
	HGDIOBJ hGdiObj
);


void GUIAPI 
DeleteObject(
	HGDIOBJ  hObject
);


//==========================================================================
//									CARET								
//==========================================================================

BOOL GUIAPI 
CreateCaret (
	HWND hWnd, 
	PBITMAP pBitmap, 
	int nWidth, 
	int nHeight
);

BOOL GUIAPI 
DestroyCaret(
	HWND hWnd
);

UINT GUIAPI 
GetCaretBlinkTime(
	HWND hWnd
);

BOOL GUIAPI 
SetCaretBlinkTime(
	HWND hWnd, 
	UINT uTime
);


BOOL GUIAPI 
ShowCaret(
	HWND hWnd
);

BOOL GUIAPI 
HideCaret(
	HWND hWnd
);


BOOL GUIAPI
SetCaretPos(
	HWND hWnd, 
	int x, 
	int y
);

BOOL GUIAPI 
ChangeCaretSize(
	HWND hWnd, 
	int newWidth, 
	int newHeight
);

// Get Caret Position
BOOL GUIAPI 
GetCaretPos(
	HWND hWnd, 
	PPOINT pPt
);

//==========================================================================
//									GDI								
//==========================================================================
COLORREF GUIAPI 
SetBkColor(
	HDC hDC, 
	COLORREF crColor
);


COLORREF GUIAPI 
SetTextColor(
	HDC hDC, 
	COLORREF crColor
);


COLORREF GUIAPI /*inline*/ 
SetPixel(
	HDC hDC, 
	int x, 
	int y, 
	COLORREF crColor
);


COLORREF GUIAPI /*inline*/ 
GetPixel(
	HDC hDC, 
	int x, 
	int y
);


BOOL GUIAPI 
MoveToEx(
	HDC hDC, 
	int X, 
	int Y, 
	LPPOINT lpPoint
);


BOOL GUIAPI 
LineTo(
	HDC hDC, 
	int nXEnd, 
	int nYEnd
);



BOOL GUIAPI 
Rectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect
);


BOOL GUIAPI
FillRect(
	HDC hDC, 
	RECT *lprc
	,HBRUSH hbr
);//exclude border



BOOL GUIAPI 
FillRectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect,
	COLORREF crColor,
	COLORREF crFillColor
);


BOOL GUIAPI 
Circle(
	 HDC hDC, 
	 int xCenter, 
	 int yCenter, 
	 int radius
);


BOOL GUIAPI 
Ellipse(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect
);


BOOL GUIAPI 
Polyline(
	HDC hDC, 
	POINT *lpPoints, 
	int nCount
);


BOOL GUIAPI 
Polygon(
	HDC hDC, 
	POINT *lpPoints,
	int nCount
);


//==========================================================================
//									BUTTON								
//==========================================================================

#define BS_ANIMATIONBUTTON  	0x80000000L
#define BS_TEXT         			0x40000000L// PushButton
#define BS_BITMAP      			0x20000000L// Specifies that the button displays a bitmap.
#define BS_GROUP				0x10000000L// RadioBox group

#define BS_TYPEMASK         		0x0f000000L
#define BS_PUSHBUTTON       		0x00000000L
#define BS_CHECKBOX         		0x01000000L
#define BS_AUTOCHECKBOX		0x02000000L//check state automatically toggles between checked and unchecked
#define BS_RADIOBUTTON      		0x03000000L
#define BS_AUTORADIOBUTTON  	0x04000000L//automatically sets the button's check state to checked and sets the check state for all other buttons in the same group to unchecked.

#define BS_STATUSMASK			0x00f00000L
#define BS_BUTTON_PRESSDOWN	0x00100000L
#define BS_BUTTON_CHECKED		0x00100000L


#define CHECKED					0X00000000L
#define UNCHECKED				0x00000001L


//==========================================================================
//									LISTBOX								
//==========================================================================


#define LBS_TYPEMASK        	0xf0000000L
#define LBS_NOTIFY			0x10000000L
#define LBS_SORT			0x20000000L
#define LBS_VSCROLL			0x40000000L
#define LBS_STANDARD		0x70000000L


#define LBIF_NORMAL         	0x00100000L
#define LBIF_SELECTED       	0x00200000L
#define LBIF_CHECKED        	0x00300000L


//==========================================================================
//									SCROLLBAR							
//==========================================================================

#define SB_CTL  0
#define SB_HORZ 1
#define SB_VERT 2

#define SB_LINEUP				0x0001
#define SB_LINEDOWN			0x0002
#define SB_PAGEUP				0x0003
#define SB_PAGEDOWN			0x0004
#define SB_THUMBTRACK			0x0005
#define SB_LINELEFT				SB_LINEUP
#define SB_LINERIGHT			SB_LINEDOWN

#define SIF_ALL			0x000000ff
#define SIF_RANGE		0x00000001
#define SIF_PAGE		0x00000002
#define SIF_POS			0x00000004
#define SIF_TRACKPOS	0x00000008



//==========================================================================
//									INITGUI							
//==========================================================================

BOOL GUIAPI 
InitGUIServer();


BOOL GUIAPI 
InitGUIClient();


void GUIAPI 
TerminateGUIServer();


void GUIAPI 
TerminateGUIClient();


//==========================================================================
//									MESSAGE							
//==========================================================================


typedef struct _MSG {     // msg 
	HWND	hWnd;     
	UINT	message; 
	WPARAM	wParam; 
	LPARAM	lParam; 
	void*	pData;
} MSG;
typedef MSG* PMSG;



typedef struct _NtfMsgLink
{
	acoral_res_t res;
	MSG		msg;
	struct _NtfMsgLink* pNext;
} NtfMsgLink;
typedef NtfMsgLink* PNtfMsgLink;


typedef struct _SyncMsgLink
{
	MSG		msg;
	int		iRetValue;
	sem_t	sem;
	struct _SyncMsgLink* pNext;
} SyncMsgLink;

typedef SyncMsgLink* PSyncMsgLink;


typedef struct _WndMailBox
{
	MSG			msg[SIZE_WND_MAILBOX];
	int			iReadPos;
	int			iWritePos;
} WndMailBox;
typedef WndMailBox* PWndMailBox;



typedef struct _MsgQueue
{
	mutex_t 	mutex;       
	sem_t			sem;  

	DWORD			dwState;              
         
	PSyncMsgLink		pHeadSyncMsg;
	PSyncMsgLink		pTailSyncMsg;

	PNtfMsgLink		pHeadNtfMsg;
	PNtfMsgLink		pTailNtfMsg;

	PNtfMsgLink		pHeadPntMsg;
	PNtfMsgLink		pTailPntMsg;

	WndMailBox		wndMailBox;

    	HWND			TimerOwner[NUM_WIN_TIMERS];        
    	int				TimerID[NUM_WIN_TIMERS];            
    	WORD			TimerMask;						
} 	MsgQueue;
typedef MsgQueue* 	PMsgQueue;



BOOL GUIAPI
GetMessage(
	PMSG pMsg,
	HWND hWnd
);


BOOL GUIAPI
PostMessage(
	HWND hWnd,
	int iMsg,
	WPARAM wParam,
	LPARAM lParam
);

BOOL GUIAPI
PostQuitMessage(
	HWND hWnd
);

int GUIAPI
SendMessage(
	HWND hWnd,
	int iMsg,
	WPARAM wParam,
	LPARAM lParam
);


BOOL GUIAPI
SendNotifyMessage(
	HWND hWnd,
	int iMsg,
	WPARAM wParam,
	LPARAM lParam
);


BOOL GUIAPI
TranslateMessage(
	PMSG pMsg
);


int GUIAPI
DispatchMessage(
	PMSG pMsg
);


BOOL GUIAPI
NotifyParent(
	HWND hWnd,
	int iMsg
);



//==========================================================================
//									REGCLASS						
//==========================================================================
typedef struct tagWNDCLASSEX {
	UINT					cbSize;				//size of this structure
	UINT					style;				//style of application window
	WNDPROC					lpfnWndProc;		//callback function pointer
	HINSTANCE				hInstance;			//
	HICON					hIcon;				//
	HICON					hIconSm;			//
	HCURSOR					hCursor;			//
	HBRUSH					hbrBackground;		//
	char*					lpszMenuName;		//
	char*					lpszClassName;		//
	DWORD					cbClsExtra;			//reserved
	DWORD					cbWndExtra;			//reserved
	struct tagWNDCLASSEX*	pNext;
} WNDCLASSEX;
typedef	WNDCLASSEX* PWNDCLASSEX;

 

BOOL GUIAPI
RegisterClass(
	WNDCLASSEX* lpwcx
);


//get register class by class name
PWNDCLASSEX GUIAPI
GetRegClass(
	char* lpszClassName
);



//==========================================================================
//									SYSTEM						
//==========================================================================

int GUIAPI 
GetSystemMetrics(
	int nIndex
);

int GUIAPI 
GetSysCharWidth();

int GUIAPI 
GetSysCCharWidth();


int GUIAPI 
GetSysCharHeight();

//==========================================================================
//									TEXT						
//==========================================================================

BOOL GUIAPI 
DrawText(
	HDC hDC,
	char* lpString,
	int nCount,
	LPRECT lpRect,
	UINT uFormat
);


BOOL GUIAPI 
DrawTextAngle(
	HDC hDC,
	char* lpString,
	int nCount,POINT* 
	lpPoint,double fDirection
);
//==========================================================================
//									TIMER						
//==========================================================================
/*BOOL  GUIAPI 
SetTimer(
	HWND hWnd,
	int id,
	int speed
);

BOOL GUIAPI 
KillTimer(
	HWND hWnd,
	int id
);
*/
BOOL GUIAPI 
ResetTimer(
	HWND hWnd,
	int id, 
	int speed
);


void GUIAPI 
StartTimer();

void GUIAPI 
EndTimer();

//==========================================================================
//									WINBASE						
//==========================================================================
typedef struct tagPAINTSTRUCT {
	BOOL bPaintDirect;
    HDC  hdc;
    BOOL fErase;
    RECT rcPaint;
    BOOL fRestore;
    BOOL fIncUpdate;
    BYTE rgbReserved[32];
} PAINTSTRUCT;
typedef PAINTSTRUCT* LPPAINTSTRUCT;

#define BYCLIENT  0
#define BYVSCROLL 1
#define BYHSCROLL 2
#define BYOKBUTTON	3
#define BYCLOSEBOX	4


BOOL GUIAPI 
InvalidateRect(
	const HWND hWnd, 
	const PRECT lpRect, 
	BOOL bErase
);


HDC GUIAPI 
BeginPaint(
	const HWND hWnd,
	LPPAINTSTRUCT lpPaint
);


BOOL GUIAPI 
EndPaint(
	const HWND hWnd,
	LPPAINTSTRUCT lpPaint
);


void GUIAPI 
ScreenToWindow(
	const HWND hWnd,
	int* px,
	int* py
);


void GUIAPI 
ScreenToWindowRect(
	const HWND hWnd,
	LPRECT lprc
);


void GUIAPI 
WindowToScreen(
	const HWND hWnd,
	int* px, 
	int* py
);


void GUIAPI 
WindowToScreenRect(
	const HWND hWnd,
	LPRECT lprc
);


void GUIAPI 
WindowToClient(
	const HWND hWnd,
	int* px,
	int* py
);


void GUIAPI 
WindowToClientRect(
	const HWND hWnd,
	LPRECT lprc
);


void GUIAPI 
ClientToWindow(
	const HWND hWnd, 
	int* px, 
	int* py
);


void GUIAPI 
ClientToWindowRect(
	const HWND hWnd,
	LPRECT lprc
);


void GUIAPI 
ScreenToClient(
	const HWND hWnd,
	int* px,
	int* py
);


void GUIAPI 
ScreenToClientRect(
	const HWND hWnd,
	LPRECT lprc
);


void GUIAPI 
ClientToScreen(
	const HWND hWnd,
	int* px,
	int* py
);


void GUIAPI 
ClientToScreenRect(
	const HWND hWnd,
	LPRECT lprc
);


BOOL GUIAPI 
GetWindowRect(
	HWND hWnd,
	LPRECT lpRect
);


BOOL GUIAPI 
GetClientRect(
	const HWND hWnd, 
	LPRECT lpRect
);


void GUIAPI 
SetFocus(
	const HWND hWnd
);


//==========================================================================
//									WINDOW					
//==========================================================================

#define WS_TYPEMASK			0x000f0000L
#define WS_DESKTOP				0x00010000L
#define WS_MAIN					0x00020000L
#define WS_CHILD           			0x00030000L
#define WS_CONTROL          		0x00040000L
#define WS_MENUWIN          		0x00050000L
#define WS_IMEWIN           		0x00060000L
#define WS_SKBWIN           		0x00070000L
//window style
#define WS_STYLEMASK        		0x0000ff00L
#define WS_BORDER				0x00000100L
#define WS_CAPTION				0x00000200L
#define WS_CLOSEBOX			0x00000400L
#define WS_OVERLAPPEDWINDOW	0x00000700L
#define WS_VSCROLL				0x00000800L
#define WS_HSCROLL				0x00001000L
#define WS_THINBORDER			0x00002000L
#define WS_THICKBORDER			0x00004000L
#define WS_OKBUTTON			0x00008000L


//window status
#define WS_STATUSMASK			0x000000ffL
#define WS_VISIBLE          			0x00000001L
#define WS_FOCUS	        		0x00000002L
#define WS_DISABLE				0x00000004L
#define WS_ACTIVE	        		0x00000008L
#define WS_TRANS				0x00000010L

#define WS_OKBUTTONPRESSED	0x00000020L
#define WS_CLOSEBOXPRESSED	0x00000040L




HWND GUIAPI 
CreateWindow(
  char* lpClassName,	// pointer to registered class name
  char* lpWindowName,	// pointer to window name
  DWORD dwStyle,        // window style & window statue
  int x,                // horizontal position of window
  int y,                // vertical position of window
  int nWidth,           // window width
  int nHeight,          // window height
  HWND hWndParent,      // handle to parent or owner window
  HMENU hMenu,          // handle to menu or child-window identifier
  HANDLE hInstance,    	// handle to application instance
  LPVOID lpParam        // pointer to window-creation data
);


void GUIAPI 
DestroyWindow(
	HWND hWnd
);


BOOL GUIAPI 
ShowWindow(
	HWND hWnd, 
	int nCmdShow
);


BOOL GUIAPI 
UpdateWindow(
	const HWND hWnd
);


BOOL GUIAPI 
MoveWindow(
	HWND hWnd, 
	int x, 
	int y
);


BOOL GUIAPI 
ResizeWindow(
	HWND hWnd,
	LPRECT lpRect
);


//==========================================================================
//									WINNC					
//==========================================================================
//scrollbarstatus
#define SS_DISABLED		0x00000001

typedef struct tagSCROLLINFO {// si
	DWORD dwStatus;//  Enalble/Disable
	UINT cbSize;
	UINT fMask;
	int  nMin;
	int  nMax;
	UINT nPage;
	int  nPos;
	int  nTrackPos;
}   SCROLLINFO;
typedef SCROLLINFO *LPSCROLLINFO;

typedef struct tagSCROLLCURSTATE {
	int iSliderBarVal;//left or top value
	int iSliderBarLen;//length(heigh or width)
	int iClickPos;
}	SCROLLCURSTATE;
typedef SCROLLCURSTATE* LPSCROLLCURSTATE;


int  GUIAPI 
SetScrollInfo(
	HWND hwnd, 
	int fnBar, 
	LPSCROLLINFO lpsi,
	BOOL fRedraw
);


int  GUIAPI 
SetScrollPos(
	HWND hWnd,
	int nBar,
	int nPos,
	BOOL bRedraw
);


BOOL GUIAPI 
SetScrollRange(
	HWND hWnd,
	int nBar,
	int nMinPos,
	int nMaxPos,
	BOOL bRedraw
);


int  GUIAPI 
GetScrollInfo (
	HWND hWnd, 
	LPSCROLLINFO lpsi
);


BOOL GUIAPI 
EnableScrollBar(
	HWND hWnd, 
	int iSBar, 
	BOOL bEnable
);



#ifdef __cplusplus
}
#endif

#endif



