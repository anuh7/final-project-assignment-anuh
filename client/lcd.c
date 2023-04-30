/* Author : Malola Simman Srinivasan Kannan
 * Date : 28 April 2023
 * mail id : masr4788@colorado.edu
 * file name : lcd.c
 * Reference : https://forums.raspberrypi.com/viewtopic.php?t=93613
 */

#include <stdio.h>
#include <stdlib.h>
#include "wiringpi.h"
#include "lcd.h"

#define LCD_EN 24
#define LCD_RS 25
#define LCD_D4 4
#define LCD_D5 17
#define LCD_D6 27
#define LCD_D7 22

void Pulse_Enable()
{
   digitalWrite (LCD_EN, HIGH) ; 
   delay(0.5); //  1/2 microsecond pause - enable pulse must be > 450ns
   digitalWrite (LCD_EN, LOW) ; 
}

void setcmd_mode()
{
  digitalWrite (LCD_RS, 0); // set for commands
}

void setchar_mode()
{
  digitalWrite (LCD_RS, 1); // set for characters
}

void lcd_byte(char bits)
{
  digitalWrite (LCD_D4,(bits & 0x10)) ;
  digitalWrite (LCD_D5,(bits & 0x20)) ;
  digitalWrite (LCD_D6,(bits & 0x40)) ;
  digitalWrite (LCD_D7,(bits & 0x80)) ;
  Pulse_Enable();

  digitalWrite (LCD_D4,(bits & 0x1)) ;
  digitalWrite (LCD_D5,(bits & 0x2)) ;
  digitalWrite (LCD_D6,(bits & 0x4)) ;
  digitalWrite (LCD_D7,(bits & 0x8)) ;
  Pulse_Enable();
}

void lcd_str(char *str)
{
  while(*str)
  {
   lcd_byte(*str++);
  }
}

void printchar(char c, int addr)
{
   setcmd_mode();
   lcd_byte(addr);
   delay(3);
   setchar_mode();
   lcd_byte(c);
}

void lcd_init()
{
   wiringPiSetupGpio () ; // use BCIM numbering
   // set up pi pins for output
   pinMode (LCD_EN,  OUTPUT);
   pinMode (LCD_RS, OUTPUT);
   pinMode (LCD_D4, OUTPUT);
   pinMode (LCD_D5, OUTPUT);
   pinMode (LCD_D6, OUTPUT);
   pinMode (LCD_D7, OUTPUT);

   // initialise LCD
   setcmd_mode();  // set for commands
   lcd_byte(0x33); // full init
   lcd_byte(0x32); // 4 bit mode
   lcd_byte(0x28); // 2 line mode
   lcd_byte(0x0C); // display on, cursor off
   lcd_byte(0x01); // clear screen
   delay(3);       // clear screen is slow!
}
