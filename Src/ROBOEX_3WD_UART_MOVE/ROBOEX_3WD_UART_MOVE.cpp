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
	Serial.begin(115200);	// UART0 115200 설정

	Motor_init();	// 모터 초기화

	LCD_init();		// LCD 초기화

	// LCD 출력
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("UART MOVE");

	for(i=0; i<3; i++)
	{
		Speed_control(i, 255);	// 모터 속도 최대
		MOTOR_DIR(i,STOP);		// 모터 정지
	}
}

void loop()
{
}

// UART0 수신 인터럽트 서비스 루틴
void serialEvent(void)
{
	char da;
	da = Serial.read();	// 수신데이터 da에 저장
	Serial.write(da);	// 수신데이터 재 송신
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
