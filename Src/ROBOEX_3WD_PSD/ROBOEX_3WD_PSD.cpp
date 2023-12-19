#include "ROBOEX_3WD_PSD.h"
#include "TEXT_LCD.h"

int pin_PSD_SEN[3] = {A4, A6, A8};

uint16_t PSD_TR(uint16_t num);

void setup()
{
	for(int i=0; i<3; i++)
	{
		pinMode(pin_PSD_SEN[i], INPUT);
	}

	analogReadResolution(12);

	LCD_init();	// LCD 초기화

	// LCD 출력
	LCD_write(0,0x80);
	LCD_printf("ADC Data : 0000");
	LCD_write(0,0xC0);
	LCD_printf("DIST :    00 cm");
}

void loop()
{
	uint16_t Dist, ADC_data;

	// PSD_SEN1 ADC
	ADC_data = analogRead(pin_PSD_SEN[2]);

	// ADC -> cm으로 변환
	Dist = PSD_TR(ADC_data);

	// ADC data 출력
	LCD_write(0, 0x8B);
	LCD_write(1, ADC_data/1000 + '0');
	LCD_write(1, (ADC_data/100)%10 + '0');
	LCD_write(1, (ADC_data/10)%10 + '0');
	LCD_write(1, ADC_data%10 + '0');

	// Dist 출력
	LCD_write(0, 0xCA);
	LCD_write(1, Dist/10 + '0');
	LCD_write(1, Dist%10 + '0');

	delay(500);
}

// ADC 값을 거리 값으로 변환하는 함수
uint16_t PSD_TR(uint16_t adc_data)
{
	float tmp = 0;
	uint16_t dist;

	tmp = adc_data;

	// 1/L = 전압 * 0.047 - 0.008 = adc_data * 3.3 / 4096 *0.0434 - 0.0046
	tmp = tmp * 3.3 /4096 * 0.047 - 0.008;
	dist = (uint16_t)(1/tmp);

	// 10 ~ 80cm로 범위 지정
	dist = constrain(dist, 10, 80);

	return dist;
}
