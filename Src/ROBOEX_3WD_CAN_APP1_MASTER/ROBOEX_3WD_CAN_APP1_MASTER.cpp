#include "ROBOEX_3WD_CAN_APP1_MASTER.h"
#include "due_can.h"
#include "font_5x7.h"

#define MAX_CAN_FRAME_DATA_LEN	8	// 전송 데이터 길이
#define CAN_TX_PRIO				5	// 우선 순위

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

// 선택 플래그 및 모드플래서 변수 선언
int Select_flag = 0, Mode_flag = 0;

// 모터제어 모듈의 현재 상태 값
int16_t data[5] = {0,0,0,0,0};	// G_led, Y_led, DC_dir, DC_speed, Servo, Step
// 모터제어 모듈의 설정될 제어 값
int16_t setup_data[5] = {0,0,0,0,0};

// LCD 출력 문자열 저장 변수
char buf_LED_G[] = "  GLED  ", buf_LED_Y[] = "  YLED  ";
char buf_3WD[] = "  3WD   ", buf_BUZZER[] = " BUZZER ";
char buf_MOVE[] = "  MOVE  ";

char buf_MODE_S[] = " SETUP ", buf_MODE_D[] = "DISPLAY";
char buf_ON[] = "ON ", buf_OFF[] = "OFF";

// 시간 체크 변수
long timer=0, Enter_timer=0;

union data_tr
{
	//transfer_data[0] -> mailbox_set_datal, transfer_data[1] -> mailbox_set_datah
	uint32_t transfer_data[2];
	uint8_t data[8];
};
data_tr CAN_BUFF;

RX_CAN_FRAME incoming;

int TX_flag = 0;	// 송신 플래그 변수
int RX_flag = 0;	// 수신 플래그 변수

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
void ENTER(void);
void UP(void);
void DOWN(void);
void LEFT(void);
void RIGHT(void);
void mode_out(void);
void name_out(void);
void data_value_out(uint8_t da);

void setup()
{
	int i;
	for(i=0; i<8; i++)
	{
		pinMode(pin_LCD_DATA[i],OUTPUT);	// LCD 데이터 포트 출력 설정
	}
	for(i=0; i<7; i++)
	{
		pinMode(pin_LCD_CTRL[i],OUTPUT);	// LCD Control 포트 출력 설정
	}
	for(i=0; i<2; i++)
	{
		pinMode(pin_LED[i],OUTPUT);		// LED 포트 출력 설정
		digitalWrite(pin_LED[i],LOW);	// LED 포트 LOW 출력
	}
	for(i=0; i<4; i++)
	{
		pinMode(pin_SWITCH[i],INPUT);	// 스위치 포트 입력 설정
	}

	// 외부 인터럽트 설정
	// 소스: ENTER 스위치, 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_SWITCH[0],ENTER,FALLING);
	// 소스: UP 스위치, 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_SWITCH[1],UP,FALLING);
	// 소스: DOWN 스위치, 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_SWITCH[2],DOWN,FALLING);
	// 소스: LEFT 스위치, 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_SWITCH[3],LEFT,FALLING);
	// 소스: RIGHT 스위치, 인터럽트 발생: FALLING(하강에지)
	attachInterrupt(pin_SWITCH[4],RIGHT,FALLING);
	delay(100);

	LCD_init();	// LCD 초기화

	LCD_clear();	// LCD clear

	Select_flag = 0;
	Mode_flag = 0;

	// CAN_BUFF 초기화
	CAN_BUFF.data[0] = 2;
	for(i=1; i<8; i++)
		CAN_BUFF.data[i] = 0;

	for(i=0; i<6; i++)
		setup_data[i] = data[i];	// 설정데이터 초기값 설정

	background_init();	// LCD 초기화면 출력

	Enter_timer = millis();	// Enter 키 입력 시간 체크

	CAN.init(SystemCoreClock, CAN_BPS_1000K);	// CAN 초기화

	CAN_RX_init(0,0);	// CAN 수신 초기화

	CAN_TX_init(1,2,CAN_TX_PRIO);	// CAN 송신 초기화

	TX_flag = 0;
}

void loop()
{
	int i;
	// CAN 송신
	if(TX_flag)
	{
		for(i=0; i<4; i++)
			CAN_BUFF.data[i+2] = setup_data[i];	// 설정 값 CAN_BUFF에 저장
		CAN_BUFF.data[6] = setup_data[4] >> 8;
		CAN_BUFF.data[7] = setup_data[4] & 0xFF;
		CAN.mailbox_set_datal(1, CAN_BUFF.transfer_data[0]);	// 하위 4바이트 송신 데이터 저장
		CAN.mailbox_set_datah(1, CAN_BUFF.transfer_data[1]);	// 상위 4바이트 송신 데이터 저장
		timer = millis();	// 현재 카운터값 저장
		RX_flag = 1;	// 수신 플래그 1로 설정
		TX_flag = 0;	// 수신 플래그 0으로 설정

		CAN.global_send_transfer_cmd(CAN_TCR_MB1);	// CAN 통신 요청
	}

	// CAN 수신 체크
	if(CAN.mailbox_get_status(0) & CAN_MSR_MRDY)
	{
		CAN.mailbox_read(0, &incoming);	// 수신 데이터 읽기

		for(i=0; i<4; i++)
		{
			data[i] = incoming.data[i+2];	// 수신 데이터 현재 상태 값에 저장
		}
		data[4] = (uint16_t)incoming.data[6]<<8 | incoming.data[7];

		for(i=0; i<6; i++)
		{
			setup_data[i] = data[i];		// 현재 상태 값을 설정 값에 저장
			data_value_out(i);				// LCD에 현재 상태 값 출력
		}
		RX_flag = 0;	// 수신 플래그 0으로 설정
	}
	if(RX_flag)
	{
		if((timer + 300) < millis())
		{
			CAN.global_send_abort_cmd(CAN_TCR_MB1);	// CAN 통신 요청 보류

			for(i=0; i<4; i++)
			{
				setup_data[i] = data[i];	// 현재 상태 값을 설정 값에 저장
				data_value_out(i);			// LCD에 현재 상태 값 출력
			}
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
	if(reversal)	// 색 반전
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

// LCD의 초기화면을 출력하는 함수
void background_init(void)
{
	int x,y,z,cursor_y;
	char da=0;
	Cursor_X(0);
	Character_print_data(" OMNI WHEEL 3WD ",1);
	Cursor_X(1);
	Character_print_data(" MODE :         ",1);

	for(x=2;x<8;x++)
	{
		Cursor_X(x);
		Cursor_Y(8);
		LCD_write(1,2,0xFF);
	}

	mode_out();	// 모드 출력
	name_out();	// 장치 명 출력
	for(x=0;x<6;x++)
		data_value_out(x);	// 현재 상태 값 출력
}

// 모드 설정하기 위한 외부 인터럽트 서비스 루틴
void ENTER(void)
{
	int x;
	if(TX_flag ==0 && RX_flag ==0)
	{
		// 키 입력시간 확인하여 100ms 내에 재입력시 무시
		if(Enter_timer+100 < millis())
		{
			if(Mode_flag)
			{
				Mode_flag = 0;	// Display 모드로 설정
				TX_flag = 1;	// 설정데이터 송신
			}
			else
			{
				Mode_flag = 1;	// Setup 모드로 설정
				Select_flag = 0;	// 커서를 맨위로 설정
			}
			mode_out();	// LCD에 현재 모드 출력
			name_out();	// Setup 모드시 커서 출력
		}
		Enter_timer = millis();	// 키 입력된 시간 저장
	}
}

// 설정 모드에서 커서를 위로 이동하기 위한 외부 인터럽트 서비스 루틴
void UP(void)
{
	if(Mode_flag)
	{
		Select_flag--;

		// 커서의 이동전 맨위 위치였을 경우 제일 아래로 이동
		if(Select_flag == -1)
		{
			Select_flag = 4;
		}
		name_out();	//선택된 커서 출력
	}
}

// 설정 모드에서 커서를 아래로 이동하기 위한 외부 인터럽트 서비스 루틴
void DOWN(void)
{
	if(Mode_flag)
	{
		Select_flag++;			// 커서의 위치를 아래로 이동

		// 커서의 이동전 맨아래 위치였을 경우 제일 위로 이동
		if(Select_flag == 5)
		{
			Select_flag = 0;
		}
		name_out();	//선택된 커서 출력
	}
}

// 설정 모드에서 설정 값을 감소하기 위한 외부 인터럽트 서비스 루틴
void LEFT(void)
{
	if(Mode_flag)
	{
		if(Select_flag != 4)
		{
			setup_data[Select_flag]--;	// STEP 모터외에 제어 값 1 감소
		}
		else
		{
			setup_data[Select_flag]-=15;	// STEP 모터의 기울기 값 10 감소
		}

		// 감소한 값이 제어최소값 보다 작을 경우 제어최대값으로 설정
		switch(Select_flag)
		{
			case 0:
			case 1:
			case 2:
			case 3:
				if(setup_data[Select_flag]<0)
				{
					setup_data[Select_flag] = 1;
				}
				break;
			case 4:
				if(setup_data[Select_flag]<0)
				{
					setup_data[Select_flag] = 345;
				}
				break;
		}
		data_value_out(Select_flag);	// 설정 값 출력
	}
}

// 설정 모드에서 설정 값을 증가하기 위한 외부 인터럽트 서비스 루틴
void RIGHT(void)
{
	if(Mode_flag)
	{
		if(Select_flag != 4)
		{
			setup_data[Select_flag]++;	// STEP 모터외에 제어 값 1 증가
		}
		else
		{
			setup_data[Select_flag]+=15;	// STEP 모터의 기울기 값 10 증가
		}

		// 증가한 값이 제어최대값 보다 클을 경우 제어최소값으로 설정
		switch(Select_flag)
		{
			case 0:
			case 1:
			case 2:
			case 3:
				if(setup_data[Select_flag]>1)
				{
					setup_data[Select_flag] = 0;
				}
				break;
			case 4:
				if(setup_data[Select_flag]==360)
				{
					setup_data[Select_flag] = 0;
				}
				break;
		}
		data_value_out(Select_flag);	// 설정 값 출력
	}
}

// 현재 모드를 LCD에 출력하는 함수
void mode_out(void)
{
	int z;
	Cursor_X(1);
	Cursor_Y(0);
	if(Mode_flag)
		for(z=0;z<7;z++)
			Character_write(2, buf_MODE_S[z],1);	// Setup 모드 출력
	else
		for(z=0;z<7;z++)
			Character_write(2, buf_MODE_D[z],1);	// Display 모드 출력
}

// 모터 제어 모듈의 제어 장치명을 LCD에 출력하는 함수
void name_out(void)
{
	int i,mask[6] = {0,0,0,0,0,0},cursor = 3;
	Cursor_Y(0);
	if(Mode_flag)	// Setup 모드 일 경우
	{
		for(i=0; i<5; i++)
		{
			if(i==Select_flag)		// 커서의 위치 표시
			{
				mask[i] = 1;
			}
		}
	}
	Cursor_X(cursor++);
	for(i=0; i<8; i++)
	{
		Character_write(1, buf_LED_G[i],mask[0]);	// "GLED" 출력
	}
	Cursor_X(cursor++);
	for(i=0; i<8; i++)
	{
		Character_write(1, buf_LED_Y[i],mask[1]);	// "YLED" 출력
	}
	Cursor_X(cursor++);
	for(i=0; i<8; i++)
		Character_write(1, buf_BUZZER[i],mask[2]);	// "BUZZER" 출력
	Cursor_X(cursor++);
	for(i=0; i<8; i++)
		Character_write(1, buf_MOVE[i],mask[3]);	// "MOVE" 출력
	Cursor_X(cursor++);
	for(i=0; i<8; i++)
		Character_write(1, buf_3WD[i],mask[4]);	// "3WD" 출력
}

// 모터제어 모듈의 현재 상태 및 제어 값 LCD에 출력하는 함수
void data_value_out(uint8_t da)
{
	int i,tmp;
	Cursor_X(da+3);
	Cursor_Y(24);
	switch(da)
	{
		// LED 및 BUZZER의 ON/OFF 출력
		case 0:
		case 1:
		case 2:
		case 3:
			if(setup_data[da])
			{
				for(i=0; i<3; i++)
				{
					Character_write(2, buf_ON[i],0);
				}
			}
			else
			{
				for(i=0; i<3; i++)
				{
					Character_write(2, buf_OFF[i],0);
				}
			}
			break;
		// 옴니휠 모듈의 이동방향 출력
		case 4:
			Character_write(2, setup_data[da]/100 + '0',0);
			Character_write(2, (setup_data[da]/10)%10 + '0',0);
			Character_write(2, setup_data[da]%10 + '0',0);
			break;
	}
}
