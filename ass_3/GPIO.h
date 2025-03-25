#ifndef GPIO_H_
#define GPIO_H_
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
#include "tm4c123gh6pm.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

void GPIO_init(void);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialise GPIO pins
*******************************************************************************/

#endif GPIO_H_
