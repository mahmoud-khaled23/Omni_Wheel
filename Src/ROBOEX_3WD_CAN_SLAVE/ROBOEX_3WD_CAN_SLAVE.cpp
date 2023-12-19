#include "ROBOEX_3WD_CAN_SLAVE.h"
#include "due_can.h"
#include "TEXT_LCD.h"

#define MAX_CAN_FRAME_DATA_LEN	8	// 전송 데이터 길이
#define CAN_TX_PRIO				10	// 우선 순위

int pin_LED[2] = {45,44};	// LED 핀 변수

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
		pinMode(pin_LED[i],OUTPUT);		// LED 포트 출력 설정
		digitalWrite(pin_LED[i],LOW);	// LED LOW 출력
	}

	LCD_init();	// LCD 초기화

	// LCD 출력
	LCD_write(0,0x80);
	LCD_printf(" HBE-ROBOEX");
	LCD_write(0,0xC0);
	LCD_printf(" OMNI WHEEL");

	CAN.init(SystemCoreClock, CAN_BPS_1000K);	// CAN 초기화

	CAN_RX_init(0,2);	// CAN 수신 초기화
	CAN_TX_init(1,0,CAN_TX_PRIO);	// CAN 송신 초기화
}

void loop()
{
	// CAN 수신 체크
	if(CAN.mailbox_get_status(0) & CAN_MSR_MRDY)
	{
		CAN.mailbox_read(0, &incoming);	// 수신 데이터 읽기
		for(int i=0; i<8; i++)
			CAN_BUFF.data[i] = incoming.data[i];	// 데이터를 CAN_BUFF에 저장

		// 수신 데이터에 따른 LED 제어
		digitalWrite(pin_LED[0],CAN_BUFF.data[3]);	// Green LED 제어
		digitalWrite(pin_LED[1],CAN_BUFF.data[5]);	// Yellow LED 제어

		CAN.mailbox_set_datal(1, CAN_BUFF.transfer_data[0]);	// 하위 4바이트 송신 데이터 저장
		CAN.mailbox_set_datah(1, CAN_BUFF.transfer_data[1]);	// 상위 4바이트 송신 데이터 저장

		CAN.global_send_transfer_cmd(CAN_TCR_MB1);	// CAN 통신 요청
	}
}

// CAN 송신 mailbox 초기화 함수
void CAN_TX_init(uint8_t MailBox_num,uint8_t CAN_ID,uint8_t PRIORITY)
{
	CAN.mailbox_init(MailBox_num);	// mailbox 초기화
	CAN.mailbox_set_mode(MailBox_num, CAN_MB_TX_MODE);	// mailbox 모드 설정
	CAN.mailbox_set_priority(MailBox_num, PRIORITY);	// mailbox 우선 순위 설정
	CAN.mailbox_set_accept_mask(MailBox_num, 0, false);	// mailbox 식별자 마스크 설정
	CAN.mailbox_set_id(MailBox_num, (CAN_ID | 0xC0), false);	// mailbox 식별자 ID 설정
	CAN.mailbox_set_datalen(MailBox_num, MAX_CAN_FRAME_DATA_LEN);	// 데이터 전송 길이 설정
}

// CAN 수신 mailbox 초기화 함수
void CAN_RX_init(uint8_t MailBox_num,uint8_t CAN_ID)
{
	CAN.mailbox_init(MailBox_num);	// mailbox 초기화
	CAN.mailbox_set_mode(MailBox_num, CAN_MB_RX_MODE);	// mailbox 모드 설정
	CAN.mailbox_set_accept_mask(MailBox_num, 0x7FF, false);	// mailbox 식별자 마스크 설정
	CAN.mailbox_set_id(MailBox_num, (CAN_ID | 0x80), false);	// mailbox 식별자 ID 설정
}
