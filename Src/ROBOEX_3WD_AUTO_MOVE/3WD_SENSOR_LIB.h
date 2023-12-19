#include "Arduino.h"
#include "Timer_Int.h"

#define SEND 0
#define CNT 1
#define RECV 2

#define OFFSET 0.925

void ultraSend(uint8_t num);
void US_ISR(void);
void US_cnt_ISR(void);
uint16_t US_Fire(uint8_t num);
void US_send_ISR(void);
void SENSOR_init(void);
uint16_t PSD_Read(uint8_t num);
