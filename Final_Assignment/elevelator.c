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
void elevator_init(Elevator * elevator){
    elevator->elevator_state = CALL_ELEVATOR;
    elevator->current_floor = 2;
    elevator->destination_floor = 15;
    elevator->password = 0;
    elevator->elevator_acceleration = 0;
    elevator->elevator_deceleration = 0;
    elevator->speed = 0;
    elevator->door_status = FALSE;
    elevator->numberOfTrips = 0;
}

void elevator_task(void *pvParameters){

    Elevator myElevator;
    elevator_init(&myElevator);
    red_led_init();
    green_led_init();
    yellow_led_init();

    led_controller.led_state = DOOR_CLOSED;
    while(1){

        switch(myElevator.elevator_state){
            case CALL_ELEVATOR:
                detect_hold_switch(&myElevator);
                myElevator.elevator_state = DISPLAY_FLOOR;
                break;
            case DISPLAY_FLOOR:
                display_current_floor(&myElevator, &led_controller);
                myElevator.elevator_state = OPEN_DOORS;
                break;
            case OPEN_DOORS:
                led_controller.led_state = DOOR_OPENING;
                open_doors(&myElevator);
                led_controller.led_state = DOOR_OPEN;
                myElevator.elevator_state = ENTER_CODE;
                break;
            case ENTER_CODE:
                enter_password(&myElevator);
                myElevator.elevator_state = VALIDATE_CODE;
                break;
            case VALIDATE_CODE:
                validate_password(&myElevator);
                break;
            case CHOOSE_FLOOR:
                choose_floor(&myElevator);
                myElevator.elevator_state = DISPLAY_FLOOR;
            case ACC_ELEVATOR:
                
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


void detect_hold_switch(Elevator * elevator){
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

void display_current_floor(Elevator * elevator, Led_controller *led_controller){
    INT8U startFloor = elevator->current_floor;
    INT8U endFloor = elevator->destination_floor;
    led_controller->led_state = ELEVATOR_ACCELERATING;
    while(1)
    {
        if ((endFloor-startFloor)/2 >= elevator->destination_floor - elevator->current_floor)
        {
            led_controller->led_state = ELEVATOR_DECELERATING;
        }
        

        
        INT8U i;
        char floor_str[16] = "Floor: ";
        floor_str[7] = int_to_char(elevator->current_floor / 10);
        floor_str[8] = int_to_char(elevator->current_floor % 10);

        move_LCD(0,0);
        for(i = 0; i < 9; i++){
            xQueueSend(xQueue_lcd, &floor_str[i], 0);
        }
       

        // Check if the elevator has reached the destination floor
        if(elevator->current_floor == elevator->destination_floor){
            break;
        } else {
            // Move the elevator towards the destination floor
            if(elevator->current_floor < elevator->destination_floor){
                // Move elevator up
                elevator->current_floor++;

                // Check if the elevator is at the 13th floor which doesnt exist
                // and skip it
                if(elevator->current_floor == 13){
                    elevator->current_floor++;
                }
            } else {
                // Move elevator down
                elevator->current_floor--;

                // Check if the elevator is at the 13th floor which doesnt exist
                // and skip it
                if(elevator->current_floor == 13){
                    elevator->current_floor--;
                }
            }
        }

        // Reset the LCD cursor to the beginning
        move_LCD(0, 0);
        vTaskDelay(1000 / portTICK_RATE_MS); // Delay to avoid busy waiting
    }
}

void open_doors(Elevator * elevator){
    INT8U i;
    INT8U j = 0;
    INT8U door_str[16];

    for(i = 0; i < 16; i++){
        door_str[i] = 0x7C;
    }
    while(1){


        door_str[7-j] = ' ';
        door_str[8+j] = ' ';
        if(j == 8){
            break;
        }
        j++;
        move_LCD(0, 0);
        for(i = 0; i < 16; i++){
            xQueueSend(xQueue_lcd, &door_str[i], 0);
        }

        move_LCD(0, 1);
        for(i = 0; i < 16; i++){
            xQueueSend(xQueue_lcd, &door_str[i], 0);
        }


        vTaskDelay(1000 / portTICK_RATE_MS); // Delay to avoid busy waiting
    }
}

void enter_password(Elevator * elevator){
    INT8U i;
    uint8_t  count = 0;
    clr_LCD();
    vTaskDelay(500 / portTICK_RATE_MS); // Delay to avoid busy waiting
    move_LCD(0,0);

    INT8U passowrd_str[10] = "Password: ";
    for(i = 0; i < 10; i++){
        xQueueSend(xQueue_lcd, &passowrd_str[i], 0);
    }
    INT8U star = '*';
    while (count < 4)
    {
        key = get_keyboard();
        if (key != 0 && key >= '0' && key <= '9')
        {

            elevator->password = elevator->password * 10 + (key - '0');
            count++;
            xQueueSend( xQueue_lcd, &star, 0);
        }
        vTaskDelay(10 / portTICK_RATE_MS); // Delay to avoid busy waiting
    }
}


void validate_password(Elevator * elevator){
    if((elevator->password % 8) == 0 && elevator->password > 0){
        const char* msg = "Valid password";
        move_LCD(0,0);
        while (*msg) {

            xQueueSend(xQueue_lcd, msg, 0);
            msg++;
        }
        elevator -> elevator_state = CHOOSE_FLOOR;
    } else {
        const char* msg = "Invalid password";
        move_LCD(0,0);
        while (*msg) {
            xQueueSend(xQueue_lcd, msg, 0);
            msg++;
        }
        elevator -> elevator_state = ENTER_CODE;
    }
    vTaskDelay(2000 / portTICK_RATE_MS); // Delay to avoid busy waiting
}

void choose_floor(Elevator * elevator){

    clr_LCD();
    vTaskDelay(1000 / portTICK_RATE_MS); // Delay to avoid busy waiting
    int i = 0;

    INT8U targetFloor = elevator->current_floor;
    INT8U state = 0;

    INT8U new_encoder_data = GPIO_PORTA_DATA_R & 0xE0;
    INT8U old_encoder_data = new_encoder_data;
    BOOLEAN IncDec = FALSE;
    BOOLEAN startElevator = FALSE;

    char output_str[9] = "Floor:   ";
    output_str[7] = int_to_char(elevator->current_floor / 10), 
    output_str[8] = int_to_char(elevator->current_floor % 10);

    for(i = 0; i < 9; i++){
        xQueueSend( xQueue_lcd, &output_str[i], 0);
    }

    while(!startElevator){
        new_encoder_data = GPIO_PORTA_DATA_R & 0xE0;
        switch(state){
            case 0:
                if (((new_encoder_data & 0x20) != (old_encoder_data & 0x20))){

                    if(new_encoder_data & 0x20){
                        if(new_encoder_data & 0x40){
                            targetFloor--;
                            IncDec = FALSE;
                        }else{
                            targetFloor++;
                            IncDec = TRUE;
                        }
                    }else{
                        if(new_encoder_data & 0x40){
                            targetFloor++;
                            IncDec = TRUE;
                        }else{
                            targetFloor--;
                            IncDec = FALSE;
                        }
                    }

                    if(targetFloor > 50){
                        targetFloor = 0;
                    }else if(targetFloor > 20){
                        targetFloor = 20;
                    }else if(targetFloor == 13 && IncDec == TRUE){
                        targetFloor++;
                    }else if(targetFloor == 13 && IncDec == FALSE){
                        targetFloor--;
                    }
                    state++;
                }

                if (((new_encoder_data & 0x80) != (old_encoder_data & 0x80))){
                    elevator->destination_floor = targetFloor;
                    startElevator = TRUE;
                    break;
                }

                old_encoder_data = new_encoder_data;
                break;

            case 1:
                move_LCD(0,0);
                output_str[7] = int_to_char(targetFloor / 10);
                output_str[8] = int_to_char(targetFloor % 10);
                for(i = 0; i < 9; i++){
                    xQueueSend( xQueue_lcd, &output_str[i], portMAX_DELAY);
                }
                state = 0;
                break;

            default:
                break;
        }

        vTaskDelay(1 / portTICK_RATE_MS);
    }
}

void accelerate_elevator(Elevator * elevator){
    // Accelerate the elevator
}

void decelerate_elevator(Elevator * elevator){
    // Decelerate the elevator
}

void break_elevator(Elevator * elevator){
    // Break the elevator
}

void setup_rst_elevator(Elevator * elevator){
    // Setup and reset the elevator
}

void restart_elevator(Elevator * elevator){
    // Restart the elevator
}

void fix_elevator(Elevator * elevator){
    // Fix the elevator
}

void fix_elevator_error(Elevator * elevator){
    // Handle elevator error
}

void exit_elevator(Elevator * elevator){
    // Exit the elevator
}

