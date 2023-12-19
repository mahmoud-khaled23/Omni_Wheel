#include "ROBOEX_3WD_CAN_MASTER.h"
#include "due_can.h"
#include "font_5x7.h"

#define MAX_CAN_FRAME_DATA_LEN	8
#define CAN_TX_PRIO				5

#define WRITE	1
#define READ	0

#define LCD_RST(x)	digitalWrite(pin_LCD_CTRL[0],x)
#define LCD_RS(x)	digitalWrite(pin_LCD_CTRL[1],x)
#define LCD_RW(x)	digitalWrite(pin_LCD_CTRL[2],x)
#define LCD_E(x)	digitalWrite(pin_LCD_CTRL[3],x)
#define LCD_CS1(x)	digitalWrite(pin_LCD_CTRL[4],x)
#define LCD_CS2(x)	digitalWrite(pin_LCD_CTRL[5],x)
#define LCD_DIR(x)	digitalWrite(pin_LCD_CTRL[6],x)


// LCD data 핀 변수
int pin_LCD_DATA[8] = {54,55,56,57,58,59,60,61};
// LCD control 핀 변수
int pin_LCD_CTRL[7] = {8,52,9,73,6,7,43};	//RST,RS,RW,E,CS1,CS2,LCD_DIR
											//LCD_DIR: H->Write, L->Read
// LED 핀 변수
int pin_LED[2] = {45,44};	//Green, Yellow
// Switch 핀 변수
int pin_SWITCH[5] = {38,70,71,72,65};	//Enter, Up, Down, Left, Right

// 선택 플래그 변수 및 모터 제어 모듈 LED 제어 변수
uint8_t Select_flag = 0, LED_control[2] = {0,0};

// LCD 출력 문자열 저장 변수
char buf_LED_G[] = "Green ", buf_LED_Y[] = "Yellow";
char buf_MODE_S[] = " SETUP ", buf_MODE_C[] = "DISPLAY";

// LED 상태 표시를 위한 LCD 커서 위치
int cursor_OFF = 5, cursor_ON = 41;

union data_tr
{
	uint32_t transfer_data[2];	//transfer_data[0] -> mailbox_set_datal, transfer_data[1] -> mailbox_set_datah
	uint8_t data[8];
};
data_tr CAN_BUFF;

RX_CAN_FRAME incoming;

int TX_flag = 0;	// 송신 플래그 변수
int RX_flag = 0;	// 수신 플래그 변수
long timer=0;		// 시간 카운터 변수

void CAN_TX_init(uint8_t MailBox_num,uint8_t CAN_ID,uint8_t PRIORITY);
void CAN_RX_init(uint8_t MailBox_num,uint8_t CAN_ID);
void LCD_init(void);
void LCD_write(uint8_t RS,uint8_t cs,uint8_t data);
void Cursor_X(uint8_t X);
void Cursor_Y(uint8_t Y);
void Display_Start_Line(uint8_t Z);
void LCD_data_out(char da);
void LCD_clear(void);
void Character_write(uint8_t cs, char da, uint8_t reversal);
void Character_print_data(char *str, uint8_t reversal);
void background_init(void);
void LED_OFF(void);
void G_ON(void);
void Y_ON(void);
void GY_ON(void);
void LED_con_out(void);

void setup()
{
	int i;
	for(i=0; i<8; i++)
	{
		pinMode(pin_LCD_DATA[i],OUTPUT);
	}
	for(i=0; i<7; i++)
	{
		pinMode(pin_LCD_CTRL[i],OUTPUT);
	}
	for(i=0; i<2; i++)
	{
		pinMode(pin_LED[i],OUTPUT);
		digitalWrite(pin_LED[i],LOW);
	}
	for(i=0; i<4; i++)
	{
		pinMode(pin_SWITCH[i],INPUT);
	}

	LCD_init();	// LCD 초기화

	LCD_clear();	// LCD clear

	background_init();	// LCD 초기화면 출력

	// 외부 인터럽트 설정
	// 소스: ENTER 스위치, 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_SWITCH[0],LED_OFF,FALLING);
	// 소스: UP 스위치, 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_SWITCH[1],G_ON,FALLING);
	// 소스: DOWN 스위치, 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_SWITCH[2],Y_ON,FALLING);
	// 소스: LEFT 스위치, 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_SWITCH[3],GY_ON,FALLING);
	delay(100);

	CAN.init(SystemCoreClock, CAN_BPS_1000K);	// CAN 초기화

	CAN_RX_init(0,0);	// CAN 수신 초기화
	CAN_TX_init(1,2,CAN_TX_PRIO);	// CAN 송신 초기화

	// CAN_BUFF 초기화
	CAN_BUFF.data[0] = 2;
	for(i=1; i<8; i++)
		CAN_BUFF.data[i] = 0;

	TX_flag = 0;	// 송신 플래그 0으로 설정

}

void loop()
{
	// CAN 송신
	if(TX_flag)
	{
		CAN.mailbox_set_datal(1, CAN_BUFF.transfer_data[0]);	// 하위 4바이트 송신 데이터 저장
		CAN.mailbox_set_datah(1, CAN_BUFF.transfer_data[1]);	// 상위 4바이트 송신 데이터 저장
		timer = millis();	// 현재 카운터값 저장
		RX_flag = 1;	// 수신 플래그 1로 설정
		TX_flag = 0;	// 수신 플래그 0으로 설정

		// LCD 출력
		Cursor_X(7);
		Character_print_data("            ",0);

		CAN.global_send_transfer_cmd(CAN_TCR_MB1);	// CAN 통신 요청

	}
	// CAN 수신 체크
	if(CAN.mailbox_get_status(0) & CAN_MSR_MRDY)
	{
		CAN.mailbox_read(0, &incoming);	// 수신 데이터 읽기
		RX_flag = 0;	// 수신 플래그 0으로 설정

		// 송신 데이터와 수신 데이터 비교
		if((CAN_BUFF.data[3] == incoming.data[3]) && (CAN_BUFF.data[5] == incoming.data[5]))
		{
			// LCD 출력
			Cursor_X(7);
			Character_print_data("CAN Complete",0);

			// 수신된 LED 제어 값 저장
			LED_control[0] = incoming.data[3];
			LED_control[1] = incoming.data[5];

			// LCD에 모터 제어 모듈의 LED 상태 표시
			LED_con_out();
		}
	}

	if(RX_flag)
	{

		if((timer + 300) < millis())	// 송신 후 시간 체크
		{
			// LCD 출력
			Cursor_X(7);
			Character_print_data("CAN failure ",0);

			CAN.global_send_abort_cmd(CAN_TCR_MB1);	// CAN 통신 요청 보류
			RX_flag = 0;	// 수신 플래그 0으로 설정
		}
	}
}

// CAN 송신 mailbox 초기화 함수
void CAN_TX_init(uint8_t MailBox_num,uint8_t CAN_ID,uint8_t PRIORITY)
{
	CAN.mailbox_init(MailBox_num);	// mailbox 초기화
	CAN.mailbox_set_mode(MailBox_num, CAN_MB_TX_MODE);	// mailbox 모드 설정
	CAN.mailbox_set_priority(MailBox_num, PRIORITY);	// mailbox 우선 순위 설정
	CAN.mailbox_set_accept_mask(MailBox_num, 0, false);	// mailbox 식별자 마스크 설정
	CAN.mailbox_set_id(MailBox_num, (CAN_ID | 0x80), false);	// mailbox 식별자 ID 설정
	CAN.mailbox_set_datalen(MailBox_num, MAX_CAN_FRAME_DATA_LEN);	// 데이터 전송 길이 설정
}

// CAN 수신 mailbox 초기화 함수
void CAN_RX_init(uint8_t MailBox_num,uint8_t CAN_ID)
{
	CAN.mailbox_init(MailBox_num);	// mailbox 초기화
	CAN.mailbox_set_mode(MailBox_num, CAN_MB_RX_MODE);	// mailbox 모드 설정
	CAN.mailbox_set_accept_mask(MailBox_num, 0x7FF, false);	// mailbox 식별자 마스크 설정
	CAN.mailbox_set_id(MailBox_num, (CAN_ID | 0xC0), false);	// mailbox 식별자 ID 설정
}

// LCD 초기화면
void background_init(void)
{
	int x,y,z,cursor_y;
	char da=0;
	Cursor_X(0);
	Character_print_data("  OMNI WHEEL ",0);
	Cursor_X(1);
	Character_print_data("  LED Control",0);
	Cursor_X(3);
	Character_print_data(" LED    OFF  ON",0);
	Cursor_X(4);
	Cursor_Y(0);
	for(y=0;y<64;y++)
	{
		LCD_write(1,3,0x08);
	}
	for(x=3;x<7;x++)
	{
		Cursor_X(x);
		Cursor_Y(56);
		LCD_write(1,1,0xFF);
		Cursor_Y(32);
		LCD_write(1,2,0xFF);
	}
	Cursor_X(5);
	Cursor_Y(0);
	for(z=0;z<6;z++)
		Character_write(1, buf_LED_G[z],0);
	Cursor_X(6);
	Cursor_Y(0);
	for(z=0;z<6;z++)
		Character_write(1, buf_LED_Y[z],0);
	LED_con_out();
}

// 옴니휠모듈의 LED를 OFF하기 위한 외부인터럽트 서비스 루틴
void LED_OFF(void)
{
	if(RX_flag == 0)
	{
		CAN_BUFF.data[3] = 0;
		CAN_BUFF.data[5] = 0;
		TX_flag = 1;
	}
}

// 옴니휠모듈의 Green LED를 ON하기 위한 외부인터럽트 서비스 루틴
void G_ON(void)
{
	if(RX_flag == 0)
	{
		CAN_BUFF.data[3] = 1;
		CAN_BUFF.data[5] = 0;
		TX_flag = 1;
	}
}

// 옴니휠모듈의 Yellow LED를 ON하기 위한 외부인터럽트 서비스 루틴
void Y_ON(void)
{
	if(RX_flag == 0)
	{
		CAN_BUFF.data[3] = 0;
		CAN_BUFF.data[5] = 1;
		TX_flag = 1;
	}
}

// 옴니휠모듈의 LED를 ON하기 위한 외부인터럽트 서비스 루틴
void GY_ON(void)
{
	if(RX_flag == 0)
	{
		CAN_BUFF.data[3] = 1;
		CAN_BUFF.data[5] = 1;
		TX_flag = 1;
	}
}

//  옴니휠모듈 LED의 상태를 LCD에 표시하는 함수
void LED_con_out(void)
{
	uint8_t LCD_out_data = 0,y,z;
	for(z=0;z<2;z++)
	{
		if(LED_control[z])
			LCD_out_data = 0xFF;
		else
			LCD_out_data = 0x00;
		Cursor_X(z+5);
		Cursor_Y(cursor_OFF);
		for(y=0;y<13;y++)
			LCD_write(1,2,~LCD_out_data & 0x7F);
		Cursor_Y(cursor_ON);
		for(y=0;y<13;y++)
			LCD_write(1,2,LCD_out_data & 0x7F);
	}
}

// LCD 초기화 함수
void LCD_init(void)
{
	LCD_RST(1);	// LCD reset HIGH 출력
	LCD_RW(0);	// LCD RW LOW 출력 write signal
	LCD_E(0);	// LCD E LOW 출력
	LCD_CS1(0);	// LCD CS1 LOW 출력
	LCD_CS2(0);	// LCD CS2 LOW 출력

	LCD_DIR(WRITE);	// LCD_DIR HIGH 출력 Write signal

	delay(500);
	LCD_write(0,3,0x3F);	// LCD Display ON
	delay(100);
	Display_Start_Line(0);	// LCD display start line 0
}

// LCD에 명령/데이터 write 함수
void LCD_write(uint8_t RS,uint8_t cs,uint8_t data)
{
	switch(cs)
	{
		case 0:
			LCD_CS1(0);		// LCD_CS1 OFF
			LCD_CS2(0);		// LCD_CS2 OFF
			break;
		case 1:
			LCD_CS1(1);		// LCD_CS1 ON
			LCD_CS2(0);		// LCD_CS2 OFF
			break;
		case 2:
			LCD_CS1(0);		// LCD_CS1 OFF
			LCD_CS2(1);		// LCD_CS2 ON
			break;
		case 3:
			LCD_CS1(1);		// LCD_CS1 ON
			LCD_CS2(1);		// LCD_CS2 ON
			break;
	}

	LCD_RS(RS);		// LCD_RS 0->instruction, 1->data

	LCD_data_out(data);
	delayMicroseconds(1);
	LCD_E(1);		// LCD_E enable HIGH
	delayMicroseconds(1);
	LCD_E(0);		// LCD_E enable LOW
	delayMicroseconds(1);
}

// LCD page select function
void Cursor_X(uint8_t X)	//Page 0 ~ 7
{
	X = (X & 0x07) | 0xB8;
	LCD_write(0,3,X);
}

// LCD Y address select function
void Cursor_Y(uint8_t Y)	// Y address 0 ~ 63
{
	Y = (Y & 0x3F) | 0x40;
	LCD_write(0,3,Y);
}

// LCD Display Start line setting function
void Display_Start_Line(uint8_t Z)	// Z address 0 ~ 63
{
	Z = (Z & 0x3F) | 0xC0;
	LCD_write(0,3,Z);
}

// LCD data Port da value write function
void LCD_data_out(char da)
{
	int z;
	for(z=0;z<8;z++)
		digitalWrite(pin_LCD_DATA[z],(da>>z) & 0x1);
}

// LCD clear function
void LCD_clear(void)
{
	int x,y;
	Cursor_Y(0);
	for(x=0;x<8;x++)
	{
		Cursor_X(x);
		for(y=0;y<64;y++)
			LCD_write(1,3,0);
	}
}
// LCD 문자 write function
void Character_write(uint8_t cs, char da, uint8_t reversal)
{
	int z;
	if(reversal)
	{
		LCD_write(1,cs,0xFF);
		for(z=0;z<5;z++)
			LCD_write(1,cs,~font[da-0x20][z]);
		LCD_write(1,cs,0xFF);
		LCD_write(1,cs,0xFF);
	}
	else
	{
		LCD_write(1,cs,0);
		for(z=0;z<5;z++)
			LCD_write(1,cs,font[da-0x20][z]);
		LCD_write(1,cs,0);
		LCD_write(1,cs,0);
	}
}

// LCD 문자열 write function
void Character_print_data(char *str, uint8_t reversal)
{
  unsigned char i=0,cs;
  Cursor_Y(0);
  while(str[i]!='\0')
  {
	  if(i==16)
		  break;
	  else if(i < 8)
		  cs=1;
	  else
		  cs=2;
	  Character_write(cs, str[i++],reversal);
  }
}
