#include "apps.h"

long double calc_acc, calc_input, calc_mem;
char calc_str[30];
int calc_disp_flag;						// 0:calc_input表示中、1:calc_acc表示中、2:calc_str表示中
int calc_ope;

void calc_d2a(long double a, char *str);
long double calc_a2d(char *str);
void calc_disp();

/********************************************************************************/
/*		calc_main																*/
/********************************************************************************/
struct menu calc_main_menu[]={
	{15, 60, 3, 100, "MR"},
	{15,100, 3, 101, "MI"},
	{15,140, 3, 102, "C"},
	{15,180, 3, 103, "AC"},
	
	{63, 60, 3, '7', "7"},
	{63,100, 3, '4', "4"},
	{63,140, 3, '1', "1"},
	{63,180, 3, '0', "0"},
	
	{111, 60, 3, '8', "8"},
	{111,100, 3, '5', "5"},
	{111,140, 3, '2', "2"},
	{111,180, 3, '.', "."},
	
	{159, 60, 3, '9', "9"},
	{159,100, 3, '6', "6"},
	{159,140, 3, '3', "3"},
	{159,180, 3, '=', "="},
	
	{207, 60, 3, '/', "/"},
	{207,100, 3, 'X', "X"},
	{207,140, 3, '-', "-"},
	{207,180, 3, '+', "+"},
	
//	{255, 60, 3, 104, "sqr"},
	{0,0,0,0,0},
};

void calc_main()
{
	int cmd, len;
	
	lcd_clear(BLACK);
	for(;;){
		calc_disp();
		btn_back_color = YELLOW;
		cmd = menu_select_break(calc_main_menu);
		switch(cmd){
			case 100:						// MR
				calc_d2a(calc_mem, calc_str);
				calc_disp_flag = 2;			// calc_str 表示中
				break;
			case 101:						// MI
				if(calc_disp_flag==0)		// input
					calc_mem = calc_input;
				else if(calc_disp_flag==1)	// acc
					calc_mem = calc_acc;
				else if(calc_disp_flag==2){	// calc_str
					calc_mem = calc_a2d(calc_str);
				}
				break;
			case 102:						// C
				calc_input = 0;
				calc_disp_flag = 0;			// input
				break;
			case 103:						// AC
				calc_acc = calc_input = 0;
				calc_str[0] = 0;
				calc_disp_flag = 0;
				calc_ope = 0;
				break;
			case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '.':
				if(calc_disp_flag != 2){	// calc_str表示中でなければ
					calc_str[0] = 0;
					calc_disp_flag = 2;		// calc_str表示中にする
				}
				len = strlen(calc_str);
				calc_str[len] = cmd;
				calc_str[len+1] = 0;
				break;
			case '/':case 'X':case '-':case '+':case '=':
				if(calc_ope=='/'){
					if(calc_disp_flag==2){		// calc_str表示中
						calc_input = calc_a2d(calc_str);
						calc_acc /= calc_input;
					}
				}
				else if(calc_ope=='X'){
					if(calc_disp_flag==2){		// calc_str表示中
						calc_input = calc_a2d(calc_str);
						calc_acc *= calc_input;
					}
				}
				else if(calc_ope=='-'){
					if(calc_disp_flag==2){		// calc_str表示中
						calc_input = calc_a2d(calc_str);
						calc_acc -= calc_input;
					}
				}
				else if(calc_ope=='+'){
					if(calc_disp_flag==2){		// calc_str表示中
						calc_input = calc_a2d(calc_str);
						calc_acc += calc_input;
					}
				}
				else{
					if(calc_disp_flag==2){		// calc_str表示中
						calc_input = calc_a2d(calc_str);
						calc_acc = calc_input;
					}
				}
				calc_disp_flag = 1;				// acc 表示中
				calc_str[0] = 0;
				calc_ope = cmd;
				break;
		}
	}
}
/********************************************************************************/
/*		calc_d2a																*/
/********************************************************************************/
void calc_d2a(long double a, char *str)
{
//	sprintf(str, "%lf", a);

	int b, len;
	long long l;
	char *pt;
	
	b = 0;
	if(a==0){
		strcpy(str, "0");
		return;
	}
	if(a < 0){
		a = -a;
		str[0] = '-';
		pt = &str[1];
	}
	else{
		pt = str;
	}
	if(a >= 10000000000){
		strcpy(str, "overflow");
		return;
	}
	if(a < 0.000000001){
		strcpy(str, "underflow");
		return;
	}
	for(;;){
		l = (long long)a+5;
		if(l >= 10000000000){
			if(b){
				l /= 10;
				--b;
			}
			sprintf(pt, "000000000000%lld", l);
			/*** お尻の０削除	***/
			while(b && pt[strlen(pt)-1]=='0'){
				--b;
				pt[strlen(pt)-1] = 0;
			}
			/*** 小数点挿入	***/
			len = strlen(pt);
			if(b){
				memmove(pt+len-b+1, pt+len-b, b+1);
				pt[len-b] = '.';
			}
			/*** 頭の０削除	***/
			while(pt[0]=='0' && pt[1]!='.'){
				memmove(pt, pt+1, 25);
			}
            return;
		}
		a *= 10;
		++b;
	}
}
/********************************************************************************/
/*		calc_a2d																*/
/********************************************************************************/
long double calc_a2d(char *str)
{
//	return (long double)atof(str);

	long double a, b;
	
	a = b = 0;
	while(true){
		if(*str=='.'){
			if(b==0)
				b = 1;
		}
		else if(*str>='0' && *str<='9'){
			a = a*10 + *str - '0';
			if(b)
				b *= 10;
		}
		else{
			if(b)
				return a / b;
			else
				return a;
		}
		str++;
	}
}
/********************************************************************************/
/*		calc_disp																*/
/********************************************************************************/
void calc_disp()
{
	if(calc_disp_flag==0){				// calc_input 表示
		calc_d2a(calc_input, calc_str);
	}
	else if(calc_disp_flag==1){			// calc_acc 表示
		calc_d2a(calc_acc, calc_str);
	}
	else if(calc_disp_flag==2){			// calc_str 表示
	}
	else{								// error
		strcpy(calc_str, "*** bug1");
	}
	lcd_clearA(0,0,319,40,BLACK);
	lcd_disp_str_x2(0,0,calc_str);
}
