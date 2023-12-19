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
	// ������ ��Ʈ ����
	for(int i=0; i<6; i++)
	{
		pinMode(pin_US_TX[i],OUTPUT);	// ������ �۽� ��Ʈ ��� ����
		pinMode(pin_US_RX[i],INPUT);	// ������ ���� ��Ʈ �Է� ����
		digitalWrite(pin_US_TX[i],HIGH);	// ������ �۽� ��Ʈ HIGH ���
	}
	// PSD ��Ʈ ����
	for(int i=0; i<3; i++)
	{
		pinMode(pin_PSD_SEN[i],INPUT);
	}

	analogReadResolution(12);

	startTimer(TC3_IRQn,100000,US_cnt_ISR);
	startTimer(TC4_IRQn,80000,US_send_ISR);
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

void US_ISR(void)
{
	// ������ ���°� CNT�� ����� ���� ����
	if(US_flag[US_select] == CNT)
	{
		US_count++;
		if(US_count > 6)
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
	// ������ ���°� CNT�� ����� US_cnt[US_select] 1����
	if(US_flag[US_select] == CNT)
	{
		US_cnt[US_select]++;
	}
}

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
		// 20 ~ 240cm�� ���� ����
		US_data[US_select] = constrain(US_data[US_select], 20, 240);

		return US_data[US_select];
	}
	else	// �ð� ���� �Ÿ� �� ������ 250�� ��ȯ
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

		// 1/L = ���� * 0.047 - 0.008 = adc_data * 3.3 / 4096 *0.0434 - 0.0046
		tmp = tmp * 3.3 /4096 * 0.047 - 0.008;
		dist = (uint16_t)(1/tmp);

		// 10 ~ 80cm�� ���� ����
		dist = constrain(dist, 10, 80);
	}
	else
	{
		dist = 80;
	}

	return dist;
}

