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
	// ���� �ʱ�ȭ
	Motor_init();

	// �ܺ� ���ͷ�Ʈ ����
	// �ҽ�: pin_ENCODER[0](Enter) , ���ͷ�Ʈ �߻�: FALLING(�ϰ�����)
	attachInterrupt(pin_SWITCH[0],ENTER,FALLING);
	// �ҽ�: pin_ENCODER[1](UP) , ���ͷ�Ʈ �߻�: FALLING(�ϰ�����)
	attachInterrupt(pin_SWITCH[1],UP,FALLING);
	// �ҽ�: pin_ENCODER[2](DOWN) , ���ͷ�Ʈ �߻�: FALLING(�ϰ�����)
	attachInterrupt(pin_SWITCH[2],DOWN,FALLING);

	// PID ��� ���� Ÿ�̸� ���ͷ�Ʈ ����
	startTimer(TC5_IRQn,25, Motor_contorl);

	mode_flag = 0;

	PID_flag = 0;

	angle_value = 0;

	LCD_init();	// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf("Press the Enter");
	LCD_write(0,0xC0);
	LCD_printf("ANGLE :  000");

	timer = millis();
}

void loop()
{
	// Timer_flag �� PID_flag �� 1�� ��� ����
	if(Timer_flag && PID_flag)
	{
		// PID ���� ����
		PID_control();
		Timer_flag = 0;
	}
	// �̵� �Ϸ� üũ
	if((Encoder_flag == 0) && (PID_flag == 1))
	{
		// ���� �Է� ���� ���
		LCD_write(0,0x80);
		LCD_printf("Press the Enter");
		PID_flag = 0;
	}
}

// PID ��� ���� Ÿ�̸� ���ͷ�Ʈ ���� ��ƾ
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
