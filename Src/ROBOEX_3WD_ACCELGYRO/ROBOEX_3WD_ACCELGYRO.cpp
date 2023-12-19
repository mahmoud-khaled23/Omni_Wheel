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
	Serial.begin(115200);	// UART0 115200 ����

	LCD_init();	// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf("HBE-ROBOEX 3WD");
	LCD_write(0,0xC0);
	LCD_printf("MPU-6050 Sensor");

	Wire.begin();

	Serial.println("Initializing I2C devices...\r");
	accelgyro.initialize();	// ���ӵ�����(MPU-6050)�ʱ�ȭ

	Serial.println("Testing device connections...\r");

	// ���ӵ����̷� ���� ���� Ȯ��
	Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful\r" : "MPU6050 connection failed\r");
}

void loop()
{
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);	// ���ӵ� �� ���̷� ������ �б�

    Serial.print("Accel : ");	// ���ӵ��� ���
    Serial.print(ax);
    Serial.print("\t");
    Serial.print(ay);
    Serial.print("\t ");
    Serial.print(az);
    Serial.print("\t Gyro : ");	// ���̷� �� ���
    Serial.print(gx);
    Serial.print("\t ");
    Serial.print(gy);
    Serial.print("\t ");
    Serial.println(gz);
    delay(200);
}
