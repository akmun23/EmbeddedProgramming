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
    yellow
};
enum lightState normalModeState = red;

void switchLightState();


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

   volatile int direction = 1;
   GPIO_PORTF_DATA_R |= ~GREEN;
   GPIO_PORTF_DATA_R |= RED;
   GPIO_PORTF_DATA_R |= YELLOW;
   // Loop forever

   while(1){


      while(ticks <= TIM_2_SEC);
      ticks = 0;
      switchLightState();

   }
}

void switchLightState(){

    switch(normalModeState){
    case red:
        GPIO_PORTF_DATA_R |= RED;
        GPIO_PORTF_DATA_R &= ~(RED+YELLOW);
        normalModeState = red_and_yellow;
        break;
    case red_and_yellow:
        GPIO_PORTF_DATA_R |= (RED+YELLOW);
        GPIO_PORTF_DATA_R &= ~GREEN;
        normalModeState = green;
        break;
    case green:
        GPIO_PORTF_DATA_R |= GREEN;
        GPIO_PORTF_DATA_R &= ~YELLOW;
        normalModeState = yellow;
        break;
    case yellow:
        GPIO_PORTF_DATA_R |= YELLOW;
        GPIO_PORTF_DATA_R &= ~RED;
        normalModeState = red;
        break;
    }
}
