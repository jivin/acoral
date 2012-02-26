#include "driver.h"
#include"../include/test.h"
#include "screen.h"
#include "kernel.h"
acoral_u16 *screen_buf;
acoral_id screen_id;
acoral_screen_info *screen_info;

void put_pixel(acoral_u32 x,acoral_u32 y, acoral_u32 c )
{
	if ( (x < screen_info->xres) && (y < screen_info->yres) )
		*(screen_buf+(y*screen_info->xres+x))=c ;
}

void clear_screen( acoral_u32 c)
{
    acoral_u32 x,y ;
    for( y = 0 ; y < screen_info->yres ; y++ )
    {
    	for( x = 0 ; x < screen_info->xres ; x++ )
    	{
		put_pixel(x,y,c);
    	}
    }
}

void draw_line(acoral_32 x1,acoral_32 y1,acoral_32 x2,acoral_32 y2,acoral_32 color)
{
	acoral_32 dx,dy,e;
	dx=x2-x1; 
	dy=y2-y1;
    
	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					put_pixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					put_pixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					put_pixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					put_pixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					put_pixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					put_pixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					put_pixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					put_pixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
}

/**************************************************************
在LCD屏幕上画一个矩形
**************************************************************/
void draw_rect(acoral_32 x1,acoral_32 y1,acoral_32 x2,acoral_32 y2,acoral_32 color)
{
    draw_line(x1,y1,x2,y1,color);
    draw_line(x2,y1,x2,y2,color);
    draw_line(x1,y2,x2,y2,color);
    draw_line(x1,y1,x1,y2,color);
}

/**************************************************************
在LCD屏幕上用颜色填充一个矩形
**************************************************************/
void fill_rect(acoral_32 x1,acoral_32 y1,acoral_32 x2,acoral_32 y2,acoral_32 color)
{
    acoral_32 i;
    for(i=y1;i<=y2;i++)
	draw_line(x1,i,x2,i,color);
}

ACORAL_COMM_THREAD test(void *args){
	acoral_u32 top,right,v_black;
	screen_id=acoral_dev_open("screen");
	if(screen_id==ACORAL_DEV_ERR_ID){
		acoral_prints("open screen error\n");
	}
	acoral_dev_config(screen_id,CMD_SCREEN_INFO,&screen_info,0);
	screen_buf=screen_info->screen_buf;

	#define BLANK 12
	top=(screen_info->xres -BLANK*2);
	right	=(screen_info->xres-BLANK*2 );
	v_black	=((screen_info->yres -BLANK*4 ) / 6 );

	fill_rect( BLANK, BLANK, ( screen_info->xres - BLANK ), ( screen_info->yres - BLANK ),0x0000);//fill a Rectangle with some color
	fill_rect( (BLANK*2), (BLANK*2 + v_black*0), (right), (BLANK*2 + v_black*1),0x001f);		//fill a Rectangle with some color
	fill_rect( (BLANK*2), (BLANK*2 + v_black*1), (right), (BLANK*2 + v_black*2),0x07e0);		//fill a Rectangle with some color
	fill_rect( (BLANK*2), (BLANK*2 + v_black*2), (right), (BLANK*2 + v_black*3),0xf800);		//fill a Rectangle with some color
	fill_rect( (BLANK*2), (BLANK*2 + v_black*3), (right), (BLANK*2 + v_black*4),0xffe0);		//fill a Rectangle with some color
	fill_rect( (BLANK*2), (BLANK*2 + v_black*4), (right), (BLANK*2 + v_black*5),0xf81f);		//fill a Rectangle with some color
	fill_rect( (BLANK*2), (BLANK*2 + v_black*5), (right), (BLANK*2 + v_black*6),0x07ff);		//fill a Rectangle with some color

	draw_line( BLANK,BLANK, (screen_info->xres-BLANK), (screen_info->yres-BLANK), 0x0000 ) ;
	draw_line( BLANK,(screen_info->yres-BLANK), (screen_info->xres-BLANK), BLANK, 0x0000 ) ;
	draw_line( (screen_info->xres/2),(BLANK*2 + v_black*0), (screen_info->xres/2), (BLANK*2 + v_black*6), 0x0000 ) ;
}

void screen_test(){
	acoral_create_thread(test,ACORAL_PRINT_STACK_SIZE,NULL,NULL,26,1);
}

TEST_CALL(screen_test)
