#include "../include/common.h"
#include "../include/lguibase.h"
#include "../include/lgui.h"
#include "../include/app.h"

static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void WinMain(void *args);
#define  ID_BUTTON 100
struct AppCfg Art={
	WinMain,
	"art",
	"art.bmp"
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
	wcex.hbrBackground	= CreateSolidBrush(RGB(147,222,252));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "artwin";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);

	hWnd = CreateWindow("artwin", "电脑美术", WS_MAIN | WS_VISIBLE | 
			WS_THINBORDER|WS_CAPTION | WS_OKBUTTON |WS_CLOSEBOX,
			 1, 20,240, 200, NULL, NULL, NULL, NULL);	



	if (!hWnd)  return;
	ShowWindow(hWnd, true);

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
	
	static HWND hWndCtr1,hWndCtr2;

	switch(message)
	{
		case LMSG_CREATE:
			hWndCtr1 = CreateWindow("static", "关闭", WS_CONTROL  | BS_PUSHBUTTON | WS_BORDER | WS_VISIBLE | BS_CHECKBOX,
				40, 150, 80 , 20, hWnd, (HMENU)0, NULL, NULL);
			hWndCtr2 = CreateWindow("listbox", "How are you? Are you ok?", WS_CONTROL | WS_BORDER | WS_VISIBLE
				 ,
				10,10, 130 , 80, hWnd, (HMENU)1, NULL, NULL);
			break;
		case LMSG_COMMAND:
			break;
		case LMSG_PENDOWN:/*桌面的mouse事件*/
			CaptureMouse(hWnd,BYCLIENT);
			break;
		case LMSG_PENMOVE:
			break;
		case LMSG_PENUP:
			DisCaptureMouse();
			break;
		case LMSG_TIMER:
			break;
		case LMSG_PAINT:
		{
			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}
			EndPaint(hWnd, &ps);
			break;
		}
		case LMSG_CLOSE:
			PostQuitMessage(hWnd);
			break;		
		case LMSG_DESTROY:
			PostQuitMessage(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return true;
}
