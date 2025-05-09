// -----------------
// encoder.c
// -----------------
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "encoder.h"


INT16U get_encoder()
{
    return(GPIO_PORTD_DATA_R & 0xE0);
}