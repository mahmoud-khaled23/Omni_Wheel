#include "ROBOEX_3WD_AUTO_MOVE.h"
#include "3WD_SENSOR_LIB.h"
#include "3WD_MOTOR_LIB.h"
#include "Timer_Int.h"
#include "TEXT_LCD.h"

// 거리 비교값
#define	dist_default	21

int pin_SWITCH[5] = {38,70,71,72,65}; //Enter, UP, DOWN, LEFT, RIGHT

int Timer_flag = 0, Sensor_flag = 1;
uint16_t Ultrasonic_data[4];
uint16_t PSD_data;
int Sensor_count  = 0;

int forward_speed = 2, rotation_speed = 2;

enum{STOP,FORWARD,BACK,LEFT,RIGHT};

uint16_t PSD_Read(uint8_t num);
void Motor_contorl(void);
uint8_t Switch_read(void);


void setup()
{
	int i;
	Serial.begin(115200);	// UART0 115200 설정

	// 스위치 입력 설정
	for(i=0; i<5; i++)
	{
		pinMode(pin_SWITCH[i],INPUT);
	}

	// 센서 초기화
	// 초음파 및 PSD 초기화
	SENSOR_init();

	// 모터 초기화
	Motor_init();

	LCD_init();	// LCD 초기화

	// LCD 출력
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("Press the Button");

	// 스위치를 누를 때까지 기다림
	while(Switch_read() == 0);

	// LCD 출력
	LCD_write(0,0xC0);
	LCD_printf("AUTO MOVE       ");

	// PID 제어를 위한 타이머 인터럽트
	startTimer(TC5_IRQn,25, Motor_contorl);

	// 전진으로 80RPM의 속도로 제어
	Motor_DIR_speed_setup(FORWARD,forward_speed,0);
}

void loop()
{
	int error = 0,i;
	if(Timer_flag)
	{
		PID_control();

		if(Sensor_flag)
		{
			if(Sensor_count != 4)
			{
				// 초음파 센서 거리 측정
				Ultrasonic_data[Sensor_count] = US_Fire(Sensor_count+1);
			}
			else
			{
				// PSD 센서 거리 측정
				PSD_data = PSD_Read(1);
				for(int i=0; i<2; i++)
				{
					error <<= 1;
					if(Ultrasonic_data[i] < dist_default)	// 기준 거리 값 비교
					{
						error |= 0x01;
					}
				}
				error <<= 1;
				if(PSD_data < dist_default)	// 기준 거리 값 비교
				{
					error |= 0x01;
				}
				for(int i=2; i<4; i++)
				{
					error <<= 1;
					if(Ultrasonic_data[i] < dist_default)	// 기준 거리 값 비교
					{
						error |= 0x01;
					}
				}

				Serial.print(" dist ");
				int mask = 0x10;
				// error를 2진수로 출력
				for(int mask=4,i=0;i<5;i++,mask--)
				{
					Serial.print((error>>mask) & 0x1);
				}
				// US2, US3 거리 값 출력
				for(int i=0; i<2; i++)
				{
					Serial.print(" ");
					Serial.print(Ultrasonic_data[i]);
				}
				// PSD2 거리 값 출력
				Serial.print(" ");
				Serial.print(PSD_data);
				// US4, US5 거리 값 출력
				for(int i=2; i<4; i++)
				{
					Serial.print(" ");
					Serial.print(Ultrasonic_data[i]);
				}
				Serial.println();
				if(error & 0x0E)	// PSD2, US3, US4 체크
				{
					switch(error)
					{
						case 0x02:
						case 0x03:
						case 0x04:
						case 0x05:
						case 0x06:
						case 0x07:
							// error 값이 001XX or  0001X일 경우
							// 좌회전으로 80RPM의 속도로 12cm 이동
							Motor_DIR_speed_setup(LEFT,rotation_speed,12);
							break;
						case 0x09:
						case 0x0A:
						case 0x0B:
						case 0x0D:
						case 0x0E:
						case 0x0F:
							// error 값이 01XXX 일 경우 -> XXX는 X00일 경우 제외
							// 좌회전으로 80RPM의 속도로 24m 이동
							Motor_DIR_speed_setup(LEFT,rotation_speed,24);
							break;
						case 0x13:
						case 0x15:
						case 0x17:
						case 0x19:
						case 0x1B:
						case 0x1D:
						case 0x1F:
							// error 값이 1XXX1 일 경우 -> XXX는 000일 경우 제외
							// 좌회전으로 80RPM의 속도로 47cm 이동
							Motor_DIR_speed_setup(LEFT,rotation_speed,47);
							break;
						case 0x08:
						case 0x0C:
						case 0x14:
						case 0x18:
						case 0x1C:
							// error 값이 X1X00 or 10100 일 경우
							// 우회전으로 80RPM의 속도로 12cm 이동
							Motor_DIR_speed_setup(RIGHT,rotation_speed,12);
							break;
						case 0x12:
						case 0x16:
						case 0x1A:
						case 0x1E:
							// error 값이 1XX10 일 경우
							// 우회전으로 80RPM의 속도로 24cm 이동
							Motor_DIR_speed_setup(RIGHT,rotation_speed,24);
							break;
					}
					Sensor_flag = 0;
				}
			}
			if(Sensor_count == 4)
			{
				Sensor_count = 0;
			}
			else
			{
				Sensor_count++;
			}
		}
		else
		{
			if(Encoder_flag == 0)
			{
				// 앞으로 이동
				Motor_DIR_speed_setup(FORWARD,forward_speed,0);
				Sensor_flag = 1;
			}
		}
		Timer_flag = 0;
	}
}

void Motor_contorl(void)
{
	Timer_flag = 1;
}


uint8_t Switch_read(void)
{
	uint8_t da=0;
	for(int i=0; i<5; i++)
	{
		da  |= (!digitalRead(pin_SWITCH[i]))<<i;
	}
	return da;
}

