#include "TEXT_LCD.h"

int pin_LCD_DATA[8] = {22,23,24,25,26,27,28,29};
int pin_LCD_CTRL[4] = {40,41,39,108};	//RS,RW,E,LCD_DIR		//LCD_DIR -> H-write, L-Read

// LCD�� 1byte ���/�����͸� ����ϴ� �Լ�
void LCD_write(unsigned char rs, char data)
{
	digitalWrite(pin_LCD_CTRL[0],rs);	// ���/������ ��ȣ ����
	LCD_data_out(data);					// ������ ���
	delayMicroseconds(1);
	digitalWrite(pin_LCD_CTRL[2],HIGH);	// Enable ��ȣ HIGH
	delayMicroseconds(1);
	digitalWrite(pin_LCD_CTRL[2],LOW);	// Enable ��ȣ LOW
	delayMicroseconds(41);
}

// LCD data ��Ʈ�� da ���� ����ϱ� ���� �Լ�
void LCD_data_out(char da)
{
	int z;
	for(z=0; z<8; z++)
	{
		digitalWrite(pin_LCD_DATA[z],(da>>z) & 0x1);
	}
}

// LCD �ʱ�ȭ�ϴ� �Լ�
void LCD_init(void)
{
	digitalWrite(pin_LCD_CTRL[3],HIGH);	// MCU->LCD�� �����͸� �������� HIGH ���
	digitalWrite(pin_LCD_CTRL[2],LOW);	// Enable ��ȣ LOW�� �ʱ�ȭ
	digitalWrite(pin_LCD_CTRL[1],LOW);	// ���/�����͸� �������� RW��ȣ HIGH ���

	LCD_write(0,0x38);		//8bit data mode, 2 line, 5x7 dot
	LCD_write(0,0x38);		//8bit data mode, 2 line, 5x7 dot
	LCD_write(0,0x0C);		//display on
	LCD_write(0,0x01);		//Display Clear
	delay(2);
	LCD_write(0,0x06);		//Entry mode
}

// LCD�� ���ڿ��� ����ϴ� �Լ�
void LCD_printf(char *str)
{
	unsigned char i=0;
	while(str[i]!='\0')
	{
		LCD_write(1, str[i++]);
	}
}
