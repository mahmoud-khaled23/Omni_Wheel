#include "ROBOEX_3WD_MOTOR_PID.h"
#include "Timer_Int.h"
#include "3WD_MOTOR_LIB.h"
#include "TEXT_LCD.h"

#define RPM 100	// 바퀴 회전 속도

#define KP	0.25	// 비례 상수
#define KI	0.00025	// 적분 상수
#define KD	0.0125	// 미분 상수

int pin_ENCODER[6] = {66,67,94,95,52,78};
int PID_flag = 0;

volatile int32_t Encoder_cnt1 = 0;
volatile int32_t pre_Encoder_cnt1 = 0;

struct PID_buff
{
	volatile int16_t pre_err;	// 이전 오차
	volatile int16_t cur_err;	// 현재 오차
	volatile int16_t cur_cnt;	// 현재 카운터
	volatile int16_t dt_err;	// 현재 오차 - 이전 오차
	volatile int16_t err_sum;	// 오차 누적 값
	volatile int16_t pwm_value;	// 모터 제어 PWM 값
	volatile int16_t PWM_default;	// 속도 기준 값
	volatile int32_t encoder_cnt;	// 엔코더 카운트 값
};

PID_buff PID_MOTOR1;	// PID_MOTOR1 구조체 선언

void PID_buff_init(void);
void ENC_cnt1(void);
void PID_control(void);

void setup()
{
	int i;
	Serial.begin(115200);	// UART0 115200 설정

	for(i=0; i<6; i++)
	{
		pinMode(pin_ENCODER[i],INPUT);	// Encoder 입력 설정
	}

	Motor_init();

	LCD_init();	// LCD 초기화

	// LCD 출력
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("PID Control");

	// 외부 인터럽트 설정
	// 엔코더 카운터시 1바퀴당 390 -> 4배인 1560으로 하기 위해 CHANGE로 설정
	// 소스: pin_ENCODER[0](1WD_ENC_A) , 인터럽트 발생: CHANGE(상승하강에지)
	attachInterrupt(pin_ENCODER[0],ENC_cnt1,CHANGE);
	// 소스: pin_ENCODER[0](1WD_ENC_B) , 인터럽트 발생: CHANGE(상승하강에지)
	attachInterrupt(pin_ENCODER[1],ENC_cnt1,CHANGE);

	MOTOR_DIR(0,CW);	// 1WD 정회전

	startTimer(TC3_IRQn,25,PID_control);

	PID_buff_init();	// PID_buff 초기화

	PID_flag = 0;
}

void loop()
{
	int i;
	if(PID_flag)
	{
		// 이전 오차 값 저장
		PID_MOTOR1.pre_err = PID_MOTOR1.cur_err;
		// 현재 카운터 값 저장
		PID_MOTOR1.cur_cnt = Encoder_cnt1 - pre_Encoder_cnt1;
		// 이전 엔코더 카운터 값 저장
		pre_Encoder_cnt1 = Encoder_cnt1;
		// 현재 오차값 저장
		PID_MOTOR1.cur_err = PID_MOTOR1.PWM_default - PID_MOTOR1.cur_cnt;
		// 오차 누적
		PID_MOTOR1.err_sum += PID_MOTOR1.cur_err;
		// 오차 미분값 저장
		PID_MOTOR1.dt_err = PID_MOTOR1.cur_err - PID_MOTOR1.pre_err;

		// 1WD 제어 속도 계산 값 = (현재 오차 * KP) + (누적 오차 * KI) + (오차 미분값 * KD)
		PID_MOTOR1.pwm_value += (PID_MOTOR1.cur_err * KP) + (PID_MOTOR1.err_sum * KI) + (PID_MOTOR1.dt_err * KD);

		// 0 ~ 255 값으로 범위 지정
		PID_MOTOR1.pwm_value = constrain(PID_MOTOR1.pwm_value, 0, 255);

		// 1WD 속도 제어
		Speed_control(0,PID_MOTOR1.pwm_value);

		// 현재 카운트 값 출력
		Serial.print("\n\r cur_cnt : ");
		Serial.print(PID_MOTOR1.cur_cnt);
		Serial.print(" ");

		// 속도 제어 기준 값 출력
		Serial.print(" PWM_default : ");
		Serial.print(PID_MOTOR1.PWM_default);
		Serial.print(" ");

		// 제어 속도 계산값 출력
		Serial.print(" pwm_value : ");
		Serial.print(PID_MOTOR1.pwm_value);
		Serial.print(" ");

		PID_flag = 0;
	}
}

// PID_buff 초기화 함수
void PID_buff_init(void)
{
	// 40ms당 측정되는 엔코더 카운트 수
	// 1바퀴당 엔코더 카운트 수 : 390 * 4 = 1560
	// RPM은 분당 회전수 이므로 40ms당 엔코더 카운트 수로 변환하면 다음 수식과 같다.
	PID_MOTOR1.PWM_default = RPM * 1560 / 60 / 25;
	PID_MOTOR1.cur_cnt = 0;
	PID_MOTOR1.cur_err = 0;
	PID_MOTOR1.dt_err = 0;
	PID_MOTOR1.encoder_cnt = 0;
	PID_MOTOR1.err_sum = 0;
	PID_MOTOR1.pre_err = 0;
	PID_MOTOR1.pwm_value = 0;
}

// 1WD encoder를 카운터하는 외부인터럽트 서비스 루틴
void ENC_cnt1(void)
{
	PID_MOTOR1.encoder_cnt++;
}

// 타이머 인터럽트 서비스 루틴(40ms)
void PID_control(void)
{
	Encoder_cnt1 = PID_MOTOR1.encoder_cnt;

	PID_flag = 1;
}
