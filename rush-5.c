#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <process.h>

#include "apps.h"



//	┏━━━━━━┓
//	┃・・・・・・┃
//	┃・・・・・・┃
//	┃・・・・・・
//	┃・・・・・・┃
//	┃・・・・・・┃
//	┃・・・・・・┃
//	┗━━━━━━┛

char rush_ban[8][8];	//	aaab bbbb
//	rush_ban[y][x]
//	aaa=1～4:車番号、5,6:赤い車
//	bbbbb=1からの通し番号

const char rush_car_pat[7][3][3]={
	{
		{0,0,0},	// dummy data
		{0,0,0},
		{0,0,0},
	},
	{
		{1,0,0},	// 車番号１
		{1,0,0},
		{0,0,0}
	},
	{
		{1,1,0},	// 車番号２
		{0,0,0},
		{0,0,0}
	},
	{
		{1,0,0},	// 車番号３
		{1,0,0},
		{1,0,0}
	},
	{
		{1,1,1},	// 車番号４
		{0,0,0},
		{0,0,0}
	},
	{
		{1,1,0},	// 車番号５ 赤い車
		{0,0,0},
		{0,0,0}
	},
	{
		{1,1,1},	// 車番号６ 赤い車
		{0,0,0},
		{0,0,0}
	}
};

#define C (char)
const char rush_ban_test1[8][8]={	//
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,       32+1,          0,     32*2+7,     32*2+7,    32*1+10,          0,      20},
	{20,       32+1,     32*2+4,     32*2+4,     32*3+8,    32*1+10,    32*3+12,      20},
	{20,          0,  C(32*5+5),  C(32*5+5),     32*3+8,    32*1+11,    32*3+12,       0},
	{20,     32*2+2,     32*2+2,     32*1+6,     32*3+8,    32*1+11,    32*3+12,      20},
	{20,          0,          0,     32*1+6,  C(32*4+9),  C(32*4+9),  C(32*4+9),      20},
	{20,     32*2+3,     32*2+3,          0,          0,          0,          0,      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
const char rush_ban_test2[8][8]={	//
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,          0,     32*2+1,     32*2+1,          0,     32*1+4,     32*3+5,      20},
	{20,          0,     32*1+2,          0,          0,     32*1+4,     32*3+5,      20},
	{20,          0,     32*1+2,  C(32*5+3),  C(32*5+3),          0,     32*3+5,       0},
	{20,          0,          0,          0,     32*1+9,          0,          0,      20},
	{20,     32*1+6,     32*1+7,     32*1+8,     32*1+9,    32*2+10,    32*2+10,      20},
	{20,     32*1+6,     32*1+7,     32*1+8, C(32*4+11), C(32*4+11), C(32*4+11),      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
const char rush_ban_test3[8][8]={	// 解いた
	{20,         20,         20,         20,        20,          20,         20,      20},
	{20,          0,          0,     32*3+1,         0,      32*2+2,     32*2+2,      20},
	{20,          0,          0,     32*3+1,         0,           0,          0,      20},
	{20,  C(32*5+3),  C(32*5+3),     32*3+1,         0,           0,          0,       0},
	{20,  C(32*4+4),  C(32*4+4),  C(32*4+4),         0,           0,     32*3+5,      20},
	{20,          0,          0,          0,         0,           0,     32*3+5,      20},
	{20,          0,          0,          0,         0,           0,     32*3+5,      20},
	{20,         20,         20,         20,        20,          20,         20,      20},
};
const char rush_ban_test4[8][8]={ // 解いた
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,  C(32*4+1),  C(32*4+1),  C(32*4+1),     32*1+2,     32*3+3,     32*1+4,      20},
	{20,          0,          0,          0,     32*1+2,     32*3+3,     32*1+4,      20},
	{20,  C(32*5+7),  C(32*5+7),    32*3+10,          0,     32*3+3,     32*1+5,       0},
	{20,     32*1+8,     32*1+9,    32*3+10,          0,          0,     32*1+5,      20},
	{20,     32*1+8,     32*1+9,    32*3+10,          0,     32*2+6,     32*2+6,      20},
	{20,          0,          0,          0,          0,          0,          0,      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
const char rush_ban_test5[8][8]={	// 
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,     32*3+1,  C(32*4+2),  C(32*4+2),  C(32*4+2),          0,     32*3+6,      20},
	{20,     32*3+1,          0,          0,     32*1+4,     32*1+5,     32*3+6,      20},
	{20,     32*3+1,  C(32*5+3),  C(32*5+3),     32*1+4,     32*1+5,     32*3+6,       0},
	{20,          0,     32*1+7,     32*1+8,  C(32*4+9),  C(32*4+9),  C(32*4+9),      20},
	{20,          0,     32*1+7,     32*1+8,    32*1+10,          0,          0,      20},
	{20,          0,          0,          0,    32*1+10,          0,          0,      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
const char rush_ban_test6[8][8]={	// 
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,     32*1+1,          0,          0,     32*3+4,     32*2+5,     32*2+5,      20},
	{20,     32*1+1,          0,     32*1+3,     32*3+4,          0,     32*3+7,      20},
	{20,  C(32*5+2),  C(32*5+2),     32*1+3,     32*3+4,     32*1+6,     32*3+7,       0},
	{20,          0,          0,          0,          0,     32*1+6,     32*3+7,      20},
	{20,     32*1+8,     32*1+9,    32*2+10,    32*2+10,          0,          0,      20},
	{20,     32*1+8,     32*1+9,          0, C(32*4+11), C(32*4+11), C(32*4+11),      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
const char rush_ban_test7[8][8]={	// 
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,     32*3+1,     32*2+2,     32*2+2,          0,     32*3+5,     32*3+6,      20},
	{20,     32*3+1,     32*1+3,          0,          0,     32*3+5,     32*3+6,      20},
	{20,     32*3+1,     32*1+3,  C(32*5+4),  C(32*5+4),     32*3+5,     32*3+6,       0},
	{20,          0,          0,          0,     32*1+9,    32*2+11,    32*2+11,      20},
	{20,          0,          0,     32*1+8,     32*1+9,    32*2+12,    32*2+12,      20},
	{20,     32*2+7,     32*2+7,     32*1+8,    32*2+10,    32*2+10,          0,      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
const char rush_ban_test8[8][8]={	// 
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,     32*1+1,          0,          0,     32*2+5,     32*2+5,     32*3+7,      20},
	{20,     32*1+1,     32*2+2,     32*2+2,     32*1+6,          0,     32*3+7,      20},
	{20,  C(32*5+3),  C(32*5+3),     32*1+4,     32*1+6,          0,     32*3+7,       0},
	{20,          0,          0,     32*1+4,  C(32*4+8),  C(32*4+8),  C(32*4+8),      20},
	{20,          0,          0,          0,    32*1+10,    32*2+11,    32*2+11,      20},
	{20,          0,     32*2+9,     32*2+9,    32*1+10,    32*2+12,    32*2+12,      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
const char rush_ban_test9[8][8]={	// 
	{20,         20,         20,         20,         20,         20,         20,      20},
	{20,       32+1,          0,     32*2+5,     32*2+5,    32*3+11,          0,      20},
	{20,       32+1,          0,       32+6,       32+7,    32*3+11,    32*3+12,      20},
	{20,  C(32*5+2),  C(32*5+2),       32+6,       32+7,    32*3+11,    32*3+12,       0},
	{20,          0,       32+3,  C(32*4+8),  C(32*4+8),  C(32*4+8),    32*3+12,      20},
	{20,          0,       32+3,     32*2+9,     32*2+9,    32*2+10,    32*2+10,      20},
	{20,  C(32*4+4),  C(32*4+4),  C(32*4+4),          0,          0,          0,      20},
	{20,         20,         20,         20,         20,         20,         20,      20},
};
const char rush_ban_init_data[8][8]={
	{20,20,20,20,20,20,20,20},
	{20, 0, 0, 0, 0, 0, 0,20},
	{20, 0, 0, 0, 0, 0, 0,20},
	{20, 0, 0, 0, 0, 0, 0, 0},
	{20, 0, 0, 0, 0, 0, 0,20},
	{20, 0, 0, 0, 0, 0, 0,20},
	{20, 0, 0, 0, 0, 0, 0,20},
	{20,20,20,20,20,20,20,20},
};
const int rush_color[21]={
	0xffff,
	0xc618,0x53d7,0xcd7f,0x0e5f,0xb73d,0x1f36,0xeed3,0xbc8e,0xa254,0xca47,0xed00,0x4d84,
	RGB(255,200,100),RGB(128,255,200),RGB(220,100,255),RGB(230,230,180),RGB(180,230,230),RGB(230,180,230),RGB(0,0,255),
	0x0
};

#define R RED
static const short pat1[20*20*3];
static const short pat2[20*20*2]={
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
	R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,
};

char *rush_msg1, *rush_msg2, *rush_msg3, *rush_msg4;
/********************************************************************************/
/*		rush_ban_init															*/
/********************************************************************************/
void rush_ban_init()
{
	memcpy(rush_ban, rush_ban_init_data, sizeof(rush_ban));
}
/********************************************************************************/
/*		rush_koma_disp															*/
/*		x,y で指定した位置に正方形を１個表示する、色指定もできる				*/
/********************************************************************************/
void rush_koma_disp(int x, int y, int color)
{
	lcd_clearA(80+20*x, 20*y, 80+19+20*x, 19+20*y, color);
}
/********************************************************************************/
/*		rush_disp_ban															*/
/*		盤を表示する															*/
/********************************************************************************/
void rush_disp_ban()
{
	int x, y, num, car;
	
	for(y=0; y<8; y++){
		for(x=0; x<8; x++){
			car = (rush_ban[y][x] >> 5) & 7;
			num = rush_ban[y][x] & 0x1f;
			if(car <= 4){
				rush_koma_disp(x, y, rush_color[num]);
			}
			else{					// 赤い車
				rush_koma_disp(x, y, RGB(255,0,0));	// 赤
			}
		}
	}
	for(y=1; y<7; y++){
		for(x=0; x<7; x++){
			if(rush_ban[y][x] != rush_ban[y][x+1])
				lcd_clearA(80+19+20*x, 20*y, 80+21+20*x, 19+20*y, BLACK);
		}
	}
	for(x=1; x<7; x++){
		for(y=0; y<7; y++){
			if(rush_ban[y][x] != rush_ban[y+1][x])
				lcd_clearA(80+20*x, 19+20*y, 80+19+20*x, 21+20*y, BLACK);
		}
	}
}
/********************************************************************************/
/*		rush_display															*/
/********************************************************************************/
//void rush_display(int color)
//{
//	lcd_clear(WHITE);
//	rush_disp_ban();
//	text_color = color;
//	if(rush_msg1)
//		lcd_disp_str_x1(0,176,rush_msg1);
//	text_color = BLACK;
//	if(rush_msg2)
//		lcd_disp_str_x1(0,192,rush_msg2);
//	if(rush_msg3)
//		lcd_disp_str_x1(0,208,rush_msg3);
//	if(rush_msg4)
//		lcd_disp_str_x1(0,224,rush_msg4);
//}
/********************************************************************************/
/*		rush_disp_car															*/
/*		車を１台表示する														*/
/*		car: 1～6 の車番号														*/
/*		str: 全角１文字															*/
/********************************************************************************/
void rush_disp_car(int x, int y, int car, int tooshi)
{
	int x1, y1, car1;
	
	for(y1=0; y1<3; y1++){
		for(x1=0; x1<3; x1++){
			if(car <= 4){
				if(rush_car_pat[car][y1][x1]){
					rush_koma_disp(x+x1, y+y1, rush_color[tooshi]);
				}
			}
			else{					// 赤い車
				car1 = car==5 ? 2 : 4;
				if(rush_car_pat[car1][y1][x1]){
					rush_koma_disp(x+x1, y+y1, RGB(255,0,0));	// 背景色　赤
				}
			}
		}
	}
}
/********************************************************************************/
/*		rush_put_car_test														*/
/*		return 1: 置ける、0:置けない											*/
/********************************************************************************/
int rush_put_car_test(char ban[8][8], int x, int y, int car_numb)
{
	int x1, y1;
	
	for(y1=0; y1<3; y1++){
		for(x1=0; x1<3; x1++){
			if(ban[y+y1][x+x1] && rush_car_pat[car_numb][y1][x1])
				return 0;				// 置けない
		}
	}
	return 1;							// 置ける
}
/********************************************************************************/
/*		rush_put_car															*/
/*		車を盤に配置する														*/
/********************************************************************************/
const struct menu_pat rush_put_car_menu[]={
	{20,   20*9, 20,  20*2,1, pat1},
	{20*3, 20*10,20*2,20,  2, pat1},
	{20*6, 20*9, 20,  20*3,3, pat1},
	{20*8, 20*10,20*3,20,  4, pat1},
	{20*12,20*9, 20*2,20,  5, pat2},
	{12,20,12*3,16,10.0},
	{12,60,12*4,16,11,0},
	{0,0,0,0,0},
};

void rush_put_car()
{
	int cmd, ec_flag, tooshibango, x, y, w, h, sw, ret, x1, y1, cnt;
	const short *pat;
    short save[20*20*3];
	char save_ban[8][8];

    pat = 0;
    cnt = -1;
    x = y = w = h = 0;
	ec_flag = 0;									// error correct 出来ない
	tooshibango = 1;
	rush_ban_init();
	memcpy(save_ban, rush_ban, sizeof(save_ban));
	for(;;){
		/***	車を選択する	***/
		lcd_clear(WHITE);
		rush_disp_ban();
		lcd_disp_str_x1(12,20,"ｵﾜﾘ");
		lcd_disp_str_x1(12,60,"ﾓﾄﾞｽ");
		cmd = menu_pat_select(rush_put_car_menu);	// 車を選ぶ、又は　オワリ　モドス
		switch(cmd){
			case 10:						// オワリ
				return;
			case 11:						// モドス
				if(ec_flag==0)				// ec 出来ない
					continue;
				ec_flag = 0;
				memcpy(rush_ban, save_ban, sizeof(rush_ban));	// 盤を１つ前に戻す
				tooshibango--;				// 駒が１個取り除かれたから通し番号を減らす
				rush_disp_ban();			// 盤表示
				continue;
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				x = rush_put_car_menu[cmd-1].x;
				y = rush_put_car_menu[cmd-1].y;
				cnt = -1;
				w = rush_put_car_menu[cmd-1].w;
				h = rush_put_car_menu[cmd-1].h;
				pat = rush_put_car_menu[cmd-1].pat;
				break;
		}
		
		/***	車をドラッグする	***/
		tch_drag_write(x,y, w,h, pat, 0x10000, save);	// save に読んでおく
		for(;;){
			ret = tch_read(&x1, &y1);		// if 0 then no touch
			sw  = sw_read();
			if(sw & 2){						// sw2: exit
				longjmp(main_env, 1);
			}
			if(ret==0){						// no touch
				wait_1ms();
				if(cnt < 0){
					if(--cnt < -200){
						break;
					}
				}
				else{
					cnt = -1;
				}
			}
			else{							// touch
				cnt = 0;
				if(x != x1 || y != y1){
					tch_drag_recover(x,y, w,h, save);				// 画面を戻す
					x = x1;											// 移動する
					y = y1;
					tch_drag_write(x,y, w,h, pat, 0x10000, save);	// 書く
				}
				else{
					wait_ms(20);
				}
			}
		}
		
		/***	車を駐車する	***/
		tch_drag_recover(x,y, w,h, save);	// 画面を戻す
		x = ((x+10)-80)/20;				// 配列添え字にする
		y = (y+10)/20;					// 配列添え字にする
		if(x<1 || x>6 || y<1 || y>6 || rush_put_car_test(rush_ban, x, y, cmd)==0){
			/***	駐車できない	***/
			lcd_clearA(0, 20*8, 319, 239, WHITE);
			lcd_cdisp_str_x2(190,"ﾁｭｳｼｬﾃﾞｷﾏｾﾝ");
			sw_updown();
			continue;
		}
		ec_flag = 1;					// error correct 可能にする
		memcpy(save_ban, rush_ban, sizeof(save_ban));
		for(y1=0; y1<3; y1++){			// 駐車出来るから駐車する
			for(x1=0; x1<3; x1++){
				if(rush_car_pat[cmd][y1][x1]){
					rush_ban[y+y1][x+x1] = (cmd<<5) + tooshibango;
				}
			}
		}
		++tooshibango;
	}
}
/********************************************************************************/
/*		rush_move_test															*/
/*		車を設置できるかどうかテストする										*/
/*		return 1:OK, 0:NG														*/
/********************************************************************************/
//int rush_move_test(int x, int y, int car_numb)
//{
//	int x1, y1;
//	
//	for(y1=0; y1<3; y1++){
//		for(x1=0; x1<3; x1++){
//			if(rush_car_pat[car_numb][y1][x1] && rush_ban[y+y1][x+x1])
//				return 0;
//		}
//	}
//	return 1;								// 設置出来る
//}
/********************************************************************************/
/*		rush_make_param															*/
/*		x,y 位置の駒から car_numb と tooshi_bango を作る						*/
/********************************************************************************/
void rush_make_param(int x, int y, int *p_car_numb, int *p_tooshi_bango)
{
	*p_car_numb = (rush_ban[y][x] >> 5) & 7;
	*p_tooshi_bango = rush_ban[y][x] & 0x1f;
}
/********************************************************************************/
/*		rush_move_car															*/
/*		車を移動して遊ぶ														*/
/********************************************************************************/
void rush_move_car()
{
	int i, sw, cnt, koma, savex, savey, x, y, x1, y1, nextx, nexty, car_numb, tooshibango;
	
	cnt = -1;
	x = y = x1 = y1 = 0;
    lcd_clear(WHITE);
	rush_disp_ban();
	for(;;){
		/***	車を選択する	***/
		for(;;){
			sw = sw_updown();
			if(sw & 2){				// sw2: exit
				longjmp(main_env, 1);
			}
			if(sw & 4){			// touch
				savex = tch_x;
				savey = tch_y;
				x = ((savex)-80)/20;			// 配列添え字にする
				y = (savey)/20;				// 配列添え字にする
				if(x<1 || x>6 || y<1 || y>6 || rush_ban[y][x]==0)
					continue;		// 車でない
				break;
			}
		}
		
		/***	車を移動する	***/
		rush_make_param(x, y, &car_numb, &tooshibango);	// パラメータ作成
		
		koma = (unsigned char)rush_ban[y][x];	// x,y を左上にする
		while(koma==(unsigned char)rush_ban[y][x-1]){
			x--;
		}
		while(koma==(unsigned char)rush_ban[y-1][x]){
			y--;
		}
//		for(i=0; i<sizeof(rush_ban); i++){		// rush_ban から koma 削除
//			if(koma==((unsigned char*)rush_ban)[i])
//				((unsigned char*)rush_ban)[i] = 0;
//		}

		cnt = 0;
		for(;;){
			if(tch_read(&x1, &y1)==0){		// no touch
				wait_1ms();
				if(--cnt < -200){
					break;					// 再度　車の選択に戻る
				}
			}
			else{							// touch
				cnt = 0;
				/* 移動したかテストする	*/
				if(car_numb==1 || car_numb==3){	// 上下移動
					nextx = x;
					if(y1-savey > 20)
						nexty = y+1;
					else if(y1-savey < -20)
						nexty = y-1;
					else
						continue;
				}
				else{							// 左右移動
					nexty = y;
					if(x1-savex > 20)
						nextx = x+1;
					else if(x1-savex < -20)
						nextx = x-1;
					else
						continue;
				}
				/***	移動できるかテストする	***/
				for(i=0; i<sizeof(rush_ban); i++){		// rush_ban から koma 削除
					if(koma==((unsigned char*)rush_ban)[i])
						((unsigned char*)rush_ban)[i] = 0;
				}
				if(rush_put_car_test(rush_ban, nextx, nexty, car_numb)){	// 移動できる
					x = nextx;								// 現在位置更新
					y = nexty;
                    savex = x1;
                    savey = y1;
				}
				for(y1=0; y1<3; y1++){				// 現在位置に駐車する
					for(x1=0; x1<3; x1++){
						if(rush_car_pat[car_numb][y1][x1]){
							rush_ban[y+y1][x+x1] = (car_numb<<5) + tooshibango;
						}
					}
				}
				rush_disp_ban();
				if(((rush_ban[3][7] >> 5) & 0x7)==5){	// パズル完成
					lcd_clearA(0, 20*8, 319, 239, WHITE);
					lcd_cdisp_str_x2(190,"ｵﾒﾃﾞﾄｳｺﾞｻﾞｲﾏｽ");
					sw_updown();
					return;
				}
			}
		}
	}
}
/********************************************************************************/
/*		main																	*/
/********************************************************************************/
const struct menu rush_main_menu[]={
	{80, 60,12, 1, "ﾃﾆｭﾘｮｸ"},
	{80,120,12, 2, "ﾚｲﾀﾞｲ"},
	{0,0,0},
};
const struct menu rush_file_menu[]={
	{0, 0,12, 1, "ﾚｲﾀﾞｲ 1"},
	{0,60,12, 2, "ﾚｲﾀﾞｲ 2"},
//	{0,100,12,3, "ﾚｲﾀﾞｲ 3"},
	{0,120,12,4, "ﾚｲﾀﾞｲ 3"},
	{0,180,12,5, "ﾚｲﾀﾞｲ 4"},

	{156, 0,12,6, "ﾚｲﾀﾞｲ 5"},
	{156,60,12,7, "ﾚｲﾀﾞｲ 6"},
	{156,120,12,8, "ﾚｲﾀﾞｲ 7"},
	{156,180,12,9, "ﾚｲﾀﾞｲ 8"},
	{0,0,0},
};

void rush_main()
{
	int cmd;
	
	rush_ban_init();						// 盤初期化
	lcd_clear(WHITE);
	btn_back_color = RED;
	cmd = menu_select0_break(rush_main_menu);

	if(cmd==1){								// manual input
		lcd_clear(WHITE);
		lcd_cdisp_str_x2(100,"ｸﾙﾏｦﾊｲﾁｽﾙ");
		lcd_cdisp_str_x1(150,"<<< touch >>>");
		sw_updown();
		rush_put_car();						// 車を配置する
		lcd_clear(WHITE);
		lcd_cdisp_str_x2(100,"ｹﾞｰﾑｶｲｼ");
		sw_updown();
		lcd_cdisp_str_x1(150,"<<< touch >>>");
		rush_move_car();					// 車を移動して遊ぶ
	}
	else if(cmd==2){						// file input
		lcd_clear(WHITE);
		btn_back_color = GREEN;
		cmd = menu_select0_break(rush_file_menu);
		
		switch(cmd){
			case 1:
				memcpy(rush_ban, rush_ban_test1, sizeof(rush_ban));
				break;
			case 2:
				memcpy(rush_ban, rush_ban_test2, sizeof(rush_ban));
				break;
			case 3:
				memcpy(rush_ban, rush_ban_test3, sizeof(rush_ban));
				break;
			case 4:
				memcpy(rush_ban, rush_ban_test4, sizeof(rush_ban));
				break;
			case 5:
				memcpy(rush_ban, rush_ban_test5, sizeof(rush_ban));
				break;
			case 6:
				memcpy(rush_ban, rush_ban_test6, sizeof(rush_ban));
				break;
			case 7:
				memcpy(rush_ban, rush_ban_test7, sizeof(rush_ban));
				break;
			case 8:
				memcpy(rush_ban, rush_ban_test8, sizeof(rush_ban));
				break;
			case 9:
				memcpy(rush_ban, rush_ban_test9, sizeof(rush_ban));
				break;
			default:						// exit
				return;
		}
		rush_move_car();					// 車を移動して遊ぶ
	}
}
