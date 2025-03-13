#ifndef LCD_H_
#define LCD_H_
/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: lcd.h
*
* PROJECT....: EMP
*
* DESCRIPTION: Contains functionality of the LCD display
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 150215  MoH    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"

/*****************************    Defines    *******************************/
#define clear 0x01
#define setMode 0x28
#define autoIncrement 0x40
#define cursorAndDisplay 0x0E
#define D4 0x10
#define D5 0x20
#define D6 0x40
#define D7 0x80
#define EN 0x08
#define RS 0x04
#define shiftLeft 0x16
#define shiftRight 0x1A
#define row1 0x80
#define row2 0xC0

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

void lcd_init(void);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize the LCD display
******************************************************************************/

void Printdata(unsigned char data);
/*****************************************************************************
*   Input    : char
*   Output   : -
*   Function : Outputs data on pins over two iterations
******************************************************************************/

void lcd_cmd(unsigned char cmd);
/*****************************************************************************
*   Input    : char
*   Output   : -
*   Function : Selects the intended command
******************************************************************************/

void lcd_string(char *str, unsigned char len);
/*****************************************************************************
*   Input    : char, char
*   Output   : -
*   Function : Iterates through a given string and outputs it on dispaly
******************************************************************************/

void lcd_data(unsigned char data);
/*****************************************************************************
*   Input    : char
*   Output   : -
*   Function : Outputs the given char to the pins over two iterations
******************************************************************************/

void delay(long val);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Makes a delay
*******************************************************************************/

void clearDisplay(void);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Clears display
*******************************************************************************/

void setCursor(int row, int column);
/*****************************************************************************
*   Input    : int, int
*   Output   : -
*   Function : Sets cursor to designated destination
*******************************************************************************/

void displayTime(char *Time);
/*****************************************************************************
*   Input    : int arrray
*   Output   : -
*   Function : Displays time on LCD
*******************************************************************************/

#endif LCD_H_
