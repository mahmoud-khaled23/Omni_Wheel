#include "ROBOEX_3WD_BUZZER.h"

int pin_BUZZER = 7;	// ���� �� ��ȣ ����

void setup()
{
	pinMode(pin_BUZZER,OUTPUT);		// ���� ��Ʈ�� ������� ����
	digitalWrite(pin_BUZZER,LOW);	// ���� OFF
}

// The loop function is called in an endless loop
void loop()
{
	digitalWrite(pin_BUZZER,HIGH);	// ���� OFF
	delay(1000);					// 1000ms �ð�����
	digitalWrite(pin_BUZZER,LOW);	// ���� OFF
	delay(1000);					// 1000ms �ð�����
}
