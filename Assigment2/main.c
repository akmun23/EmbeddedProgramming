#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "systick.h"

// Variable for RGB
#define RED   0x02
#define YELLOW  0x04
#define GREEN 0x08

// Defines value for tick
#define TIM_2_SEC  400
#define TIM_1_SEC  200
#define TIM_100_MSEC  20

// States of button
typedef enum button_State{
    BS_IDLE,
    BS_FIRST_PUSH,
    BS_FIRST_RELEASE,
    BS_SECOND_PUSH,
    BS_LONG_PUSH
};
// States of traffic light
typedef enum trafficLight{
    normalMode,
    norwegian,
    emergency
};
enum trafficLight traffic_Light = normalMode;

// States of lights during normal mode
typedef enum normalModeStates{
    red,
    red_and_yellow,
    green,
    yellow,
};
enum normalModeStates normalModeLightController = red;



/*
 * Input: none
 * Output: none
 * Function: changes light of traffic light depending on state
 */
void switchLightState();


/*
 * Input: none
 * Output: TRUE or FALSE
 * Function: outputs TRUE if button is pressed
 */

int button_pushed();

/*
 * Input: int
 * Output: none
 * Function: detects type of click and changes current timer to match with current operation
 */
void select_button(int* currenTimer);

/*
 * Input: none
 * Output: none
 * Function: initializes GPIO ports
 */
void init_gpio(void);

// variable for systick timer
extern int ticks;

int main(void){

   // Initialize sysTick interrupt
   init_systick();

   // Initialize GPIO ports
   init_gpio();

   // sets current timer up for normal operation
   int currentTimer = TIM_2_SEC;

   // initializes alivetimer to match current timer
   int aliveTimer = currentTimer;
   // Loop forever
   while(1){

      // makes program below execute every 5 ms
      while( !ticks );
      // decrements ticks
      ticks--;

      // alive timer times out go into if statement
      if(! --aliveTimer){
          // reset alive timer
          aliveTimer = currentTimer;
          // toggles leds depending on state of traffic light
          switchLightState();
      }
      // checks for button state
      select_button(&currentTimer);


   }
}

void switchLightState(){

    switch (traffic_Light){ // traffic light state
    case normalMode:
        switch(normalModeLightController){ // Normal mode state
        case red:
            GPIO_PORTF_DATA_R &= ~YELLOW;
            normalModeLightController = red_and_yellow;
            break;
        case red_and_yellow:
            GPIO_PORTF_DATA_R |= (RED+YELLOW);  // turns off red and yellow
            GPIO_PORTF_DATA_R &= ~GREEN;        // turns on green
            normalModeLightController = green;  // changes state of normal mode
            break;
        case green:
            GPIO_PORTF_DATA_R |= GREEN;
            GPIO_PORTF_DATA_R &= ~YELLOW;
            normalModeLightController = yellow;
            break;
        case yellow:
            GPIO_PORTF_DATA_R |= YELLOW;
            GPIO_PORTF_DATA_R &= ~RED;
            normalModeLightController = red;
            break;
        }
        break;
    case norwegian:
        GPIO_PORTF_DATA_R |= (GREEN+RED);
        GPIO_PORTF_DATA_R ^= YELLOW;        // Toggles yellow on and off
        break;
    case emergency:
        GPIO_PORTF_DATA_R |= (GREEN+YELLOW);
        GPIO_PORTF_DATA_R &= ~RED;
        break;
    }
}



int button_pushed()
{
  return( !(GPIO_PORTF_DATA_R & 0x10) );  // SW at PF4
}

void select_button(int* currentTimer)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  static int  button_state = BS_IDLE;
  static int button_timer;

  switch( button_state )
  {
    case BS_IDLE:
        if( button_pushed() )                   // if button pushed
        {
            button_state = BS_FIRST_PUSH;
            button_timer = TIM_2_SEC;           // start timer = 2 sec;
        }
        break;
    case BS_FIRST_PUSH:
        if( ! --button_timer )                  // if timeout
        {
            button_state = BS_LONG_PUSH;
            traffic_Light = normalMode;
            *currentTimer = TIM_2_SEC;

        }
        else
        {
            if( !button_pushed() )                  // if button released
            {
                button_state = BS_FIRST_RELEASE;
                button_timer = TIM_100_MSEC;        // start timer = 100 milli sec;
            }
        }
        break;
    case BS_FIRST_RELEASE:
        if( ! --button_timer )                  // if timeout
        {
            button_state = BS_IDLE;
            traffic_Light = norwegian;
            *currentTimer = TIM_1_SEC;
        }
        else
        {
            if( button_pushed() )                   // if button pressed
            {
                 button_state = BS_SECOND_PUSH;
                 button_timer = TIM_2_SEC;          // start timer = 2 sec;
            }
        }
        break;
    case BS_SECOND_PUSH:
        if( ! --button_timer )                  // if timeout
        {
            button_state = BS_LONG_PUSH;
            traffic_Light = normalMode;
            *currentTimer = TIM_2_SEC;
        }
        else
        {
            if( !button_pushed() )                  // if button released
            {
                  button_state = BS_IDLE;
                  traffic_Light = emergency;
            }
        }
        break;
    case BS_LONG_PUSH:
        if( !button_pushed() )                  // if button released
            button_state = BS_IDLE;
        break;
    default:
        break;
  }
}

void init_gpio(void){
    // Start variables
    int dummy;
    volatile int hasChanged = 0;

    // Enable the GPIO port that is used for the on-board LEDs and switches
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;

    // Do a dummy read to insert a few cycles after enabling the peripheral
    dummy = SYSCTL_RCGC2_R;

    // allow changes to PF4-0
    GPIO_PORTF_CR_R = 0x1E;

    // Set the direction as output (PF1 - PF3)
    GPIO_PORTF_DIR_R = 0x0E;

    // Enable the GPIO pins for digital function (PF1 - PF4)
    GPIO_PORTF_DEN_R = 0x1E;

    // Enable internal pull-up (PF4)
    GPIO_PORTF_PUR_R = 0x1E;

    // turns off leds except red on emp board
    GPIO_PORTF_DATA_R |= (YELLOW+GREEN);

}
