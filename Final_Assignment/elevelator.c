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
void elevator_init(Elevator * elevator){
    elevator->elevator_state = CALL_ELEVATOR;
    elevator->elevator_state_prev = CALL_ELEVATOR;
    elevator->current_floor = 2;
    elevator->destination_floor = 20;
    elevator->password = 0;
    elevator->elevator_acceleration = 0;
    elevator->elevator_deceleration = 0;
    elevator->speed = 0;
    elevator->door_status = FALSE;
    elevator->numberOfTrips = 0;
    elevator->rot_direction = 0;
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
                if(myElevator.endOfTrip == 1){
                    myElevator.elevator_state = EXIT_ELEVATOR;
                }else{
                    myElevator.elevator_state = ENTER_CODE;
                }
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
                myElevator.elevator_state = CLOSE_DOORS;
                break;  
            case BREAK_ELEVATOR:
                led_controller.led_state = ERROR;
                myElevator.elevator_state = SETUP_RST_ELEVATOR;
                break;
            case SETUP_RST_ELEVATOR:
                setup_rst_elevator(&myElevator);
                myElevator.elevator_state = RESTART_ELEVATOR;
                break;
            case RESTART_ELEVATOR:
                restart_elevator(&myElevator);
                myElevator.elevator_state = FIX_ELEVATOR;
                break;
            case FIX_ELEVATOR:
                fix_elevator(&myElevator);
                break;
            case FIX_ELEVATOR_ERROR:
                fix_elevator_error(&myElevator);
                myElevator.elevator_state = FIX_ELEVATOR;
                break;
                // Display error for wrong rotation direction
            case EXIT_ELEVATOR:
                exit_elevator(&myElevator);
                myElevator.elevator_state = CALL_ELEVATOR;
                // Save floor, close elevator, log trip
                break;
            case CLOSE_DOORS:
                led_controller.led_state = DOOR_OPENING;
                close_doors(&myElevator);
                led_controller.led_state = DOOR_OPEN;
                if (myElevator.endOfTrip == 0){
                    if(myElevator.numberOfTrips % 4 == 0){
                        myElevator.elevator_state = BREAK_ELEVATOR;
                    } else {
                        myElevator.elevator_state = DISPLAY_FLOOR;
                    }
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
    led_controller->led_state = ELEVATOR_ACCELERATING;
    INT16U increasedSpeed = 0;
    while(1)
    {
        if(elevator->destination_floor > elevator->current_floor){
            if ((elevator->destination_floor-startFloor)/2 >= elevator->destination_floor - elevator->current_floor)
            {
                led_controller->led_state = ELEVATOR_DECELERATING;
                increasedSpeed -= 200;
            }else{
                increasedSpeed += 200;
            }
        }else{
            if ((startFloor-elevator->destination_floor)/2 >= elevator->current_floor - elevator->destination_floor)
            {
                led_controller->led_state = ELEVATOR_DECELERATING;
                increasedSpeed -= 200;
            }else{
                increasedSpeed += 200;
            }
        }
        
        char floor_str[16] = "Floor: ";
        floor_str[7] = int_to_char(elevator->current_floor / 10);
        floor_str[8] = int_to_char(elevator->current_floor % 10);

        // Move the cursor to the first line of the LCD
        move_LCD(0,0);

        // Send the floor string to the LCD
        INT8U i;
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
        vTaskDelay((TIME_BETWEEN_FLOORS-increasedSpeed) / portTICK_RATE_MS); // Delay to avoid busy waiting
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
    INT8U key;
    clr_LCD();
    vTaskDelay(500 / portTICK_RATE_MS); // Delay to avoid busy waiting
    xQueueGenericReset(xQueue_key, pdFALSE); // Reset the LCD queue
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
    clr_LCD();                                                  // Clear the LCD 
    vTaskDelay(1000 / portTICK_RATE_MS);                        // Delay to be sure the LCD is cleared

    INT8U targetFloor = elevator->current_floor;                // Set the target floor to the current floor since this is the floor to move from
    INT8U state = 0;                                            // State variable to keep track of the state    

    BOOLEAN IncDec = FALSE;                                     // Variable to keep track of the direction of the encoder
    BOOLEAN startElevator = FALSE;                              // Variable to keep track of when to start the elevator and exit the loop

    char output_str[9] = "Floor:   ";                           // String to display the current floor
    output_str[7] = int_to_char(elevator->current_floor / 10);  // Set the first digit of the current floor  
    output_str[8] = int_to_char(elevator->current_floor % 10);  // Set the second digit of the current floor

    INT8U i = 0;                    
    for(i = 0; i < 9; i++){                                     // Send the initial string to the LCD
        xQueueSend( xQueue_lcd, &output_str[i], 0);
    }
    INT8U encoder_data = get_encoder();                         // New data of the encoder
    INT8U prev_data = encoder_data;                         // New data of the encoder

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
                    elevator->destination_floor = targetFloor;  // Set the destination floor to the target floor
                    elevator->numberOfTrips++;                  // Increment the number of trips
                    startElevator = TRUE;                       // Set the start elevator to true since the elevator is started                               
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
                for(i = 0; i < 9; i++){
                    xQueueSend( xQueue_lcd, &output_str[i], portMAX_DELAY);
                }

                // Set state to 0 to get new encoder data
                state = 0;
                break;

            default:
                break;
        }

        vTaskDelay(1 / portTICK_RATE_MS);   // Delay to avoid busy waiting
    }
}


void setup_rst_elevator(Elevator * elevator){
    // Setup and reset the elevator
    elevator->goal_number = 4095 % 361;                           // Generates a random number between 0 to 99;
    int i;                                                          // I 😦
    char goal_str[10] = "Goal:  ";                                    // Generates a string of length 13

    goal_str[7] = int_to_char(elevator->goal_number / 100);  // Inserts the hundreds digit    
    goal_str[8] = int_to_char((elevator->goal_number / 10) % 10);                        // Inserts the tens digit
    goal_str[9] = int_to_char(elevator->goal_number % 10);                        // Inserts the ones digit

    move_LCD(0, 0);                                              //Put curser on posistion 0
    for(i = 0; i < 10; i++)                                           // For loop to itterate though str
    {
        xQueueSend(xQueue_lcd, &goal_str[i], portMAX_DELAY);     //put char out on LCD
    }
    vTaskDelay(100 / portTICK_RATE_MS);                        // Delay to be sure the LCD is cleared
}

void restart_elevator(Elevator * elevator){
    // Restart the elevator
    INT16U raw_adc;
    INT32U pot_val;
    INT8U i;
    INT8U digits[3];

    char pot_str[10] = "Value: ";                          // Initilizes string pot_str

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
        if(pot_val == elevator->goal_number){                  // If statment to check if pot_val is the same as Goal_number
            char rst_string[15] = "TIME TO FIX IT";             // Very cool string B-)
            rst_string[14] = RST;                               // Null terminator
            for(i = 0; i < 14 ; i++){                               // For loop to itterate though the cool string B-)
                xQueueSend(xQueue_lcd, &rst_string[i], portMAX_DELAY);  // Thoughts that cool shit out on LCD B-)
            }            
            vTaskDelay(3000 /portTICK_RATE_MS);
            xQueueSend(xQueue_lcd, &rst_string[14], portMAX_DELAY);  // Thoughts that cool shit out on LCD B-)
            break;
        }else{
            for(i = 0; i < 10; i++)                                      // For loop to itterate though pot_str
            {
                xQueueSend(xQueue_lcd, &pot_str[i], portMAX_DELAY); // Outputs chars on to LCD

            }
        }
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}

void fix_elevator(Elevator * elevator){
    INT8U action;
    INT8U state = 0;                                            // State variable to keep track of the state
    short sign = 1;
    short angle = 0;                                            // Variable to keep track of the angle of the encoder
    char output_str[12] = " Angle:  000";                        // String to display the current angle
    output_str[0] = RST;
    INT8U i;
    xQueueSend( xQueue_lcd, &output_str[0], portMAX_DELAY);     
    for(i = 1; i < 12; i++){
        xQueueSend( xQueue_lcd, &output_str[i], portMAX_DELAY);     
    }
    INT8U encoder_data = get_encoder();                         // New data of the encoder
    INT8U prev_data = encoder_data;                         // New data of the encoder
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
                
                if((angle == 360) && (elevator->rot_direction == 0)){    // Check if the direction is 0
                    elevator->elevator_state = DISPLAY_FLOOR;        // Set the elevator state to accelerate elevator
                    elevator->rot_direction = 1;                    // Set the direction to 1
                } else if((angle == -360) && (elevator->rot_direction == 1)) {     // Check if the direction is 1
                    elevator->elevator_state = DISPLAY_FLOOR;        // Set the elevator state to accelerate elevator
                    elevator->rot_direction = 0;                    // Set the direction to 0
                } else {
                    elevator->elevator_state = FIX_ELEVATOR_ERROR;  // Set the elevator state to fix elevator error
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
            INT8U i;
            for(i = 0; i < 11; i++){
                xQueueSend( xQueue_lcd, &output_str[i], portMAX_DELAY);     
            }
            state = 0;
            break;
        
        default:
            break;
        }
        if(elevator->elevator_state != FIX_ELEVATOR){
            break;
        }

        vTaskDelay(1 / portTICK_RATE_MS);   // Delay to avoid busy waiting
    }
    

}

void fix_elevator_error(Elevator * elevator){
    // Fix elevator error
    const char* msg = "Wrong direction";
    move_LCD(0,0);
    while (*msg) {
        xQueueSend(xQueue_lcd, msg, portMAX_DELAY);
        msg++;
    }

    move_LCD(0,1);
    if(elevator->rot_direction == 0){
        msg = "Turn to +";
    } else {
        msg = "Turn to -";
    }
    while (*msg) {
        xQueueSend(xQueue_lcd, msg, portMAX_DELAY);
        msg++;
    }
    vTaskDelay(2000 / portTICK_RATE_MS); // Delay to avoid busy waiting
}

void exit_elevator(Elevator * elevator){
    // Exit the elevator
    move_LCD(0,0);
    clr_LCD();

    INT8U i;
    const char* exit_string = "Have a good day";

    for(i = 0; i < 16; i++){
        xQueueSend(xQueue_lcd, &exit_string[i], 0);

    }
    elevator->numberOfTrips++;
}

static void log_event(Elevator *elevator, TripEvent_t ev) {

    TripLog_tlog = &elevator->log;
    TripLogEntry_t entry = &log->entries[ log->head ];

    entry->event = ev;
    entry->tick  = xTaskGetTickCount();
    entry->floor = elevator->current_floor;   // grab it right from the struct

    log->head  = (log->head + 1) % MAX_LOG_ENTRIES;
    if (log->count < MAX_LOG_ENTRIES) log->count++;
}

void dump_trip_log_uart(Elevator *elevator) {
    const TripLog_t log = &elevator->log;
    int idx = log->head;
    int n   = log->count;

    while (n--) {
        idx = (idx - 1 + MAX_LOG_ENTRIES) % MAX_LOG_ENTRIES;
        const TripLogEntry_tentry = &log->entries[idx];

        uint32_t ms = entry->tick * portTICK_PERIOD_MS;

        char buf[40];
        if (entry->event == TRIP_START) {
            snprintf(buf, sizeof(buf), "S@%u:%lums\r\n",
                     (unsigned)entry->floor, (unsigned long)ms);
        } else {
            snprintf(buf, sizeof(buf), "E@%u:%lums\r\n",
                     (unsigned)entry->floor, (unsigned long)ms);
        }

        for (char p = buf;p; ++p) {
            while (!uart0_tx_rdy()) {}
            uart0_putc(*p);
        }
    }
}


void close_doors(Elevator * elevator){
    INT8U i;
    INT8U j = 0;
    INT8U door_str[16];

    for(i = 0; i < 16; i++){
        door_str[i] = ' ';
    }
    while(1){


        door_str[j] = 0x7C;
        door_str[15-j] = 0x7C;
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