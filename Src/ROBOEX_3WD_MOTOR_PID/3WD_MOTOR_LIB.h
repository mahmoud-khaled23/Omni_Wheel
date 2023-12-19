#include "Arduino.h"

#define STOP	0
#define CW		1
#define CCW		2

void Motor_init(void);
void MOTOR_DIR(uint8_t num, uint8_t DIR);
void Speed_control(uint8_t num, uint16_t speed);

extern int pin_MOTOR[6];
extern int pin_MOTOR_EN[3];
