#include "GPIO.h"

void GPIO_init(void){
    int dummy;

    // Enable the GPIO port that is used for the on-board LEDs and switches
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOC | SYSCTL_RCGC2_GPIOD | SYSCTL_RCGC2_GPIOF;

    // Do a dummy read to insert a few cycles after enabling the peripheral
    dummy = SYSCTL_RCGC2_R;

    // allow changes to PC4-7
    GPIO_PORTC_CR_R = 0xF0;
    // allow changes to PD2-3
    GPIO_PORTD_CR_R = 0x0C;

    // Set the direction as output (PC4 - PC7)
    GPIO_PORTC_DIR_R = 0xF0;
    // Set the direction as output (PD2 - PD3)
    GPIO_PORTD_DIR_R = 0x0C;

    // Enable the GPIO pins for digital function (PC4 - PC7)
    GPIO_PORTC_DEN_R = 0xF0;
    // Enable the GPIO pins for digital function (PD2 - PD3)
    GPIO_PORTD_DEN_R = 0x0C;
}
