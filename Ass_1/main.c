#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "systick.h"

extern int ticks;

int main(void)
{
   init_systick();

   int dummy;
   int RED = 0x02;
   int BLUE = 0x04;
   int GREEN = 0x08;

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

   // Initialize the colors for the LED
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

   volatile int counter = 0;
   volatile int direction = 1;

   // Loop forever
   while(1){
       if(~GPIO_PORTF_DATA_R & 0x10){
           GPIO_PORTF_DATA_R &= ~RGB_Colors[counter];

           counter += direction;

           if(counter == 8){
               counter = 0;
           } else if(counter == -1){
               counter = 7;
           }

           GPIO_PORTF_DATA_R |= RGB_Colors[counter];
           while(~GPIO_PORTF_DATA_R & 0x10);
           ticks = 0;

           while(ticks < 50){
               if(~GPIO_PORTF_DATA_R & 0x10){
                   direction *= -1;
                   break;
               }
           }
           while(~GPIO_PORTF_DATA_R & 0x10);
       }
   }

    return 0;
}
