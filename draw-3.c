#include "apps.h"

struct p {
	int x;
	int y;
};

struct dot {
	int n;
	struct p d[8];
};

const static struct dot dot1={
	1,{
		{0,0},
	},
};

const static struct dot dot2={
	4,{
		{0,0},{1,0},{0,1},{1,1},
	},
};

const static struct dot dot3={
	4,{
		{-1,0},{0,-1},{0,1},{1,0},
	},
};

const static struct dot dot5={
	8,{
		{0,-2,},{1,-1},{2,0},{1,1},{0,2},{-1,1},{-2,0},{-1,-1},
	},
};


void draw_line(int x1, int y1, int x2, int y2, int color, const struct dot *d);
void tch_line(int x1, int y1, int x2, int y2, int color);
void draw_clear();
void draw_lineA(int x1, int y1, int x2, int y2, int color);

/********************************************************************************/
/*		draw_main																*/
/********************************************************************************/
void draw_main()
{
	int x,y, savex,savey, ret, sw, line_color;
	const struct dot *d;
	
	lcd_clear(BLACK);
	text_color = WHITE;
	back_color = BRUE;
	lcd_cdisp_str_x2(50, "µ¥∂∑øÃƒ");
	lcd_disp_str_x1(100,140,"sw1: ∏ÿ±");
	lcd_disp_str_x1(100,160,"sw2: º≠≥ÿÆ≥");
	text_color = BLACK;
	back_color = WHITE;
	sw_updown();
	draw_clear();
	savex = savey = -1;
	d = &dot3;
	line_color = BLACK;
	for(;;){
#ifdef LCD_2_4INCH
		sw = sw_downA();
		ret = sw & 4;
		x = tch_x;
		y = tch_y;
#else
		ret = tch_read(&x, &y);				// if 0 then no touch
		sw  = sw_read();					// if 0 then not push
#endif
		if(sw & 1){							// sw1: clear
			draw_clear();
			savex = savey = -1;
			continue;
		}
		if(sw & 2){							// sw2: exit
			return;
		}
		if(ret==0){							// no touch
			savex = -1;
		}
		else{								// touch
			if(savex < 0){					// new touch
				if(x >= 290){				// ê¸ÇÃëæÇ≥ê›íË
					bz_short();
					if(y < 30)
						d = &dot1;
					else if(y < 60)
						d = & dot2;
					else if(y < 90)
						d = &dot3;
					else if(y < 120)
						d = &dot5;
					continue;
				}
				else if(x >=260){			// ê¸ÇÃêFê›íË
					bz_short();
					if(y < 30)
						line_color = RGB(0,0,0);
					else if(y < 60)
						line_color = RGB(0,0,255);
					else if(y < 90)
						line_color = RGB(0,255,0);
					else if(y < 120)
						line_color = RGB(0,255,255);
					else if(y < 150)
						line_color = RGB(255,0,0);
					else if(y < 180)
						line_color = RGB(255,0,255);
					else if(y < 210)
						line_color = RGB(255,255,0);
					else
						line_color = RGB(255,255,255);
					continue;
				}
				savex = x;
				savey = y;
			}
			else{
				if(x >= 260){				// âÊñ îÕàÕäO
					continue;
				}
//				if(abs1(savex-x)>20 || abs1(savey-y)>20)
//					continue;
				draw_line(savex, savey, x, y, line_color, d);
				savex = x;
				savey = y;
			}
		}
	}
}
/********************************************************************************/
/*		draw_clear																*/
/********************************************************************************/
void draw_clear()
{
	lcd_clear(WHITE);
	
	lcd_clearA(260,  0, 289, 29, RGB(0,0,0));
	lcd_clearA(260, 30, 289, 59, RGB(0,0,255));
	lcd_clearA(260, 60, 289, 89, RGB(0,255,0));
	lcd_clearA(260, 90, 289,119, RGB(0,255,255));
	lcd_clearA(260,120, 289,149, RGB(255,0,0));
	lcd_clearA(260,150, 289,179, RGB(255,0,255));
	lcd_clearA(260,180, 289,209, RGB(255,255,0));
	lcd_clearA(260,210, 289,239, RGB(255,255,255));

	draw_line(290+5,  0+15, 319-5,  0+15, BLACK, &dot1);
	draw_line(290+5, 30+15, 319-5, 30+15, BLACK, &dot2);
	draw_line(290+5, 60+15, 319-5, 60+15, BLACK, &dot3);
	draw_line(290+5, 90+15, 319-5, 90+15, BLACK, &dot5);
}
/********************************************************************************/
/*		draw_line																*/
/********************************************************************************/
void draw_line(int x1, int y1, int x2, int y2, int color, const struct dot *d)
{
	int i, xoff, yoff;
	
	for(i=0; i<d->n; i++){
		xoff = d->d[i].x;
		yoff = d->d[i].y;
		tch_line(x1+xoff, y1+yoff, x2+xoff, y2+yoff, color);
	}
}
/********************************************************************************/
/*		draw_lineA																*/
/********************************************************************************/
void draw_lineA(int x1, int y1, int x2, int y2, int color)
{
	int i, tmp, x_dist, y_dist;
	
	x_dist = abs1(x1-x2);
	y_dist = abs1(y1-y2);
	if(x_dist >= y_dist){
		if(x_dist==0)
			return;
		if(x1 <= x2){
			for(i=x1; i<=x2; i++){
				tmp = y1 + ((y2-y1)*(i-x1)+x_dist/2)/x_dist;
				lcd_clearA(i,tmp, i,tmp, color);
			}
		}
		else{
			for(i=x2; i<=x1; i++){
				tmp = y2 + ((y1-y2)*(i-x2)+x_dist/2)/x_dist;
				lcd_clearA(i,tmp, i,tmp, color);
			}
		}
	}
	else{
		if(y_dist==0)
			return;
		if(y1 <= y2){
			for(i=y1; i<=y2; i++){
				tmp = x1 + ((x2-x1)*(i-y1)+y_dist/2)/y_dist;
				lcd_clearA(tmp,i, tmp,i, color);
			}
		}
		else{
			for(i=y2; i<=y1; i++){
				tmp = x2 + ((x1-x2)*(i-y2)+y_dist/2)/y_dist;
				lcd_clearA(tmp,i, tmp,i, color);
			}
		}
	}
}
