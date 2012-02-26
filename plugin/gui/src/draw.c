#include "../include/common.h"
#include "../include/lguibase.h"
#include "../include/lgui.h"
#include "../include/app.h"

static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void WinMain(void *args);
#define  ID_BUTTON 100
struct AppCfg Draw={
	WinMain,
	"draw",
	"draw.bmp"
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
	wcex.lpfnWndProc		= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor			= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(255,255,255));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "draw";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);

	hWnd = CreateWindow("draw", "DrawDemo", WS_MAIN | WS_VISIBLE | 
			WS_THICKBORDER|WS_CAPTION | WS_OKBUTTON |WS_CLOSEBOX,
			 1, 20,239, 281, NULL, NULL, NULL, NULL);	


	if (!hWnd)  return ;
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
	int x,y;

	switch(message)
	{
		case LMSG_CREATE:
		
			//hWndButton = CreateWindow("button", "Close Window", WS_CONTROL  |BS_PUSHBUTTON | WS_BORDER | WS_VISIBLE,
			//	0,0, 90 , 20, hWnd, (HMENU)ID_BUTTON, NULL, NULL);
			break;
		case LMSG_COMMAND:
			switch(HIWORD(wParam)){
			case BN_CLICKED://桌面图标的点击事件处理

				switch(LOWORD(wParam)){

				case ID_BUTTON:
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
		case LMSG_PENDOWN://桌面的mouse事件
			CaptureMouse(hWnd,BYCLIENT);
			break;
		case LMSG_PENMOVE:
			hDC=GetDC(hWnd);
			x=(int)wParam;
			y=(int)lParam;
//			printf("%d,%d\n",x,y);
			ScreenToClient(hWnd,&x,&y);
			SetPixel(hDC,x,y ,RGB(255,0,0));
			ReleaseDC(hWnd,hDC);
			break;
		case LMSG_PENUP://桌面的mouse事件
		DisCaptureMouse();
			break;
		case LMSG_PAINT:
		{
			POINT MyPoint;
			HGDIOBJ  brush;
			HGDIOBJ	 pen;
			POINT points[8];
			POINT pp[3];
			POINT p;
			
			points[0].x = 3;
			points[0].y = -33;
			points[1].x = 13;
			points[1].y = 150;
			points[2].x = 100;
			points[2].y = 150;
			points[3].x = 100;
			points[3].y = 300;
			points[4].x = 250;
			points[4].y = 150;
			points[5].x = 150;
			points[5].y = 150;
			points[6].x = 250;
			points[6].y = 50;
			points[7].x = 100;
			points[7].y = 100;
			
			pp[0].x = 50;
			pp[0].y = -10;
			pp[1].x = 0;
			pp[1].y = 50;
			pp[2].x = 80;
			pp[2].y = 50;

			
			ps.bPaintDirect=false;

			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}
			MyPoint.x = 10;
			MyPoint.y =20;
			DrawTextAngle(hDC,"ABC",3,&MyPoint,0.0);
			MyPoint.x = 80;
			MyPoint.y =60;			
			DrawTextAngle(hDC,"江上挨打aaaa",1,&MyPoint,180.0);
//BOOL GUIAPI DrawTextAngle(HDC hDC,char* lpString,int nCount,POINT* lpPoint,double fDirection)
			
			#if 1
			brush = CreateSolidBrush(RGB(23,43,133));
			pen = CreatePen(PS_SOLID,1,RGB(230,0,255));
			SelectObject(hDC,brush);
			SelectObject(hDC,pen);
			DeleteObject(brush);
			DeleteObject(pen);	
			
			SetPixel(hDC,-60,0,RGB(0,0,0));
			MoveToEx(hDC,-10,-10,&p);
			LineTo(hDC,10,10);
			Rectangle(hDC, -10,-20,200,200);
			Ellipse(hDC,-20,-20,80,100);
			Circle(hDC,20,20,40);
			Polygon(hDC,points,8);
			Rectangle(hDC,-10,20,130,34);
			//printf("aaaa\n");
			//SetPixel(hDC,-10,40,RGB(0,0,0));
			#endif
			#if 1
			//ShowBitmap(hDC,0,0,"/usr/local/lgui/temp/record.bmp");
			Rectangle(hDC, 10,20,290,200);
			brush = CreateSolidBrush(RGB(23,43,133));
			pen = CreatePen(PS_SOLID,1,RGB(230,0,255));
			SelectObject(hDC,brush);
			SelectObject(hDC,pen);
			DeleteObject(brush);
			DeleteObject(pen);	
			brush = CreateSolidBrush(RGB(250,200,0));
			SelectObject(hDC,brush);
			DeleteObject(brush);
			Rectangle(hDC,-10,20,130,34);
			//break;
			//brush = CreateSolidBrush(RGB(48,10,0));
			//SelectObject(hDC,brush);
			//DeleteObject(brush);			
			Polygon(hDC,points,8);
			//Polygon(hDC,pp,3);
			//Polyline(hDC,points,8);
			brush = CreateSolidBrush(RGB(0,200,0));
			SelectObject(hDC,brush);
			DeleteObject(brush);
			Ellipse(hDC,20,20,80,100);
			Circle(hDC,50,20,10);
			brush = CreateSolidBrush(RGB(0,24,230));
			SelectObject(hDC,brush);
			//DeleteObject(pen);//modify,delete,trash delete
			Circle(hDC,10,10,30);
			Circle(hDC,200,200,50);
			
			#endif
			
			EndPaint(hWnd, &ps);
			break;
		}
		case LMSG_CLOSE:
			PostQuitMessage(hWnd);
			break;		
		case LMSG_DESTROY:
			PostQuitMessage(hWnd);//用来退出消息循环
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return true;
}
