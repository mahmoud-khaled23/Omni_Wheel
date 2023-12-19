#include "Arduino.h"

void LCD_write(unsigned char rs, char data);
void LCD_data_out(char da);
void LCD_init(void);
void LCD_printf(char *str);
