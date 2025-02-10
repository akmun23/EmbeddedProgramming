#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "systick.h"

// Variable for RGB
#define RED   0x02
#define BLUE  0x04
#define GREEN 0x08

#define OFF         0x00
#define CAYN        (BLUE | GREEN)
#define YELLOW      (RED  | GREEN)
#define MAGENTA     (RED  | BLUE)
#define WHITE       (RED  | BLUE | GREEN)

// Defines value for tick
#define TIM_2_SEC  400
#define TIM_200_MS  40
#define TIM_100_MS  20

// Variable for all colors
const int RGB_Colors[8] = {
    OFF,
    GREEN,
    BLUE,
    CAYN,
    RED,
    YELLOW,
    MAGENTA,
    WHITE
};

extern int ticks;
volatile int counter = 0;

void incLED(int direction);
/*
 * Input: Direction
 * Output: None
 * Function: Increments the value of the LED depending on the direction from the input
 */

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
   GPIO_PORTF_PUR_R = 0x10;

   volatile int direction = 1;

   // Loop forever
   while(1){
       // Checks for button press
       if(~GPIO_PORTF_DATA_R & 0x10){

           // Sets counter to 0
           ticks = 0;

           // Waits for button to be unpressed or ticks to be above 400
           while((~GPIO_PORTF_DATA_R & 0x10) && (ticks < TIM_2_SEC));

           // If the time is above 2s
           if(ticks >= TIM_2_SEC){
               // Starts autoMode even though button is still pressed
               while(~GPIO_PORTF_DATA_R & 0x10){
                   if (ticks > TIM_200_MS){
                      incLED(direction);
                      ticks = 0;
                   }
               }

               // Continues autoMode when button is unpressed
               while(GPIO_PORTF_DATA_R & 0x10){
                   if (ticks > TIM_200_MS){
                      incLED(direction);
                      ticks = 0;
                   }
               }

           // Increments LED as normal
           } else {
               // Sets counter to 0
               ticks = 0;

               // Waits to see if button is pressed again
               while(ticks < TIM_100_MS){
                   // Changes direction of the incrementation
                   if(~GPIO_PORTF_DATA_R & 0x10){
                       direction *= -1;
                       hasChanged = 1;
                       break;
                   }
               }
               // Waits for button to be unpressed
               while(~GPIO_PORTF_DATA_R & 0x10);

              // If no change has occured increment the LED
              if(!hasChanged){
                  incLED(direction);
              }
              hasChanged = 0;
           }
       }
   }

    return 0;
}

void incLED(int direction){
    // Turns off color
    GPIO_PORTF_DATA_R &= ~RGB_Colors[counter];

    // Increments counter
    counter += direction;

    // Checks if edge numbers
    if(counter == 8){
        counter = 0;
    } else if(counter == -1){
        counter = 7;
    }

    // Sets color
    GPIO_PORTF_DATA_R |= RGB_Colors[counter];

    return;
}

