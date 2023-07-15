#include "definitions.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdarg.h>

#define LCD_2_4INCH

//
//  15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
// |←   BRUE   →|←    GREEN    →|←    RED   →|
//	R:0～255, G:0～255, B:0～255 ... 上位 5又は6bit 使用
#define RGB(R,G,B) ((((B) & 0xf8)<<8) | (((G) & 0xfc)<<3) | (((R) & 0xf8)>>3))
#define RGB2R(RGB) (((RGB)<<3) & 0xf8)
#define RGB2G(RGB) (((RGB)>>3) & 0xfc)
#define RGB2B(RGB) (((RGB)>>8) & 0xf8)

#define BLACK RGB(0,0,0)
#define WHITE RGB(255,255,255)
#define GRAY  RGB(192,192,192)
#define RED   RGB(255,0,0)
#define GREEN RGB(0,255,0)				// ここまで
#define BRUE  RGB(0,0,255)
#define CYAN  RGB(0,255,255)
#define MAGENTA RGB(255,0,255)
#define YELLOW  RGB(255,255,0)

#define LCD_RST	0x0008					// RF3
#define LCD_CS	0x0004					// RE2
#define LCD_RS	0x0010					// RE4
#define LCD_WR	0x0010					// RF4
#define LCD_RD	0x0020					// RF5
#define LCD_ALL_S   {LATFSET=LCD_RST|LCD_WR|LCD_RD;LATESET=LCD_RS|LCD_CS;}
#define LCD_CS_S    LATESET=LCD_CS
#define LCD_CS_C    LATECLR=LCD_CS
#define LCD_RS_S    LATESET=LCD_RS
#define LCD_RS_C    LATECLR=LCD_RS
#define LCD_WR_S    LATFSET=LCD_WR
#define LCD_WR_C    LATFCLR=LCD_WR
#define LCD_RD_S    LATFSET=LCD_RD
#define LCD_RD_C    LATFCLR=LCD_RD
#define LCD_DIR_IN  TRISDSET=0xff
#define LCD_DIR_OUT TRISDCLR=0xff
#define LCD_RS_IN   TRISESET=LCD_RS
#define LCD_RS_OUT  TRISECLR=LCD_RS
#define LCD_CS_IN   TRISESET=LCD_CS
#define LCD_CS_OUT  TRISECLR=LCD_CS
#define LCD_D0_IN   TRISDSET=1
#define LCD_D1_IN   TRISDSET=2
#define LCD_D0_PULLDOWN    CNPDDSET=1
#define LCD_D0_NONPULLDOWN CNPDDCLR=1
#define LCD_CS_ANA  ANSELESET=LCD_CS
#define LCD_CS_DIGI ANSELECLR=LCD_CS
#define LCD_RS_ANA  ANSELESET=LCD_RS
#define LCD_RS_DIGI ANSELECLR=LCD_RS
#define LCD_INPUT   (PORTD&0xff)
#define LCD_OUTPUT(data) LATD=((data & 0xff) | (LATD & ~0xff))
#define EEPROM_SDA	0x0200				// RD9
#define EEPROM_SCL	0x0400				// RD10

#define LCD_ALL_H   LCD_ALL_S
#define LCD_CS_L    LCD_CS_C
#define LCD_DC_L    LCD_RS_C
#define LCD_DC_H    LCD_RS_S

#define ColumnAddressSet 0x2a
#define PageAddressSet   0x2b
#define MemoryRead       0x2e
#define MemoryWrite      0x2c
#define VSCRSADD         0x37

//#define SW123_PORT_DATA	(~PORTB)
#define SW_PORT			PORTB
#define SW1_BIT			0x200			// RB9
#define SW2_BIT			0x400			// RB10
#define SW3_BIT			0x800			// RB11

#define PAI 3.14159
#define abs1(x) ((x)>0 ? (x) : -(x))

struct menu_old {
	int line_numb;
	int cmd;
	char *msg;
};

struct menu{
	int x;
	int y;
	int n;
	int cmd;
	char *msg;
};

struct menu_pat{
	int x;
	int y;
	int w;								// w が 0 ならストッパー
	int h;
	int cmd;
	const short *pat;							// pat==0 なら描画しない
};

struct C{
	float r;
	float i;
};

struct osci_v {
	short *disp_buf;				// osci_v.buf の後半 0x4000 word
	short *s_buf;					// osci_v.buf の前半 0x5000 word
	int osci_PR3;
	int no_of_data;
	int trigger_mode;
	int trigger_point;
	const char *osci_footer;
	int spe_PR3;
	int spe_no_of_data;
	const char *spe_footer;
	int m_PR3;
	char *m_linear_footer;
	char *m_log_footer;
	float buf[0x9000*2/4];			// メインバッファ 4096 x (short + 複素数 + 複素数)
};

struct cal_v {
	int key1_x;
	int key1_y;
	int error;
	char *error_msg;
	int keypage;
	unsigned char linebuf[256];					// keycode の１行バッファ
	unsigned char *linetop;						// 現在行先頭
	unsigned char prg_buf[4096+256];			// プログラムメモリ
	char str_buf[4096];
	char eeprom_top[8+1][40];					// プログラム先頭１行
	long double ans;
	long double A[12];								// A～L
	char *AS[6];								// A$～F$
	int start_time;
	int auto_flag;
	int alinenumb;
	jmp_buf env1;
};

struct eeprom_data {					// プログラムパラメータを eeprom の 0番地から保存する
	int ss_numb;						// screen shot file number
	int prg_numb;						// 電卓プログラム file number
};

extern struct cal_v cal_v;
extern struct osci_v osci_v;
extern int back_color;
extern int text_color;
extern int keisen_color;
extern int graph_color;
extern int btn_back_color;
extern volatile int TickCount;
extern const short chara_gene[256][14];
extern int lcd_x;						// 0～25
extern int lcd_y;						// 0～14
extern int lcd_noofline;				// １画面の行数
extern char lcd_text_buf[15][26];
extern int timer3_M;
extern const unsigned short m_ushort[4095];
extern const unsigned short i_ushort[4096];
extern int buzzer_flag;					// 0:normal, 1:短音指定(50ms 1KHz), 2:長音指定(500ms 1KHz)
extern int tch_x, tch_y;
extern jmp_buf main_env;


void lcd_write_cmd(int cmd);			// 1byte コマンド送信
void lcd_write_data(int data);			// 1byte data 送信
void lcd_write_color(int color);		// 1pixel write
void lcd_init();
int lcd_area_set(int x1, int y1, int x2, int y2);
int lcd_clearA(int x1, int y1, int x2, int y2, int color);
int lcd_clear(int color);
int lcd_disp_char_x1(int x, int y, char c);
int lcd_disp_char_x2(int x, int y, char c);
void lcd_disp_str_x1(int x, int y, const char *str);
void lcd_disp_str_x2(int x, int y, const char *str);
void lcd_cdisp_str_x1(int y, const char *str);
void lcd_cdisp_str_x2(int y, const char *str);
void wait_1ms();
void wait_ms(int ms);
void wait_1ms_lcd();
void lcd_test();
void lcd_disp_photo1();
void lcd_printf(const char *fmt, ...);
void lcd_putc(char c);
void lcd_puts(const char *str);
void lcd_write_square(int x, int y, int w, int h, const short *pat);
void fatal(const char *msg1, const char *msg2, const char *msg3);
int message(const char *fmt, ...);
void timer_init();
void graph_draw(unsigned char high[251], unsigned char low[251]);
void graph_set(short *ptr, float step, unsigned char high[251], unsigned char low[251], int norm);
void osci_title();
int menu_select(const struct menu *tbl);
void oscillo();
void osci_init();
void cal_main();
int cal_calculator();
int cal_program();
void wait_1us();
int ee_update_ss();
int ee_update_prg();
void ee_read(void *data, int len, int startaddr);
void ee_write(void *data, int len, int startaddr);
void ee_test();
void ee_clear();
void keyin(char *buf, int maxlen);
int menu_pat_select(const struct menu_pat *tbl);
int menu_key(const struct menu *tbl);
int menu_select0(const struct menu *tbl);
int menu_select0_break(const struct menu *tbl);
int menu_select(const struct menu *tbl);
int menu_select_break(const struct menu *tbl);
int menu_select2(const struct menu *tbl);
int menu_select3(const struct menu *tbl);
int menu_select_old(const struct menu_old *tbl);
void fft1024(struct C *x, struct C *X);
void fft_mado1024(struct C *x);
void fft4096(struct C *x, struct C *X);
void fft_mado4096(struct C *x);
float m_calc(struct C *c1);
void m_measure();
int m_freq_set();
void graph_memori_m_linear(const char *footer);
void i_measure();
void calc_save();
void calc_load();
int f7_scrn_shot();
void lcd_read_mem(int x1, int y1, int x2, int y2, short *buf);
int lcd_read_data();
void lcd_write_sub(int data);
int sw_downup();
void f7_main();
void rush_main();
void othe_main();
void pentomino(int flag);
void cc_main();
void hako_main();
void su_main();
int sw_read();
void sw_up();
int sw_downA();
int sw_downB();
int sw_updown();
int tch_read(int *x, int *y);
void tch_input(int *x, int *y, int flag);
void tch_line(int x1, int y1, int x2, int y2, int color);
void tch_drag_write(int x, int y, int w, int h, const short *pat, int toumei_color, short *save);
void tch_drag_recover(int x, int y, int w, int h, short *save);
void draw_main();
void bz_long();
void bz_short();
