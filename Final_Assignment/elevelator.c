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
    myElevator.numberOfTrips = 3;
    myElevator.rot_direction = 0;
    myElevator.endOfTrip = 0;
}

void elevator_task(void *pvParameters){

    elevator_init();                                                // Initialize the elevator
    red_led_init();                                                 // Initialize the red LED
    green_led_init();                                               // Initialize the green LED
    yellow_led_init();                                              // Initialize the yellow LED

    led_controller.led_state = DOOR_CLOSED;                         // Set the initial state of the LED controller
    while(1){
        switch(myElevator.elevator_state){                          // Check the state of the elevator
            case CALL_ELEVATOR:                                     // CALL_ELEVATOR state 
                detect_hold_switch();                               // Enter the hold switch function
                myElevator.elevator_state = DISPLAY_FLOOR;          // If hold_switch is exited, set the elevator state to DISPLAY_FLOOR
                break;
            case DISPLAY_FLOOR:                                     // DISPLAY_FLOOR state
                display_current_floor(&led_controller);             // Call the display_current_floor function
                myElevator.elevator_state = OPEN_DOORS;             // Set the elevator state to OPEN_DOORS
                break;
            case OPEN_DOORS:                                        // OPEN_DOORS state
                led_controller.led_state = DOOR_OPENING;            // Set the LED state to DOOR_OPENING
                open_doors();                                       // Call the open_doors function
                led_controller.led_state = DOOR_OPEN;               // Set the LED state to DOOR_OPEN
                if(myElevator.endOfTrip == 1){                      // Check if the elevator is at the end of the trip
                    myElevator.elevator_state = EXIT_ELEVATOR;      // Then set the elevator state to EXIT_ELEVATOR
                }else{                                                                
                    myElevator.elevator_state = ENTER_CODE;         // Else set the elevator state to ENTER_CODE
                }
                break;
            case ENTER_CODE:                                        // ENTER_CODE state
                enter_password();                                   // Call the enter_password function
                myElevator.elevator_state = VALIDATE_CODE;          // Set the elevator state to VALIDATE_CODE
                break;
            case VALIDATE_CODE:                                     // VALIDATE_CODE state
                validate_password();                                // Call the validate_password function
                break;
            case CHOOSE_FLOOR:                                      // CHOOSE_FLOOR state
                choose_floor();                                     // Call the choose_floor function
                myElevator.elevator_state = CLOSE_DOORS;            // Set the elevator state to CLOSE_DOORS
                break;  
            case BREAK_ELEVATOR:                                    // BREAK_ELEVATOR state
                led_controller.led_state = ERROR;                   // Set the LED state to ERROR
                myElevator.elevator_state = SETUP_RST_ELEVATOR;     // Set the elevator state to SETUP_RST_ELEVATOR
                break;
            case SETUP_RST_ELEVATOR:                                // SETUP_RST_ELEVATOR state
                setup_rst_elevator();                               // Call the setup_rst_elevator function
                myElevator.elevator_state = RESTART_ELEVATOR;       // Set the elevator state to RESTART_ELEVATOR
                break;
            case RESTART_ELEVATOR:                                  // RESTART_ELEVATOR state
                restart_elevator();
                myElevator.elevator_state = FIX_ELEVATOR;           // Set the elevator state to FIX_ELEVATOR
                break;
            case FIX_ELEVATOR:                                      // FIX_ELEVATOR state
                fix_elevator();                                     // Call the fix_elevator function
                break;
            case FIX_ELEVATOR_ERROR:                                // FIX_ELEVATOR_ERROR state
                fix_elevator_error();                               // Call the fix_elevator_error function
                myElevator.elevator_state = FIX_ELEVATOR;           // Set the elevator state to FIX_ELEVATOR
                break;
            case EXIT_ELEVATOR:                                     // EXIT_ELEVATOR state
                exit_elevator();                                    // Call the exit_elevator function
                log_event(TRIP_END);                                // Log the end of the trip
                myElevator.elevator_state = CLOSE_DOORS;            // Set the elevator state to CLOSE_DOORS
                break;
            case CLOSE_DOORS:                                       // CLOSE_DOORS state
                led_controller.led_state = DOOR_OPENING;            // Set the LED state to DOOR_OPENING
                close_doors();                                      // Call the close_doors function
                led_controller.led_state = DOOR_OPEN;               // Set the LED state to DOOR_OPEN
                if (myElevator.endOfTrip == 0){                     // Check if the elevator is at the end of the trip
                    if(myElevator.numberOfTrips % 4 == 0){          // Check if the number of trips is a multiple of 4
                        myElevator.elevator_state = BREAK_ELEVATOR; // Then set the elevator state to BREAK_ELEVATOR
                    } else {
                        myElevator.elevator_state = DISPLAY_FLOOR;  // Else set the elevator state to DISPLAY_FLOOR
                    }
                    myElevator.endOfTrip = 1;                       // Set the end of trip flag to 1
                }else{
                    myElevator.elevator_state = CALL_ELEVATOR;      // Else set the elevator state to CALL_ELEVATOR
                    myElevator.endOfTrip = 0;                       // Set the end of trip flag to 0
                }   
                break;
            default:
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
    INT8U resetLCD = RST;
    xQueueSend( xQueue_lcd, &resetLCD, portMAX_DELAY);

    INT8U startFloor = myElevator.current_floor;
    INT8U endFloor = myElevator.destination_floor;
    led_controller->led_state = ELEVATOR_ACCELERATING;
    INT16U increasedSpeed = 0;
    while(1){

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
        
        // Create a string to display the current floor
        char floor_str[16] = "Floor: ";
        floor_str[7] = int_to_char(myElevator.current_floor / 10);
        floor_str[8] = int_to_char(myElevator.current_floor % 10);

        // Move the cursor to the first line of the LCD
        move_LCD(0,0);
        wr_str_LCD(&floor_str);
       
        // Check if the elevator has reached the destination floor
        if(myElevator.current_floor == myElevator.destination_floor){
            vTaskDelay(1000 / portTICK_RATE_MS);
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

    move_LCD(0, 0);
    wr_str_LCD(&door_str);

    move_LCD(0, 1);
    wr_str_LCD(&door_str);
    vTaskDelay(700 / portTICK_RATE_MS); // Delay

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
    uint8_t count = 0;
    INT8U key;
    INT8U rst_LCD = RST;
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
    move_LCD(0,0);
    const char* msg;

    if((myElevator.password % 8) == 0 && myElevator.password > 0){
        msg = "Valid password";
        myElevator.elevator_state = CHOOSE_FLOOR;
    } else {
        msg = "Invalid password";
        myElevator.elevator_state = ENTER_CODE;
    }

    wr_str_LCD(msg);

    vTaskDelay(2000 / portTICK_RATE_MS); // Delay to avoid busy waiting
}

void choose_floor(void){
    INT8U resetLCD = RST;                                                // Clear the LCD
    xQueueSend( xQueue_lcd, &resetLCD, portMAX_DELAY);
    vTaskDelay(100 / portTICK_RATE_MS);                         // Delay to be sure the LCD is cleared

    INT8U targetFloor = myElevator.current_floor;                // Set the target floor to the current floor since this is the floor to move from
    INT8U state = 0;                                            // State variable to keep track of the state    

    BOOLEAN IncDec = FALSE;                                     // Variable to keep track of the direction of the encoder
    BOOLEAN startElevator = FALSE;                              // Variable to keep track of when to start the elevator and exit the loop

    INT8U* floor_str = "Floor: ";                           // String to display the current floor
    INT8U floorNumber[2];

    floorNumber[0] = int_to_char(myElevator.current_floor / 10);  // Set the first digit of the current floor
    floorNumber[1] = int_to_char(myElevator.current_floor % 10);  // Set the second digit of the current floor
    wr_str_LCD(floor_str);
    wr_str_LCD(&floorNumber);

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
                        INT8U* tripConfirm = "Trip confirmed!";
                        move_LCD(0, 1);
                        wr_str_LCD(tripConfirm);
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
                floorNumber[0] = int_to_char(targetFloor / 10);  // Set the first digit of the target floor
                floorNumber[1] = int_to_char(targetFloor % 10);  // Set the second digit of the target floor
                
                // Send the target floor to the LCD
                wr_str_LCD(floor_str);
                wr_str_LCD(&floorNumber);

                // Set state to 0 to get new encoder data
                state = 0;
                break;

            default:
                break;
        }
        if(startElevator){
            vTaskDelay(1000 / portTICK_RATE_MS);   // Delay
        } else {
            vTaskDelay(1 / portTICK_RATE_MS);   // Delay to avoid busy waiting
        }
    }
}


void setup_rst_elevator(void){
    INT8U resetLCD = RST;
    xQueueSend( xQueue_lcd, &resetLCD, portMAX_DELAY);

    // Setup and reset the elevator
    myElevator.goal_number = rand() % 360;                           // Generates a random number between 0 to 360;
    INT8U* goal_str = "Goal:  ";                                    // Generates a string of length 13
    INT8U goal_val[3];
    goal_val[0] = int_to_char(myElevator.goal_number / 100);  // Inserts the hundreds digit
    goal_val[1] = int_to_char((myElevator.goal_number / 10) % 10);                        // Inserts the tens digit
    goal_val[2] = int_to_char(myElevator.goal_number % 10);                        // Inserts the ones digit

    move_LCD(0, 0);                                              //Put curser on posistion 0
    wr_str_LCD(goal_str);
    wr_str_LCD(&goal_val);

    vTaskDelay(200 / portTICK_RATE_MS);                        // Delay
}

void restart_elevator(void){
    // Restart the elevator
    INT16U raw_adc;                                             // Variable to hold the value of the adc
    INT32U pot_val;                                             // Variable to hold the value of the pot
    INT8U digits[3];                                            // Array to hold the digits of the pot_val
    INT8U* pot_str = "Value: ";                              // Initilizes string pot_str

    while(1){
        raw_adc = get_adc();                                    // Gets value from adc
        pot_val = (raw_adc*360) / 4095;                         // skaler value to between 0 to 99

        digits[0] = int_to_char(pot_val / 100);                 // Inserts the hundreds digit
        digits[1] = int_to_char((pot_val / 10) % 10);           // Gets the tenth digit
        digits[2] = int_to_char(pot_val % 10);                  // gets the first digit

        move_LCD(0, 1);                                         // Moves the curser down to secound line
        if(pot_val == myElevator.goal_number){                  // If statment to check if pot_val is the same as Goal_number
            INT8U* rst_string = "TIME TO FIX IT";             // Very cool string B-)
            wr_str_LCD(rst_string);
            vTaskDelay(3000 /portTICK_RATE_MS);

            break;
        }else{
            wr_str_LCD(pot_str);
            wr_str_LCD(&digits);
        }

        vTaskDelay(10 / portTICK_RATE_MS);
    }
}

void fix_elevator(void){
    INT8U resetLCD = RST;
    INT8U action;                                                   // Action variable to keep track of the action of the encoder
    INT8U state = 0;                                                // State variable to keep track of the state
    short sign = 1;                                                 // Variable to keep track of the sign of the angle
    short angle = 0;                                                // Variable to keep track of the angle of the encoder
    INT8U* output_str = "Angle: ";                                  // String to display the current angle
    INT8U sign_str = '+';
    INT8U angle_val[3] = "000";
    INT8U encoder_data = get_encoder();                             // New data of the encoder
    INT8U prev_data = encoder_data;                                 // New data of the encoder

    xQueueSend(xQueue_lcd, &resetLCD, portMAX_DELAY);              // Reset the LCD queue
    vTaskDelay(100 / portTICK_RATE_MS);

    wr_str_LCD(output_str);                                         // Send the string to the LCD
    wr_ch_LCD(sign_str);                                           // Send
    wr_str_LCD(&angle_val);

    while (myElevator.elevator_state == FIX_ELEVATOR)               // Loop until the elevator is fixed/error
    {
        switch (state) {
        case 0:
            encoder_data = get_encoder();                           // New data of the encoder
            action = get_action(encoder_data, prev_data);           // Get the action of the encoder
            if(action == 0){                                        // Check if the encoder is turned to the right
                angle += 12;                                        // Increment the angle
                state = 1;                                          // Set the state to 1 since the encoder is turned
            }else if(action == 1){                                  // Check if the encoder is turned to the left
                angle -= 12;                                        // Decrement the angle
                state = 1;                                          // Set the state to 1 since the encoder is turned
            }else if(action == 2){                                  // Check if the encoder is pressed               
                if((angle == 360) && (myElevator.rot_direction == 0)){              // Check if the direction is 0
                    myElevator.elevator_state = DISPLAY_FLOOR;                      // Set the elevator state to accelerate elevator
                    myElevator.rot_direction = 1;                                   // Set the direction to 1
                } else if((angle == -360) && (myElevator.rot_direction == 1)) {     // Check if the direction is 1
                    myElevator.elevator_state = DISPLAY_FLOOR;                      // Set the elevator state to accelerate elevator
                    myElevator.rot_direction = 0;                                   // Set the direction to 0
                } else {
                    myElevator.elevator_state = FIX_ELEVATOR_ERROR;                 // Set the elevator state to fix elevator error
                }
            }
            prev_data = encoder_data;                                               // Set the previous data to the new data
            break;

        case 1:
            move_LCD(0,0);                                              // Move the cursor to the first line of the LCD
            if(angle < 0){                                              // Check if the angle is less than 0
                sign_str = '-';                                         // Set the first digit of the angle to '-'
                sign = -1;
            }else{
                sign_str = '+';                                         // Set the first digit of the angle to '-'
                sign = 1;
            }
            angle_val[0] = int_to_char((sign*angle) / 100);               // Set the first digit of the angle
            angle_val[1] = int_to_char(((sign*angle) % 100) / 10);        // Set the second digit of the angle
            angle_val[2] = int_to_char((sign*angle) % 10);               // Set the third digit of the angle
            
            // Send the target floor to the LCD
            wr_str_LCD(output_str);
            wr_ch_LCD(sign_str);
            wr_str_LCD(&angle_val);

            state = 0;

            vTaskDelay(100 / portTICK_RATE_MS);
            break;
        
        default:
            break;
        }

        vTaskDelay(1 / portTICK_RATE_MS);   // Delay to avoid busy waiting
    }
    

}

void fix_elevator_error(void){
    // Fix elevator error
    INT8U* msg = "Wrong input";
    move_LCD(0,0);
    wr_str_LCD(msg);

    move_LCD(0,1);
    if(myElevator.rot_direction == 0){
        msg = "Turn to +360";
    } else {
        msg = "Turn to -360";
    }
    wr_str_LCD(msg);

    vTaskDelay(2000 / portTICK_RATE_MS); // Delay to avoid busy waiting
}

void exit_elevator(void){
    // Exit the elevator
    INT8U rstLCD = RST;
    xQueueSend( xQueue_lcd, &rstLCD, portMAX_DELAY);

    INT8U* exit_string = "Have a good day";
    wr_str_LCD(exit_string);

    vTaskDelay(2000 / portTICK_RATE_MS);
}



void close_doors(void){
    INT8U i;
    INT8U door_str[16];

    for(i = 0; i < 16; i++){
        door_str[i] = ' ';
    }

    i = 0;

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

static void log_event(TripEvent_t event) {
    if(event == TRIP_START){
        myElevator.log[myElevator.numberOfTrips - 1].id = myElevator.numberOfTrips;
        myElevator.log[myElevator.numberOfTrips - 1].startFloor = myElevator.current_floor;

    } else if(event == TRIP_END){
        myElevator.log[myElevator.numberOfTrips - 1].endFloor = myElevator.destination_floor;
    }
}

void getLog(void) {
    INT8U j;

    newLine();

    if (myElevator.numberOfTrips > 0) {
            INT8U* IDstr = "Trip ID: ";
            INT8U* startFloorStr = " Start Floor: ";
            INT8U* endFloorStr = " End Floor: ";

        for (j = 0; j < myElevator.numberOfTrips; j++) {
            char elevatorID[2] = {int_to_char(myElevator.log[j].id / 10),
                                  int_to_char(myElevator.log[j].id % 10)};

            char startFloor[2] = {int_to_char(myElevator.log[j].startFloor / 10),
                                  int_to_char(myElevator.log[j].startFloor % 10)};

            char endFloor[2] = {int_to_char(myElevator.log[j].endFloor / 10),
                                  int_to_char(myElevator.log[j].endFloor % 10)};

            sendString(IDstr);
            sendString(&elevatorID);
            sendString(startFloorStr);
            sendString(&startFloor);
            sendString(endFloorStr);
            sendString(&endFloor);    
            newLine();
        }
    } else {
        const char *noTrips = "No trips has been taken";
        sendString(noTrips);
        newLine();
    }
    vTaskDelay(10 / portTICK_RATE_MS);
}
