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
typedef struct{
    INT8U elevator_state;               // Elevator state
    INT8U current_floor;                // Current floor of the elevator
    INT8U destination_floor;            // Destination floor
    INT8U password[4];                  // Password entered by user
    INT8U elevator_acceleration;        // Elevator acceleration value
    INT8U elevator_deceleration;        // Elevator deceleration value
    INT8U speed;                        // Elevator speed
    BOOLEAN door_status;                // Door status (open/closed)
    INT8U numberOfTrips;                // Number of trips made
} elevator;

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/
void elevator_init(void){
    // Initialize elevator system
    myElevator.elevator_state = CALL_ELEVATOR;
    myElevator.current_floor = 2;
    myElevator.destination_floor = 2;
    INT8U temp_password[4] = {7, 9, 1, 3};
    memcpy(myElevator.password, temp_password, sizeof(temp_password));
    myElevator.elevator_acceleration = 0;
    myElevator.elevator_deceleration = 0;
    myElevator.speed = 0;
    myElevator.door_status = FALSE;
    myElevator.numberOfTrips = 0;
}

void elevator_task(void *pvParameters){

    elevator_init();
    red_led_init();
    green_led_init();
    yellow_led_init();
    GPIO_PORTF_DATA_R &= ~(0x02);
    GPIO_PORTF_DATA_R |= 0x04;
    GPIO_PORTF_DATA_R |= 0x08;
    while(1){


        switch(myElevator.elevator_state){
            case CALL_ELEVATOR:
                detect_hold_switch(NULL);
                myElevator.elevator_state = DISPLAY_FLOOR;
                GPIO_PORTF_DATA_R &= ~(0x08);
                GPIO_PORTF_DATA_R |= 0x02;
                break;
            case DISPLAY_FLOOR:
                display_current_floor(NULL);
                myElevator.elevator_state = OPEN_DOORS;
                break;
            case OPEN_DOORS:
                open_doors(NULL);
                myElevator.elevator_state = ENTER_CODE;
                // Open doors when elevator arrives at floor
                break;
            case ENTER_CODE:
                enter_password(NULL);
                myElevator.elevator_state = VALIDATE_CODE;
                // Enter 4 digit password from keypad
                break;
            case VALIDATE_CODE:
                validate_password(NULL);
                myElevator.elevator_state = CHOOSE_FLOOR;
                // Validate if password is divisible by 8
                break;
            case CHOOSE_FLOOR:
                choose_floor(NULL);
                myElevator.elevator_state = ACC_ELEVATOR;
                // Choose destination floor with rotary encoder
                break;
            case ACC_ELEVATOR:
                accelerate_elevator(NULL);
                myElevator.elevator_state = DEC_ELEVATOR;
                // Accelerate elevator (yellow LED blinking)
                break;
            case DEC_ELEVATOR:
                decelerate_elevator(NULL);
                myElevator.elevator_state = BREAK_ELEVATOR;
                // Decelerate elevator (red LED blinking)
                break;
            case BREAK_ELEVATOR:
                break_elevator(NULL);
                myElevator.elevator_state = SETUP_RST_ELEVATOR;
                // Every 4th trip all LEDs blink
                break;
            case SETUP_RST_ELEVATOR:
                setup_rst_elevator(NULL);
                myElevator.elevator_state = RESTART_ELEVATOR;
                // Generate random number and display on screen
                break;
            case RESTART_ELEVATOR:
                restart_elevator(NULL);
                myElevator.elevator_state = FIX_ELEVATOR;
                // Use pot to reach goal value shown on screen
                break;
            case FIX_ELEVATOR:
                fix_elevator(NULL);
                myElevator.elevator_state = FIX_ELEVATOR_ERROR;
                // Turn rotary encoder 360 degrees
                break;
            case FIX_ELEVATOR_ERROR:
                fix_elevator_error(NULL);
                myElevator.elevator_state = EXIT_ELEVATOR;
                // Display error for wrong rotation direction
                break;
            case EXIT_ELEVATOR:
                exit_elevator(NULL);
                myElevator.elevator_state = CALL_ELEVATOR;
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
    uint32_t start_time = xTaskGetTickCount();
    uint32_t current_time;
    uint32_t elapsed_time;

    BOOLEAN hold_switch_pressed = FALSE;
    while(1){

        // Check if hold switch is pressed
        if(!(GPIO_PORTF_DATA_R & 0x10) && !hold_switch_pressed){
            // Hold switch is pressed, start timer
            start_time = xTaskGetTickCount();
            hold_switch_pressed = TRUE;
        }else if((GPIO_PORTF_DATA_R & 0x10) && hold_switch_pressed){
            // Hold switch is released, reset timer
            hold_switch_pressed = FALSE;
        }

        if(hold_switch_pressed){
            current_time = xTaskGetTickCount();
            elapsed_time = (current_time - start_time)*portTICK_PERIOD_MS; // Convert to ms
    
            if(elapsed_time >= 2000.0){

                break;

            }
        }
        vTaskDelay(10 / portTICK_RATE_MS); // Delay to avoid busy waiting

    }
}   

// When the button has been pressed for 2 seconds, the elevator will start moving
// to the floor where the button was pressed and the elevator will stop at that floor
// This will be displayed on the LCD
void display_current_floor(void *pvParameters){
    while(1)
    {
        INT8U i;
        char floor_str[16] = "Floor: ";
        char floor_str[8] = int_to_char(myElevator.current_floor / 10);
        char floor_str[9] = int_to_char(myElevator.current_floor % 10);

        for(i = 0; i < 16; i++){
            xQueueSend(xQueueLCD, &floor_str[i], 0);
        }
       
        // Wait for 1 second
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        // Check if the elevator has reached the destination floor
        if(myElevator.current_floor == myElevator.destination_floor){
            break;
        } else {
            // Move the elevator towards the destination floor
            if(myElevator.current_floor < myElevator.destination_floor){
                // Move elevator up
                myElevator.current_floor++;

                // Check if the elevator is at the 13th floor which doesnt exist
                // and skip it
                if(myElevator.current_floor == 13){
                    myElevator.current_floor++;
                }
            } else {
                // Move elevator down
                myElevator.current_floor--;

                // Check if the elevator is at the 13th floor which doesnt exist
                // and skip it
                if(myElevator.current_floor == 13){
                    myElevator.current_floor--;
                }
            }
        }

        // Reset the LCD cursor to the beginning
        move_LCD(0, 0);
    }
}

void open_doors(void *pvParameters){
    // Open the elevator doors
}

void enter_password(void *pvParameters){
    // Enter the password using the keypad
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

