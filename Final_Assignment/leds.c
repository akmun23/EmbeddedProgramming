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
#include "leds.h"


/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void red_led_init(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :
*****************************************************************************/
{
  INT8S dummy;
  // Enable the GPIO port that is used for the on-board LED.
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;

  // Do a dummy read to insert a few cycles after enabling the peripheral.
  dummy = SYSCTL_RCGC2_R;

  GPIO_PORTF_DIR_R |= 0x02;
  GPIO_PORTF_DEN_R |= 0x02;
}

void yellow_led_init(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :
*****************************************************************************/
{
  INT8S dummy;
  // Enable the GPIO port that is used for the on-board LED.
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;

  // Do a dummy read to insert a few cycles after enabling the peripheral.
  dummy = SYSCTL_RCGC2_R;

  GPIO_PORTF_DIR_R |= 0x04;
  GPIO_PORTF_DEN_R |= 0x04;
}

void green_led_init(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :
*****************************************************************************/
{
  INT8S dummy;
  // Enable the GPIO port that is used for the on-board LED.
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;

  // Do a dummy read to insert a few cycles after enabling the peripheral.
  dummy = SYSCTL_RCGC2_R;

  GPIO_PORTF_DIR_R |= 0x08;
  GPIO_PORTF_DEN_R |= 0x08;
}


void red_led_task(void *pvParameters)
{
  INT16U adc_value;
  portTickType delay;

  red_led_init();

  while(1)
  {
    // Toggle red led
    GPIO_PORTF_DATA_R ^= 0x02;
    adc_value = get_adc();
    delay = 1000 - adc_value / 5;
    vTaskDelay( delay / portTICK_RATE_MS); // wait 100-1000 ms. (200-1000)
  }
}

void yellow_led_task(void *pvParameters)
{
  yellow_led_init();

  while(1)
  {
    // Toggle yellow led
    GPIO_PORTF_DATA_R ^= 0x04;
    vTaskDelay( 1000 / portTICK_RATE_MS); // wait 1000 ms.
  }
}

void green_led_task(void *pvParameters)
{
  green_led_init();

  while(1)
  {
    // Toggle green led
    GPIO_PORTF_DATA_R ^= 0x08;
    vTaskDelay( 1500 / portTICK_RATE_MS); // wait 1500 ms.
  }
}



void elevator_led_task(void *pvParameters)
{
  led_controller.led_state = DOOR_CLOSED;
  INT8U reset = 0;
  INT8U counter = 0;

  while(1)
  {
    // Toggle elevator led
    switch(led_controller.led_state)
    {
      case DOOR_CLOSED:
        GPIO_PORTF_DATA_R &= ~(0x02); // Turn on red led
        GPIO_PORTF_DATA_R |= 0x04;    // Turn off yellow led
        GPIO_PORTF_DATA_R |= 0x08;    // Turn off green led
        break;
      case DOOR_MOVING:
        GPIO_PORTF_DATA_R |= 0x02; // Turn off red led
        GPIO_PORTF_DATA_R &= ~(0x04);    // Turn on yellow led
        GPIO_PORTF_DATA_R |= 0x08;    // Turn off green led
        break;
      case DOOR_OPEN:
        GPIO_PORTF_DATA_R |= 0x02; // Turn off red led
        GPIO_PORTF_DATA_R |= 0x04;    // Turn off yellow led
        GPIO_PORTF_DATA_R &= ~(0x08);    // Turn on green led
        break;
      case ELEVATOR_ACCELERATING:
        // Call the elevatorMoving_ledControl function
        elevatorMoving_ledControl();
        break;
      case ELEVATOR_DECELERATING:
        // Call the elevatorMoving_ledControl function
        elevatorMoving_ledControl();
        break;
      case ERROR:
        if (reset == 0){
          GPIO_PORTF_DATA_R |= 0x02;  // Turn off red led
          GPIO_PORTF_DATA_R |= 0x04;    // Turn off yellow led
          GPIO_PORTF_DATA_R |= 0x08;    // Turn on green led
          reset = 1;                    // Mark that the error has been reset
        }
        else{
          GPIO_PORTF_DATA_R ^= 0x02; // Toggle red led
          GPIO_PORTF_DATA_R ^= 0x04; // Toggle yellow led
          GPIO_PORTF_DATA_R ^= 0x08; // Toggle green led
        }
        break;

    }
    // Toggle back the reset of the error case
    if(led_controller.led_state != ERROR){
      reset = 0;
    }

    vTaskDelay(LED_TASK_DELAY ); // wait 100 ms.
  }
}

void elevatorMoving_ledControl(){
  uint32_t start_time = xTaskGetTickCount(); // Record the start time
  uint32_t current_time = start_time;        // Current time
  uint32_t elapsed_time = 0;                 // Elapsed time in milliseconds
  double counter = 1;                        // Counter to adjust LED toggle speed


    while(led_controller.led_state == ELEVATOR_ACCELERATING || led_controller.led_state == ELEVATOR_DECELERATING){
      current_time = xTaskGetTickCount(); // Get the current time
        elapsed_time = (current_time - start_time)*portTICK_PERIOD_MS; // Convert to ms

      if(elapsed_time >= 500.0 / counter){ // Check if it's time to toggle LEDs

            if (led_controller.led_state == ELEVATOR_ACCELERATING){
              // During acceleration: Turn off red LED, toggle yellow LED, and turn off green LED
              GPIO_PORTF_DATA_R |= 0x02; // Turn off red led
              GPIO_PORTF_DATA_R ^= 0x04;  // Toggle yellow led
              GPIO_PORTF_DATA_R |= 0x08;    // Turn off green led
              counter += 0.1;
            }
            else if (led_controller.led_state == ELEVATOR_DECELERATING){
              // During deceleration: Toggle red LED, turn off yellow LED, and toggle green LED
              GPIO_PORTF_DATA_R ^= 0x02;    // Toggle red led
              GPIO_PORTF_DATA_R |= 0x04;    // Turn off yellow led
              GPIO_PORTF_DATA_R |= 0x08;    // Toggle green led
              counter -= 0.1;
            }
          start_time = current_time; // Reset the start time
        }
        vTaskDelay(LED_TASK_DELAY/10 ); // Delay to avoid busy waiting
    }
}
/****************************** End Of Module *******************************/




