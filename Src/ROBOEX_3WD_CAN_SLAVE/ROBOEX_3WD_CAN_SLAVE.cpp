#include "ROBOEX_3WD_CAN_SLAVE.h"
#include "due_can.h"
#include "TEXT_LCD.h"

#define MAX_CAN_FRAME_DATA_LEN	8	// ���� ������ ����
#define CAN_TX_PRIO				10	// �켱 ����

int pin_LED[2] = {45,44};	// LED �� ����

RX_CAN_FRAME incoming;

union data_tr
{
	//transfer_data[0] -> mailbox_set_datal, transfer_data[1] -> mailbox_set_datah
	uint32_t transfer_data[2];
	uint8_t data[8];
};
data_tr CAN_BUFF;

void CAN_TX_init(uint8_t MailBox_num,uint8_t CAN_ID,uint8_t PRIORITY);
void CAN_RX_init(uint8_t MailBox_num,uint8_t CAN_ID);

void setup()
{
	for(int i=0; i<2; i++)
	{
		pinMode(pin_LED[i],OUTPUT);		// LED ��Ʈ ��� ����
		digitalWrite(pin_LED[i],LOW);	// LED LOW ���
	}

	LCD_init();	// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf(" HBE-ROBOEX");
	LCD_write(0,0xC0);
	LCD_printf(" OMNI WHEEL");

	CAN.init(SystemCoreClock, CAN_BPS_1000K);	// CAN �ʱ�ȭ

	CAN_RX_init(0,2);	// CAN ���� �ʱ�ȭ
	CAN_TX_init(1,0,CAN_TX_PRIO);	// CAN �۽� �ʱ�ȭ
}

void loop()
{
	// CAN ���� üũ
	if(CAN.mailbox_get_status(0) & CAN_MSR_MRDY)
	{
		CAN.mailbox_read(0, &incoming);	// ���� ������ �б�
		for(int i=0; i<8; i++)
			CAN_BUFF.data[i] = incoming.data[i];	// �����͸� CAN_BUFF�� ����

		// ���� �����Ϳ� ���� LED ����
		digitalWrite(pin_LED[0],CAN_BUFF.data[3]);	// Green LED ����
		digitalWrite(pin_LED[1],CAN_BUFF.data[5]);	// Yellow LED ����

		CAN.mailbox_set_datal(1, CAN_BUFF.transfer_data[0]);	// ���� 4����Ʈ �۽� ������ ����
		CAN.mailbox_set_datah(1, CAN_BUFF.transfer_data[1]);	// ���� 4����Ʈ �۽� ������ ����

		CAN.global_send_transfer_cmd(CAN_TCR_MB1);	// CAN ��� ��û
	}
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
