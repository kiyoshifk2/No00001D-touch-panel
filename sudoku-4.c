#include "apps.h"


const static char *Q1[9]={				// 初級x
	"17 9 5 46",
	" 5  4   2",
	"8  7     ",
	"26    437",
	"3   6 5  ",
	" 95 17268",
	"638  2195",
	"5 91  8 4",
	"714598 23"
};
const static char *Q2[9]={				// 中級x
	"    4    ",
	"6   3   1",
	"   75 396",
	"9 3 6  5 ",
	"74     63",
	"8    394 ",
	" 7   9 2 ",
	"     4   ",
	"4 86 1   "
};
const static char *Q3[9]={				// 上級x
	" 4 6     ",
	"   78    ",
	"2      5 ",
	"     3 9 ",
	"        8",
	" 8    1 7",
	"    1   6",
	"  3      ",
	"9 5  2   "
};
const static char *Q4[9]={				// 専門級x
	" 57    48",
	"89  1 72 ",
	"4     6  ",
	" 4  8  3 ",
	"   5 3   ",
	"   2 6   ",
	"3     9  ",
	"26  9 81 ",
	" 89    74"
};
const static char *Q5[9]={				// 最高級x
	"   6   32",
	"5 63  9  ",
	"     2 4 ",
	"  1   49 ",
	"3     2  ",
	"67      8",
	" 1   8   ",
	" 53 4    ",
	"8   7 3 4"
};
const static char *Q6[9]={
	"   7429 3",
	"7  3     ",
	"      58 ",
	"9    3  4",
	"  4  67  ",
	" 85 1 3  ",
	" 7    45 ",
	"4 3      ",
	"6  5     "
};
const static char *Q7[9]={
	"  7      ",
	"   5  8  ",
	" 2  6    ",
	"5  3 6 1 ",
	"   1  58 ",
	"  374    ",
	"    5 79 ",
	" 8   3 2 ",
	"  68 7   "
};


char su_ban_flag[9][9][9];				// 0=配置可能、1=配置不能x
//char su_ban[9][9];						// 1～9 初期配置、-1～-9 テスト配置x
char su_ban[9][9];

struct su_xy {
	char x;
	char y;
	char cnt;
}su_xy[81];
int su_xy_cnt;


/********************************************************************************/
/*		su_ban_init																*/
/*		su_ban をセットする、未確定の位置は配置可能にしておく					*/
/********************************************************************************/
void su_ban_init(const char *tbl[])
{
	int x, y, c;
	
	for(y=0; y<9; y++){
		for(x=0; x<9; x++){
			c = tbl[y][x];
			if(c==' ')
				su_ban[y][x] = 0;		// 未確定 x
			else if(c>='1' || c<='9')
				su_ban[y][x] = c-'0';	// 1～9x
			else{
				message("*** su_ban_init error\n");
				sw_updown();
			}
		}
	}
}
/********************************************************************************/
/*		su_disp_ban																*/
/*		初期配置を白数字、テスト石を青数字で表示する							*/
/********************************************************************************/
void su_disp_ban()
{
	int x, y, d;
	
	for(x=0; x<=9; x++){
		if(x%3==0){							// 太線x
			lcd_clearA(30+x*20-1, 30-1, 30+x*20+1, 30+9*20+1, BLACK);
		}
		else{								// 細線x
			lcd_clearA(30+x*20, 30-1, 30+x*20, 30+9*20+1, BLACK);
		}
	}
	for(y=0; y<=9; y++){
		if(y%3==0){							// 太線x
			lcd_clearA(30-1, 30+y*20-1, 30+9*20+1, 30+y*20+1, BLACK);
		}
		else{								// 細線x
			lcd_clearA(30-1, 30+y*20, 30+9*20+1, 30+y*20, BLACK);
		}
	}
	
	for(y=0; y<9; y++){
		for(x=0; x<9; x++){
			d = su_ban[y][x];
			if(d>0){						// 初期配置データx
				text_color = BLACK;
				lcd_disp_char_x1(30+x*20+4, 30+y*20+3, '0'+d);
			}
			else if(d<0){					// 答えx
				text_color = RED;
				lcd_disp_char_x1(30+x*20+4, 30+y*20+3, '0'-d);
			}
			else{
				lcd_disp_char_x1(30+x*20+4, 30+y*20+3, ' ');
			}
		}
	}
	
	text_color = BLACK;
}
/********************************************************************************/
/*		su_start_test															*/
/*		初期配置で配置できるかどうかテストする									*/
/*		return 1: OK, 0:NG														*/
/********************************************************************************/
int su_start_test()
{
	int d, i, cnt, x1, y1, x2, y2;
	int flag[9];
	
	for(y1=0; y1<9; y1+=3){
		for(x1=0; x1<9; x1+=3){
			memset(flag, 0, sizeof(flag));
			cnt = 0;
			for(y2=0; y2<3; y2++){
				for(x2=0; x2<3; x2++){
					d = su_ban[y1+y2][x1+x2];
					if(d > 0){				// d 確定 1～9
						for(i=0; i<cnt; i++){
							if(flag[i]==d)
								return 0;	// NG
						}
						flag[cnt++] = d;
					}
				}
			}
		}
	}
	for(y1=0; y1<9; y1++){
		memset(flag, 0, sizeof(flag));
		cnt = 0;
		for(x1=0; x1<9; x1++){
			d = su_ban[y1][x1];
			if(d > 0){						// d 0,1～9
				for(i=0; i<cnt; i++){
					if(flag[i]==d)
						return 0;			// NG
				}
				flag[cnt++] = d;
			}
		}
	}
	for(x1=0; x1<9; x1++){
		memset(flag, 0, sizeof(flag));
		cnt = 0;
		for(y1=0; y1<9; y1++){
			d = su_ban[y1][x1];
			if(d > 0){						// d 0,1～9
				for(i=0; i<cnt; i++){
					if(flag[i]==d)
						return 0;			// NG
				}
				flag[cnt++] = d;
			}
		}
	}
	return 1;								// OK
}
/********************************************************************************/
/*		su_simple_analize														*/
/********************************************************************************/
int su_simple_analize(int ff)
{
	int d, i, cnt, n, x1, y1, x2, y2, x3, y3;
	
	for(y1=0; y1<9; y1+=3){
		for(x1=0; x1<9; x1+=3){
			for(y2=0; y2<3; y2++){
				for(x2=0; x2<3; x2++){
					d = su_ban[y1+y2][x1+x2];
					if(d < 0){
						d = -d;
					}
					if(d > 0){				// d 確定 1～9 x
						for(y3=0; y3<9; y3++){
							su_ban_flag[y3][x1+x2][d-1] = 1;	// 配置できない x
						}
						for(x3=0; x3<9; x3++){
							su_ban_flag[y1+y2][x3][d-1] = 1;
						}
						for(y3=y1; y3<y1+3; y3++){
							for(x3=x1; x3<x1+3; x3++){
								su_ban_flag[y3][x3][d-1] = 1;
							}
						}
					}
				}
			}
		}
	}
	n = 0;
	for(y1=0; y1<9; y1++){
		for(x1=0; x1<9; x1++){
			if(su_ban[y1][x1]==0){		// 未確定 x
				cnt = 0;
				for(i=1; i<=9; i++){
					if(su_ban_flag[y1][x1][i-1]==0){	// i が配置可能x
						n = i;
						cnt++;
					}
				}
				if(cnt==0){
					return -1;				// 回答不能問題 x
				}
				if(cnt==1){					// 確定x
					su_ban[y1][x1]= -n;
					return 1;				// 新たに確定したx
				}
			}
		}
	}
	return 0;								// 0:変化なし x
}

int su_analize(int ff)
{
	int ret, x, y;

	for(;;){
		ret = su_simple_analize(ff);
		if(ret==0)
			break;						// 変化しなくなった x
		if(ret > 0)
			continue;					// 変化しているからもう一度 x
		return -1;						// 回答不能問題 x
	}
	for(y=0; y<9; y++){
		for(x=0; x<9; x++){
			if(su_ban[y][x]==0)
				return 0;				// 普通の return x
		}
	}
	return 1;							// 全確定した x
}
/********************************************************************************/
/*		su_select																*/
/********************************************************************************/
void su_select()
{
	int x, y, i, cnt, flag;
	struct su_xy tmp;
	
	su_xy_cnt = 0;
	memset(su_xy, 0, sizeof(su_xy));
	for(y=0; y<9; y++){
		for(x=0; x<9; x++){
			if(su_ban[y][x]==0){		// 未確定x
				su_xy[su_xy_cnt].x = x;
				su_xy[su_xy_cnt].y = y;
				cnt = 0;
				for(i=1; i<=9; i++){
					if(su_ban_flag[y][x][i-1]==0){	// 数字 i 使用可能x
						cnt++;
					}
				}
				su_xy[su_xy_cnt].cnt = cnt;
				su_xy_cnt++;
			}
		}
	}
	for(;;){
		flag = 0;
		for(i=0; i<su_xy_cnt-1; i++){
			if(su_xy[i].cnt > su_xy[i+1].cnt){
				flag = 1;
				tmp = su_xy[i];
				su_xy[i] = su_xy[i+1];
				su_xy[i+1] = tmp;
			}
		}
		if(flag==0)
			return;
	}
}
/********************************************************************************/
/*		su_main																	*/
/********************************************************************************/
int su_solve()
{
	int x, y, i, j, k, n, ret, back;
	
//	printf("\033[2J");fflush(stdout);	// クリア x
//	su_ban_init(Q7);
	if(su_start_test()==0){								// NG
		text_color = RED;
		lcd_disp_str_x1(30+9*20+6, 40, "ﾄｹﾏｾﾝ       ");
		text_color = BLACK;
		return 0;
	}
	memset(su_ban_flag, 0, sizeof(su_ban_flag));		// クリア x
	ret=su_analize(1);					// 初期解析x
	su_disp_ban();
	if(ret < 0){
		text_color = RED;
		lcd_disp_str_x1(30+9*20+6, 40, "ﾄｹﾏｾﾝ       ");
		text_color = BLACK;
		return 0;
	}
	if(ret==1){							// 全確定したx
		lcd_disp_str_x1(30+9*20+6, 40, "ﾄｹﾏｼﾀ       ");
		return 0;
	}
	su_select();
	x = y = n = j = 0;
	back = 0;
	for(i=0; i<su_xy_cnt; ){
		x = su_xy[i].x;
		y = su_xy[i].y;
		n = su_ban[y][x];
		su_ban[y][x] = 0;
		if(n==0){						// 初めてx
			j = 1;
		}
		else{
			if(back==0){
				i++;
				continue;
			}
			j = -n + 1;					// j = 2～10x
		}
		memset(su_ban_flag, 0, sizeof(su_ban_flag));
//		su_disp_ban();					// 表示x
		ret = su_analize(0);			// 繰り返し解析x
		if(ret < 0){
			goto backword;
		}
		if(ret==1){						// 全確定したx
			su_disp_ban();
			lcd_disp_str_x1(30+9*20+6, 40, "ﾄｹﾏｼﾀ       ");
			return 0;
		}

		// su_analize() で設定された値をクリアするx
		if(su_ban[y][x]){
			su_ban_flag[y][x][-su_ban[y][x]-1] = 0;
		}
		for( ; j<=9; j++){
			if(su_ban_flag[y][x][j-1]==0){	// 配置可能x
				break;
			}
		}
		if(j > 9){						// 配置不能x
backword:;
			for(k=i; k<su_xy_cnt; k++){
				x = su_xy[k].x;
				y = su_xy[k].y;
				su_ban[y][x] = 0;
			}
			i--;
			if(i < 0){					// NG
				text_color = RED;
				lcd_disp_str_x1(30+9*20+6, 40, "ﾄｹﾏｾﾝ       ");
				text_color = BLACK;
				return 0;
			}
			back = 1;
			continue;
		}
		/***	配置可能	***/
		su_ban[y][x] = -j;				// とりあえずナンバー設定x
		i++;
		back = 0;
	}
	lcd_disp_str_x1(30+9*20+6, 40, "*** error   ");
	
	return 0;
}
/********************************************************************************/
/*		su_question																*/
/********************************************************************************/
const struct menu su_question_menu[]={
	{0,  20, 12, 1, "ﾓﾝﾀﾞｲ No.1"},
	{0,  60, 12, 2, "ﾓﾝﾀﾞｲ No.2"},
	{0, 100, 12, 3, "ﾓﾝﾀﾞｲ No.3"},
	{0, 140, 12, 4, "ﾓﾝﾀﾞｲ No.4"},
	{0, 180, 12, 5, "ﾓﾝﾀﾞｲ No.5"},
	{156,20, 12, 6, "ﾓﾝﾀﾞｲ No.6"},
	{156,60, 12, 7, "ﾓﾝﾀﾞｲ No.7"},
	{0,0,0,0,0}
};


void su_question()
{
	int cmd;
	const char **ptr;
	
	lcd_clear(BLACK);
    btn_back_color = CYAN;
	cmd = menu_select(su_question_menu);
	lcd_clear(WHITE);
	switch(cmd){
		case 1:
			ptr = Q1;
			break;
		case 2:
			ptr = Q2;
			break;
		case 3:
			ptr = Q3;
			break;
		case 4:
			ptr = Q4;
			break;
		case 5:
			ptr = Q5;
			break;
		case 6:
			ptr = Q6;
			break;
		case 7:
			ptr = Q7;
			break;
		default:
			return;
	}
	
	lcd_clear(WHITE);
	su_ban_init(ptr);
    su_disp_ban();
	lcd_disp_str_x1(30+9*20+6, 40, "<touch>");
    sw_updown();
	su_solve();
	sw_updown();
}
/********************************************************************************/
/*		su_input																*/
/********************************************************************************/
const struct menu su_input_menu[]={
	{230, 20, 2, 1, "1"},
	{230, 40, 2, 2, "2"},
	{230, 60, 2, 3, "3"},
	{230, 80, 2, 4, "4"},
	{230,100, 2, 5, "5"},
	{230,120, 2, 6, "6"},
	{230,140, 2, 7, "7"},
	{230,160, 2, 8, "8"},
	{230,180, 2, 9, "9"},
	{230,200, 2, 100," "},
	{0,0,0,0,0},
};

void su_input()
{
	int x, y, sw, cmd;
	
	memset(su_ban, 0, sizeof(su_ban));
	for(;;){
		/***	駒を選択する	***/
		lcd_clear(WHITE);
		su_disp_ban();
		lcd_disp_str_x1(30+90-18, 30+9*20+2, "end");
		for(;;){
			sw = sw_updown();
			if(sw & 2)						// sw2: exit
				return;
			if(sw & 4){						// touch
				if(tch_x >= 30+90-18 && tch_x <= 30+90-18+3*12-1 &&
						tch_y >= 30+9*20+3 && tch_y <= 30+9*20+3+16-1)
					goto next1;				// "end"
				x = (tch_x-30)/20;
				y = (tch_y-30)/20;
				if(x>=0 && x<9 && y>=0 && y<9){
					back_color = BLACK;
					lcd_disp_char_x1(30+x*20+4, 30+y*20+3, ' ');	// カーソル表示
					back_color = WHITE;
					break;
				}
			}
		}
		/***	数字を設定する	***/
//		lcd_clear(BLACK);
    	btn_back_color = MAGENTA;
		cmd = menu_select(su_input_menu);
//		lcd_clear(WHITE);
		if(cmd==100)
			su_ban[y][x] = 0;
		else
			su_ban[y][x] = cmd;
	}
next1:;
	
    lcd_clear(WHITE);
    su_disp_ban();
	lcd_disp_str_x1(30+9*20+6, 40, "<touch>");
	sw_updown();
	su_solve();
	sw_updown();
}
/********************************************************************************/
/*		su_main																	*/
/********************************************************************************/
const struct menu su_main_menu[]={
	{80, 60, 12,  1, "ﾓﾝﾀﾞｲｼｭｳ"},
	{80,120, 12,  2, "ﾃﾆｭｳﾘｮｸ"},
	{0,0,0,0,0}
};

void su_main()
{
	int cmd;
	
    lcd_clear(BLACK);
    btn_back_color = BRUE;
	cmd = menu_select0_break(su_main_menu);
	lcd_clear(WHITE);
	if(cmd==1){							// 問題集
		su_question();
    }
	else if(cmd==2){						// 手入力
		su_input();
    }
}
