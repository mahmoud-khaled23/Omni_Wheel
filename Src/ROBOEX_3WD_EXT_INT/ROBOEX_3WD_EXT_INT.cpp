#include "ROBOEX_3WD_EXT_INT.h"
#include "TEXT_LCD.h"

int pin_ID[4] = {10,11,12,13};

uint8_t DIP_Switch_flag = 0,DIP_Switch_flag_pre = 0;

void ID_0(void);
void ID_1(void);
void ID_2(void);
void ID_3(void);
void Timer_ISR(void);

void setup()
{
	int i;

	attachInterrupt(pin_ID[0],ID_0,CHANGE);
	attachInterrupt(pin_ID[1],ID_1,CHANGE);
	attachInterrupt(pin_ID[2],ID_2,CHANGE);
	attachInterrupt(pin_ID[3],ID_3,CHANGE);

	LCD_init();

	LCD_write(0,0x80);
	LCD_printf("Switch     1234");
	LCD_write(0,0xC0);
	LCD_printf("ID value : 0000");


	DIP_Switch_flag = 0;
}

void loop()
{
	if(DIP_Switch_flag != DIP_Switch_flag_pre)
	{
		DIP_Switch_flag_pre = DIP_Switch_flag;
		LCD_write(0,0xCB);
		for(int i=0;i<4;i++)
			LCD_write(1,(DIP_Switch_flag_pre>>i & 0x01) + '0');
	}
}

void ID_0(void)
{
	if(digitalRead(pin_ID[0]))
		DIP_Switch_flag &= ~0x01;
	else
		DIP_Switch_flag |= 0x01;
}
void ID_1(void)
{
	if(digitalRead(pin_ID[1]))
		DIP_Switch_flag &= ~0x02;
	else
		DIP_Switch_flag |= 0x02;
}
void ID_2(void)
{
	if(digitalRead(pin_ID[2]))
		DIP_Switch_flag &= ~0x04;
	else
		DIP_Switch_flag |= 0x04;
}
void ID_3(void)
{
	if(digitalRead(pin_ID[3]))
		DIP_Switch_flag &= ~0x08;
	else
		DIP_Switch_flag |= 0x08;
}
