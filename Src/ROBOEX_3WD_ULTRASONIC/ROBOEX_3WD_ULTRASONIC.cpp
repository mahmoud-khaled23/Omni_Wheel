#include "ROBOEX_3WD_ULTRASONIC.h"
#include "Timer_Int.h"
#include "TEXT_LCD.h"

#define OFFSET 0.925

int pin_US_TX[6] = {34,37,103,97,99,101};
int pin_US_RX[6] = {33,35,36,96,98,100};

// ������ ���� ����
// SEND : ������ �۽�
// CNT : ������ �۽� �� ���ŵǴ� �ð� üũ
// RECV : �Ÿ� ���� �Ϸ�
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
	// ������ ��Ʈ ����
	for(i=0; i<6; i++)
	{
		pinMode(pin_US_TX[i],OUTPUT);	// ������ �۽� ��Ʈ ��� ����
		pinMode(pin_US_RX[i],INPUT);	// ������ ���� ��Ʈ �Է� ����
		digitalWrite(pin_US_TX[i],HIGH);	// ������ �۽� ��Ʈ HIGH ���
	}
	Serial2.begin(115200);

	LCD_init();	// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf("Ultrasonic US1 ");
	LCD_write(0,0xC0);
	LCD_printf("DIST :       cm");

	// ������ �ð�
	startTimer(TC3_IRQn,100000,US_cnt_ISR);

	// �����ĸ� 40KHz�� �۽��ϱ� ���� Ÿ�̸� ���ͷ�Ʈ
	startTimer(TC4_IRQn,80000,US_send_ISR);
}

void loop()
{
	uint16_t Dist,i;
	Dist = US_Fire(0);
	// Dist ���
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

// ������ ��� �Լ� - 40KHz�� �޽��� ����
void US_send_ISR(void)
{
	if(US_flag[US_select] == SEND)
	{
		if(Send_count != 16)
		{
			// ���ļ� 40KHz�� 8���� �޽� ����
			if((Send_count % 2) == 0)	// ¦���� ��� LOW ���
			{
				digitalWrite(pin_US_TX[US_select],LOW);
			}
			else	// Ȧ���� ��� HIGH ���
			{
				digitalWrite(pin_US_TX[US_select],HIGH);
			}
			Send_count++;
		}
		else
		{
			Send_count = 0;
			// ������ �۽� �Ϸ� �� ���� ��ȯ
			US_flag[US_select] = CNT;
		}
	}
}

// ������ �Ÿ��� �����ϱ� ���� �ܺ����ͷ�Ʈ ���� ��ƾ
void US_ISR(void)
{
	// ������ ���°� CNT�� ����� ���� ����
	if(US_flag[US_select] == CNT)
	{
		count++;
		if(count > 6)
		{
			// �����İ� �պ��� �ð�
			US_data[US_select] = US_cnt[US_select];
			// �����ķ� ������ �պ��� �Ÿ�
			US_data[US_select] *= 0.34;
			// �����ķ� ������ �Ÿ�
			US_data[US_select] >>= 1;
			// ������ �Ÿ��� Offset ����
			US_data[US_select] *= OFFSET;
			// ������ �Ÿ� ���� �Ϸ�
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

// ������ �Ÿ��� ����ϱ� ���� Ÿ�̸� ���ͷ�Ʈ ���� ��ƾ
// �ֱ� : 10us
void US_cnt_ISR(void)
{
	// ������ ���°� CNT�� ����� US_cnt[US_select] 1����
	if(US_flag[US_select] == CNT)
	{
		US_cnt[US_select]++;
	}
}

// ������ �Ÿ��� �����ϴ� �Լ�
uint16_t US_Fire(uint8_t num)
{
	// num = �����Ĺ�ȣ - 1, ��)US_RX1 or US_TX1�̸� num�� 0
	US_select = num;

	US_flag[US_select] = SEND;

	while(US_flag[US_select] == SEND);

	// 10cm ���� �����ǵ��� �ð� ����
	delayMicroseconds(588);

	// �ܺ� ���ͷ�Ʈ ����
	// �ҽ�: pin_US_RX[US_select](US1~6) , ���ͷ�Ʈ �߻�: FALLING(�ϰ�����)
	attachInterrupt(pin_US_RX[US_select],US_ISR,FALLING);

	// ���� ī���� �� ����
	timer=millis();

	// ������ ���� �� üũ �� �ð� üũ
	// ���� �ð����� �Ÿ� ������ �Ϸ�� ������ ����
	while(US_flag[US_select] != RECV)
	{
		// �Ÿ� �� ������ 12ms �� �� ���� ����
		if((timer+12)<millis())
		{
			break;
		}
	}

	// �ܺ� ���ͷ�Ʈ ����
	// �ҽ�: pin_US_RX[US_select](US1~6)
	detachInterrupt(pin_US_RX[US_select]);

	if(US_flag[US_select] == RECV)	// �ð� ���� �Ÿ� ������ ���� �Ÿ� �� ��ȯ
	{
		// 10 ~ 240cm�� ���� ����
		US_data[US_select] = constrain(US_data[US_select], 10, 240);

		return US_data[US_select];
	}
	else	// �ð� ���� �Ÿ� �� ������ 250�� ��ȯ
	{
		count = 0;
		US_cnt[US_select] = 0;
		US_flag[US_select] = RECV;
		return 250;
	}
}
