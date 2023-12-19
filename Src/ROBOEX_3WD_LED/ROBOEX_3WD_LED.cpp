// Do not remove the include below
#include "ROBOEX_3WD_LED.h"

int pin_LED[2] = {45,44}; //Green, Yellow

void LED_control(uint8_t da);

//The setup function is called once at startup of the sketch
void setup()
{
// Add your initialization code here
	for(int i=0; i<2; i++)
	{
		pinMode(pin_LED[i],OUTPUT);		// LED 포트 출력 설정
		digitalWrite(pin_LED[i],LOW);	// LED OFF
	}
}

// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here
	for(int i=0; i<4; i++)
	{
		LED_control(i);	// LED를 i 값에 따라 제어
		delay(500);
	}
}

void LED_control(uint8_t da)
{
	for(int i=0; i<2; i++)
	{
		digitalWrite(pin_LED[i],da>>i & 0x01);
	}
}
