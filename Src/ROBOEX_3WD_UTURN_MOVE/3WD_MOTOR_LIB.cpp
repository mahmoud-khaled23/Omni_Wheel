#include "3WD_MOTOR_LIB.h"


int pin_MOTOR[6] = {49,48,43,42,92,93};
int pin_MOTOR_EN[3] = {3,5,6};

void Motor_init(void)
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

// ������ �ӵ��� �����ϴ� �Լ�
// xWD_EN ��ȣ PWM ����
void Speed_control(uint8_t num, uint16_t speed)
{
	analogWrite(pin_MOTOR_EN[num],speed);
}
