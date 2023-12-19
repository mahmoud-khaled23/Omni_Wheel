#include "ROBOEX_3WD_MOTOR_PWM.h"
#include "TEXT_LCD.h"
#include "3WD_MOTOR_LIB.h"

enum{num1, num2, num3}; // ���� ��ȣ

void Speed_control(uint8_t num, uint16_t speed);

void setup()
{
	Motor_init();	// ���� ���� ��ȣ ��� ����

	LCD_init();	// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("PWM Control");

	MOTOR_DIR(num1,CW);	// 1WD ��ȸ��
}

void loop()
{
	int speed1;
	for(speed1=0; speed1<256; speed1++)		// �ӵ� ����
	{
		Speed_control(num1,speed1);
		delay(20);
	}
	for(speed1=255; speed1>=0; speed1--)	// �ӵ� ����
	{
		Speed_control(num1,speed1);
		delay(20);
	}
}

// ������ �ӵ��� �����ϴ� �Լ�
// xWD_EN ��ȣ PWM ����
void Speed_control(uint8_t num, uint16_t speed)
{
	analogWrite(pin_MOTOR_EN[num],speed);
}
