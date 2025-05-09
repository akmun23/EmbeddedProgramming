// -----------------
// encoder.c
// -----------------
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "encoder.h"



INT16U get_encoder()
{

}

void encoderHandler(void){

    INT8U new_data;
    new_data = GPIO_PORTD_DATA_R;

    if(new_data & 0x40){
        currentAngle--;
    }else{
        currentAngle++;
    }
    currentAngle++;
    GPIO_PORTD_DATA_R = 0x40;


}

void init_encoder()
{
    currentAngle = 0;
    /*int dummy;

    SYSCTL_RCGCGPIO_R |= 0x01;

    dummy = SYSCTL_RCGCGPIO_R;

    GPIO_PORTA_DIR_R &= ~(0xE0);
    GPIO_PORTA_DEN_R |= 0xE0;

    GPIO_PORTA_IEV_R |= 0x20;
    GPIO_PORTA_IM_R |= 0x20;*/



    NVIC_EN0_R = NVIC_EN0_INT_M;
}
