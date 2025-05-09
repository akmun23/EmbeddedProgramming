/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: leds.c
*
* PROJECT....: ECP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/

#include "elevator.h"
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


/*****************************   Variables   *******************************/

INT8U elevator_state = CALL_ELEVATOR; // Elevator state

/*****************************   Functions   *******************************/


void elevator_task(void *pvParameters){


    while(1){

        switch(elevator_state){
            case CALL_ELEVATOR:
                // Call elevator by holding SW1
            case DISPLAY_FLOOR:
                // Display the elevator's current floor
            case OPEN_DOORS:
                // Open doors when elevator arrives at floor
            case ENTER_CODE:
                // Enter 4 digit password from keypad
            case VALIDATE_CODE:
                // Validate if password is divisible by 8
            case CHOOSE_FLOOR:
                // Choose destination floor with rotary encoder
            case ACC_ELEVATOR:
                // Accelerate elevator (yellow LED blinking)
            case DEC_ELEVATOR:
                // Decelerate elevator (red LED blinking)
            case BREAK_ELEVATOR:
                // Every 4th trip all LEDs blink
            case SETUP_RST_ELEVATOR:
                // Generate random number and display on screen
            case RESTART_ELEVATOR:
                // Use pot to reach goal value shown on screen
            case FIX_ELEVATOR:
                // Turn rotary encoder 360 degrees
            case FIX_ELEVATOR_ERROR:
                // Display error for wrong rotation direction
            case EXIT_ELEVATOR:
                // Save floor, close elevator, log trip
                break;
            default:
                // Invalid state, handle error
                break;
        }
    }
}



char int_to_char(INT8U number){
    switch(number){
        case 0:
            return '0';
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        case 9:
            return '9';
    }

}


