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

typedef enum lightState{
    red,
    red_and_yellow,
    green,
    yellow,
    Norwegian_Night
};
enum lightState trafficLight = Norwegian_Night;

/*
 * Input:
 * Output:
 * Function:
 */
void switchLightState();


/*
 * Input:
 * Output:
 * Function:
 */
void buttonClick();


extern int ticks;



int main(void){

   // Initialize sysTick interrupt
   init_systick();

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

   GPIO_PORTF_DATA_R |= (YELLOW+GREEN);
   // Loop forever

   while(1){


      while(ticks <= TIM_2_SEC);
      ticks -= TIM_2_SEC;
      switchLightState();

   }
}

void switchLightState(){

    switch(trafficLight){
    case red:
        GPIO_PORTF_DATA_R &= ~YELLOW;
        trafficLight = red_and_yellow;
        break;
    case red_and_yellow:
        GPIO_PORTF_DATA_R |= (RED+YELLOW);
        GPIO_PORTF_DATA_R &= ~GREEN;
        trafficLight = green;
        break;
    case green:
        GPIO_PORTF_DATA_R |= GREEN;
        GPIO_PORTF_DATA_R &= ~YELLOW;
        trafficLight = yellow;
        break;
    case yellow:
        GPIO_PORTF_DATA_R |= YELLOW;
        GPIO_PORTF_DATA_R &= ~RED;
        trafficLight = red;
        break;
    case Norwegian_Night:
        GPIO_PORTF_DATA_R |= (GREEN+RED);
        GPIO_PORTF_DATA_R ^= YELLOW;
        break;
    }
}

void buttonClick(){


}

