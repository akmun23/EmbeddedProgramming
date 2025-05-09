/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: elevator.h
*
* PROJECT....: EMP
*
* DESCRIPTION: Control of the elevator task and operations
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 09/05/20252  MoH   Module created.
*
*****************************************************************************/

#ifndef ELEVATOR_H_
#define ELEVATOR_H_

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FreeRTOS.h"
#include "FRT_Files/inc/Task.h"
#include "FRT_Files/inc/queue.h"
#include "FRT_Files/inc/semphr.h"
#include "UI_task.h"
#include "adc.h"
#include "encoder.h"
#include "lcd.h"
/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/


void elevator_task(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Elevator task
******************************************************************************/

char int_to_char(INT8U number);
/*****************************************************************************
*   Input    : INT8U
*   Output   : char
*   Function : Change the input a number between 0-9 to a corresponding char between '0' - '9'
******************************************************************************/


/****************************** End Of Module *******************************/

#endif /* ELEVATOR_H_ */





