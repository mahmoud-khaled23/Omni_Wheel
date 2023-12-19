#include "3WD_SENSOR_LIB.h"

int pin_US_TX[6] = {34,37,103,97,99,101};
int pin_US_RX[6] = {33,35,36,96,98,100};
int pin_PSD_SEN[3] = {A4, A6, A8};


volatile uint8_t US_flag[6] = {RECV,RECV,RECV,RECV,RECV,RECV};
volatile uint16_t US_data[6] = {0,0,0,0,0,0};
volatile uint16_t US_cnt[6] = {0,0,0,0,0,0};
volatile uint8_t US_select = 0;
volatile uint8_t US_count = 0,Send_count = 0, count_flag = RECV;
long timer=0;

void SENSOR_init(void)
{
	// 초음파 포트 설정
	for(int i=0; i<6; i++)
	{
		pinMode(pin_US_TX[i],OUTPUT);	// 초음파 송신 포트 출력 설정
		pinMode(pin_US_RX[i],INPUT);	// 초음파 수신 포트 입력 설정
		digitalWrite(pin_US_TX[i],HIGH);	// 초음파 송신 포트 HIGH 출력
	}
	// PSD 포트 설정
	for(int i=0; i<3; i++)
	{
		pinMode(pin_PSD_SEN[i],INPUT);
	}

	analogReadResolution(12);

	startTimer(TC3_IRQn,100000,US_cnt_ISR);
	startTimer(TC4_IRQn,80000,US_send_ISR);
}

// 초음파 출력 함수 - 40KHz의 펄스를 만듬
void US_send_ISR(void)
{
	if(US_flag[US_select] == SEND)
	{
		if(Send_count != 16)
		{
			// 주파수 40KHz인 8개의 펄스 생성
			if((Send_count % 2) == 0)	// 짝수일 경우 LOW 출력
			{
				digitalWrite(pin_US_TX[US_select],LOW);
			}
			else	// 홀수일 경우 HIGH 출력
			{
				digitalWrite(pin_US_TX[US_select],HIGH);
			}
			Send_count++;
		}
		else
		{
			Send_count = 0;
			// 초음파 송신 완료 후 상태 변환
			US_flag[US_select] = CNT;
		}
	}
}

void US_ISR(void)
{
	// 초음파 상태가 CNT일 경우일 때만 동작
	if(US_flag[US_select] == CNT)
	{
		US_count++;
		if(US_count > 6)
		{
			// 초음파가 왕복한 시간
			US_data[US_select] = US_cnt[US_select];
			// 초음파로 측정된 왕복한 거리
			US_data[US_select] *= 0.34;
			// 초음파로 측정된 거리
			US_data[US_select] >>= 1;
			// 측정된 거리의 Offset 적용
			US_data[US_select] *= OFFSET;
			// 초음파 거리 측정 완료
			US_flag[US_select] = RECV;
			US_count = 0;
			US_cnt[US_select] = 0;
		}
	}
	else
	{
		US_count = 0;
	}
}

void US_cnt_ISR(void)
{
	// 초음파 상태가 CNT일 경우일 US_cnt[US_select] 1증가
	if(US_flag[US_select] == CNT)
	{
		US_cnt[US_select]++;
	}
}

uint16_t US_Fire(uint8_t num)
{
	// num = 초음파번호 - 1, 예)US_RX1 or US_TX1이면 num은 0
	US_select = num;

	US_flag[US_select] = SEND;

	while(US_flag[US_select] == SEND);

	// 10cm 부터 측정되도록 시간 지연
	delayMicroseconds(588);

	// 외부 인터럽트 설정
	// 소스: pin_US_RX[US_select](US1~6) , 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_US_RX[US_select],US_ISR,FALLING);

	// 현태 카운터 값 저장
	timer=millis();

	// 초음파 상태 값 체크 및 시간 체크
	// 일정 시간동안 거리 측정이 완료될 때까지 실행
	while(US_flag[US_select] != RECV)
	{
		// 거리 미 측정시 12ms 이 후 실행 정지
		if((timer+12)<millis())
		{
			break;
		}
	}

	// 외부 인터럽트 해제
	// 소스: pin_US_RX[US_select](US1~6)
	detachInterrupt(pin_US_RX[US_select]);

	if(US_flag[US_select] == RECV)	// 시간 내에 거리 측정시 측정 거리 값 반환
	{
		// 20 ~ 240cm로 범위 지정
		US_data[US_select] = constrain(US_data[US_select], 20, 240);

		return US_data[US_select];
	}
	else	// 시간 내에 거리 미 측정시 250을 반환
	{
		US_count = 0;
		US_cnt[US_select] = 0;
		US_flag[US_select] = RECV;
		return 250;
	}
}

uint16_t PSD_Read(uint8_t num)
{
	float tmp = 0;
	uint16_t dist, adc_data = analogRead(pin_PSD_SEN[num]);

	if(adc_data > 540)
	{
		tmp = adc_data;

		// 1/L = 전압 * 0.047 - 0.008 = adc_data * 3.3 / 4096 *0.0434 - 0.0046
		tmp = tmp * 3.3 /4096 * 0.047 - 0.008;
		dist = (uint16_t)(1/tmp);

		// 10 ~ 80cm로 범위 지정
		dist = constrain(dist, 10, 80);
	}
	else
	{
		dist = 80;
	}

	return dist;
}

