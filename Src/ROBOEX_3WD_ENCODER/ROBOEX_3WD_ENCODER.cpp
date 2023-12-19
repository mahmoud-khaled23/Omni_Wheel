#include "ROBOEX_3WD_ENCODER.h"
#include "TEXT_LCD.h"
#include "3WD_MOTOR_LIB.h"

enum{num1, num2, num3}; // ���� ��ȣ

// 1WD_ENC_A, 1WD_ENC_B, 2WD_ENC_A, 2WD_ENC_B, 3WD_ENC_A, 3WD_ENC_B
int pin_ENCODER[6] = {66,67,94,95,52,78};

long encoder_counter1 = 0, encoder_value;	// ���ڴ� �� ���� ����

void ENC_count1(void);

//The setup function is called once at startup of the sketch
void setup()
{
	int i;
	Motor_init();

	for(i=0; i<6; i++)
	{
		pinMode(pin_ENCODER[i], INPUT); // Encoder �Է� ����
	}
	LCD_init();	// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf("Count :  000000");
	LCD_write(0,0xC0);
	LCD_printf("Rotation : 0000");

	// �ܺ� ���ͷ�Ʈ ����
	// �ҽ�: pin_ENCODER[0](1WD_ENC_A) , ���ͷ�Ʈ �߻�: FALLING(�ϰ�����)
	attachInterrupt(pin_ENCODER[0],ENC_count1,FALLING);

	encoder_counter1 = 0;	// ���ڴ� ī���� ���� 0���� �ʱ�ȭ

	digitalWrite(pin_MOTOR_EN[0],1);	// 1WD ���� ON

	MOTOR_DIR(num1,CW);	// 1WD ���� ��ȸ��
}

void loop()
{
	encoder_value = encoder_counter1;
	if(encoder_value > 999999)
	{
		encoder_counter1 = 0;
		encoder_value = 0;
	}

	LCD_write(0,0x89);
	// 1WD Encoder ī���� �� ���
	LCD_write(1, encoder_value/100000 + '0');
	LCD_write(1, (encoder_value/10000)%10 + '0');
	LCD_write(1, (encoder_value/1000)%10 + '0');
	LCD_write(1, (encoder_value/100)%10 + '0');
	LCD_write(1, (encoder_value/10)%10 + '0');
	LCD_write(1, encoder_value%10 + '0');

	encoder_value /= 390;  // 1WD ȸ���� ���

	LCD_write(0,0xCB);
	// 1WD ���� ȸ���� ���
	LCD_write(1, encoder_value/1000 + '0');
	LCD_write(1, (encoder_value/100)%10 + '0');
	LCD_write(1, (encoder_value/10)%10 + '0');
	LCD_write(1, encoder_value%10 + '0');

	delay(500);
}

// 1WD encoder�� ī�����ϴ� �ܺ����ͷ�Ʈ ���� ��ƾ
void ENC_count1(void)
{
	encoder_counter1++;
}
