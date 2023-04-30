/* Author : Malola Simman Srinivasan Kannan
 * Date : 28 April 2023
 * mail id : masr4788@colorado.edu
 * file name : lcd.h
 * Reference : https://forums.raspberrypi.com/viewtopic.php?t=93613
 */
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
