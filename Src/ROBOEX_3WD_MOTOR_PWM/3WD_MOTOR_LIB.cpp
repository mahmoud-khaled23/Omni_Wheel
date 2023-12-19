#include "3WD_MOTOR_LIB.h"


int pin_MOTOR[6] = {49,48,43,42,92,93};
int pin_MOTOR_EN[3] = {3,5,6};

void Motor_init(void)
{
	int i;
	for(i=0; i<6; i++)
	{
		pinMode(pin_MOTOR[i], OUTPUT);	// DC 모터 DIR 신호 출력 설정
	}
	for(i=0; i<3; i++)
	{
		pinMode(pin_MOTOR_EN[i], OUTPUT);	// DC 모터 EN 신호 출력 설정
		digitalWrite(pin_MOTOR_EN[i],LOW);	// DC 모터 OFF
	}
}

// 모터 번호에 따른 회전 방향 제어 함수
void MOTOR_DIR(uint8_t num, uint8_t DIR)
{
	switch(DIR)
	{
		case CW:	// 정회전
			digitalWrite(pin_MOTOR[num*2],HIGH);	// xWD_A HIGH
			digitalWrite(pin_MOTOR[num*2+1],LOW);	// xWD_B LOW
			break;
		case CCW:	// 역회전
			digitalWrite(pin_MOTOR[num*2],LOW);		// xWD_A LOW
			digitalWrite(pin_MOTOR[num*2+1],HIGH);	// xWD_B HIGH
			break;
		default:	// 정지
			digitalWrite(pin_MOTOR[num*2],LOW);		// xWD_A LOW
			digitalWrite(pin_MOTOR[num*2+1],LOW);	// xWD_B LOW
			break;
	}
}
