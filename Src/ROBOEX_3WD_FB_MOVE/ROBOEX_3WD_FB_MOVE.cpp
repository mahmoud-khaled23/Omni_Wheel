#include "ROBOEX_3WD_FB_MOVE.h"
#include "TEXT_LCD.h"
#include "3WD_MOTOR_LIB.h"

enum{num1, num2, num3}; // ���� ��ȣ

void setup()
{
	int i;
	Motor_init();	// ���� �ʱ�ȭ

	LCD_init();		// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("FB MOVE");

	for(i=0; i<3; i++)
	{
		Speed_control(i, 255);	// ���� �ӵ� �ִ�
		MOTOR_DIR(i,STOP);		// ���� ����
	}
}

void loop()
{
	// ����
	MOTOR_DIR(num2,CW);		// 2WD ��ȸ��
	MOTOR_DIR(num3,CCW);	// 3WD ��ȸ��
	delay(4000);
	//����
	MOTOR_DIR(num2,STOP);
	MOTOR_DIR(num3,STOP);
	delay(1000);
	// ����
	MOTOR_DIR(num2,CCW);	// 2WD ��ȸ��
	MOTOR_DIR(num3,CW);		// 3WD ��ȸ��
	delay(4000);
	//����
	MOTOR_DIR(num2,STOP);
	MOTOR_DIR(num3,STOP);
	delay(1000);
}
