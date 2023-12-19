#include "ROBOEX_3WD_CAN_APP1_SLAVE.h"
#include "3WD_MOTOR_LIB.h"
#include "Timer_Int.h"
#include "TEXT_LCD.h"
#include "due_can.h"

#define MAX_CAN_FRAME_DATA_LEN	8	// ���� ������ ����
#define CAN_TX_PRIO				10	// �켱 ����

int pin_LED[2] = {45,44};	// LED �� ����
int pin_BUZZER = 7;			// BUZZER �� ����

int angle_value = 0, angle_value1 = 0;;
float angle[3];
int PID_flag = 0, Timer_flag = 0;
int speed = 2;
uint32_t timer = 0;

RX_CAN_FRAME incoming;

union data_tr
{
	//transfer_data[0] -> mailbox_set_datal, transfer_data[1] -> mailbox_set_datah
	uint32_t transfer_data[2];
	uint8_t data[8];
};
data_tr CAN_BUFF;

void Motor_contorl(void);
void CAN_TX_init(uint8_t MailBox_num,uint8_t CAN_ID,uint8_t PRIORITY);
void CAN_RX_init(uint8_t MailBox_num,uint8_t CAN_ID);

void setup()
{
	// LED �ʱ�ȭ
	pinMode(pin_LED[0],OUTPUT);
	pinMode(pin_LED[1],OUTPUT);
	digitalWrite(pin_LED[0],LOW);
	digitalWrite(pin_LED[1],LOW);

	// BUZZER �ʱ�ȭ
	pinMode(pin_BUZZER,OUTPUT);
	digitalWrite(pin_BUZZER,LOW);

	// ���� �ʱ�ȭ
	Motor_init();

	LCD_init();	// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX MOTOR");
	LCD_write(0,0xC0);
	LCD_printf(" Remote Control");

	CAN.init(SystemCoreClock, CAN_BPS_1000K);	// CAN �ʱ�ȭ

	CAN_RX_init(0,2);	// CAN ���� �ʱ�ȭ
	CAN_TX_init(1,0,CAN_TX_PRIO);	// CAN �۽� �ʱ�ȭ

	// PID ��� ���� Ÿ�̸� ���ͷ�Ʈ ����
	startTimer(TC5_IRQn,25, Motor_contorl);
}

void loop()
{
	int i;
	// CAN ���� üũ
	if(CAN.mailbox_get_status(0) & CAN_MSR_MRDY)
	{
		CAN.mailbox_read(0, &incoming);
		for(int i=0; i<8; i++)
			CAN_BUFF.data[i] = incoming.data[i];

		CAN.mailbox_set_datal(1, CAN_BUFF.transfer_data[0]);
		CAN.mailbox_set_datah(1, CAN_BUFF.transfer_data[1]);

		CAN.global_send_transfer_cmd(CAN_TCR_MB1);

		// LED ����
		for(int i=0; i<2; i++)
			digitalWrite(pin_LED[i],CAN_BUFF.data[i+2]);

		// BUZZER ����
		digitalWrite(pin_BUZZER,CAN_BUFF.data[4]);

		angle_value = (uint16_t)CAN_BUFF.data[6]<<8 | CAN_BUFF.data[7];
		if(CAN_BUFF.data[5])
		{
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
	}
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
		PID_flag = 0;
	}
}

// PID ��� ���� Ÿ�̸� ���ͷ�Ʈ ���� ��ƾ
void Motor_contorl(void)
{
	Timer_flag = 1;
}

// CAN �۽� mailbox �ʱ�ȭ �Լ�
void CAN_TX_init(uint8_t MailBox_num,uint8_t CAN_ID,uint8_t PRIORITY)
{
	CAN.mailbox_init(MailBox_num);	// mailbox �ʱ�ȭ
	CAN.mailbox_set_mode(MailBox_num, CAN_MB_TX_MODE);	// mailbox ��� ����
	CAN.mailbox_set_priority(MailBox_num, PRIORITY);	// mailbox �켱 ���� ����
	CAN.mailbox_set_accept_mask(MailBox_num, 0, false);	// mailbox �ĺ��� ����ũ ����
	CAN.mailbox_set_id(MailBox_num, (CAN_ID | 0xC0), false);	// mailbox �ĺ��� ID ����
	CAN.mailbox_set_datalen(MailBox_num, MAX_CAN_FRAME_DATA_LEN);	// ������ ���� ���� ����
}

// CAN ���� mailbox �ʱ�ȭ �Լ�
void CAN_RX_init(uint8_t MailBox_num,uint8_t CAN_ID)
{
	CAN.mailbox_init(MailBox_num);	// mailbox �ʱ�ȭ
	CAN.mailbox_set_mode(MailBox_num, CAN_MB_RX_MODE);	// mailbox ��� ����
	CAN.mailbox_set_accept_mask(MailBox_num, 0x7FF, false);	// mailbox �ĺ��� ����ũ ����
	CAN.mailbox_set_id(MailBox_num, (CAN_ID | 0x80), false);	// mailbox �ĺ��� ID ����
}
