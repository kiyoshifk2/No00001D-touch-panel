#include "apps.h"


#define GPH_HIGH	667		// 2.15V(664) にて return=220
#define GPH_LOW		47		// 0.15V(47)  にて return=20
#define GPH_CNTR	((GPH_HIGH+GPH_LOW)/2)

struct osci_v osci_v={
	(short*)&osci_v.buf[0x5000*2/4],	// disp_buf 0x4000 word
	(short*)&osci_v.buf[0],				// s_buf 0x5000 word
	40-1,								// osci_PR3
	5000,								// no_of_data
	1,									// trigger_mode  1:Auto+, 2:Auto-, 3:Single+, 4:Single-
	0,									// trigger_point
	"[ms]0   1   2   3   4   5",		// osci_footer
	999,								// spe_PR3
	1024,								// spe_no_of_data
	"KHz 0   4   8   12  16  20",		// spe_footer
};


extern const char *osci_footer[];


void graph_hokan1(unsigned char high[251], unsigned char low[251]);
void graph_draw(unsigned char high[251], unsigned char low[251]);
void graph_set1(short *ptr, float step, unsigned char high[251], unsigned char low[251], int norm);
void graph_set2(short *ptr, float step, unsigned char high[251], unsigned char low[251], int norm);
void  graph_set(short *ptr, float step, unsigned char high[251], unsigned char low[251], int norm);
void top_mark(unsigned char high[251], float db_top);
void graph_draw(unsigned char high[251], unsigned char low[251]);
void graph_memori();
void graph_memori_s();
void graph_memori_spe();
float cabs1(struct C *c1);
char *exp_sweep_speed(int *no_of_data);
void fft_mado1024(struct C *x);
void fft1024(struct C *x, struct C *X);

/********************************************************************************/
/*		osci_init																*/
/********************************************************************************/
void osci_init()
{
	/***	ADC		***/
	AD1CON1 = 0;
	AD1CON1bits.ON = 1;
	AD1CON1bits.FORM = 0;				// Integer 16bit
	AD1CON1bits.SSRC = 2;				// Trigger source: Timer3
	AD1CON1bits.ASAM = 1;				// sample auto start
//	AD1CON1bits.SAMP = 1;				// sampling
	
	AD1CON2 = 0;
	AD1CON2bits.VCFG = 0;				// VREFH/L = AVDD/AVSS
	AD1CON2bits.CSCNA = 0;				// Do not scan inputs
	
	AD1CON3 = 0;
	AD1CON3bits.ADRC = 0;				// PBclk 使用
	AD1CON3bits.ADCS = 0;				// 2*TPB=TAD = 0.05us
	
	AD1CHS = 0;
	AD1CHSbits.CH0NA = 0;				// negative input is VREFL
	AD1CHSbits.CH0NB = 0;				// negative input is VREFL
	AD1CHSbits.CH0SA = 8;				// positive input is AN8;
	AD1CHSbits.CH0SB = 8;				// positive input is AN8;
    /***    timer3    ***/
	PR3 = 40-1;							// 40MHz/40=1MHz
    TMR3 = 0;
    T3CON = 0;
    IPC3bits.T3IP = 5;
    IFS0bits.T3IF = 0;
//    IEC0bits.T3IE = 1;
    T3CONbits.ON = 1;
	
	IPC5bits.AD1IP = 5;
	IFS0bits.AD1IF = 0;
//	IEC0bits.AD1IE = 1;

	//DMA
	DMACON = 0x8000;						// DMA Enable
	
	//DMA channel 1                         // ADC
	DCH1CON = 0;
	DCH1CONbits.CHAEN = 0;					// channel automatic off
	DCH1ECONbits.CHSIRQ = _ADC_IRQ;
	DCH1ECONbits.SIRQEN = 1;				// channel start IRQ enable bit
	DCH1SSA = ((uint32_t) (&ADC1BUF0))&0x1FFFFFFF;
//	DCH1DSA = ((uint32_t) wave)&0x1FFFFFFF;
	DCH1SSIZ = 2;
	DCH1DSIZ = 100*2;						// 総転送バイト数
	DCH1CSIZ = 2;							// 1回の転送バイト数

	//DMA channel 2                         // M系列
	DCH2CON = 0;
	DCH2CONbits.CHAEN = 1;					// channel automatic on
	DCH2ECONbits.CHSIRQ = _TIMER_3_IRQ;
	DCH2ECONbits.SIRQEN = 1;				// channel start IRQ enable bit
	DCH2SSA = ((uint32_t) m_ushort)&0x1FFFFFFF;
	DCH2DSA = ((uint32_t) (&LATB))&0x1FFFFFFF;
	DCH2SSIZ = 4096*2;
	DCH2DSIZ = 2;							// 総転送バイト数
	DCH2CSIZ = 2;							// 1回の転送バイト数
}
/********************************************************************************/
/*		osci_measure_spe														*/
/********************************************************************************/
void osci_measure_spe()
{
	int i, flag, tmp /*, top_x, top_y*/;
	struct C *x, *X;
	float step, db, db_top;
	unsigned char high[251+1], low[251+1];
//	short *s_buf;
	
//	sw_up();
	flag = 0;
	graph_memori_spe();
//	sw_up();
	for(;;){
		/*** get data	***/
		PR3 = osci_v.spe_PR3;
		TMR3 = 0;
    	AD1CHSbits.CH0SA = 8;				// positive input is AN8;
    	AD1CHSbits.CH0SB = 8;				// positive input is AN8;
		DCH1CONbits.CHAEN = 0;					// channel automatic off
		DCH1DSA = ((uint32_t)osci_v.buf) & 0x1FFFFFFF;
		DCH1DSIZ = (osci_v.spe_no_of_data+100)*2;	// 総転送バイト数
		DCH1INTbits.CHBCIF = 0;				// 割り込みフラグクリア
		DCH1CONbits.CHEN = 1;				// DMA start
		while(DCH1INTbits.CHBCIF==0){
			i = sw_read();
			if(i & 4){						// touch: exit
//				sw_downup();
				return;
			}
		}
		/*** fft	***/
//		s_buf = (short*)osci_v.buf;
		x = (struct C*)&osci_v.s_buf[osci_v.spe_no_of_data+100]; // メモリ割り当て
		X = &x[osci_v.spe_no_of_data];                       // メモリ割り当て
		for(i=0; i<osci_v.spe_no_of_data; i++){
			x[i].r = (float)(osci_v.s_buf[i+100]-GPH_CNTR);
			x[i].i = 0;
		}
		if(osci_v.spe_no_of_data != 1024)
			message("osci_mesure_spe()\nspe_no_of_data=%d\n", osci_v.spe_no_of_data);
		fft_mado1024(x);
		fft1024(x, X);
		/*** グラフ	***/
//AAAAA		ptr = osci_v.disp_buf;				//(short*)&X[1024];
		db_top = -1000;
		for(i=0; i<osci_v.spe_no_of_data/2; i++){
			db = 20*log10f(cabs1(&X[i])) -103;
			osci_v.disp_buf[i] = (short)floor(db*2+0.5) + 200;	// 1dB 2ドット, 0dB が 200line
			if(db_top < db)
				db_top = db;
			if(osci_v.disp_buf[i] < 0)
				osci_v.disp_buf[i] = 0;
		}
		step = (250.0+0.5) / (osci_v.spe_no_of_data/2-1);
		graph_set(osci_v.disp_buf, step, high, low, 0);
        
		graph_draw(high, low);				// グラフ描画
		top_mark(high, db_top);				// top をマークする
        
		do{
			tmp = sw_read();
#ifdef LCD_2_4INCH
            if(tmp){
                sw_downB();
                sw_up();
            }
#endif
			if(tmp & 3)						// sw1/2: start/stop
				flag = !flag;
			if(tmp & 4)						// touch: exit
				return;
		}
        while(flag);
	}
}
/********************************************************************************/
/*		graph_memori_spe														*/
/********************************************************************************/
void graph_memori_spe()
{
	static const char *msg[]={"+10","0dB","-10","-20","-30","-40","-50","-60","-70","-80","-90"};
	int i;

	lcd_clear(WHITE);
	
	back_color = BRUE;
	text_color = WHITE;
//	lcd_disp_str_x1(112, 0, "sw1:stop");
	
	back_color = WHITE;
	text_color = BLACK;
	for(i=0; i<=10; i++){
		lcd_disp_str_x1(6, i*20+20-7, msg[i]);
	}

	lcd_disp_str_x1(0, 224, osci_v.spe_footer);
}
/********************************************************************************/
/*		getbuf_auto																*/
/*		return 0:normall, 4:sw-exit												*/
/*		v->trigger_mode 1:auto+, 2:auto-, 3:single+, 4:single-					*/
/********************************************************************************/
int osci_getbuf_auto()
{
	int i, flag;
	short tmp;
//	short *s_buf;
//
//	DMA の現在位置を知るには DCH1DPTR を参照する
//

//	s_buf = (short*)osci_v.buf;
	PR3 = osci_v.osci_PR3;				// 40MHz/(PR3+1)
    TMR3 = 0;
	AD1CHSbits.CH0SA = 8;				// positive input is AN8;
	AD1CHSbits.CH0SB = 8;				// positive input is AN8;
	DCH1CONbits.CHAEN = 0;					// channel automatic off
	DCH1DSA = ((uint32_t)osci_v.s_buf) & 0x1FFFFFFF;
	DCH1DSIZ = 0x4000*2;				// 総転送バイト数は 0x4000x2、データ数は 0x4000データ
	DCH1INTbits.CHBCIF = 0;				// 割り込みフラグクリア
	DCH1CONbits.CHEN = 1;				// DMA start
	while(DCH1INTbits.CHBCIF==0){
		i = sw_read();
		if(i & 2){						// sw2: exit
			return 4;
		}
	}
	flag = 0;
	for(i=100; i<0x4000 - (osci_v.no_of_data+100); i++){		// 0x4000データを調べる
		tmp = osci_v.s_buf[i];
		if(osci_v.trigger_mode==1 || osci_v.trigger_mode==3){	// + trigger
			if(flag){
				if(tmp > 512){			// triggered
					memmove(osci_v.disp_buf, &osci_v.s_buf[i-10], (osci_v.no_of_data+100) * 2);
					return 0;			// normal end
				}
			}
			if(flag==0){
				if(tmp < 512-20)
					flag = 1;
			}
		}
		else{							// - trigger
			if(flag){
				if(tmp < 512){			// triggered
					memmove(osci_v.disp_buf, &osci_v.s_buf[i-10], (osci_v.no_of_data+100) * 2);
					return 0;			// normal end
				}
			}
			if(flag==0){
				if(tmp > 512+20)
					flag = 1;
			}
		}
	}
	memmove(osci_v.disp_buf, &osci_v.s_buf[i-10], (osci_v.no_of_data+100) * 2);
	return 0;						// normal end
}
/********************************************************************************/
/*		osci_move_data															*/
/*		s_buf: 0x4000データのリングバッファ										*/
/*		disp_buf: 0x4000データのストレートバッファ								*/
/********************************************************************************/
void osci_move_data(int src)
{
	int i;
//	short *s_buf = (short*)osci_v.buf;
	
	for(i=0; i<0x4000; i++){
		osci_v.disp_buf[i] = osci_v.s_buf[(src+i)&0x3fff];
	}
}
/********************************************************************************/
/*		getbuf_single															*/
/*		return 0:normall, 4:sw-exit												*/
/*		v->trigger_mode 1:auto+, 2:auto-, 3:single+, 4:single-					*/
/********************************************************************************/
int osci_getbuf_single()
{
//	int i, flag, tmp;
//
//	DMA の現在位置を知るには DCH1DPTR を参照する
//
	short *s_buf = osci_v.s_buf;

	PR3 = osci_v.osci_PR3;				// 40MHz/(PR3+1)
    TMR3 = 0;
	AD1CHSbits.CH0SA = 8;				// positive input is AN8;
	AD1CHSbits.CH0SB = 8;				// positive input is AN8;
	DCH1CONbits.CHAEN = 1;				// channel continuously enabled
	DCH1DSA = ((uint32_t)osci_v.buf) & 0x1FFFFFFF;
	DCH1DSIZ = 0x4000*2;				// 総転送バイト数 0x4000x2、データ数 0x4000 のリングバッファにする
	DCH1INTbits.CHBCIF = 0;				// 割り込みフラグクリア
	DCH1CONbits.CHEN = 1;				// DMA start
	while(DCH1DPTR < 0x800*2)
		;
	if(osci_v.trigger_mode==3){			// +trigger
		for(;;){
			if(s_buf[((DCH1DPTR>>1)-1)&0x3fff] < 512-20)
				break;
			if(~SW_PORT & SW2_BIT){		// sw2
				sw_read();
				return 4;				// sw2 による exit
			}
		}
		for(;;){
			if(s_buf[((DCH1DPTR>>1)-1)&0x3fff] > 512)
				break;
			if(~SW_PORT & SW2_BIT){		// sw2
				sw_read();
				return 4;				// sw2 による exit
			}
		}
	}
	else{								// -trigger
		for(;;){
			if(s_buf[((DCH1DPTR>>1)-1)&0x3fff] > 512+20)
				break;
			if(~SW_PORT & SW2_BIT){		// sw2
				sw_read();
				return 4;				// sw2 による exit
			}
		}
		for(;;){
			if(s_buf[((DCH1DPTR>>1)-1)&0x3fff] < 512)
				break;
			if(~SW_PORT & SW2_BIT){		// sw2
				sw_read();
				return 4;				// sw2 による exit
			}
		}
	}
	osci_v.trigger_point = DCH1DPTR >> 1;
	while((((DCH1DPTR>>1)-osci_v.trigger_point)&0x3fff) < 0x3800)
		;
	DCH1CONbits.CHEN = 0;				// DMA stop
	osci_v.trigger_point = DCH1DPTR >> 1;
	osci_move_data(osci_v.trigger_point);
	return 0;
}
/********************************************************************************/
/*		osci_measure															*/
/********************************************************************************/
void osci_measure()
{
    int i, tmp, pos, no_of_data;
	unsigned char high[251], low[251];
	float step;
	char *footer;
	
	graph_memori();
//	sw_up();
	for(;;){
		if(osci_v.trigger_mode >= 3){		// single trigger
			memset(high, 0, sizeof(high));
			memset(low,  0, sizeof(low));
			graph_draw(high, low);			// 罫線のみ表示
		}
		if(osci_v.trigger_mode >= 3){		// single trigger
			if(osci_getbuf_single()==4){	// sw2 による exit
//				sw_downup();
				return;
			}
		}
		else{								// auto trigger
			if(osci_getbuf_auto()){			// get data
//				sw_downup();
				return;						// sw2: exit
			}
		}
		step = 250.0/(osci_v.no_of_data);
		graph_set(osci_v.disp_buf+10, step, high, low, 1);
		graph_draw(high, low);

		tmp = sw_read();
		if(tmp & 2){                 	// sw2: exit
			return;
		}

		if(osci_v.trigger_mode >= 3 || (tmp & 4)){	// single trigger || touch
			if(osci_v.trigger_mode <= 2)					// Auto
				osci_v.trigger_point = 0x1000;
			for(;;){
				if(tmp & 1){					// sw1: trigger ready, start
					break;
				}
				else if(tmp & 4){				// touch: scroll
					footer = exp_sweep_speed(&no_of_data);
					lcd_clear(WHITE);
					if(footer==0)			// exit
						return;
					graph_memori_s();
					lcd_disp_str_x1(0, 224, footer);
					pos = osci_v.trigger_point;
					for(;;){
						i = sw_read();
						if((SW_PORT & SW2_BIT)==0){		// sw2: right
							pos += no_of_data/20;
							if(((osci_v.trigger_point - pos)&0x3fff) < no_of_data) //AAAAA
								pos -= no_of_data/20;                              //AAAAA
							pos &= 0x3fff;                                         //AAAAA
						}
						else if((SW_PORT & SW1_BIT)==0){	// sw1: left
							pos -= no_of_data/20;
							if(((osci_v.trigger_point - pos)&0x3fff) < no_of_data) //AAAAA
								pos += no_of_data/20;                              //AAAAA
							pos &= 0x3fff;                                         //AAAAA
						}
						else if(i & 4){		// touch; 拡張メニューに入る
//							sw_downup();
							footer = exp_sweep_speed(&no_of_data);
							lcd_clear(WHITE);
							if(footer==0)			// exit を選択した
								return;
							graph_memori_s();
							lcd_disp_str_x1(0, 224, footer);
						}
//						while(pos > 0x4000-no_of_data)
//							pos -= no_of_data/10;
//						while(pos < 0)
//							pos += no_of_data/10;
						osci_move_data(pos);
						step = 250.0/no_of_data;
						graph_set(osci_v.disp_buf+10, step, high, low, 1);
						graph_draw(high, low);
					}
				}
				else if(tmp & 4){				// sw3: exit
					return;
				}
				tmp = sw_read();
			}
		}
	}
}
/********************************************************************************/
/*		graph_memori															*/
/********************************************************************************/
void graph_memori_s()
{
	static const char *msg1[] = {"  5 ","  4 ","  3 ","  2 ","  1 ","  0 "," -1 "," -2 "," -3 "," -4 "," -5 "};
	int i;

	lcd_clearA(0,0, 47,239, WHITE);
	for(i=0; i<=10; i++){
		lcd_disp_str_x1(6, i*20+20-7, msg1[i]);
	}
}

void graph_memori()
{
	lcd_clear(WHITE);
	
	back_color = BRUE;
	text_color = WHITE;
//	if(osci_v.trigger_mode <= 2)		// auto
//		lcd_disp_str_x1(112, 0, "sw1:stop");
//	else								// single trigger
//		lcd_cdisp_str_x1(0, "sw1:clr,sw2:menu");
	
	back_color = WHITE;
	text_color = BLACK;
	
	graph_memori_s();
	lcd_disp_str_x1(0, 224, osci_v.osci_footer);
}
/********************************************************************************/
/*		graph_test																*/
/********************************************************************************/
void graph_test()
{
	unsigned char high[251], low[251];
	int i;
	
	for(i=0; i<251; i++){
		high[i] = low[i] = (int)(100*(sinf(5*3.14159f*i/250)) + 128);
	}
	graph_hokan1(high, low);
	graph_draw(high, low);
}
/********************************************************************************/
/*      top_mark                                                                */
/********************************************************************************/
void top_mark_s(int x, int y)
{
    if(x>=48 && x<=298 && y>=20 && y<=220)
        lcd_clearA(x,y, x,y, BRUE);
}

void top_mark(unsigned char high[251], float db_top)
{
    int i, top_x, top_y;
    char str[50];
    
    top_x = top_y = -1;
    for(i=0; i<251; i++){
        if(top_y < high[i]){
            top_y = high[i];
            top_x = i;
        }
    }
	top_x = 48  + top_x;
	top_y = 239 - top_y;
	for(i=-1; i<=1; i++){
		top_mark_s(top_x+i, top_y-1);
		top_mark_s(top_x+i, top_y+1);
	}
	top_mark_s(top_x+1, top_y);
	top_mark_s(top_x-1, top_y);
	
	sprintf(str, "  %3.1fdB  ", (double)db_top);
//	lcd_disp_str_x1(210, 22, str);
	lcd_disp_str_x1(140,0,str);
}
/********************************************************************************/
/*		graph_draw																*/
/*		グラフエリアは 251x201 座標は (48,20)									*/
/*		high[251] と low[251] を描画する, data は 20～220 までを描く			*/
/*		back_color, keisen_color, graph_color を使用する						*/
/********************************************************************************/
void graph_draw(unsigned char high[251], unsigned char low[251])
{
#ifdef LCD_SPI
	int x, y, off;
	
	off = 20;
	back_color = WHITE;
	text_color = BLACK;
	keisen_color = GRAY;
	graph_color  = RED;
	lcd_area_set(48, 20, 298, 220);
	lcd_write_cmd(MemoryWrite);
	LCD_ALL_H;								// all set, idle
	LCD_CS_L;								// cs clear
	LCD_DC_H;
    for(y=200+off; y>=0+off; y--){
		for(x=0; x<251; x++){
			if(high[x] >= y && y >= low[x]){
//				lcd_write_color(graph_color);
				while(SPI2STATbits.SPITBE==0)
					;
				SPI2BUF = graph_color>>8;

				while(SPI2STATbits.SPITBE==0)
					;
				SPI2BUF = graph_color;
			}
			else if((y-off)%20==0 || x%25==0){
//				lcd_write_color(keisen_color);
				while(SPI2STATbits.SPITBE==0)
					;
				SPI2BUF = keisen_color>>8;

				while(SPI2STATbits.SPITBE==0)
					;
				SPI2BUF = keisen_color;
			}
			else{
//				lcd_write_color(back_color);
				while(SPI2STATbits.SPITBE==0)
					;
				SPI2BUF = back_color>>8;

				while(SPI2STATbits.SPITBE==0)
					;
				SPI2BUF = back_color;
			}
		}
	}
	while(SPI2STATbits.SPIBUSY)
		;
	LCD_ALL_H;
#else
	int x, y, off;
	
	off = 20;
	back_color = WHITE;
	text_color = BLACK;
	keisen_color = GRAY;
	graph_color  = RED;
	lcd_area_set(48, 20, 298, 220);
	lcd_write_cmd(MemoryWrite);
    for(y=200+off; y>=0+off; y--){
		for(x=0; x<251; x++){
			if(high[x] >= y && y >= low[x])
				lcd_write_color(graph_color);
			else if((y-off)%20==0 || x%25==0)
				lcd_write_color(keisen_color);
			else
				lcd_write_color(back_color);
		}
	}
#endif
}
/********************************************************************************/
/*		graph_dataset_test														*/
/********************************************************************************/
void graph_dataset_test()
{
	unsigned char high[251], low[251];
	short ptr[5000];
	float step = 5.3;
	int i;
	float p;
	
	p = 0;
	for(i=0; i<5000; i++){
		ptr[i] = (int)(300*(sinf(10*3.14159f*p/250)) + 512);
		p += step;
	}
	graph_set(ptr+100, step, high, low, 1);
	graph_draw(high, low);
}
/********************************************************************************/
/*		graph_function															*/
/*		data: 10bit データ														*/
/*		return: 0～240、0はdataが小さい											*/
/********************************************************************************/
int graph_function(int data, int norm)
{
	int tmp;
	
	if(norm)
		tmp = (data-GPH_LOW)*200 / (GPH_HIGH-GPH_LOW) + 20;
	else
		tmp = data;
	if(tmp < 0)
		tmp = 0;
	else if(tmp > 240)
		tmp = 240;
	return tmp;
}
/********************************************************************************/
/*		graph_dataset															*/
/*		グラフエリアは 251x201 座標は (48,20)									*/
/*		step の値で graph_dataset1 又は graph_dataset2 を呼ぶ					*/
/*		*ptr は 10bit data → 上位 8bit を採用する								*/
/********************************************************************************/
void graph_set(short *ptr, float step, unsigned char high[251], unsigned char low[251], int norm)
{
	if(step > 1)
		graph_set2(ptr, step, high, low, norm);	// データが疎
	else
		graph_set1(ptr, step, high, low, norm);	// データが密
}
/********************************************************************************/
/*		graph_dataset2															*/
/*		グラフエリアは 251x201 座標は (48,20)									*/
/*		データが疎な場合、step >= 1												*/
/*		*ptr は 10bit data → 上位 8bit を採用する								*/
/********************************************************************************/
void graph_set2(short *ptr, float step, unsigned char high[251], unsigned char low[251], int norm)
{
	float p, x, y, y0, y1, y2, y3, a, b, c;
	int i, pp, tmp;
	
	if(step < 1){							// error
		memset(high, 128, 251);
		memset(low,  128, 251);
		return;
	}
	/*** 補間する y = ax*x*x + bx*x + cx	***/
	p = 0;									// x 座標: 0, step, 2*step, ...
	pp = 0;									// x 座標: 0, 1, 2, ...
	i = 0;
	for(;;){
//		y0 = (float)ptr[pp-1];
//		y1 = (float)ptr[pp] - y0;
//		y2 = (float)ptr[pp+1] - y0;
//		y3 = (float)ptr[pp+2] - y0;
		y0 = (float)graph_function(ptr[pp-1], norm);
		y1 = (float)graph_function(ptr[pp], norm) - y0;
		y2 = (float)graph_function(ptr[pp+1], norm) - y0;
		y3 = (float)graph_function(ptr[pp+2], norm) - y0;
        a = (y1/2 - y2/2 + y3/6)/(step*step*step);
        b = (-y1*5/2 + y2*2 - y3/2)/(step*step);
        c = (y1*3 - y2*3/2 + y3/3)/step;
		
		for( ; i<251; i++){
			if(i > p+step)
				break;
			x = i-(p-step);
			y = a*x*x*x + b*x*x + c*x;
//			tmp = (int)(y+y0+2)>>2;
//			if(tmp<0) tmp = 0;
//			if(tmp>255) tmp = 255;
			tmp = (int)(y+y0);
			if(tmp<0) tmp = 0;
			if(tmp>240) tmp = 240;
			high[i] = low[i] = tmp;
		}
		if(i>=251)
			break;
		p += step;
		pp++;
	}
	graph_hokan1(high, low);
}

/********************************************************************************/
/*		graph_dataset1															*/
/*		グラフエリアは 251x201 座標は (48,20)                                    */
/*		データが密な場合、step <= 1												*/
/*		*ptr は 10bit data → 上位 8bit を採用する								*/
/********************************************************************************/
void graph_set1(short *ptr, float step, unsigned char high[251], unsigned char low[251], int norm)
{
	float p;
	int i, d, pp;
	char flag[251];
	
	if(step > 1){							// error
		memset(high, 128, 251);
		memset(low,  128, 251);
		return;
	}
	memset(flag, -1, sizeof(flag));
	p = 0;
	pp = 0;
	for(;;){
		i = (int)(p+0.5);
		if(i >= 251)
			break;
//		d = ptr[pp++]>>2;					// 上位 8bit にする
		d = graph_function(ptr[pp++], norm);
		if(flag[i]<0){
			flag[i] = 0;
			high[i] = low[i] = d;
		}
		else{
			if(d > high[i])
				high[i] = d;
			if(d < low[i])
				low[i] = d;
		}
		p += step;
	}
	graph_hokan1(high, low);
}
/********************************************************************************/
/*		graph_hokan1															*/
/*		グラフエリアは 251x201 座標は (48,20)									*/
/*		high[251] と low[251] は飛ばずに全部有効データが入っている				*/
/*		グラフを上下方向接続する												*/
/********************************************************************************/
void graph_hokan1(unsigned char high[251], unsigned char low[251])
{
	int i, tmp;
	
	for(i=0; i<251; i++){
		if(i != 0){
			if(low[i-1] > high[i]){
				tmp = (low[i-1]+high[i])/2;
				high[i] = tmp;
				low[i-1] = tmp+1;
			}
			if(low[i] > high[i-1]){
				tmp = (low[i]+high[i-1])/2;
				high[i-1] = tmp;
				low[i] = tmp+1;
			}
		}
		if(i != 250){
			if(low[i+1] > high[i]){
				tmp = (low[i+1]+high[i])/2;
				high[i] = tmp;
				low[i+1] = tmp+1;
			}
			if(low[i] > high[i+1]){
				tmp = (low[i]+high[i+1])/2;
				high[i+1] = tmp;
				low[i] = tmp+1;
			}
		}
	}
}
/********************************************************************************/
/*		osci_spectrum_setting													*/
/********************************************************************************/
const struct menu spectrum_menu[]={
	{50,  40, 9,  1, "500KHz"},
	{50,  80, 9,  2, "200KHz"},
	{50, 120, 9,  3, "100KHz"},
	{50, 160, 9,  4, " 50KHz"},
	{50, 200, 9,  5, " 20KHz"},
	{170, 40, 9,  6, " 10KHz"},
	{170, 80, 9,  7, "  5KHz"},
	{170,120, 9,  8, "  2KHz"},
	{170,160, 9,  9, "  1KHz"},
	{170,200, 9, 10, " 500Hz"},
	{0,0,0,0,0}
};
const int spe_PR3[]			={	39,		// 40MHz/(   39+1)=1MHz   → 500KHz
								99,		// 40MHz/(   99+1)=400KHz → 200KHz
								199,	// 40MHz/(  199+1)=200KHz → 100KHz
								399,	// 40MHz/(  399+1)=100KHz →  50KHz
								999,	// 40MHz/(  999+1)= 40KHz →  20KHz
								1999,	// 40MHz/( 1999+1)= 20KHz →  10KHz
								3999,	// 40MHz/( 3999+1)= 10KHz →   5KHz
								9999,	// 40MHz/( 9999+1)=  4KHz →   2KHz
								19999,	// 40MHz/(19999+1)=  2KHz →   1KHz
								39999,	// 40MHz/(39999+1)=  1KHz →  500Hz
};

const char *spe_footer[]   ={"KHz 0  100 200 300 400 500",
                             "KHz 0  40  80  120 160 200",
                             "KHz 0  20  40   60  80 100",
                             "KHz 0  10  20   30  40  50",
                             "KHz 0   4   8   12  16  20",
                             "KHz 0   2   4   6   8   10",
                             "KHz 0   1   2   3   4   5",
                             "KHz 0  0.4 0.8 1.2 1.6  2",
                             "KHz 0  0.2 0.4 0.6 0.8  1",
                             " Hz 0  100 200 300 400 500",
};

void osci_spectrum_setting()
{
	int cmd;
	
	lcd_clear(BLACK);
	back_color = BRUE;
	text_color = WHITE;
	lcd_cdisp_str_x2(4, "Max Frequency");
	back_color = WHITE;
	text_color = BLACK;
	btn_back_color = CYAN;
	cmd = menu_select(spectrum_menu);
	lcd_clear(WHITE);
	if(cmd==-1)									// sw3: exit
		return;
	osci_v.spe_PR3 = spe_PR3[cmd-1];
	osci_v.spe_footer = spe_footer[cmd-1];
	osci_v.spe_no_of_data = 1024;
}
/********************************************************************************/
/*		osci_trigger_mode														*/
/********************************************************************************/
const struct menu trigger_mode_menu[]={
	{100, 60, 9, 1, "Auto+"},
	{100,100, 9, 2, "Auto-"},
	{100,140, 9, 3, "Single+"},
	{100,180, 9, 4, "Single-"},
	{0,0,0,0,0}
};

void osci_trigger_mode()
{
	int cmd;
	
	lcd_clear(BLACK);
	back_color = BRUE;
	text_color = WHITE;
	lcd_disp_str_x2(52, 5, "ﾄﾘｶﾞｰﾓｰﾄﾞ");
	back_color = WHITE;
	text_color = BLACK;
	btn_back_color = BRUE;
	cmd = menu_select(trigger_mode_menu);
	lcd_clear(WHITE);
	osci_v.trigger_mode = cmd;
}
/********************************************************************************/
/*		exp_sweep_speed															*/
/*		return 0:sw3 exit														*/
/********************************************************************************/
const struct menu exp_1Msps_menu[]={
	{40,  40, 9, 0, "20us"},
	{40,  80, 9, 1, "50us"},
	{40, 120, 9, 2, "100us"},
	{40, 160, 9, 3, "200us"},
	{40, 200, 9, 4, "500us"},
	{160, 40, 9, 5, "1ms"},
	{160, 80, 9, 6, "2ms"},
	{160,120, 9, 7, "5ms"},
	{160,160, 9, 8, "10ms"},
	{160,200, 9,100,"exit"},
	{0,0,0,0,0}
};
const int no_of_data_1Msps[]={20,50,100,200,500,1000,2000,5000,10000};

const struct menu exp_500Ksps_menu[]={
	{40,  40, 9, 0, "50us"},
	{40,  80, 9, 1, "100us"},
	{40, 120, 9, 2, "200us"},
	{40, 160, 9, 3, "500us"},
	{40, 200, 9, 4, "1ms"},
	{160, 40, 9, 5, "2ms"},
	{160, 80, 9, 6, "5ms"},
	{160,120, 9, 7, "10ms"},
	{160,160, 9, 8, "20ms"},
	{160,200, 9,100,"exit"},
	{0,0,0,0,0}
};
const int no_of_data_500Ksps[]={25,50,100,250,500,1000,2500,5000,10000};

const struct menu exp_200Ksps_menu[]={
	{40,  40, 9, 0, "100us"},
	{40,  80, 9, 1, "200us"},
	{40, 120, 9, 2, "500us"},
	{40, 160, 9, 3, "1ms"},
	{40, 200, 9, 4, "2ms"},
	{160, 40, 9, 5, "5ms"},
	{160, 80, 9, 6, "10ms"},
	{160,120, 9, 7, "20ms"},
	{160,160, 9, 8, "50ms"},
	{160,200, 9,100,"exit"},
	{0,0,0,0,0}
};
const int no_of_data_200Ksps[]={20,40,100,200,400,1000,2000,4000,10000};

const struct menu exp_100Ksps_menu[]={
	{40,  40, 9, 0, "200us"},
	{40,  80, 9, 1, "500us"},
	{40, 120, 9, 2, "1ms"},
	{40, 160, 9, 3, "2ms"},
	{40, 200, 9, 4, "5ms"},
	{160, 40, 9, 5, "10ms"},
	{160, 80, 9, 6, "20ms"},
	{160,120, 9, 7, "50ms"},
	{160,160, 9, 8, "100ms"},
	{160,200, 9,100,"exit"},
	{0,0,0,0,0}
};
const int no_of_data_100Ksps[]={20,50,100,200,500,1000,2000,5000,10000};

const struct menu exp_50Ksps_menu[]={
	{40,  40, 9, 0, "500us"},
	{40,  80, 9, 1, "1ms"},
	{40, 120, 9, 2, "2ms"},
	{40, 160, 9, 3, "5ms"},
	{40, 200, 9, 4, "10ms"},
	{160, 40, 9, 5, "20ms"},
	{160, 80, 9, 6, "50ms"},
	{160,120, 9, 7, "100ms"},
	{160,160, 9, 8, "200ms"},
	{160,200, 9,100,"exit"},
	{0,0,0,0,0}
};
const int no_of_data_50Ksps[]={25,50,100,250,500,1000,2500,5000,10000};

const struct menu exp_20Ksps_menu[]={
	{40,  40, 9, 0, "1ms"},
	{40,  80, 9, 1, "2ms"},
	{40, 120, 9, 2, "5ms"},
	{40, 160, 9, 3, "10ms"},
	{40, 200, 9, 4, "20ms"},
	{160, 40, 9, 5, "50ms"},
	{160, 80, 9, 6, "100ms"},
	{160,120, 9, 7, "200ms"},
	{160,160, 9, 8, "500ms"},
	{160,200, 9,100,"exit"},
	{0,0,0,0,0}
};
const int no_of_data_20Ksps[]={20,40,100,200,400,1000,2000,4000,10000};

const struct menu exp_10Ksps_menu[]={
	{40,  40, 9, 0, "2ms"},
	{40,  80, 9, 1, "5ms"},
	{40, 120, 9, 2, "10ms"},
	{40, 160, 9, 3, "20ms"},
	{40, 200, 9, 4, "50ms"},
	{160, 40, 9, 5, "100ms"},
	{160, 80, 9, 6, "200ms"},
	{160,120, 9, 7, "500ms"},
	{160,160, 9, 8, "1s"},
	{160,200, 9,100,"exit"},
	{0,0,0,0,0}
};
const int no_of_data_10Ksps[]={20,50,100,200,500,1000,2000,5000,10000};


char *exp_sweep_speed(int *no_of_data)
{
	char str[50];
	int cmd;
	
	lcd_clear(BLACK);
	back_color = BRUE;
	text_color = WHITE;
	lcd_cdisp_str_x2(0, "ｻﾕｳ ｽｸﾛｰﾙ");
	back_color = WHITE;
	text_color = BLACK;
	btn_back_color = GREEN;
	switch(osci_v.osci_PR3){
		case 39:							// 1Msps
			cmd = menu_select_break(exp_1Msps_menu);
			lcd_clear(WHITE);
			*no_of_data = no_of_data_1Msps[cmd];
			return cmd==100 ? 0 : (char*)osci_footer[cmd];
		case 79:							// 500Ksps
			cmd = menu_select_break(exp_500Ksps_menu);
			lcd_clear(WHITE);
			*no_of_data = no_of_data_500Ksps[cmd];
			return cmd==100 ? 0 : (char*)osci_footer[cmd+1];
		case 199:							// 200Ksps
			cmd = menu_select_break(exp_200Ksps_menu);
			lcd_clear(WHITE);
			*no_of_data = no_of_data_200Ksps[cmd];
			return cmd==100 ? 0 : (char*)osci_footer[cmd+2];
		case 399:							// 100Ksps
			cmd = menu_select_break(exp_100Ksps_menu);
			lcd_clear(WHITE);
			*no_of_data = no_of_data_100Ksps[cmd];
			return cmd==100 ? 0 : (char*)osci_footer[cmd+3];
		case 799:							// 50Ksps
			cmd = menu_select_break(exp_50Ksps_menu);
			lcd_clear(WHITE);
			*no_of_data = no_of_data_50Ksps[cmd];
			return cmd==100 ? 0 : (char*)osci_footer[cmd+4];
		case 1999:							// 20Ksps
			cmd = menu_select_break(exp_20Ksps_menu);
			lcd_clear(WHITE);
			*no_of_data = no_of_data_20Ksps[cmd];
			return cmd==100 ? 0 : (char*)osci_footer[cmd+5];
		case 3999:							// 10Ksps
			cmd = menu_select_break(exp_10Ksps_menu);
			lcd_clear(WHITE);
			*no_of_data = no_of_data_10Ksps[cmd];
			return cmd==100 ? 0 : (char*)osci_footer[cmd+6];
		default:
			lcd_clear(WHITE);
			sprintf(str, "osci_PR3=%d", osci_v.osci_PR3);
			fatal("exp_sweep_speed", str, 0);
			return 0;
	}
	return 0;
}
/********************************************************************************/
/*		osci_sweep_speed														*/
/********************************************************************************/
const struct menu sweep_speed_menu[]={
	{0,   20, 7, 1, "20us"},
	{0,   60, 7, 2, "50us"},
	{0,  100, 7, 3, "100us"},
	{0,  140, 7, 4, "200us"},
	{0,  180, 7, 5, "500us"},
	{96,  20, 7, 6, "1ms"},
	{96,  60, 7, 7, "2ms"},
	{96, 100, 7, 8, "5ms"},
	{96, 140, 7, 9, "10ms"},
	{96, 180, 7,10, "20ms"},
	{192, 20, 7,11, "50ms"},
	{192, 60, 7,12, "100ms"},
	{192,100, 7,13, "200ms"},
	{192,140, 7,14, "500ms"},
	{192,180, 7,15, "1s"},
	{0,0,0}
};
//                          01234567890123456789012345
const char *osci_footer[]={
                           "[us]0   4   8   12  16  20",
                           "[us]0  10  20   30  40  50",
                           "[us]0  20  40   60  80 100",
                           "[us]0  40  80  120 160 200",
                           "[us]0  100 200 300 400 500",
                           "[ms]0  0.2 0.4 0.6 0.8  1",
                           "[ms]0  0.4 0.8 1.2 1.6  2",
                           "[ms]0   1   2   3   4   5",
                           "[ms]0   2   4   6   8   10",
                           "[ms]0   4   8   12  16  20",
                           "[ms]0  10  20   30  40  50",
                           "[ms]0  20  40   60  80 100",
                           "[ms]0  40  80  120 160 200",
                           "[ms]0  100 200 300 400 500",
                           "[s] 0  0.2 0.4 0.6 0.8  1",
};

const int osci_PR3[]={39,39,39,39,39,39,39,39,39,79,199,399,799,1999,3999};

const int no_of_data[]={20,50,100,200,500,1000,2000,5000,10000,10000,10000,10000,10000,10000,10000};

void osci_sweep_speed()
{
	int cmd;
	
	lcd_clear(BLACK);
	btn_back_color = RED;
	cmd = menu_select(sweep_speed_menu);
	lcd_clear(WHITE);
	osci_v.osci_PR3    = osci_PR3[cmd-1];
	osci_v.no_of_data  = no_of_data[cmd-1];
	osci_v.osci_footer = osci_footer[cmd-1];
}
/********************************************************************************/
/*		oscillo																	*/
/********************************************************************************/
const struct menu oscillo_menu[]={
	{30, 30, 20, 1, "Oscillo measure"},
	{30, 90, 20, 2, "Oscillo setup"},
	{30,150, 20, 3, "exit"},
	{0,0,0,0,0}
};

void oscillo()
{
    int cmd;
    
	for(;;){
		lcd_clear(BLACK);
		btn_back_color = YELLOW;
		cmd = menu_select0_break(oscillo_menu);
		lcd_clear(WHITE);
		switch(cmd){
			case 1:							// Oscillo measure
				osci_measure();
				break;
			case 2:							// Oscillo setup
				osci_sweep_speed();
				osci_trigger_mode();
				osci_measure();
				break;
			case 3:							// exit
				return;
		}
	}
}

const struct menu speana_menu[]={
	{30, 30, 20, 1, "SpeAna measure"},
	{30, 90, 20, 2, "SpeAna setup"},
	{30,150, 20, 3, "exit"},
	{0,0,0,0,0},
};

void spectrumanalizer()
{
	int cmd;
	
	for(;;){
		lcd_clear(BLACK);
		btn_back_color = MAGENTA;
		cmd = menu_select0_break(speana_menu);
		lcd_clear(WHITE);
		switch(cmd){
			case 1:						// SpeAna measure
				osci_measure_spe();
				break;
			case 2:						// SpeAna setup
				osci_spectrum_setting();
				osci_measure_spe();
				break;
			case 3:						// exit
				return;
		}
	}
}
