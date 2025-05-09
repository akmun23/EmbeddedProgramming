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
/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
static uint16_t password = 1424;


static INT8U   key;

/*****************************   Functions   *******************************/


void elevator_task(void *pvParameters){

    INT8U elevator_state = CALL_ELEVATOR; // Elevator state
    while(1){

        switch(elevator_state){
            case CALL_ELEVATOR:
                enter_password();
                // Call elevator by holding SW1
            case DISPLAY_FLOOR:


                // Display the elevator's current floor
            case OPEN_DOORS:


            case ENTER_CODE:


            case VALIDATE_CODE:

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


void detect_hold_switch(void *pvParameters){
    // Detect if the hold switch is pressed
    // If pressed, call elevator
}

void display_current_floor(void *pvParameters){
    // Display the current floor on the LCD
}

void open_doors(void *pvParameters){
    // Open the elevator doors
}

void enter_password(void){
    uint8_t  count = 0;
    uint16_t code = 0;

    xQueueSend( xQueue_lcd, 'h', 0);
    vTaskDelay(pdMS_TO_TICKS(50));
    xQueueSend( xQueue_lcd, 'e', 0);
    vTaskDelay(pdMS_TO_TICKS(50));
    xQueueSend( xQueue_lcd, 'j', 0);
    vTaskDelay(pdMS_TO_TICKS(50));

    while (count < 4)
        {
            key = get_keyboard();
            if (key != 0 && key >= '0' && key <= '9')
            {
                code = code * 10 + (key - '0');
                count++;
                xQueueSend( xQueue_lcd, &key, 0);
            }
            vTaskDelay(pdMS_TO_TICKS(50));
        }

}

void validate_password(void *pvParameters){
    // Validate the entered password
}

void choose_floor(void *pvParameters){
    // Choose the destination floor using the rotary encoder
}

void accelerate_elevator(void *pvParameters){
    // Accelerate the elevator
}

void decelerate_elevator(void *pvParameters){
    // Decelerate the elevator
}

void break_elevator(void *pvParameters){
    // Break the elevator
}

void setup_rst_elevator(void *pvParameters){
    // Setup and reset the elevator
}

void restart_elevator(void *pvParameters){
    // Restart the elevator
}

void fix_elevator(void *pvParameters){
    // Fix the elevator
}

void fix_elevator_error(void *pvParameters){
    // Handle elevator error
}

void exit_elevator(void *pvParameters){
    // Exit the elevator
}

