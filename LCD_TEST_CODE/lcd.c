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

static int lcd_addr[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
                       0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8 ,0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	               0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0X97, 0x98, 0x99, 0x9A, 0x9B, 0x9C ,0x9D, 0x9E, 0x9F,
	               0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC ,0xDD, 0xDE, 0xDF};

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


int main(int argc, char *argv [])
{
   lcd_init();
   setchar_mode();
   char str[] = "Hello_world :)";
   for(int i=0; str[i] !='\0'; i++)
   {
	   printchar(str[i],lcd_addr[i]);
	   if(i==63)
	   {
		   i=0;
	   }
   }
   return 0 ;
}

