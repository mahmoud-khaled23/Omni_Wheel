#include "Arduino.h"

void Motor_Stop(void);
void PID_buff_init(uint8_t num);
void ENC_cnt1(void);
void ENC_cnt2(void);
void ENC_cnt3(void);
void PID_control(void);
void Motor_DIR(uint8_t num,uint8_t DIR);
void Motor_init(void);
void Motor_DIR_speed_setup(uint8_t DIR,uint8_t SPEED, uint8_t dist);


extern int Encoder_flag;
