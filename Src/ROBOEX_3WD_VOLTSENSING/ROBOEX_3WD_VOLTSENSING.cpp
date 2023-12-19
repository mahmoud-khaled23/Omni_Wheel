#include "ROBOEX_3WD_VOLTSENSING.h"
#include "TEXT_LCD.h"

int pin_VOLT_SEN = A1;

void setup()
{
	LCD_init();

	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX MOTOR");
	LCD_write(0,0xC0);
	LCD_printf("POWER :  12.0[V]");

	analogReadResolution(12);
}

void loop()
{
	int volt=0, adc_data = analogRead(pin_VOLT_SEN);

	volt = (float)adc_data / 124.09 * 4.11875;	//adc_data/4095*3.3* 10 * 4.11875
								//측정전압 1.6/6.59임 따라서 1/4.11875임
	LCD_write(0,0xC9);
	if(volt >= 100)
		LCD_write(1,volt/100 + '0');
	else
		LCD_write(1,' ');
	LCD_write(1,(volt/10)%10 + '0');
	LCD_write(1,'.');
	LCD_write(1,volt%10 + '0');
	delay(500);
}
