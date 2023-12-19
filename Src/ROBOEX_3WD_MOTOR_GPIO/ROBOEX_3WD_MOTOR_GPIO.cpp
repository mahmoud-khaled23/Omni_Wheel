#include "ROBOEX_3WD_MOTOR_GPIO.h"
#include "TEXT_LCD.h"

enum{STOP,CW,CCW};		// ���� ȸ�� ����
enum{num1, num2, num3}; // ���� ��ȣ

// ���� ���� �����ȣ
int pin_MOTOR[6] = {49,48,43,42,92,93};	// 1WD_A, 1WD_B, 2WD_A, 2WD_B, 3WD_A, 3WD_B
// ���� ON/OFF ���� ��ȣ
int pin_MOTOR_EN[3] = {3,5,6};			// 1WD_EN, 2WD_EN, 3WD_EN


void MOTOR_DIR(uint8_t num, uint8_t DIR);

void setup()
{
	int i;
	for(i=0; i<6; i++)
	{
		pinMode(pin_MOTOR[i], OUTPUT);	// DC ���� DIR ��ȣ ��� ����
	}
	for(i=0; i<3; i++)
	{
		pinMode(pin_MOTOR_EN[i], OUTPUT);	// DC ���� EN ��ȣ ��� ����
		digitalWrite(pin_MOTOR_EN[i],LOW);	// DC ���� OFF
	}

	LCD_init();	// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("DC Motor Control");
}

void loop()
{
	MOTOR_DIR(num1,CW);		// 1�� ���� ��ȸ��
	digitalWrite(pin_MOTOR_EN[num1],HIGH);	// 1�� ���� enable
	delay(4000);
	digitalWrite(pin_MOTOR_EN[num1],LOW);	// 1�� ���� disable
	delay(100);
	MOTOR_DIR(num1,CCW);	// 1�� ���� ��ȸ��
	digitalWrite(pin_MOTOR_EN[num1],HIGH);	// 1�� ���� enable
	delay(4000);
	digitalWrite(pin_MOTOR_EN[num1],LOW);	// 1�� ���� disable
	delay(100);
}

// ���� ��ȣ�� ���� ȸ�� ���� ���� �Լ�
void MOTOR_DIR(uint8_t num, uint8_t DIR)
{
	switch(DIR)
	{
		case CW:	// ��ȸ��
			digitalWrite(pin_MOTOR[num*2],HIGH);	// xWD_A HIGH
			digitalWrite(pin_MOTOR[num*2+1],LOW);	// xWD_B LOW
			break;
		case CCW:	// ��ȸ��
			digitalWrite(pin_MOTOR[num*2],LOW);		// xWD_A LOW
			digitalWrite(pin_MOTOR[num*2+1],HIGH);	// xWD_B HIGH
			break;
		default:	// ����
			digitalWrite(pin_MOTOR[num*2],LOW);		// xWD_A LOW
			digitalWrite(pin_MOTOR[num*2+1],LOW);	// xWD_B LOW
			break;
	}
}
