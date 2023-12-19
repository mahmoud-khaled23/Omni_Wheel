#include "ROBOEX_3WD_EVERYWHERE.h"
#include "3WD_MOTOR_LIB.h"
#include "Timer_Int.h"
#include "TEXT_LCD.h"

int pin_SWITCH[5] = {38,70,71,72,65}; //Enter, UP, DOWN, LEFT, RIGHT

int angle_value = 0, angle_value1 = 0;;
float angle[3];
int PID_flag = 0, Timer_flag = 0, mode_flag = 0;
int speed = 2;
uint32_t timer = 0;

void Motor_contorl(void);
void ENTER(void);
void UP(void);
void DOWN(void);

void setup()
{
	for(int i=0; i<3; i++)
	{
		pinMode(pin_SWITCH[i],INPUT);
	}
	// 모터 초기화
	Motor_init();

	// 외부 인터럽트 설정
	// 소스: pin_ENCODER[0](Enter) , 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_SWITCH[0],ENTER,FALLING);
	// 소스: pin_ENCODER[1](UP) , 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_SWITCH[1],UP,FALLING);
	// 소스: pin_ENCODER[2](DOWN) , 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_SWITCH[2],DOWN,FALLING);

	// PID 제어를 위한 타이머 인터럽트 설정
	startTimer(TC5_IRQn,25, Motor_contorl);

	mode_flag = 0;

	PID_flag = 0;

	angle_value = 0;

	LCD_init();	// LCD 초기화

	// LCD 출력
	LCD_write(0,0x80);
	LCD_printf("Press the Enter");
	LCD_write(0,0xC0);
	LCD_printf("ANGLE :  000");

	timer = millis();
}

void loop()
{
	// Timer_flag 및 PID_flag 가 1일 경우 실행
	if(Timer_flag && PID_flag)
	{
		// PID 제어 실행
		PID_control();
		Timer_flag = 0;
	}
	// 이동 완료 체크
	if((Encoder_flag == 0) && (PID_flag == 1))
	{
		// 각도 입력 상태 출력
		LCD_write(0,0x80);
		LCD_printf("Press the Enter");
		PID_flag = 0;
	}
}

// PID 제어를 위한 타이머 인터럽트 서비스 루틴
void Motor_contorl(void)
{
	Timer_flag = 1;
}

void ENTER(void)
{
	int i;
	if((timer + 200) < millis())
	{
		if(mode_flag)
		{
			mode_flag = 0;
			LCD_write(0,0x80);
			LCD_printf("   3WD MOVE    ");

			for(i=0; i<3; i++)
			{
				angle[i] = (i * 120) - angle_value;
			}
			for(i=0; i<3; i++)
			{
				if(angle[i] <= -180)
				{
					angle[i] += 360;
				}
				else if(angle[i] > 180)
				{
					angle[i] -= 360;
				}
			}
			Motor_DIR_speed_setup(speed, angle);
			PID_flag = 1;
		}
		else
		{
			mode_flag = 1;
			LCD_write(0,0x80);
			LCD_printf("Input ANGLE    ");
		}
	}
	timer = millis();
}

void UP(void)
{
	if(mode_flag)
	{
		if((timer + 200) < millis())
		{
			angle_value +=45;
			if(angle_value > 180)
			{
				angle_value = -135;
			}
			angle_value1 = angle_value;
			LCD_write(0,0xC8);
			if(angle_value1 < 0)
			{
				LCD_write(1,'-');
				angle_value1 = -angle_value1;
			}
			else
			{
				LCD_write(1,' ');
			}
			LCD_write(1,angle_value1/100 + '0');
			LCD_write(1,(angle_value1/10)%10 + '0');
			LCD_write(1,angle_value1%10 + '0');
		}
		timer = millis();
	}
}

void DOWN(void)
{
	if(mode_flag)
	{
		if((timer + 200) < millis())
		{
			angle_value -=45;
			if(angle_value == -180)
			{
				angle_value = 180;
			}
			angle_value1 = angle_value;
			LCD_write(0,0xC8);
			if(angle_value1 < 0)
			{
				LCD_write(1,'-');
				angle_value1 = -angle_value1;
			}
			else
			{
				LCD_write(1,' ');
			}
			LCD_write(1,angle_value1/100 + '0');
			LCD_write(1,(angle_value1/10)%10 + '0');
			LCD_write(1,angle_value1%10 + '0');
		}
		timer = millis();
	}
}
