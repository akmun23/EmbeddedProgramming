#include <stdint.h>
#include "tm4c123gh6pm.h"

#DEFINE RED = 0x02
#DEFINE BLUE = 0x04
#DEFINE GREEN = 0x08

int main(void)
{
   int dummy;

   // Enable the GPIO port that is used for the on-board LEDs and switches
   SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;

   // Do a dummy read to insert a few cycles after enabling the peripheral
   dummy = SYSCTL_RCGC2_R;

   // 2) unlock GPIO Port F
   GPIO_PORTF_LOCK_R = 0x4C4F434B;

   // allow changes to PF4-0
   GPIO_PORTF_CR_R = 0x1F;

   // Set the direction as output (PF1 - PF3)
   GPIO_PORTF_DIR_R = 0x0E;

   // Enable the GPIO pins for digital function (PF1 - PF4)
   GPIO_PORTF_DEN_R = 0x1F;

   // Enable internal pull-up (PF4)
   GPIO_PORTF_PUR_R = 0x11;

   // Initialize the colors for the LED
   const uint_t RGB_Colors[8] = {
        0x00,           // OFF
        GREEN,          // GREEN
        BLUE,           // BLUE
        BLUE|GREEN,     // CAYN
        RED,            // RED
        RED|GREEN,      // YELLOW
        RED|BLUE,       // MAGENTA
        RED|BLUE|GREEN, // WHITE
   }

   volatile int counter = 0;

   // Loop forever
   while(1){
       if(~GPIO_PORTF_DATA_R & 0x11){
           counter += 1;
           if(counter == 8){
               counter = 0;
           }
       }

       GPIO_PORTF_DATA_R &= ~RGB_Colors[counter];

   }

    return 0;
}
