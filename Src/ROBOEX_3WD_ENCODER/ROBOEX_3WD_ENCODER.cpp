#include "ROBOEX_3WD_ENCODER.h"
#include "TEXT_LCD.h"
#include "3WD_MOTOR_LIB.h"

enum{num1, num2, num3}; // 모터 번호

// 1WD_ENC_A, 1WD_ENC_B, 2WD_ENC_A, 2WD_ENC_B, 3WD_ENC_A, 3WD_ENC_B
int pin_ENCODER[6] = {66,67,94,95,52,78};

long encoder_counter1 = 0, encoder_value;	// 엔코더 값 저장 변수

void ENC_count1(void);

//The setup function is called once at startup of the sketch
void setup()
{
	int i;
	Motor_init();

	for(i=0; i<6; i++)
	{
		pinMode(pin_ENCODER[i], INPUT); // Encoder 입력 설정
	}
	LCD_init();	// LCD 초기화

	// LCD 출력
	LCD_write(0,0x80);
	LCD_printf("Count :  000000");
	LCD_write(0,0xC0);
	LCD_printf("Rotation : 0000");

	// 외부 인터럽트 설정
	// 소스: pin_ENCODER[0](1WD_ENC_A) , 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_ENCODER[0],ENC_count1,FALLING);

	encoder_counter1 = 0;	// 엔코더 카운터 수를 0으로 초기화

	digitalWrite(pin_MOTOR_EN[0],1);	// 1WD 모터 ON

	MOTOR_DIR(num1,CW);	// 1WD 모터 정회전
}

void loop()
{
	encoder_value = encoder_counter1;
	if(encoder_value > 999999)
	{
		encoder_counter1 = 0;
		encoder_value = 0;
	}

	LCD_write(0,0x89);
	// 1WD Encoder 카운터 수 출력
	LCD_write(1, encoder_value/100000 + '0');
	LCD_write(1, (encoder_value/10000)%10 + '0');
	LCD_write(1, (encoder_value/1000)%10 + '0');
	LCD_write(1, (encoder_value/100)%10 + '0');
	LCD_write(1, (encoder_value/10)%10 + '0');
	LCD_write(1, encoder_value%10 + '0');

	encoder_value /= 390;  // 1WD 회전수 계산

	LCD_write(0,0xCB);
	// 1WD 모터 회전수 출력
	LCD_write(1, encoder_value/1000 + '0');
	LCD_write(1, (encoder_value/100)%10 + '0');
	LCD_write(1, (encoder_value/10)%10 + '0');
	LCD_write(1, encoder_value%10 + '0');

	delay(500);
}

// 1WD encoder를 카운터하는 외부인터럽트 서비스 루틴
void ENC_count1(void)
{
	encoder_counter1++;
}
