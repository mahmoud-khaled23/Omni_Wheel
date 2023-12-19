#include "ROBOEX_3WD_MOTOR_PID.h"
#include "Timer_Int.h"
#include "3WD_MOTOR_LIB.h"
#include "TEXT_LCD.h"

#define RPM 100	// ���� ȸ�� �ӵ�

#define KP	0.25	// ��� ���
#define KI	0.00025	// ���� ���
#define KD	0.0125	// �̺� ���

int pin_ENCODER[6] = {66,67,94,95,52,78};
int PID_flag = 0;

volatile int32_t Encoder_cnt1 = 0;
volatile int32_t pre_Encoder_cnt1 = 0;

struct PID_buff
{
	volatile int16_t pre_err;	// ���� ����
	volatile int16_t cur_err;	// ���� ����
	volatile int16_t cur_cnt;	// ���� ī����
	volatile int16_t dt_err;	// ���� ���� - ���� ����
	volatile int16_t err_sum;	// ���� ���� ��
	volatile int16_t pwm_value;	// ���� ���� PWM ��
	volatile int16_t PWM_default;	// �ӵ� ���� ��
	volatile int32_t encoder_cnt;	// ���ڴ� ī��Ʈ ��
};

PID_buff PID_MOTOR1;	// PID_MOTOR1 ����ü ����

void PID_buff_init(void);
void ENC_cnt1(void);
void PID_control(void);

void setup()
{
	int i;
	Serial.begin(115200);	// UART0 115200 ����

	for(i=0; i<6; i++)
	{
		pinMode(pin_ENCODER[i],INPUT);	// Encoder �Է� ����
	}

	Motor_init();

	LCD_init();	// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("PID Control");

	// �ܺ� ���ͷ�Ʈ ����
	// ���ڴ� ī���ͽ� 1������ 390 -> 4���� 1560���� �ϱ� ���� CHANGE�� ����
	// �ҽ�: pin_ENCODER[0](1WD_ENC_A) , ���ͷ�Ʈ �߻�: CHANGE(����ϰ�����)
	attachInterrupt(pin_ENCODER[0],ENC_cnt1,CHANGE);
	// �ҽ�: pin_ENCODER[0](1WD_ENC_B) , ���ͷ�Ʈ �߻�: CHANGE(����ϰ�����)
	attachInterrupt(pin_ENCODER[1],ENC_cnt1,CHANGE);

	MOTOR_DIR(0,CW);	// 1WD ��ȸ��

	startTimer(TC3_IRQn,25,PID_control);

	PID_buff_init();	// PID_buff �ʱ�ȭ

	PID_flag = 0;
}

void loop()
{
	int i;
	if(PID_flag)
	{
		// ���� ���� �� ����
		PID_MOTOR1.pre_err = PID_MOTOR1.cur_err;
		// ���� ī���� �� ����
		PID_MOTOR1.cur_cnt = Encoder_cnt1 - pre_Encoder_cnt1;
		// ���� ���ڴ� ī���� �� ����
		pre_Encoder_cnt1 = Encoder_cnt1;
		// ���� ������ ����
		PID_MOTOR1.cur_err = PID_MOTOR1.PWM_default - PID_MOTOR1.cur_cnt;
		// ���� ����
		PID_MOTOR1.err_sum += PID_MOTOR1.cur_err;
		// ���� �̺а� ����
		PID_MOTOR1.dt_err = PID_MOTOR1.cur_err - PID_MOTOR1.pre_err;

		// 1WD ���� �ӵ� ��� �� = (���� ���� * KP) + (���� ���� * KI) + (���� �̺а� * KD)
		PID_MOTOR1.pwm_value += (PID_MOTOR1.cur_err * KP) + (PID_MOTOR1.err_sum * KI) + (PID_MOTOR1.dt_err * KD);

		// 0 ~ 255 ������ ���� ����
		PID_MOTOR1.pwm_value = constrain(PID_MOTOR1.pwm_value, 0, 255);

		// 1WD �ӵ� ����
		Speed_control(0,PID_MOTOR1.pwm_value);

		// ���� ī��Ʈ �� ���
		Serial.print("\n\r cur_cnt : ");
		Serial.print(PID_MOTOR1.cur_cnt);
		Serial.print(" ");

		// �ӵ� ���� ���� �� ���
		Serial.print(" PWM_default : ");
		Serial.print(PID_MOTOR1.PWM_default);
		Serial.print(" ");

		// ���� �ӵ� ��갪 ���
		Serial.print(" pwm_value : ");
		Serial.print(PID_MOTOR1.pwm_value);
		Serial.print(" ");

		PID_flag = 0;
	}
}

// PID_buff �ʱ�ȭ �Լ�
void PID_buff_init(void)
{
	// 40ms�� �����Ǵ� ���ڴ� ī��Ʈ ��
	// 1������ ���ڴ� ī��Ʈ �� : 390 * 4 = 1560
	// RPM�� �д� ȸ���� �̹Ƿ� 40ms�� ���ڴ� ī��Ʈ ���� ��ȯ�ϸ� ���� ���İ� ����.
	PID_MOTOR1.PWM_default = RPM * 1560 / 60 / 25;
	PID_MOTOR1.cur_cnt = 0;
	PID_MOTOR1.cur_err = 0;
	PID_MOTOR1.dt_err = 0;
	PID_MOTOR1.encoder_cnt = 0;
	PID_MOTOR1.err_sum = 0;
	PID_MOTOR1.pre_err = 0;
	PID_MOTOR1.pwm_value = 0;
}

// 1WD encoder�� ī�����ϴ� �ܺ����ͷ�Ʈ ���� ��ƾ
void ENC_cnt1(void)
{
	PID_MOTOR1.encoder_cnt++;
}

// Ÿ�̸� ���ͷ�Ʈ ���� ��ƾ(40ms)
void PID_control(void)
{
	Encoder_cnt1 = PID_MOTOR1.encoder_cnt;

	PID_flag = 1;
}
