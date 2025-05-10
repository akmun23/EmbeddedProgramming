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
#include "leds.h"
#include "uart0.h"
/*****************************    Defines    *******************************/

#define CALL_ELEVATOR       0   // Call elevator by holding SW1
#define DISPLAY_FLOOR       1   // Display the elevator's current floor
#define OPEN_DOORS          2   // Open doors when elevator arrives at floor
#define ENTER_CODE          3   // Enter 4 digit password from keypad
#define VALIDATE_CODE       4   // Validate if password is divisible by 8
#define CHOOSE_FLOOR        5   // Choose destination floor with rotary encoder
#define BREAK_ELEVATOR      6   // Every 4th trip all LEDs blink
#define SETUP_RST_ELEVATOR  7   // Generate random number and display on screen
#define RESTART_ELEVATOR    8  // Use pot to reach goal value shown on screen
#define FIX_ELEVATOR        9  // Turn rotary encoder 360 degrees
#define FIX_ELEVATOR_ERROR  10  // Display error for wrong rotation direction
#define EXIT_ELEVATOR       11  // Save floor, close elevator, log trip

#define TIME_BETWEEN_FLOORS 1000 // Time between floors in ms

#include <time.h>    // for struct tm, time_t if you have an RTC

#define MAX_LOG_ENTRIES  32  // adjust to taste

typedef enum { 
    TRIP_START, 
    TRIP_END 
} TripEvent_t;

typedef struct{
    INT8U elevator_state;               // Elevator state
    INT8U current_floor;                // Current floor of the elevator
    INT8U destination_floor;            // Destination floor
    INT16U password;                    // Password entered by user
    INT8U elevator_acceleration;        // Elevator acceleration value
    INT8U elevator_deceleration;        // Elevator deceleration value
    INT8U speed;                        // Elevator speed
    BOOLEAN door_status;                // Door status (open/closed)
    INT8U numberOfTrips;                // Number of trips made
    INT8U rot_direction;                // Direction of the rotary encoder
    INT16U goal_number;
    TripLog_t log[128];

} Elevator;

typedef struct {
    int id;
    int startFloor;    
    int endFloor;   
} TripLog_t;

extern QueueHandle_t xQueue_key, xQueue_lcd;
extern Led_controller led_controller;
/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

void elevator_init(Elevator * elevator);


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
void detect_hold_switch(Elevator * elevator);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
******************************************************************************/

void display_current_floor(Elevator * elevator, Led_controller *led_controller);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
******************************************************************************/

void open_doors(Elevator * elevator);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
******************************************************************************/

void enter_password(Elevator * elevator);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
******************************************************************************/

void validate_password(Elevator * elevator);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void choose_floor(Elevator * elevator);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void accelerate_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void decelerate_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void break_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void setup_rst_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void restart_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void fix_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void fix_elevator_error(Elevator * elevator);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void exit_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

void dump_trip_log_uart(const Elevator *elev);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : 
***********************************************************************************/

static void log_event(TripLog_t *log, TripEvent_t ev);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Write low part of control data to LCD.
******************************************************************************/

/****************************** End Of Module *******************************/

#endif /* ELEVATOR_H_ */





