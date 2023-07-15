#include "apps.h"

int tch_x, tch_y;

/********************************************************************************/
/*		bz_long																	*/
/********************************************************************************/
void bz_long()
{
    buzzer_flag = 2;                // 長音(500ms 1KHz)
}
void bz_short()
{
	buzzer_flag = 1;
}
/********************************************************************************/
/*		tch_drag_write															*/
/*		tch_drag_recover														*/
/*																				*/
/*		tch_drag_write: pat(w * h) を書き込む、画面は save に保存する			*/
/*		toumei_color: pat の透明色指定											*/
/*		tch_drag_recover: save に保存していた画像を書き戻す						*/
/********************************************************************************/
void tch_drag_write(int x, int y, int w, int h, const short *pat, int toumei_color, short *save)
{
	lcd_read_mem(x, y, x+w-1, y+h-1, save);	// 画面を save に保存する
	
#ifdef LCD_SPI
	int i, color;
	
	lcd_area_set(x, y, x+w-1, y+h-1);
	lcd_write_cmd(MemoryWrite);
	LCD_ALL_H;
	LCD_CS_L;
	LCD_DC_H;                           // data
	for(i=0; i<w*h; i++){
		color = (unsigned short)pat[i];
		if(color==toumei_color)
			color = save[i];

		while(SPI2STATbits.SPITBE==0)
			;
		SPI2BUF = color>>8;

		while(SPI2STATbits.SPITBE==0)
			;
		SPI2BUF = color;
	}
	while(SPI2STATbits.SPIBUSY)
		;
	LCD_ALL_H;
#else
	int i, color;
	
	lcd_area_set(x, y, x+w-1, y+h-1);
	lcd_write_cmd(MemoryWrite);
	for(i=0; i<w*h; i++){
		color = pat[i];
		if(color==toumei_color)
			color = save[i];
		lcd_write_color(color);
    }
#endif
}

void tch_drag_recover(int x, int y, int w, int h, short *save)
{
	lcd_write_square(x, y, w, h, save);
}
/********************************************************************************/
/*		tch_int																	*/
/*		return 1: touch															*/
/*																				*/
/*		Y-:LCD_RS, Y+:LCD_D0, X-:LCD_CS, X+:LCD_D1								*/
/********************************************************************************/
int tch_int()
{
	int tmp;
	
	LCD_RS_IN;							// LCD_RS:input
	LCD_D0_IN;							// LCD_D0:input
	LCD_D0_PULLDOWN;					// LCD_D0:pulldown
	LCD_CS_S;							// LCD_CS:High
	LCD_OUTPUT(2);						// LCD_D1:High
	
	wait_1us();
	tmp = (LCD_INPUT & LCD_INPUT & LCD_INPUT) & 1;	// test D0
	
	LCD_D0_NONPULLDOWN;
	LCD_DIR_OUT;
	LCD_RS_OUT;
    return tmp;
}
/********************************************************************************/
/*		tch_read_x																*/
/********************************************************************************/
int tch_read_x()
{
	int i, sum;
	
	LCD_RS_IN;
	LCD_RS_ANA;
	LCD_D0_IN;
	LCD_CS_C;							// X-
	LCD_OUTPUT(0xff);					// X+
	PR3 = 40*5-1;						// 40MHz/40*5=200KHz
    TMR3 = 0;
    T3CON = 0;
    T3CONbits.ON = 1;
	AD1CHSbits.CH0SA = 21;				// positive input is AN21;
	AD1CHSbits.CH0SB = 21;				// positive input is AN21;
    AD1CON1bits.ON = 1;
	sum = 0;
	for(i=0; i<1; i++){
		while(AD1CON1bits.DONE==0)
			;
        AD1CON1bits.DONE = 0;
		sum += ADC1BUF0;
	}
	LCD_DIR_OUT;
	LCD_RS_DIGI;
	LCD_RS_OUT;
	return sum / 1;
}
/********************************************************************************/
/*		tch_read_y																*/
/********************************************************************************/
int tch_read_y()
{
	int i, sum;
	
	LCD_CS_IN;
	LCD_CS_ANA;
	LCD_D1_IN;
	LCD_RS_C;							// Y-
	LCD_OUTPUT(0xff);					// Y+
	PR3 = 40*5-1;						// 40MHz/40*5=200KHz
    TMR3 = 0;
    T3CON = 0;
    T3CONbits.ON = 1;
	AD1CHSbits.CH0SA = 20;				// positive input is AN20;
	AD1CHSbits.CH0SB = 20;				// positive input is AN20;
    AD1CON1bits.ON = 1;
	sum = 0;
	for(i=0; i<1; i++){
		while(AD1CON1bits.DONE==0)
			;
        AD1CON1bits.DONE = 0;
		sum += ADC1BUF0;
	}
	LCD_DIR_OUT;
	LCD_CS_DIGI;
	LCD_CS_OUT;
	return sum / 1;
}
/********************************************************************************/
/*		tch_read																*/
/*		*x=0～319, *y=0～239													*/
/*      return 1: success                                                       */
/********************************************************************************/
#define LEFT	77
#define RIGHT	860
#define UP		120
#define DOWN	890

int tch_read(int *x, int *y)
{
	int i, tmp, t;
	
    tmp = 0;
    for(i=0; i<100; i++){
        t = tch_read_x();
        if(t<=0 || t>=1023)
            return 0;                   // error
        tmp += t;
        if(tch_int()==0)
            return 0;                   // error
    }
	tmp /= 100;
	tmp = (tmp-LEFT)*319/(RIGHT-LEFT);
	if(tmp < 0) tmp = 0;
	if(tmp > 319) tmp = 319;
	*x = tmp;
	
    tmp = 0;
    for(i=0; i<100; i++){
        t = tch_read_y();
        if(t<=0 || t>=1023)
            return 0;                   // error
        tmp += t;
        if(tch_int()==0)
            return 0;                   // error
    }
	tmp /= 100;
	tmp = (tmp-UP)*239/(DOWN-UP);
	if(tmp < 0) tmp = 0;
	if(tmp > 239) tmp = 239;
	*y = tmp;
    return 1;                           // success
}
/********************************************************************************/
/*		tch_input																*/
/********************************************************************************/
//	入力されるまで待つ(ブロック)、50ms up が続いてから、入力待ち
//	flag==1 なら sw2 で break

void tch_input(int *x, int *y, int flag)
{
	int sw;
	
	for(;;){
		sw = sw_updown();
		if(flag && (sw & 2))			// sw2: break
			longjmp(main_env, 1);
		if(sw & 4)						// touch
			break;
	}
	*x = tch_x;
	*y = tch_y;
}
/********************************************************************************/
/*		tch_line																*/
/********************************************************************************/
void tch_line(int x1, int y1, int x2, int y2, int color)
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
/********************************************************************************/
/*		sw																		*/
/*		bit0 : sw1																*/
/*		bit1 : sw2																*/
/*		bit2 : touch															*/
/********************************************************************************/
int sw_read()
{
	int i, sw;
	
	SYS_Tasks();
	sw = 0;
	if(~SW_PORT & SW1_BIT)
		sw |= 1;
	if(~SW_PORT & SW2_BIT)
		sw |= 2;
	/***	touch	***/
	if(tch_int() & tch_int() & tch_int())
		sw |= 4;
	/***	screen shot	***/
	if(~SW_PORT & SW3_BIT){
		for(i=0; i<2000; i++){
			wait_1ms();
			if((~SW_PORT & SW3_BIT)==0)
				return sw;
		}
        bz_long();                		// 長音(500ms 1KHz)
		f7_scrn_shot();
        bz_long();                		// 長音(500ms 1KHz)
	}
	return sw;
}

void sw_up()
{
	int i;
	
	/*** 20ms sw 入力無しを確認する	***/
	for(;;){
		for(i=0; i<20; i++){
			wait_1ms();
            if(sw_read())
				break;
		}
		if(i==20)
			break;
	}
}

//	入力無ければすぐに return
int sw_downA()
{
	int i, sw;
	
	sw = sw_read();
	if(sw){
		for(i=0; i<10; i++){
			wait_1ms();
			if(sw != sw_read())
				return 0;
		}
	}
    if(sw & 4){                     // touch
        if(tch_read(&tch_x, &tch_y)==0) // error
            sw &= ~4;
    }
	return sw;
}

//	入力が入るまで待つ
int sw_downB()
{
	int sw;
	
	while((sw = sw_downA())==0)
		;
	bz_short();						// 短音(50ms 1KHz)
	return sw;
}

int sw_updown()
{
	sw_up();
	return sw_downB();
}
