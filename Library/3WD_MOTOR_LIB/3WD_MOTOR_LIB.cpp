#include "3WD_MOTOR_LIB.h"

//#define DEBUG
#define RPM 40

#define KP	0.25
#define KI	0.00025
#define KD	0.0125

int pin_MOTOR[6] = {49,48,43,42,92,93};
int pin_MOTOR_EN[3] = {3,5,6};
int pin_ENCODER[6] = {66,67,94,95,52,78};
int Encoder_flag = 0, Timer_on_flag = 0, Timer_freq = 0;

volatile int32_t Encoder_cnt[3] = {0,0,0};
volatile int32_t pre_Encoder_cnt[3] = {0,0,0};
volatile int32_t Move_default[3];

long timer_check = 0;

enum{STOP,CW,CCW};
enum{num1,num2,num3};

struct PID_buff
{
	volatile int16_t pre_err;
	volatile int16_t cur_err;
	volatile int16_t cur_cnt;
	volatile int16_t dt_err;
	volatile int16_t err_sum;
	volatile int16_t pwm_value;
	volatile int16_t PWM_default;
	volatile int32_t encoder_cnt;
};

PID_buff PID_MOTOR[3];

void Motor_init(void)
{
	int i;
	Serial.begin(115200);
	for(i=0; i<6; i++)
	{
		pinMode(pin_MOTOR[i],OUTPUT);
		pinMode(pin_ENCODER[i],INPUT);
		digitalWrite(pin_MOTOR[i],0);
	}
	for(i=0; i<3; i++)
	{
		pinMode(pin_MOTOR_EN[i],OUTPUT);
		digitalWrite(pin_MOTOR_EN[i],1);
	}

	attachInterrupt(pin_ENCODER[0],ENC_cnt1,CHANGE);
	attachInterrupt(pin_ENCODER[1],ENC_cnt1,CHANGE);
	attachInterrupt(pin_ENCODER[2],ENC_cnt2,CHANGE);
	attachInterrupt(pin_ENCODER[3],ENC_cnt2,CHANGE);
	attachInterrupt(pin_ENCODER[4],ENC_cnt3,CHANGE);
	attachInterrupt(pin_ENCODER[5],ENC_cnt3,CHANGE);

}

void PID_control(void)
{
	int i,error = 0;
	float speed;
	for(i=0; i<3; i++)
	{
		Encoder_cnt[i] = PID_MOTOR[i].encoder_cnt;
	}
	for(i=0; i<3; i++)
	{
		PID_MOTOR[i].pre_err = PID_MOTOR[i].cur_err;
		PID_MOTOR[i].cur_cnt = Encoder_cnt[i] - pre_Encoder_cnt[i];
		pre_Encoder_cnt[i] = Encoder_cnt[i];
		PID_MOTOR[i].cur_err = PID_MOTOR[i].PWM_default - PID_MOTOR[i].cur_cnt;
		PID_MOTOR[i].err_sum += PID_MOTOR[i].cur_err;
		PID_MOTOR[i].dt_err = PID_MOTOR[i].cur_err - PID_MOTOR[i].pre_err;

		PID_MOTOR[i].pwm_value += (PID_MOTOR[i].cur_err * KP) + (PID_MOTOR[i].err_sum * KI) + (PID_MOTOR[i].dt_err * KD);

		if(PID_MOTOR[i].pwm_value > 255)
		{
			PID_MOTOR[i].pwm_value = 255;
		}
		else if(PID_MOTOR[i].pwm_value < 0)
		{
			PID_MOTOR[i].pwm_value = 0;
		}
	}
	for(i=0; i<3; i++)
	{
		analogWrite(pin_MOTOR_EN[i],PID_MOTOR[i].pwm_value);
	}

	#ifdef DEBUG
		Serial.print("\n\r cur_cnt : ");
		for(i=0; i<3; i++)
		{
			Serial.print(PID_MOTOR[i].cur_cnt);
			Serial.print(" ");
		}
		Serial.print(" PWM_default : ");
		for(i=0; i<3; i++)
		{
			Serial.print(PID_MOTOR[i].PWM_default);
			Serial.print(" ");
		}
		Serial.print(" pwm_value : ");
		for(i=0; i<3; i++)
		{
			Serial.print(PID_MOTOR[i].pwm_value);
			Serial.print(" ");
		}
	#endif

	if(Encoder_flag)
	{
		for(i=0; i<3; i++)
		{
			if(Move_default[i] < PID_MOTOR[i].encoder_cnt)
			{
				PID_MOTOR[i].encoder_cnt = 0;
				Encoder_flag = 0;
				error = 1;
			}
		}
		if(error)
		{
			Motor_Stop();
		}
	}
}

void Motor_Stop(void)
{
	Motor_DIR(num1,STOP);
	Motor_DIR(num2,STOP);
	Motor_DIR(num3,STOP);
}

void PID_buff_init(uint8_t num)
{
	PID_MOTOR[num].PWM_default = 0;
	PID_MOTOR[num].cur_cnt = 0;
	PID_MOTOR[num].cur_err = 0;
	PID_MOTOR[num].dt_err = 0;
	PID_MOTOR[num].encoder_cnt = 0;
	PID_MOTOR[num].err_sum = 0;
	PID_MOTOR[num].pre_err = 0;
	PID_MOTOR[num].pwm_value = 0;
}

void ENC_cnt1(void)
{
	PID_MOTOR[0].encoder_cnt++;
}

void ENC_cnt2(void)
{
	PID_MOTOR[1].encoder_cnt++;
}

void ENC_cnt3(void)
{
	PID_MOTOR[2].encoder_cnt++;
}

void Motor_DIR(uint8_t num,uint8_t DIR)
{
	switch(DIR)
	{
		case STOP:
			digitalWrite(pin_MOTOR[num*2],LOW);
			digitalWrite(pin_MOTOR[num*2+1],LOW);
			break;
		case CW:
			digitalWrite(pin_MOTOR[num*2],HIGH);
			digitalWrite(pin_MOTOR[num*2+1],LOW);
			break;
		case CCW:
			digitalWrite(pin_MOTOR[num*2],LOW);
			digitalWrite(pin_MOTOR[num*2+1],HIGH);
			break;
	}
}

void Motor_DIR_speed_setup(uint8_t SPEED, float* moveangle)
{
	float speed = RPM;
	int i;
	int32_t Move_max = 0;
	for(i=0; i<3; i++)
	{
		PID_buff_init(i);
		analogWrite(pin_MOTOR_EN[i],0);
		pre_Encoder_cnt[i] = 0;
		Encoder_cnt[i] = 0;
	}
	for(i=0; i<3; i++)
	{
		PID_MOTOR[i].PWM_default = (float)(sin(moveangle[i]/180 * PI) * 1.04 * speed * SPEED);
		Move_default[i] =  (float)(sin(moveangle[i]/180 * PI) * MOVE_DIST * 82.76);
		if(Move_default[i] > Move_max)
		{
			Move_max = Move_default[i];
		}
	}
	for(i=0; i<3; i++)
	{
		if(Move_default[i] == 0)
		{
			Move_default[i] = Move_max;
		}
		if(Move_default[i] < 0)
		{
			Move_default[i] = -Move_default[i];
		}
	}
	for(i=0; i<3; i++)
	{
		if(PID_MOTOR[i].PWM_default == 0)
		{
			Motor_DIR(i,STOP);
		}
		else if(PID_MOTOR[i].PWM_default < 0)
		{
			Motor_DIR(i,CCW);
			PID_MOTOR[i].PWM_default = -PID_MOTOR[i].PWM_default;
		}
		else
		{
			Motor_DIR(i,CW);
		}
	}
	Encoder_flag = 1;
}
