#include "ROBOEX_3WD_ACCELGYRO_DMP.h"
#include "Text_LCD.h"
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

MPU6050 mpu;

uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, roll, pitch]   yaw/roll/pitch container and gravity vector

void setup()
{
	Wire.begin();

	Serial.begin(115200);

	LCD_init();	// LCD �ʱ�ȭ

	// LCD ���
	LCD_write(0,0x80);
	LCD_printf("Yaw  Pitch  Roll");
	LCD_write(0,0xC0);
	LCD_printf("000    00    00");

	Serial.println(F("Initializing I2C devices..."));
	mpu.initialize();	// ���ӵ�����(MPU-6050)�ʱ�ȭ

	Serial.println(F("Testing device connections..."));
	// ���ӵ����̷� ���� ���� Ȯ��
	Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

	Serial.println(F("Initializing DMP..."));
	// DMP �ʱ�ȭ
	devStatus = mpu.dmpInitialize();

	if (devStatus == 0) {

		Serial.println(F("Enabling DMP..."));
		// DMP Enable
		mpu.setDMPEnabled(true);

		packetSize = mpu.dmpGetFIFOPacketSize();
	}
	else
	{
		Serial.print(F("DMP Initialization failed (code "));
		Serial.print(devStatus);
		Serial.println(F(")"));
	}
}

void loop()
{
	int16_t ypr_data[3], i;
	while(1)
	{
		// MPU6050�� FIFO buffer reset
		mpu.resetFIFO();
		// ���� ���� Ȯ��
		mpuIntStatus = mpu.getIntStatus();
		// FIFO ī��Ʈ �� �б�
		fifoCount = mpu.getFIFOCount();
		if(mpuIntStatus & 0x02)
		{
			while(fifoCount < packetSize)
			{
				// FIFO ī��Ʈ �� �б�
				fifoCount = mpu.getFIFOCount();
			}
			// MPU6050�� FIFO buffer ���� fifoBuffer ������ ����
			mpu.getFIFOBytes(fifoBuffer, packetSize);
			fifoCount -= packetSize;
			// fifoBuffer�� ���� q �� ���
			mpu.dmpGetQuaternion(&q, fifoBuffer);
			// q ���� ���� gravity �� ���
			mpu.dmpGetGravity(&gravity, &q);
			// q, gravity ���� ���� yaw/roll/pitch �� ���
			mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
			break;
		}
	}
	for(i=0; i<3; i++)
	{
		// ������ ��ȯ
		ypr_data[i] = ypr[i] * 180/M_PI;
	}

	// -180 ~ 180���� 0 ~ 360 ���� ǥ���ǵ��� ��
	if(ypr_data[0] < 0)
	{
		ypr_data[0] += 360;
	}

	LCD_write(0,0xC0);
	// �ȴ��� ����� �ٶ󺸴� ���� ���
	if(ypr_data[0] > 100)
	{
		LCD_write(1, ypr_data[0]/100 + '0');
	}
	else
	{
		LCD_write(1, ' ');
	}
	LCD_write(1, (ypr_data[0]/10)%10 + '0');
	LCD_write(1, ypr_data[0]%10 + '0');

	LCD_write(0,0xC6);
	// �ȴ��� ����� �յڷ� ������ ���� ���
	if(ypr_data[2] < 0)
	{
		LCD_write(1, '-');
		ypr_data[2] = -ypr_data[2];
	}
	else
	{
		LCD_write(1, ' ');
	}
	LCD_write(1, ypr_data[2]/10 + '0');
	LCD_write(1, ypr_data[2]%10 + '0');

	LCD_write(0,0xCC);
	// �ȴ��� ����� �¿�� ������ ���� ���
	if(ypr_data[1] < 0)
	{
		LCD_write(1, '-');
		ypr_data[1] = -ypr_data[1];
	}
	else
	{
		LCD_write(1, ' ');
	}
	LCD_write(1, ypr_data[1]/10 + '0');
	LCD_write(1, ypr_data[1]%10 + '0');

	delay(200);
}
