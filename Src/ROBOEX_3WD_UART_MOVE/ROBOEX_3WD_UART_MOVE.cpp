#include "ROBOEX_3WD_UART_MOVE.h"
#include "TEXT_LCD.h"
#include "3WD_MOTOR_LIB.h"

#define FORWARD		0x24
#define BACK		0x18
#define LEFT		0x15
#define RIGHT		0x2A
#define STOP		0

void Motor_Move(uint8_t dir);

void setup()
{
	int i;
	Serial.begin(115200);	// UART0 115200 ����

	Motor_init();	// ���� �ʱ�ȭ

	LCD_init();		// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("UART MOVE");

	for(i=0; i<3; i++)
	{
		Speed_control(i, 255);	// ���� �ӵ� �ִ�
		MOTOR_DIR(i,STOP);		// ���� ����
	}
}

void loop()
{
}

// UART0 ���� ���ͷ�Ʈ ���� ��ƾ
void serialEvent(void)
{
	char da;
	da = Serial.read();	// ���ŵ����� da�� ����
	Serial.write(da);	// ���ŵ����� �� �۽�
	switch(da)
	{
		case 'y':
			Motor_Move(FORWARD);
			Serial.println(" FORWARD");
			break;
		case 'b':
			Motor_Move(BACK);
			Serial.println(" BACK");
			break;
		case 'g':
			Motor_Move(LEFT);
			Serial.println(" LEFT");
			break;
		case 'j':
			Motor_Move(RIGHT);
			Serial.println(" RIGHT");
			break;
		case 's':
			Motor_Move(STOP);
			Serial.println(" STOP");
			break;
	}
}

void Motor_Move(uint8_t dir)
{
	int i;
	for(i=0; i<3; i++)
	{
		MOTOR_DIR(i, (dir >> (i*2)) & 0x03);
	}
}
