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

/*****************************   Functions   *******************************/
static void log_event(TripEvent_t event) {
    if(event == TRIP_START){
        myElevator.log[myElevator.numberOfTrips - 1].id = myElevator.numberOfTrips;
        myElevator.log[myElevator.numberOfTrips - 1].startFloor = myElevator.current_floor;

    } else if(event == TRIP_END){
        myElevator.log[myElevator.numberOfTrips - 1].endFloor = myElevator.destination_floor;
    }
}

void getLog(void) {
    INT8U j, k;
    char newline = '\n';
    char resetline = '\r';

    while (xQueueSend(xQueue_UART_TX, &newline, 10) != pdTRUE) vTaskDelay(1);
    while (xQueueSend(xQueue_UART_TX, &resetline, 10) != pdTRUE) vTaskDelay(1);

    if (myElevator.numberOfTrips > 0) {
        for (j = 0; j < myElevator.numberOfTrips; j++) {
            char NextStrQueue[43] = "Trip ID: 00, Start Floor: 00, End Floor: 00";
            NextStrQueue[9] = int_to_char(myElevator.log[j].id / 10);
            NextStrQueue[10] = int_to_char(myElevator.log[j].id % 10);

            NextStrQueue[26] = int_to_char(myElevator.log[j].startFloor / 10);
            NextStrQueue[27] = int_to_char(myElevator.log[j].startFloor % 10);

            NextStrQueue[41] = int_to_char(myElevator.log[j].endFloor / 10);
            NextStrQueue[42] = int_to_char(myElevator.log[j].endFloor % 10);

            for (k = 0; k < 44; k++) {
                while (xQueueSend(xQueue_UART_TX, &NextStrQueue[k], 10) != pdTRUE) vTaskDelay(1);
            }

            while (xQueueSend(xQueue_UART_TX, &newline, 10) != pdTRUE) vTaskDelay(1);
            while (xQueueSend(xQueue_UART_TX, &resetline, 10) != pdTRUE) vTaskDelay(1);
        }
    } else {
        const char *noTrips = "No trips has been taken";
        for (j = 0; noTrips[j] != '\0'; j++) {
            while (xQueueSend(xQueue_UART_TX, &noTrips[j], 10) != pdTRUE) vTaskDelay(1);
        }
        while (xQueueSend(xQueue_UART_TX, &newline, 10) != pdTRUE) vTaskDelay(1);
        while (xQueueSend(xQueue_UART_TX, &resetline, 10) != pdTRUE) vTaskDelay(1);
    }
    vTaskDelay(10 / portTICK_RATE_MS);
}




void elevator_init(void){
    
    myElevator.numberOfTrips = 0;
    myElevator.elevator_state = CALL_ELEVATOR;
    myElevator.current_floor = 2;
    myElevator.destination_floor = 4;
    myElevator.password = 0;
    myElevator.elevator_acceleration = 0;
    myElevator.elevator_deceleration = 0;
    myElevator.speed = 0;
    myElevator.door_status = FALSE;
    myElevator.numberOfTrips = 0;
    myElevator.rot_direction = 0;
    myElevator.endOfTrip = 0;
}

void elevator_task(void *pvParameters){

    elevator_init();
    red_led_init();
    green_led_init();
    yellow_led_init();

    led_controller.led_state = DOOR_CLOSED;
    while(1){

        switch(myElevator.elevator_state){
            case CALL_ELEVATOR:
                detect_hold_switch();
                myElevator.elevator_state = DISPLAY_FLOOR;
                break;
            case DISPLAY_FLOOR:
                display_current_floor(&led_controller);
                myElevator.elevator_state = OPEN_DOORS;
                break;
            case OPEN_DOORS:
                led_controller.led_state = DOOR_OPENING;
                open_doors();
                led_controller.led_state = DOOR_OPEN;
                if(myElevator.endOfTrip == 1){
                    myElevator.elevator_state = EXIT_ELEVATOR;
                }else{
                    myElevator.elevator_state = ENTER_CODE;
                }
                break;
            case ENTER_CODE:
                enter_password();
                myElevator.elevator_state = VALIDATE_CODE;
                break;
            case VALIDATE_CODE:
                validate_password();
                break;
            case CHOOSE_FLOOR:
                choose_floor();
                myElevator.elevator_state = CLOSE_DOORS;
                break;  
            case BREAK_ELEVATOR:
                led_controller.led_state = ERROR;
                myElevator.elevator_state = SETUP_RST_ELEVATOR;
                break;
            case SETUP_RST_ELEVATOR:
                setup_rst_elevator();
                myElevator.elevator_state = RESTART_ELEVATOR;
                break;
            case RESTART_ELEVATOR:
                restart_elevator();
                myElevator.elevator_state = FIX_ELEVATOR;
                break;
            case FIX_ELEVATOR:
                fix_elevator();
                break;
            case FIX_ELEVATOR_ERROR:
                fix_elevator_error();
                myElevator.elevator_state = FIX_ELEVATOR;
                break;
                // Display error for wrong rotation direction
            case EXIT_ELEVATOR:
                exit_elevator();
                 log_event(TRIP_END);
                myElevator.elevator_state = CLOSE_DOORS;
                // Save floor, close elevator, log trip
                break;
            case CLOSE_DOORS:
                led_controller.led_state = DOOR_OPENING;
                close_doors();
                led_controller.led_state = DOOR_OPEN;
                if (myElevator.endOfTrip == 0){
                    if(myElevator.numberOfTrips % 4 == 0){
                        myElevator.elevator_state = BREAK_ELEVATOR;
                    } else {
                        myElevator.elevator_state = DISPLAY_FLOOR;
                    }
                    myElevator.endOfTrip = 1;
                }else{
                    myElevator.elevator_state = CALL_ELEVATOR;
                    myElevator.endOfTrip = 0;
                }   
                break;
            default:
                // Invalid state, handle error
                break;
        }

        vTaskDelay(1 / portTICK_RATE_MS); // Delay to avoid busy waiting
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

void detect_hold_switch(void){
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

void display_current_floor(Led_controller *led_controller){
    INT8U startFloor = myElevator.current_floor;
    INT8U endFloor = myElevator.destination_floor;
    led_controller->led_state = ELEVATOR_ACCELERATING;
    INT16U increasedSpeed = 0;
    while(1)
    {
        if(myElevator.destination_floor > myElevator.current_floor){
            if ((myElevator.destination_floor-startFloor)/2 >= myElevator.destination_floor - myElevator.current_floor)
            {
                led_controller->led_state = ELEVATOR_DECELERATING;
                increasedSpeed -= 200;
            }else{
                increasedSpeed += 200;
            }
        }else{
            if ((startFloor-myElevator.destination_floor)/2 >= myElevator.current_floor - myElevator.destination_floor)
        {
            led_controller->led_state = ELEVATOR_DECELERATING;
                increasedSpeed -= 200;
            }else{
                increasedSpeed += 200;
            }
        }
        
        char floor_str[16] = "Floor: ";
        floor_str[7] = int_to_char(myElevator.current_floor / 10);
        floor_str[8] = int_to_char(myElevator.current_floor % 10);

        // Move the cursor to the first line of the LCD
        move_LCD(0,0);
        wr_str_LCD(&floor_str);
       
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

        vTaskDelay((TIME_BETWEEN_FLOORS-increasedSpeed) / portTICK_RATE_MS); // Delay to avoid busy waiting
    }
}

void open_doors(void){
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
        wr_str_LCD(&door_str);

        move_LCD(0, 1);
        wr_str_LCD(&door_str);

        vTaskDelay(700 / portTICK_RATE_MS); // Delay to avoid busy waiting
    }
}

void enter_password(void){
    uint8_t  count = 0;
    INT8U key;
    INT8U rst_LCD = 0xff;
    INT8U star = '*';
    INT8U password_str[10] = "Password: ";

    xQueueSend(xQueue_lcd, &rst_LCD, 0);
    vTaskDelay(500 / portTICK_RATE_MS); // Delay to avoid busy waiting
    xQueueGenericReset(xQueue_key, pdFALSE); // Reset the LCD queue

    wr_str_LCD(&password_str);

    while (count < 4)
    {
        key = get_keyboard();
        if (key != 0 && key >= '0' && key <= '9')
        {
            myElevator.password = myElevator.password * 10 + (key - '0');
            count++;
            xQueueSend( xQueue_lcd, &star, 0);
        }
        vTaskDelay(10 / portTICK_RATE_MS); // Delay to avoid busy waiting
    }
}

void validate_password(void){
    if((myElevator.password % 8) == 0 && myElevator.password > 0){
        const char* msg = "Valid password";
        move_LCD(0,0);
        wr_str_LCD(msg);
        myElevator.elevator_state = CHOOSE_FLOOR;
    } else {
        const char* msg = "Invalid password";
        move_LCD(0,0);
        wr_str_LCD(msg);
        myElevator.elevator_state = ENTER_CODE;
    }

    vTaskDelay(2000 / portTICK_RATE_MS); // Delay to avoid busy waiting
}

void choose_floor(void){
    clr_LCD();                                                  // Clear the LCD 
    vTaskDelay(1000 / portTICK_RATE_MS);                        // Delay to be sure the LCD is cleared

    INT8U targetFloor = myElevator.current_floor;                // Set the target floor to the current floor since this is the floor to move from
    INT8U state = 0;                                            // State variable to keep track of the state    

    BOOLEAN IncDec = FALSE;                                     // Variable to keep track of the direction of the encoder
    BOOLEAN startElevator = FALSE;                              // Variable to keep track of when to start the elevator and exit the loop

    char output_str[9] = "Floor:   ";                           // String to display the current floor
    output_str[7] = int_to_char(myElevator.current_floor / 10);  // Set the first digit of the current floor
    output_str[8] = int_to_char(myElevator.current_floor % 10);  // Set the second digit of the current floor

    wr_str_LCD(&output_str);

    INT8U encoder_data = get_encoder();                         // New data of the encoder
    INT8U prev_data = encoder_data;                             // New data of the encoder

    INT8U action;
    while(!startElevator){                                      // Loop until the elevator is started
        switch(state){                                          // Check the state of the encoder
            case 0:                                             // Check if the encoder is turned
                encoder_data = get_encoder();                         // New data of the encoder
                action = get_action(encoder_data, prev_data); // Get the action of the encoder
                if(action == 0){                                // Check if the encoder is turned to the right
                    targetFloor++;                              // Increment the target floor
                    IncDec = TRUE;                              // Set the direction to up
                    state++;                                    // Increment the state to 1 since the encoder is turned
                }else if(action == 1){                          // Check if the encoder is turned to the left
                    targetFloor--;                              // Decrement the target floor
                    IncDec = FALSE;                             // Set the direction to down
                    state++;                                    // Increment the state to 1 since the encoder is turned
                }else if(action == 2){                          // Check if the encoder is pressed
                    if (myElevator.numberOfTrips < 128) {
                        myElevator.destination_floor = targetFloor;  // Set the destination floor to the target floor
                        myElevator.numberOfTrips++;                  // Increment the number of trips
                        log_event(TRIP_START);
                        startElevator = TRUE;                       // Set the start elevator to true since the elevator is started 
                    }                              
                    break;
                }
                
                if(targetFloor > 50){                           // Check if the target floor is greater than 50 since targetfloor is a unsigned char it cant go into negative
                    targetFloor = 0;                            // Set the target floor to 0 since it is a unsigned char
                }else if(targetFloor > 20){                     // Check if the target floor is greater than 20 since the elevator only goes to 20th floor 
                    targetFloor = 20;                           // Set the target floor to 20 since it is the highest floor                        
                }else if(targetFloor == 13 && IncDec == TRUE){  // Check if the target floor is 13 since it is not a valid floor
                    targetFloor++;                              // Set the target floor to 14 since it is the next valid floor
                }else if(targetFloor == 13 && IncDec == FALSE){ // Check if the target floor is 13 since it is not a valid floor
                    targetFloor--;                              // Set the target floor to 12 since it is the next valid floor
                }
                prev_data = encoder_data;                      // Set the previous data to the new data

                break;

            case 1:

                move_LCD(0,0);                                  // Move the cursor to the first line of the LCD
                output_str[7] = int_to_char(targetFloor / 10);  // Set the first digit of the target floor
                output_str[8] = int_to_char(targetFloor % 10);  // Set the second digit of the target floor
                
                // Send the target floor to the LCD
                wr_str_LCD(&output_str);

                // Set state to 0 to get new encoder data
                state = 0;
                break;

            default:
                break;
        }

        vTaskDelay(1 / portTICK_RATE_MS);   // Delay to avoid busy waiting
    }
}


void setup_rst_elevator(void){
    // Setup and reset the elevator
    myElevator.goal_number = 4095 % 361;                           // Generates a random number between 0 to 99;
    int i;                                                          // I 😦
    char goal_str[10] = "Goal:  ";                                    // Generates a string of length 13

    goal_str[7] = int_to_char(myElevator.goal_number / 100);  // Inserts the hundreds digit
    goal_str[8] = int_to_char((myElevator.goal_number / 10) % 10);                        // Inserts the tens digit
    goal_str[9] = int_to_char(myElevator.goal_number % 10);                        // Inserts the ones digit

    move_LCD(0, 0);                                              //Put curser on posistion 0
    wr_str_LCD(&goal_str);

    vTaskDelay(100 / portTICK_RATE_MS);                        // Delay to be sure the LCD is cleared
}

void restart_elevator(void){
    // Restart the elevator
    INT16U raw_adc;
    INT32U pot_val;
    INT8U digits[3];

    INT8U pot_str[10] = "Value: ";                          // Initilizes string pot_str

    while(1){

        raw_adc = get_adc();                                      // Gets value from adc
        pot_val = (raw_adc*360) / 4095;                         // skaler value to between 0 to 99

        digits[0] = int_to_char(pot_val / 100);          // Inserts the hundreds digit
        digits[1] = int_to_char((pot_val / 10) % 10);                  // Gets the tenth digit
        digits[2] = int_to_char(pot_val % 10);                  // gets the first digit

        pot_str[7] = digits[0];                                 // Puts the Tenth digit on array space 7
        pot_str[8] = digits[1];                                 // Puts the first digit on array space 8
        pot_str[9] = digits[2];                  // gets the first digit


        move_LCD(0, 1);                                         // Moves the curser down to secound line
        if(pot_val == myElevator.goal_number){                  // If statment to check if pot_val is the same as Goal_number
            char rst_string[15] = "TIME TO FIX IT";             // Very cool string B-)
            wr_str_LCD(&rst_string);
            vTaskDelay(3000 /portTICK_RATE_MS);

            break;
        }else{
            wr_str_LCD(&pot_str);
        }

        vTaskDelay(10 / portTICK_RATE_MS);
    }
}

void fix_elevator(void){
    INT8U resetLCD = RST;
    INT8U action;
    INT8U state = 0;                                            // State variable to keep track of the state
    short sign = 1;
    short angle = 0;                                            // Variable to keep track of the angle of the encoder
    INT8U output_str[12] = " Angle:  000";                        // String to display the current angle
    INT8U encoder_data = get_encoder();                         // New data of the encoder
    INT8U prev_data = encoder_data;                         // New data of the encoder

    xQueueSend( xQueue_lcd, &resetLCD, portMAX_DELAY);
    wr_str_LCD(&output_str);

    while (1)
    {
        switch (state) {
        case 0:
            encoder_data = get_encoder();                         // New data of the encoder
            action = get_action(encoder_data, prev_data);     // Get the action of the encoder
            if(action == 0){                                        // Check if the encoder is turned to the right
                angle += 12;                                        // Increment the angle
                state = 1;                                          // Set the state to 1 since the encoder is turned
            }else if(action == 1){                                  // Check if the encoder is turned to the left
                angle -= 12;                                        // Decrement the angle
                state = 1;                                          // Set the state to 1 since the encoder is turned
            }else if(action == 2){                                  // Check if the encoder is pressed
                
                if((angle == 360) && (myElevator.rot_direction == 0)){    // Check if the direction is 0
                    myElevator.elevator_state = DISPLAY_FLOOR;        // Set the elevator state to accelerate elevator
                    myElevator.rot_direction = 1;                    // Set the direction to 1
                } else if((angle == -360) && (myElevator.rot_direction == 1)) {     // Check if the direction is 1
                    myElevator.elevator_state = DISPLAY_FLOOR;        // Set the elevator state to accelerate elevator
                    myElevator.rot_direction = 0;                    // Set the direction to 0
                } else {
                    myElevator.elevator_state = FIX_ELEVATOR_ERROR;  // Set the elevator state to fix elevator error
                }
            }
            prev_data = encoder_data;                              // Set the previous data to the new data
            break;

        case 1:

            move_LCD(0,0);                                          // Move the cursor to the first line of the LCD
            if(angle < 0){                                          // Check if the angle is less than 0
                output_str[7] = '-';                                // Set the first digit of the angle to '-'
                sign = -1;
            }else{
                output_str[7] = ' ';                                // Set the first digit of the angle to '-'
                sign = 1;
            }
            output_str[8] = int_to_char(sign*angle / 100);               // Set the first digit of the angle
            output_str[9] = int_to_char((sign*angle % 100) / 10);        // Set the second digit of the angle
            output_str[10] = int_to_char(sign*angle % 10);               // Set the third digit of the angle
            
            // Send the target floor to the LCD
            wr_str_LCD(&output_str);
            state = 0;
            break;
        
        default:
            break;
        }
        if(myElevator.elevator_state != FIX_ELEVATOR){
            break;
        }

        vTaskDelay(1 / portTICK_RATE_MS);   // Delay to avoid busy waiting
    }
    

}

void fix_elevator_error(void){
    // Fix elevator error
    INT8U* msg = "Wrong direction";
    move_LCD(0,0);
    wr_str_LCD(msg);

    move_LCD(0,1);
    if(myElevator.rot_direction == 0){
        msg = "Turn to +";
    } else {
        msg = "Turn to -";
    }

    wr_str_LCD(msg);

    vTaskDelay(2000 / portTICK_RATE_MS); // Delay to avoid busy waiting
}

void exit_elevator(void){
    // Exit the elevator
    INT8U rstLCD = 0xFF;

    xQueueSend( xQueue_lcd, &rstLCD, portMAX_DELAY);

    INT8U* exit_string = "Have a good day";

    wr_str_LCD(exit_string);

    vTaskDelay(2000 / portTICK_RATE_MS);
}



void close_doors(void){
    INT8U i = 0;
    INT8U door_str[16];

    for(i = 0; i < 16; i++){
        door_str[i] = ' ';
    }

    while(1){
        door_str[i] = 0x7C;
        door_str[15-i] = 0x7C;
        if(i == 8){
            break;
        }
        i++;
        move_LCD(0, 0);
        wr_str_LCD(&door_str);

        move_LCD(0, 1);
        wr_str_LCD(&door_str);

        vTaskDelay(700 / portTICK_RATE_MS); // Delay to avoid busy waiting
    }
    
}
