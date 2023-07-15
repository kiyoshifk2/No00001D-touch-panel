
/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "apps.h"


int buzzer_flag;						// 0:normal, 1:短音指定(50ms 1KHz), 2:長音指定(500ms 1KHz)
volatile int TickCount;
void lcd_printf(const char *fmt, ...);
jmp_buf main_env;


extern int f7_init_flag;
void f7_main();
void game_main();
void othe_main();
void pentomino(int fast);
void hako_main();
void hako_play();
void rush_main();
void su_main();
void calc_main();
void oscillo();
void spectrumanalizer();
void m_measure();

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

/*
 *                       Main application
 *
 *    CPU clock 80MHz, I/O clock 40MHz
 *    LCD への書き込み速度 4MByte/sec
 *        1pixcel 2byte
 *
 *    wait 200ms at 40MHz clock
 *    wait 140ms at 80MHz clock (2wait)
 *    volatile int i;
 *    for(i=0; i<1000000; i++) ;
 * 
 *   i = j;  // 35ns at 80MHz clock
 *   
 *   タッチパネル LCD_RS と LCD_D0 がペア、LCD_CS と LCD_D1 がペアになっている
 *
 */

const struct menu main_menu[]={
	{25,   0, 10, 1, "test"},
	{25,  60, 10, 2, "draw"},
	{25, 120, 10, 3, "f_mgr"},
	{25, 180, 10, 4, "Game"},
	
	{157,  0, 10, 5, "Calc"},
	{157, 60, 10, 6, "Oscillo"},
	{157,120, 10, 7, "Spe_Ana"},
	{157,180, 10, 8, "F.Resp."},
	{0,0,0,0,0},
};

int main ( void )
{
	int cmd;
	
    /* Initialize all modules */
    SYS_Initialize ( NULL );

//	__builtin_enable_interrupts();

	setjmp(main_env);

    while ( true )
    {
		lcd_clear(BLACK);
		btn_back_color = GREEN;
		cmd = menu_select0(main_menu);
		lcd_clear(WHITE);
		switch(cmd){
			case 1:						// for test
				break;
			case 2:
				draw_main();
				break;
			case 3:
				f7_main();
				break;
            case 4:
                game_main();
				break;
			case 5:
				calc_main();
				break;
			case 6:
				oscillo();
				break;
			case 7:
				spectrumanalizer();
				break;
			case 8:
				m_measure();
				break;
		}
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

/********************************************************************************/
/*		game_main																*/
/********************************************************************************/
const struct menu game_menu[]={
	{0,0,  12,1,"ｵｾﾛ"},
	{0,60, 12,2,"ﾍﾟﾝﾄﾐﾉ"},
	{0,120,12,3,"ﾍﾟﾝﾄﾐﾉ(fast)"},
	{0,180,12,4,"ﾊｺｲﾘﾑｽﾒ disp"},
	
	{156, 0, 12,5,"ﾊｺｲﾘﾑｽﾒ"},
	{156,60, 12,6,"ﾗｯｼｭｱﾜｰ"},
	{156,120,12,7,"ｽｳﾄﾞｸ"},
//	{156,180,12,8,"exit"},
	{0,0,0,0,0},
};

void game_main()
{
	int cmd;
	
	for(;;){
		lcd_clear(BLACK);
		btn_back_color = BRUE;
		cmd = menu_select0_break(game_menu);
		lcd_clear(WHITE);
		switch(cmd){
			case 1:
				othe_main();
				break;
			case 2:
				pentomino(0);
				break;
			case 3:
				pentomino(1);
				break;
			case 4:
				hako_main();
				break;
			case 5:
				hako_play();
				break;
			case 6:
				rush_main();
				break;
			case 7:
				su_main();
				break;
		}
	}
}

/********************************************************************************/
/*		timer_init																*/
/*      100μs                                                                  */
/********************************************************************************/
void timer_init()
{
	T2CON = 0;
	PR2 = 4000-1;							// 40MHz/4000 = 10KHz
	TMR2 = 0;
	IPC2bits.T2IP = 4;
	IFS0bits.T2IF = 0;
	IEC0bits.T2IE = 1;
	T2CONSET=0x8000;						// TIMER2 ON
}

void __ISR(_TIMER_2_VECTOR, IPL4AUTO) T2Interrupt()
{
	static int t2_500us, t2_cycle;
	
	++TickCount;
	IFS0bits.T2IF = 0;
	if(buzzer_flag==1){						// 1:短音指定(50ms 1KHz)
		buzzer_flag = 0;
		t2_500us = 5;
		t2_cycle= 100;
	}
	else if(buzzer_flag==2){				// 2:長音指定(500ms 1KHz)
		buzzer_flag = 0;
		t2_500us = 5;
		t2_cycle = 1000;
	}
	if(t2_500us){
		if(--t2_500us==0){
//			LATBINV = 0x4000;				// RB14 反転
			LATGINV = 0x0004;				// RG2 反転
			if(t2_cycle){
				if(--t2_cycle){
					t2_500us = 5;
				}
			}
		}
	}
}

/********************************************************************************/
/*		timer3																	*/
/*		ADC timer, impulseタイマー												*/
/********************************************************************************/
#if 0
int t3cnt;
int lfsr=1;
int timer3_M;

void __ISR(_TIMER_3_VECTOR, IPL5AUTO) T3Interrupt()
{
	IFS0bits.T3IF = 0;
    if(timer3_M){
    	lfsr = (lfsr>>1) ^ (-(lfsr & 1) & 0xe08);
        LATBbits.LATB12 = lfsr & 1;
        return;
    }
	if(++t3cnt==4096){
		t3cnt = 0;
		LATBSET = 0x1000;
		return;
	}
	LATBCLR = 0x1000;
}
/********************************************************************************/
/*		ADC																		*/
/********************************************************************************/

extern const char m_keiretu[4095];
int ADC_int_tmp;
    
void __ISR(_ADC_VECTOR, IPL5AUTO) ADCIntgerrupt()
{
	lfsr = (lfsr>>1) ^ (-(lfsr & 1) & 0xe08);
	LATBbits.LATB12 = lfsr & 1;

    ADC_int_tmp = ADC1BUF0;
	IFS0bits.AD1IF = 0;
}
#endif

/********************************************************************************/
/*		APP_Tasks																*/
/********************************************************************************/
void APP_Tasks()
{
}
/********************************************************************************/
/*		APP_Initialize															*/
/********************************************************************************/
void APP_Initialize()
{
//	f7_init_flag = 0;
	timer_init();
	lcd_init();
	osci_init();
}

#if 0
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
	PR3 = 40*5-1;						// 40MHz/40*5=200KHz
    TMR3 = 0;
    T3CON = 0;
    IPC3bits.T3IP = 5;
    IFS0bits.T3IF = 0;
//    IEC0bits.T3IE = 1;
    T3CONbits.ON = 1;
	
	IPC5bits.AD1IP = 5;
	IFS0bits.AD1IF = 0;
//	IEC0bits.AD1IE = 1;
}
#endif
