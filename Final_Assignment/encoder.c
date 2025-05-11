// -----------------
// encoder.c
// -----------------
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "encoder.h"


INT8U get_encoder()
{
    return(GPIO_PORTA_DATA_R & 0xE0);
}

// Return the action of the encoder
// 0 = right
// 1 = left
// 2 = pressed
// 3 = no action
INT8U get_action(INT8U encoder_data ,INT8U prev_data){
        
    if (((encoder_data & 0x20) != (prev_data & 0x20))){         // Check if the encoder data is different                            // Set the previous data to the new data
        if(encoder_data & 0x20){                                // Check if encoder A is active
            if(encoder_data & 0x40){                            // Check if B is also active
                return 1;                                       // Then the encoder is turned to the left
            }else{
                return 0;                                       // Else the encoder is turned to the right
            }
        }else{                                                  // Else encoder A is inactive  
            if(encoder_data & 0x40){                            // Check if B is active
                return 0;                   	                // Then the encoder is turned to the right
            }else{
                return 1;                                       // Else the encoder is turned to the left
            }
        }
    } else if (((encoder_data & 0x80) != (prev_data & 0x80))){  // Check if the encoder is pressed                           // Set the previous data to the new data
        return 2;
    }
    
    return 3;                                                   // No action
}
