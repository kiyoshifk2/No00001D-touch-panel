#include "apps.h"


void ee_write_sub(char *data, int len, int startaddr);

/********************************************************************************/
/*		ee_update_ss															*/
/*		31K 以後をパラメータ用に使用する										*/
/*		ss: スクリーンショット通し番号											*/
/********************************************************************************/
int ee_update_ss()
{
	struct eeprom_data data;
	
	ee_read(&data, sizeof(data), 0x400 * 31);
	++data.ss_numb;
	ee_write(&data, sizeof(data), 0x400 * 31);
	return data.ss_numb;
}
int ee_update_prg()
{
	struct eeprom_data data;
	
	ee_read(&data, sizeof(data), 0x400 * 31);
	++data.prg_numb;
	ee_write(&data, sizeof(data), 0x400 * 31);
	return data.prg_numb;
}
/********************************************************************************/
/*		wait_1us																*/
/*		at 80MHz clock															*/
/********************************************************************************/
void wait_1us()
{
	volatile int i;
	
    i = 1;
//    i = j;
	for(i=0; i<6; i++)
		;
}
/********************************************************************************/
/*		ee_send																	*/
/*		send 1 byte																*/
/*		return Ack																*/
/********************************************************************************/
int ee_send(int data)
{
	int i, ack;
	
	for(i=0; i<8; i++){						// 8bit 送出
		LATDCLR = EEPROM_SCL;				// SCL = 0
		wait_1us();
		if(data & 0x80)						// 送信
			LATDSET = EEPROM_SDA;
		else
			LATDCLR = EEPROM_SDA;
		data <<= 1;
		wait_1us();
		
		LATDSET = EEPROM_SCL;				// SCL = 1
		wait_1us();
		wait_1us();
	}
	/*** Ack サイクル	***/
	LATDCLR = EEPROM_SCL;					// SCL = 0
	wait_1us();
	LATDSET = EEPROM_SDA;					// SDA = 1
	wait_1us();
	
	LATDSET = EEPROM_SCL;					// SCL = 1
	wait_1us();
	ack = PORTD & EEPROM_SDA;				// 受信
	wait_1us();
	return ack;
}
/********************************************************************************/
/*		ee_recv																	*/
/*		return: received data, Ack: 1 or 0 send									*/
/********************************************************************************/
int ee_recv(int ack)
{
	int i, data;
	
	data = 0;
	for(i=0; i<8; i++){						// 8bit 受信
		LATDCLR = EEPROM_SCL;				// SCL = 0
		wait_1us();
		LATDSET = EEPROM_SDA;				// SDA = 1
		wait_1us();
		
		LATDSET = EEPROM_SCL;				// SCL = 1
		wait_1us();
		data <<= 1;
		if(PORTD & EEPROM_SDA)				// 受信
			data |= 1;
		wait_1us();
	}
	/*** Ack サイクル	***/
	LATDCLR = EEPROM_SCL;					// SCL = 0
	wait_1us();
	if(ack)									// 送信
		LATDSET = EEPROM_SDA;
	else
		LATDCLR = EEPROM_SDA;
	wait_1us();
	
	LATDSET = EEPROM_SCL;					// SCL = 1
	wait_1us();
	wait_1us();
	return data;
}
/********************************************************************************/
/*		ee_start																*/
/********************************************************************************/
void ee_start()
{
	LATDCLR = EEPROM_SCL;					// SCL = 0
	wait_1us();
	LATDSET = EEPROM_SDA;					// SDA = 1
	wait_1us();
	
	LATDSET = EEPROM_SCL;					// SCL = 1
	wait_1us();
	LATDCLR = EEPROM_SDA;					// SDA = 0  START
	wait_1us();
}
/********************************************************************************/
/*		ee_stop																	*/
/********************************************************************************/
void ee_stop()
{
	LATDCLR = EEPROM_SCL;					// SCL = 0
	wait_1us();
	LATDCLR = EEPROM_SDA;					// SDA = 0
	wait_1us();
	LATDSET = EEPROM_SCL;					// SCL = 1;
	wait_1us();
	LATDSET = EEPROM_SDA;					// SDA = 1
	wait_1us();
}
/********************************************************************************/
/*		ee_addr_set																*/
/********************************************************************************/
void ee_addr_set(int startaddr)
{
    int i;
    
	for(i=0; i<20000; i++){
		ee_start();
		if(ee_send(0xa0)==0)				// send write command
			break;							// Ack OK
	}
    if(i==20000)
        fatal("eeprom busy10",0,0);
	if(ee_send(startaddr >> 8))				// high addr send
		fatal("eeprom no Ack11",0,0);
	if(ee_send(startaddr))					// low addr send
		fatal("eeprom no Ack12",0,0);
}
/********************************************************************************/
/*		ee_write																*/
/*      1 バイト当たり 40μs                                                     */
/********************************************************************************/
void ee_write(void *datax, int len, int startaddr)
{
	int end, nextpagetop, byte;
    char *data = (char*)datax;
	
	end = startaddr + len;
	for(;;){
		nextpagetop = (startaddr & ~(64-1)) + 64;
		if(end > nextpagetop){
			byte = nextpagetop - startaddr;
			ee_write_sub(data, byte, startaddr);
			data += byte;
			startaddr += byte;
		}
		else{
			byte = end - startaddr;
			ee_write_sub(data, byte, startaddr);
			break;
		}
	}
}
//
//		len は 64 以下であること
void ee_write_sub(char *data, int len, int startaddr)
{
	int i;
	
	ee_addr_set(startaddr);					// START + addr
	for(i=0; i<len; i++){
		if(ee_send(data[i]))				// send data
			fatal("eeprom no Ack13",0,0);
	}
	ee_stop();								// STOP
}
/********************************************************************************/
/*		ee_read																	*/
/*      1 バイト当たり 40μs                                                     */
/********************************************************************************/
void ee_read(void *datax, int len, int startaddr)
{
	int i;
	char *data = (char*)datax;
	
	ee_addr_set(startaddr);					// アドレスセット
	ee_start();								// START
	if(ee_send(0xa1))						// send read command
		fatal("eeprom no Ack14",0,0);
	for(i=0; i< len-1; i++){
		data[i] = ee_recv(0);				// 1 バイト受信 Ack 付
	}
	data[i] = ee_recv(1);					// 1 バイト受信 Ack 無し
	ee_stop();								// STOP
}
/********************************************************************************/
/*		ee_clear																*/
/*		256Kbit (32KByte) 全クリア  2.4秒だった									*/
/********************************************************************************/
void ee_clear()
{
	char buf[64];
	int i, time1;
	
	time1 = TickCount;
	memset(buf, 0, sizeof(buf));
	for(i=0; i<32*1024; i+=64){
		ee_write(buf, 64, i);
	}
	sprintf(buf, "erase time=%dms", (TickCount-time1)/10);
	lcd_clear(WHITE);
	back_color = GREEN;
	text_color = BLACK;
	lcd_cdisp_str_x1(100, buf);
	for(;;)
		;
}
