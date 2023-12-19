#include "ROBOEX_3WD_TIMER.h"
#include "TEXT_LCD.h"
#include "Timer_Int.h"

int pin_ID[4] = {10,11,12,13};

uint8_t DIP_Switch_flag = 0,DIP_Switch_flag_pre = 0;

uint8_t ID_read(void);
void Timer_ISR(void);

void setup()
{
	int z;
	for(z=0;z<4;z++)
	{
		pinMode(pin_ID[z],INPUT);
	}

	LCD_init();

	LCD_write(0,0x80);
	LCD_printf("Switch     1234");
	LCD_write(0,0xC0);
	LCD_printf("ID value : 0000");

	startTimer(TC3_IRQn,5,Timer_ISR);
}

void loop()
{
	if(DIP_Switch_flag != DIP_Switch_flag_pre)
	{
		DIP_Switch_flag_pre = DIP_Switch_flag;
		LCD_write(0,0xCB);
		for(int z=0;z<4;z++)
			LCD_write(1,(DIP_Switch_flag_pre>>z & 0x01) + '0');
	}
}

uint8_t ID_read(void)
{
	uint8_t read_data=0;
	for(int z=0;z<4;z++)
		read_data |= !digitalRead(pin_ID[z])<<z;
	return read_data;
}

void Timer_ISR(void)
{
	DIP_Switch_flag = ID_read();
}
