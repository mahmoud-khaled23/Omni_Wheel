#include "ROBOEX_3WD_MOTOR_GPIO.h"
#include "TEXT_LCD.h"

enum{STOP,CW,CCW};		// 모터 회전 방향
enum{num1, num2, num3}; // 모터 번호

// 모터 방향 제어신호
int pin_MOTOR[6] = {49,48,43,42,92,93};	// 1WD_A, 1WD_B, 2WD_A, 2WD_B, 3WD_A, 3WD_B
// 모터 ON/OFF 제어 신호
int pin_MOTOR_EN[3] = {3,5,6};			// 1WD_EN, 2WD_EN, 3WD_EN


void MOTOR_DIR(uint8_t num, uint8_t DIR);

void setup()
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

	LCD_init();	// LCD 초기화

	// LCD 출력
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("DC Motor Control");
}

void loop()
{
	MOTOR_DIR(num1,CW);		// 1번 모터 정회전
	digitalWrite(pin_MOTOR_EN[num1],HIGH);	// 1번 모터 enable
	delay(4000);
	digitalWrite(pin_MOTOR_EN[num1],LOW);	// 1번 모터 disable
	delay(100);
	MOTOR_DIR(num1,CCW);	// 1번 모터 역회전
	digitalWrite(pin_MOTOR_EN[num1],HIGH);	// 1번 모터 enable
	delay(4000);
	digitalWrite(pin_MOTOR_EN[num1],LOW);	// 1번 모터 disable
	delay(100);
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
