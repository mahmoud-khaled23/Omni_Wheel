#include "ROBOEX_3WD_TEMP.h"
#include "TEXT_LCD.h"

int pin_TEMP = A3;	// �µ� ���� ADC �� ����

void setup()
{
	LCD_init();	// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf("ADC Data : 0000");
	LCD_write(0,0xC0);
	LCD_printf("TEMP :  00.0[C]");

	analogReadResolution(12);	// ADC �ػ� 12bit�� ����
}

void loop()
{

	int temp=0,adc_data = 0;	// �µ� �� ADC ������ ���� ����
	adc_data = analogRead(pin_TEMP);	// ADC ��ȯ ������ read

	// ADC -> �µ� ������ ��ȯ
	//(adc_data/4095*3.3-0.5) * 100
	// v = temp / 100 + 0.5
	temp = ((float)adc_data / 4095 * 3.3 -0.5) * 1000 ;

	// Ŀ�� �̵�
	LCD_write(0,0x8B);
	// LCD�� ADC ������ ���
	LCD_write(1,adc_data/1000+'0');
	LCD_write(1,(adc_data/100)%10+'0');
	LCD_write(1,(adc_data/10)%10+'0');
	LCD_write(1,adc_data%10+'0');

	// Ŀ�� �̵�
	LCD_write(0,0xC7);

	// LCD�� ADC ������ ���
	if(temp < 0)	// �����µ��� ������ ���
	{
		LCD_write(1,'-');
		temp = -temp;
	}
	else if(temp >= 1000)	// �����µ��� 100�� �̻��� ���
	{
		LCD_write(1,'1');
	}
	else
	{
		LCD_write(1,' ');
	}
	LCD_write(1,(temp/100)%10+'0');
	LCD_write(1,(temp/10)%10+'0');
	LCD_write(1,'.');
	LCD_write(1,adc_data%10+'0');
	delay(500);
}
