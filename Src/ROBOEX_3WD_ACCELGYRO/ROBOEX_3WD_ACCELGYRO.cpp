#include "ROBOEX_3WD_ACCELGYRO.h"
#include "Text_LCD.h"
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

void setup()
{
	Serial.begin(115200);	// UART0 115200 설정

	LCD_init();	// LCD 초기화

	// LCD 출력
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("MPU-6050 Sensor");

	Wire.begin();

	Serial.println("Initializing I2C devices...\r");
	accelgyro.initialize();	// 가속도센서(MPU-6050)초기화

	Serial.println("Testing device connections...\r");

	// 가속도자이로 센서 연결 확인
	Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful\r" : "MPU6050 connection failed\r");
}

void loop()
{
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);	// 가속도 및 자이로 데이터 읽기

    Serial.print("Accel : ");	// 가속도값 출력
    Serial.print(ax);
    Serial.print("\t");
    Serial.print(ay);
    Serial.print("\t ");
    Serial.print(az);
    Serial.print("\t Gyro : ");	// 자이로 값 출력
    Serial.print(gx);
    Serial.print("\t ");
    Serial.print(gy);
    Serial.print("\t ");
    Serial.println(gz);
    delay(200);
}
