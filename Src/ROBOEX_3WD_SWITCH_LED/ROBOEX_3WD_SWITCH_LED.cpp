#include "ROBOEX_3WD_SWITCH_LED.h"

int pin_LED[2] = {45,44};	//Green, Yellow
int pin_SWITCH[5] = {38,70,71,72,65};	//Enter, Up, Down, Left, Right

uint8_t Switch_read(void);
void LED_control(uint8_t da);

void setup()
{
	int i;
	for( i=0; i<2; i++)
	{
		pinMode(pin_LED[i],OUTPUT);
		digitalWrite(pin_LED[i],LOW);
	}
	for(i=0; i<5; i++)
	{
		pinMode(pin_SWITCH[i],INPUT);
	}
}

void loop()
{
	switch(Switch_read())
	{
		case 1:
			LED_control(1);
			break;
		case 2:
			LED_control(2);
			break;
		case 3:
		case 4:
			LED_control(3);
			break;
		default:
			LED_control(0);
	}
	delay(100);
}

uint8_t Switch_read(void)
{
	uint8_t i,da=0;
	for(i=0; i<3; i++)
	{
		da |= !digitalRead(pin_SWITCH[i])<<i;
	}
	return da;
}

void LED_control(uint8_t da)
{
	for(int i=0; i<2; i++)
	{
		digitalWrite(pin_LED[i],da>>i & 0x01);
	}
}
