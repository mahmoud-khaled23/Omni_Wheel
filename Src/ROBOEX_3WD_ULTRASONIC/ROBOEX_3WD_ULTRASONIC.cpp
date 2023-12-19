#include "ROBOEX_3WD_ULTRASONIC.h"
#include "Timer_Int.h"
#include "TEXT_LCD.h"

#define OFFSET 0.925

int pin_US_TX[6] = {34,37,103,97,99,101};
int pin_US_RX[6] = {33,35,36,96,98,100};

// 초음파 측정 상태
// SEND : 초음파 송신
// CNT : 초음파 송신 후 수신되는 시간 체크
// RECV : 거리 측정 완료
enum{SEND, CNT, RECV};

volatile uint8_t US_flag[6] = {RECV,RECV,RECV,RECV,RECV,RECV};
volatile uint16_t US_data[6] = {0,0,0,0,0,0};
volatile uint16_t US_cnt[6] = {0,0,0,0,0,0};
volatile uint8_t US_select = 0;
volatile uint8_t count = 0,Send_count = 0;
long timer=0;

void ultraSend(uint8_t num);
void US_ISR(void);
void US_cnt_ISR(void);
uint16_t US_Fire(uint8_t num);
void US_send_ISR(void);

//The setup function is called once at startup of the sketch
void setup()
{
	int i;
	// 초음파 포트 설정
	for(i=0; i<6; i++)
	{
		pinMode(pin_US_TX[i],OUTPUT);	// 초음파 송신 포트 출력 설정
		pinMode(pin_US_RX[i],INPUT);	// 초음파 수신 포트 입력 설정
		digitalWrite(pin_US_TX[i],HIGH);	// 초음파 송신 포트 HIGH 출력
	}
	Serial2.begin(115200);

	LCD_init();	// LCD 초기화

	// LCD 출력
	LCD_write(0,0x80);
	LCD_printf("Ultrasonic US1 ");
	LCD_write(0,0xC0);
	LCD_printf("DIST :       cm");

	// 초음파 시간
	startTimer(TC3_IRQn,100000,US_cnt_ISR);

	// 초음파를 40KHz로 송신하기 위한 타이머 인터럽트
	startTimer(TC4_IRQn,80000,US_send_ISR);
}

void loop()
{
	uint16_t Dist,i;
	Dist = US_Fire(0);
	// Dist 출력
	LCD_write(0, 0xCA);
	if(Dist > 100)
	{
		LCD_write(1, Dist/100 + '0');
	}
	else
	{
		LCD_write(1, ' ');
	}
	LCD_write(1, (Dist/10)%10 + '0');
	LCD_write(1, Dist%10 + '0');

	delay(200);
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

// 초음파 거리를 측정하기 위한 외부인터럽트 서비스 루틴
void US_ISR(void)
{
	// 초음파 상태가 CNT일 경우일 때만 동작
	if(US_flag[US_select] == CNT)
	{
		count++;
		if(count > 6)
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
			count = 0;
			US_cnt[US_select] = 0;
		}
	}
	else
	{
		count = 0;
	}
}

// 초음파 거리를 계산하기 위한 타이머 인터럽트 서비스 루틴
// 주기 : 10us
void US_cnt_ISR(void)
{
	// 초음파 상태가 CNT일 경우일 US_cnt[US_select] 1증가
	if(US_flag[US_select] == CNT)
	{
		US_cnt[US_select]++;
	}
}

// 초음파 거리를 측정하는 함수
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
		// 10 ~ 240cm로 범위 지정
		US_data[US_select] = constrain(US_data[US_select], 10, 240);

		return US_data[US_select];
	}
	else	// 시간 내에 거리 미 측정시 250을 반환
	{
		count = 0;
		US_cnt[US_select] = 0;
		US_flag[US_select] = RECV;
		return 250;
	}
}
