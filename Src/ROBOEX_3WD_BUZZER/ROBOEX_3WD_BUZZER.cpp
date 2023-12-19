#include "ROBOEX_3WD_BUZZER.h"

int pin_BUZZER = 7;	// 부저 핀 번호 저장

void setup()
{
	pinMode(pin_BUZZER,OUTPUT);		// 부저 포트를 출력으로 설정
	digitalWrite(pin_BUZZER,LOW);	// 부저 OFF
}

// The loop function is called in an endless loop
void loop()
{
	digitalWrite(pin_BUZZER,HIGH);	// 부저 OFF
	delay(1000);					// 1000ms 시간지연
	digitalWrite(pin_BUZZER,LOW);	// 부저 OFF
	delay(1000);					// 1000ms 시간지연
}
