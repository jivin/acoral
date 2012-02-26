#include "../include/common.h"
#include "../include/lguibase.h"
#include "../include/lgui.h"
#include "../include/app.h"


static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void WinMain(void *args);

#define  ID_BUTTON 100
#define  WINDOW_WIDTH  240
#define  WINDOW_LENGTH 280
#define  BRICK_WIDTH  20
#define  BRICK_LENGTH 40
#define  BRICK_NUM_IN_X 6
#define  BRICK_NUM_IN_Y 4
#define  BALL_RADIUS  10
#define  BORDER_WIDTH 10
#define  BOARD_WIDTH  10
#define  BOARD_LENGTH 50
#define  BALL_STEP_NUM 4

int moveX;//Ð¡ÇòÒÆ¶¯x·œÏòÏòÁ¿
int moveY;//Ð¡ÇòÒÆ¶¯y·œÏòÏòÁ¿
int speed;//µ²°åÒÆ¶¯ËÙ¶È
POINT ball;//Ð¡ÇòÖÐÐÄ×ø±ê
POINT board;//µ²°å×óÉÏœÇ×ø±ê
int brick[BRICK_NUM_IN_Y][BRICK_NUM_IN_X];//ÅÐ¶Ï×©¿éÊÇ·ñŽæÔÚµÄŸØÕó£¬1ÎªŽæÔÚ
bool is_run;

struct AppCfg PingPang={
	WinMain,
	"PingPang",
	"draw.bmp"
};

void GameInit()
{
    int m=0,n=0;
	moveX=BALL_STEP_NUM;
	moveY=-BALL_STEP_NUM;
	speed=6;
	ball.x=115;
	ball.y=187;
	board.x=100;
	board.y=200;
	
	for(m=0;m<BRICK_NUM_IN_Y;m++)
		for(n=0;n<BRICK_NUM_IN_X;n++)
			brick[m][n]=1;
	is_run=true;
}

int GameAction()
{ 
    int result;
	ball.x+=moveX;
	ball.y+=moveY;
	if(ball.y>WINDOW_LENGTH-BALL_RADIUS-BORDER_WIDTH){
		is_run=false;
		return 0;
	}
	result = Collection_Check();
	switch(result)
	{
		case 1:
			moveY=BALL_STEP_NUM;break;
		case 2:
			moveY=-BALL_STEP_NUM;break;
		case 3:
			moveX=-BALL_STEP_NUM;break;
		case 4:
			moveX=BALL_STEP_NUM;break;
		default:
			break;
	}
	return 1;
}

int Collection_Check()
{ 
    int m=0,n=0;
	if(ball.x<board.x+BOARD_LENGTH && ball.x>board.x && ball.y<board.y+BOARD_WIDTH-BALL_RADIUS && ball.y>board.y-BALL_RADIUS)
		return 2;
	if(ball.x>WINDOW_WIDTH-BALL_RADIUS-BORDER_WIDTH)
		return 3;
	if(ball.x<BALL_RADIUS)
		return 4;
	if(ball.y<BALL_RADIUS)
		return 1;
	
	for(m=BRICK_NUM_IN_Y-1;m>=0;m--)
		for(n=BRICK_NUM_IN_X-1;n>=0;n--)
		{
			if(brick[m][n]==1&&ball.x<n*BRICK_LENGTH+BRICK_LENGTH-BALL_RADIUS && ball.x>n*BRICK_LENGTH+BALL_RADIUS)
				if(ball.y<m*BRICK_WIDTH+BRICK_WIDTH+BALL_RADIUS && ball.y> m*BRICK_WIDTH+BALL_RADIUS)
				{
					brick[m][n]=0;
					return 1;
				}
		} 
	return 0;
}

void TimerAction(HWND hWnd){
    if(is_run){
	  	if(GameAction())
			InvalidateRect(hWnd,NULL,true);
		else
			KillTimer(hWnd,1);
	}
}

//plus end--------------------
static void WinMain(void *args)
{

    HANDLE hWnd;
    WNDCLASSEX wcex;
    MSG msg;
    if(!InitGUIClient())
        return;

    wcex.cbSize            = sizeof(WNDCLASSEX);
    wcex.style            = 0;
    wcex.lpfnWndProc        = (WNDPROC)WndProc;
    wcex.cbClsExtra        = 0;
    wcex.cbWndExtra        = 0;
    wcex.hInstance        = 0;
    wcex.hIcon            = 0;
    wcex.hCursor            = 0;
    wcex.hbrBackground    = CreateSolidBrush(RGB(255,255,255));
    wcex.lpszMenuName    = NULL;
    wcex.lpszClassName    = "pingpang";
    wcex.hIconSm        = 0;

    RegisterClass(&wcex);

    hWnd = CreateWindow("pingpang", "GameDemo", WS_MAIN | WS_VISIBLE | 
            WS_THICKBORDER|WS_CAPTION | WS_OKBUTTON |WS_CLOSEBOX,
             1,20,WINDOW_WIDTH, WINDOW_LENGTH, NULL, NULL, NULL, NULL);    

    if (!hWnd)  return;
    ShowWindow(hWnd, true);
    while(GetMessage(&msg,hWnd)){
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

	static HWND hWndButton;
    switch(message)
    {
        case LMSG_CREATE:
				
			hWndButton = CreateWindow("button", "start", WS_CONTROL  | BS_PUSHBUTTON | WS_BORDER | WS_VISIBLE,
				106, 210, 40 , 20, hWnd, (HMENU)ID_BUTTON, NULL, NULL);

            break;
        case LMSG_COMMAND:
            switch(HIWORD(wParam)){
            case BN_CLICKED://×ÀÃæÍŒ±êµÄµã»÷ÊÂŒþŽŠÀí

                switch(LOWORD(wParam)){

                case ID_BUTTON:
					if(!is_run){
						SetTimer(hWnd,1,10);
					}
					GameInit();
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }

            break;
        case LMSG_PENDOWN://×ÀÃæµÄmouseÊÂŒþ
            CaptureMouse(hWnd,BYCLIENT); 
            x=(int)wParam;
            y=(int)lParam;
            if(x<board.x+BOARD_LENGTH/2)
               board.x-=10;
            else
               board.x+=10;            
            break;

		case LMSG_TIMER:
			TimerAction(hWnd);
			break;

        case LMSG_PENMOVE:
            break;
        case LMSG_PENUP://×ÀÃæµÄmouseÊÂŒþ
			DisCaptureMouse();
            break;
        case LMSG_PAINT:
        {
            HGDIOBJ  brush;
            HGDIOBJ  pen;
            
            ps.bPaintDirect=false;

            hDC=BeginPaint(hWnd, &ps);
            if(!hDC){
                return true;
            }
			if(is_run){
				brush = CreateSolidBrush(RGB(23,43,133));
				pen = CreatePen(PS_SOLID,1,RGB(230,0,255));
				SelectObject(hDC,brush);
				SelectObject(hDC,pen);
				DeleteObject(brush);
				DeleteObject(pen);    
				for(x=0;x<BRICK_NUM_IN_Y;x++)
					for(y=0;y<BRICK_NUM_IN_X;y++)
					{
                            if(brick[x][y]==1)
                            Rectangle(hDC,y*BRICK_LENGTH,x*BRICK_WIDTH,y*BRICK_LENGTH+BRICK_LENGTH,x*BRICK_WIDTH+BRICK_WIDTH);
				 	}
					Rectangle(hDC,board.x,board.y,board.x+BOARD_LENGTH,board.y+BOARD_WIDTH);
			   		Circle(hDC,ball.x,ball.y,BALL_RADIUS);                 
			}
            EndPaint(hWnd, &ps);
            break;
        }
        case LMSG_CLOSE:
            is_run=false;
			KillTimer(hWnd,1);
            PostQuitMessage(hWnd);
            break;        
        case LMSG_DESTROY:
            is_run=false;
			KillTimer(hWnd,1);
            PostQuitMessage(hWnd);//ÓÃÀŽÍË³öÏûÏ¢Ñ­»·
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return true;
}
