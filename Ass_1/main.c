#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "systick.h"

void incLED(int direction);

extern int ticks;
volatile int counter = 0;

// Variable for RGB
const int RED = 0x02;
const int BLUE = 0x04;
const int GREEN = 0x08;

// Variable for all colors
const int RGB_Colors[8] = {
     0x00,           // OFF
     GREEN,          // GREEN
     BLUE,           // BLUE
     BLUE|GREEN,     // CAYN
     RED,            // RED
     RED|GREEN,      // YELLOW
     RED|BLUE,       // MAGENTA
     RED|BLUE|GREEN, // WHITE
};

int main(void){

   // Initialize sysTick interrupt
   init_systick();

   int dummy;
   volatile int change = 0;

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
           while((~GPIO_PORTF_DATA_R & 0x10) && (ticks < 400));

           // If the time is above 2s
           if(ticks >= 400){
               // Starts autoMode even though button is still pressed
               while(~GPIO_PORTF_DATA_R & 0x10){
                   if (ticks > 40){
                      incLED(direction);
                      ticks = 0;
                   }
               }

               // Continues autoMode when button is unpressed
               while(GPIO_PORTF_DATA_R & 0x10){
                   if (ticks > 40){
                      incLED(direction);
                      ticks = 0;
                   }
               }

           // Increments LED as normal
           } else {
               // Sets counter to 0
               ticks = 0;

               // Waits to see if button is pressed again
               while(ticks < 20){
                   // Changes direction of the incrementation
                   if(~GPIO_PORTF_DATA_R & 0x10){
                       direction *= -1;
                       change = 1;
                       break;
                   }
               }
               // Waits for button to be unpressed
               while(~GPIO_PORTF_DATA_R & 0x10);

              // If no change has occured increment the LED
              if(!change){
                  incLED(direction);
              }
              change = 0;
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
}

