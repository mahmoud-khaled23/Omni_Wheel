#include "ROBOEX_3WD_MOTOR_PWM.h"
#include "TEXT_LCD.h"
#include "3WD_MOTOR_LIB.h"

enum{num1, num2, num3}; // 모터 번호

void Speed_control(uint8_t num, uint16_t speed);

void setup()
{
	Motor_init();	// 모터 제어 신호 출력 설정

	LCD_init();	// LCD 초기화

	// LCD 출력
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("PWM Control");

	MOTOR_DIR(num1,CW);	// 1WD 정회전
}

void loop()
{
	int speed1;
	for(speed1=0; speed1<256; speed1++)		// 속도 증가
	{
		Speed_control(num1,speed1);
		delay(20);
	}
	for(speed1=255; speed1>=0; speed1--)	// 속도 감소
	{
		Speed_control(num1,speed1);
		delay(20);
	}
}

// 모터의 속도를 제어하는 함수
// xWD_EN 신호 PWM 제어
void Speed_control(uint8_t num, uint16_t speed)
{
	analogWrite(pin_MOTOR_EN[num],speed);
}
