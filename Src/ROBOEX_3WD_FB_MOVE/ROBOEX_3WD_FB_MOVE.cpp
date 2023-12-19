#include "ROBOEX_3WD_FB_MOVE.h"
#include "TEXT_LCD.h"
#include "3WD_MOTOR_LIB.h"

enum{num1, num2, num3}; // 모터 번호

void setup()
{
	int i;
	Motor_init();	// 모터 초기화

	LCD_init();		// LCD 초기화

	// LCD 출력
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("FB MOVE");

	for(i=0; i<3; i++)
	{
		Speed_control(i, 255);	// 모터 속도 최대
		MOTOR_DIR(i,STOP);		// 모터 정지
	}
}

void loop()
{
	// 전진
	MOTOR_DIR(num2,CW);		// 2WD 정회전
	MOTOR_DIR(num3,CCW);	// 3WD 역회전
	delay(4000);
	//정지
	MOTOR_DIR(num2,STOP);
	MOTOR_DIR(num3,STOP);
	delay(1000);
	// 후진
	MOTOR_DIR(num2,CCW);	// 2WD 역회전
	MOTOR_DIR(num3,CW);		// 3WD 정회전
	delay(4000);
	//정지
	MOTOR_DIR(num2,STOP);
	MOTOR_DIR(num3,STOP);
	delay(1000);
}
