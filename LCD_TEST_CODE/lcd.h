#ifndef __LCD_H__
#define __LCD_H__

#include "wiringpi.h"

void Pulse_Enable();
void lcd_byte(char bits);
void setcmd_mode();
void setchar_mode();
void lcd_str(char *str);
void printchar(char c, int addr);
void lcd_init();
#endif
