#include "TEXT_LCD.h"

int pin_LCD_DATA[8] = {22,23,24,25,26,27,28,29};
int pin_LCD_CTRL[4] = {40,41,39,108};	//RS,RW,E,LCD_DIR		//LCD_DIR -> H-write, L-Read

// LCD에 1byte 명령/데이터를 출력하는 함수
void LCD_write(unsigned char rs, char data)
{
	digitalWrite(pin_LCD_CTRL[0],rs);	// 명령/데이터 신호 제어
	LCD_data_out(data);					// 데이터 출력
	delayMicroseconds(1);
	digitalWrite(pin_LCD_CTRL[2],HIGH);	// Enable 신호 HIGH
	delayMicroseconds(1);
	digitalWrite(pin_LCD_CTRL[2],LOW);	// Enable 신호 LOW
	delayMicroseconds(41);
}

// LCD data 포트에 da 값을 출력하기 위한 함수
void LCD_data_out(char da)
{
	int z;
	for(z=0; z<8; z++)
	{
		digitalWrite(pin_LCD_DATA[z],(da>>z) & 0x1);
	}
}

// LCD 초기화하는 함수
void LCD_init(void)
{
	digitalWrite(pin_LCD_CTRL[3],HIGH);	// MCU->LCD로 데이터를 쓰기위해 HIGH 출력
	digitalWrite(pin_LCD_CTRL[2],LOW);	// Enable 신호 LOW로 초기화
	digitalWrite(pin_LCD_CTRL[1],LOW);	// 명령/데이터를 쓰기위해 RW신호 HIGH 출력

	LCD_write(0,0x38);		//8bit data mode, 2 line, 5x7 dot
	LCD_write(0,0x38);		//8bit data mode, 2 line, 5x7 dot
	LCD_write(0,0x0C);		//display on
	LCD_write(0,0x01);		//Display Clear
	delay(2);
	LCD_write(0,0x06);		//Entry mode
}

// LCD에 문자열을 출력하는 함수
void LCD_printf(char *str)
{
	unsigned char i=0;
	while(str[i]!='\0')
	{
		LCD_write(1, str[i++]);
	}
}
