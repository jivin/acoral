#include "../include/common.h"
#include "../include/lguibase.h"
#include "../include/lgui.h"
#include "../include/app.h"

#define  ID_BUTTON 100
#define  ID_BUTTON2 200
#define  ID_NEWWIN	101
#define  ID_SUB_BUTTON 102
#define  ID_SUB_NEWWIN	103


#define ID_SNAPSHOT	1100
#define ID_BROWSE	1101
#define ID_SEND		1102
#define ID_PAGEUP	1103
#define ID_PAGEDOWN	1104


static void WinMain(void *args);
static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int CreateSubWindow(HWND hWndParent);

struct AppCfg NotePad={
	WinMain,
	"notepad",
	"notepad.bmp"
};

static void WinMain(void *args)
{

	HANDLE hWnd;
	WNDCLASSEX wcex;
	MSG msg;


	if(!InitGUIClient())
		return;


	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(255,255,255));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "notewin";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);

	hWnd = CreateWindow("notewin", "地址簿", WS_MAIN | WS_VISIBLE | 
			WS_THICKBORDER|WS_CAPTION | WS_OKBUTTON | WS_CLOSEBOX,
			 20, 20,160, 200, NULL, NULL, NULL, NULL);


	if (!hWnd)  return;
	ShowWindow(hWnd, true);
	//UpdateWindow(hWnd);


	while (GetMessage(&msg,hWnd)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	TerminateGUIClient();
}

static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HDC hDC;
	static PAINTSTRUCT ps;
	static HWND hWndButton,hWndbutton2,hWndNewWin;
	int x,y;
	switch(message)
	{
		case LMSG_CREATE:
			hWndNewWin = CreateWindow("mledit", "How are you? Are you ok?", WS_CONTROL | WS_BORDER | WS_VISIBLE
				 | WS_VSCROLL,
				10,10, 130 , 80, hWnd, (HMENU)ID_BUTTON2, NULL, NULL);

				
			hWndButton = CreateWindow("button", "关闭", WS_CONTROL  | BS_PUSHBUTTON | WS_BORDER | WS_VISIBLE,
				40, 150, 80 , 20, hWnd, (HMENU)ID_BUTTON, NULL, NULL);

			hWndbutton2 = CreateWindow("button", "子窗口", WS_CONTROL  | BS_PUSHBUTTON | WS_BORDER | WS_VISIBLE,
				40, 120, 80 , 20, hWnd, (HMENU)ID_NEWWIN, NULL, NULL);
			break;
		case LMSG_COMMAND:
			switch(HIWORD(wParam)){
			case BN_CLICKED:

				switch(LOWORD(wParam)){
				case ID_BUTTON:
					DestroyWindow(hWnd);
					break;
				case ID_NEWWIN:
					CreateSubWindow(hWnd);
					break;

				default:
					break;
				}
				break;
			default:
				break;
			}
			break;
		case LMSG_PENDOWN:
			CaptureMouse(hWnd,BYCLIENT);
			break;
			hDC=GetDC(hWnd);
			x=(int)wParam;
			y=(int)lParam;
			ScreenToClient(hWnd,&x,&y);
			SetPixel(hDC,x,y ,RGB(0,0,0));
			ReleaseDC(hWnd,hDC);
			break;
		case LMSG_PENMOVE:
			//printf("address book pen move print\n");
			break;
			hDC=GetDC(hWnd);
			x=(int)wParam;
			y=(int)lParam;
			ScreenToClient(hWnd,&x,&y);
			SetPixel(hDC,x,y ,RGB(0,0,0));
			ReleaseDC(hWnd,hDC);
			break;
		case LMSG_PENUP:
			DisCaptureMouse();
			break;
		case LMSG_PAINT:
			ps.bPaintDirect=false;

			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}
			EndPaint(hWnd, &ps);
			break;
		case LMSG_CLOSE:
			PostQuitMessage(hWnd);//用来退出消息循环
		case LMSG_DESTROY:
			PostQuitMessage(hWnd);//用来退出消息循环
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return true;
}


int CreateSubWindow(HWND hWndParent)
{
	HANDLE hWnd;
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)ChildWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(200,100,20));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "childwin";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);

	hWnd = CreateWindow("childwin", "child", WS_CHILD | WS_VISIBLE | WS_THINBORDER ,
		10, 20,100, 100, hWndParent, NULL, NULL, NULL);
	if (!hWnd)
		return false;

	ShowWindow(hWnd, true);

	return true;
}


static LRESULT ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HDC hDC;
	static PAINTSTRUCT ps;
	static HWND hWndButton;
	int x,y;
	switch(message)
	{
		case LMSG_CREATE:
			hWndButton = CreateWindow("button", "Close", WS_CONTROL  | BS_PUSHBUTTON | WS_BORDER | WS_VISIBLE,
				40, 60, 40 , 20, hWnd, (HMENU)ID_SUB_BUTTON, NULL, NULL);
			break;
		case LMSG_COMMAND:
			switch(HIWORD(wParam)){
			case BN_CLICKED:
				switch(LOWORD(wParam)){

				case ID_SUB_BUTTON:
					DestroyWindow(hWnd);
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}

			break;
		case LMSG_PENDOWN:

			CaptureMouse(hWnd,BYCLIENT);
			break;
		case LMSG_PENMOVE:
			hDC=GetDC(hWnd);
			x=(int)wParam;
			y=(int)lParam;
			ScreenToClient(hWnd,&x,&y);
			SetPixel(hDC,x,y ,RGB(0,0,0));
			ReleaseDC(hWnd,hDC);
			break;
		case LMSG_PENUP:
			DisCaptureMouse();
			break;
		case LMSG_PAINT:
			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}
			EndPaint(hWnd, &ps);
			break;
		case LMSG_DESTROY:
			PostQuitMessage(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return true;
}





