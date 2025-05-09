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
#include "glob_def.h"
#include "UI_task.h"
#include "adc.h"
#include "encoder.h"
#include "lcd.h"
/*****************************    Defines    *******************************/

#define CALL_ELEVATOR       0   // Call elevator by holding SW1
#define DISPLAY_FLOOR       1   // Display the elevator's current floor
#define OPEN_DOORS          2   // Open doors when elevator arrives at floor
#define ENTER_CODE          3   // Enter 4 digit password from keypad
#define VALIDATE_CODE       4   // Validate if password is divisible by 8
#define CHOOSE_FLOOR        5   // Choose destination floor with rotary encoder
#define ACC_ELEVATOR        6   // Accelerate elevator (yellow LED blinking)
#define DEC_ELEVATOR        7   // Decelerate elevator (red LED blinking)
#define BREAK_ELEVATOR      8   // Every 4th trip all LEDs blink
#define SETUP_RST_ELEVATOR  9   // Generate random number and display on screen
#define RESTART_ELEVATOR    10  // Use pot to reach goal value shown on screen
#define FIX_ELEVATOR        11  // Turn rotary encoder 360 degrees
#define FIX_ELEVATOR_ERROR  12  // Display error for wrong rotation direction
#define EXIT_ELEVATOR       13  // Save floor, close elevator, log trip


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


// FUNCTIONS FOR ELEVATOR TASK
void detect_hold_switch(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
******************************************************************************/

void display_current_floor(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
******************************************************************************/

void open_doors(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
******************************************************************************/

void enter_password(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
******************************************************************************/

void validate_password(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void choose_floor(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void accelerate_elevator(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void decelerate_elevator(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void break_elevator(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void setup_rst_elevator(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void restart_elevator(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void fix_elevator(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void fix_elevator_error(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void exit_elevator(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/



/****************************** End Of Module *******************************/

#endif /* ELEVATOR_H_ */





