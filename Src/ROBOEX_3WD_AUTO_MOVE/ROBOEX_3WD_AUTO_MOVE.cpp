#include "ROBOEX_3WD_AUTO_MOVE.h"
#include "3WD_SENSOR_LIB.h"
#include "3WD_MOTOR_LIB.h"
#include "Timer_Int.h"
#include "TEXT_LCD.h"

// �Ÿ� �񱳰�
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
	Serial.begin(115200);	// UART0 115200 ����

	// ����ġ �Է� ����
	for(i=0; i<5; i++)
	{
		pinMode(pin_SWITCH[i],INPUT);
	}

	// ���� �ʱ�ȭ
	// ������ �� PSD �ʱ�ȭ
	SENSOR_init();

	// ���� �ʱ�ȭ
	Motor_init();

	LCD_init();	// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("Press the Button");

	// ����ġ�� ���� ������ ��ٸ�
	while(Switch_read() == 0);

	// LCD ���
	LCD_write(0,0xC0);
	LCD_printf("AUTO MOVE       ");

	// PID ��� ���� Ÿ�̸� ���ͷ�Ʈ
	startTimer(TC5_IRQn,25, Motor_contorl);

	// �������� 80RPM�� �ӵ��� ����
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
				// ������ ���� �Ÿ� ����
				Ultrasonic_data[Sensor_count] = US_Fire(Sensor_count+1);
			}
			else
			{
				// PSD ���� �Ÿ� ����
				PSD_data = PSD_Read(1);
				for(int i=0; i<2; i++)
				{
					error <<= 1;
					if(Ultrasonic_data[i] < dist_default)	// ���� �Ÿ� �� ��
					{
						error |= 0x01;
					}
				}
				error <<= 1;
				if(PSD_data < dist_default)	// ���� �Ÿ� �� ��
				{
					error |= 0x01;
				}
				for(int i=2; i<4; i++)
				{
					error <<= 1;
					if(Ultrasonic_data[i] < dist_default)	// ���� �Ÿ� �� ��
					{
						error |= 0x01;
					}
				}

				Serial.print(" dist ");
				int mask = 0x10;
				// error�� 2������ ���
				for(int mask=4,i=0;i<5;i++,mask--)
				{
					Serial.print((error>>mask) & 0x1);
				}
				// US2, US3 �Ÿ� �� ���
				for(int i=0; i<2; i++)
				{
					Serial.print(" ");
					Serial.print(Ultrasonic_data[i]);
				}
				// PSD2 �Ÿ� �� ���
				Serial.print(" ");
				Serial.print(PSD_data);
				// US4, US5 �Ÿ� �� ���
				for(int i=2; i<4; i++)
				{
					Serial.print(" ");
					Serial.print(Ultrasonic_data[i]);
				}
				Serial.println();
				if(error & 0x0E)	// PSD2, US3, US4 üũ
				{
					switch(error)
					{
						case 0x02:
						case 0x03:
						case 0x04:
						case 0x05:
						case 0x06:
						case 0x07:
							// error ���� 001XX or  0001X�� ���
							// ��ȸ������ 80RPM�� �ӵ��� 12cm �̵�
							Motor_DIR_speed_setup(LEFT,rotation_speed,12);
							break;
						case 0x09:
						case 0x0A:
						case 0x0B:
						case 0x0D:
						case 0x0E:
						case 0x0F:
							// error ���� 01XXX �� ��� -> XXX�� X00�� ��� ����
							// ��ȸ������ 80RPM�� �ӵ��� 24m �̵�
							Motor_DIR_speed_setup(LEFT,rotation_speed,24);
							break;
						case 0x13:
						case 0x15:
						case 0x17:
						case 0x19:
						case 0x1B:
						case 0x1D:
						case 0x1F:
							// error ���� 1XXX1 �� ��� -> XXX�� 000�� ��� ����
							// ��ȸ������ 80RPM�� �ӵ��� 47cm �̵�
							Motor_DIR_speed_setup(LEFT,rotation_speed,47);
							break;
						case 0x08:
						case 0x0C:
						case 0x14:
						case 0x18:
						case 0x1C:
							// error ���� X1X00 or 10100 �� ���
							// ��ȸ������ 80RPM�� �ӵ��� 12cm �̵�
							Motor_DIR_speed_setup(RIGHT,rotation_speed,12);
							break;
						case 0x12:
						case 0x16:
						case 0x1A:
						case 0x1E:
							// error ���� 1XX10 �� ���
							// ��ȸ������ 80RPM�� �ӵ��� 24cm �̵�
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
				// ������ �̵�
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

