#include "ROBOEX_3WD_TEMP.h"
#include "TEXT_LCD.h"

int pin_TEMP = A3;	// 온도 센서 ADC 핀 변수

void setup()
{
	LCD_init();	// LCD 초기화

	// LCD 출력
	LCD_write(0,0x80);
	LCD_printf("ADC Data : 0000");
	LCD_write(0,0xC0);
	LCD_printf("TEMP :  00.0[C]");

	analogReadResolution(12);	// ADC 해상도 12bit로 설정
}

void loop()
{

	int temp=0,adc_data = 0;	// 온도 및 ADC 데이터 변수 선언
	adc_data = analogRead(pin_TEMP);	// ADC 변환 데이터 read

	// ADC -> 온도 값으로 변환
	//(adc_data/4095*3.3-0.5) * 100
	// v = temp / 100 + 0.5
	temp = ((float)adc_data / 4095 * 3.3 -0.5) * 1000 ;

	// 커서 이동
	LCD_write(0,0x8B);
	// LCD에 ADC 데이터 출력
	LCD_write(1,adc_data/1000+'0');
	LCD_write(1,(adc_data/100)%10+'0');
	LCD_write(1,(adc_data/10)%10+'0');
	LCD_write(1,adc_data%10+'0');

	// 커서 이동
	LCD_write(0,0xC7);

	// LCD에 ADC 데이터 출력
	if(temp < 0)	// 측정온도가 음수일 경우
	{
		LCD_write(1,'-');
		temp = -temp;
	}
	else if(temp >= 1000)	// 측정온도가 100도 이상일 경우
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
