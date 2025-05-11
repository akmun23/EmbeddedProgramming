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
#define CLOSE_DOORS        12  // Close doors when elevator is at floor

#define TIME_BETWEEN_FLOORS 3000 // Time between floors in ms

#include <time.h>    // for struct tm, time_t if you have an RTC

#define MAX_LOG_ENTRIES  32  // adjust to taste

typedef enum { 
    TRIP_START, 
    TRIP_END 
} TripEvent_t;


typedef struct {
    int id;
    int startFloor;
    int endFloor;
} TripLog_t;

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
    INT8U endOfTrip;                   // End of trip flag
    TripLog_t log[128];

} Elevator;

Elevator myElevator;


extern QueueHandle_t xQueue_key, xQueue_lcd, xQueue_UART_TX;
extern Led_controller led_controller;
/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/



static void log_event(Elevator *elevator, TripEvent_t event);
/*****************************************************************************
*   Input    : Elevator struct pointer, Trip event type
*   Output   : None
*   Function : Records start and end of elevator trips in the log
******************************************************************************/

void dump_trip_log_uart(const Elevator *elev);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Sends elevator trip logs to UART for monitoring
******************************************************************************/

void elevator_init(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Initializes all elevator parameters to default values
******************************************************************************/

void elevator_task(void *pvParameters);
/*****************************************************************************
*   Input    : Task parameters (unused)
*   Output   : None
*   Function : Main task function that implements the elevator state machine
******************************************************************************/

void detect_hold_switch(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Detects when SW1 is held for 2 seconds to call elevator
******************************************************************************/

void display_current_floor(Elevator * elevator, Led_controller *led_controller);
/*****************************************************************************
*   Input    : Elevator struct pointer, LED controller pointer
*   Output   : None
*   Function : Shows current floor on LCD and simulates elevator movement with LED indicators
******************************************************************************/

void open_doors(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Animates doors opening on the LCD display
******************************************************************************/

void enter_password(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Accepts a 4-digit password input from the keypad
******************************************************************************/

void validate_password(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Validates if the entered password is divisible by 8
******************************************************************************/

void choose_floor(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Allows user to select destination floor using rotary encoder
******************************************************************************/

void accelerate_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Controls the acceleration phase of elevator movement with yellow LED
******************************************************************************/

void decelerate_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Controls the deceleration phase of elevator movement with red LED
******************************************************************************/

void break_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Simulates elevator breakdown with flashing LEDs
******************************************************************************/

void setup_rst_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Generates a random target value for elevator restart procedure
******************************************************************************/

void restart_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Uses potentiometer to match target value to restart elevator
******************************************************************************/

void fix_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Requires rotating encoder 360 degrees in alternating directions
******************************************************************************/

void fix_elevator_error(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Displays error message when rotary encoder is turned in wrong direction
******************************************************************************/

void exit_elevator(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Saves floor, closes elevator, displays farewell message and logs trip
******************************************************************************/

void getLog(const Elevator * elevator);

void close_doors(Elevator * elevator);
/*****************************************************************************
*   Input    : Elevator struct pointer
*   Output   : None
*   Function : Animates doors closing on the LCD display
******************************************************************************/
/****************************** End Of Module *******************************/

#endif /* ELEVATOR_H_ */





