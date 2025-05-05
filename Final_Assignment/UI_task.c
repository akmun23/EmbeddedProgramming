/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: leds.c
*
* PROJECT....: ECP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"

#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
//#include "glob_def.h"
//#include "binary.h"
#include "UI_task.h"
#include "adc.h"

#include "lcd.h"


/*****************************    Defines    *******************************/
#define PF0     0       // Bit 0
#define setup_state   0
#define scale_state   1
#define offset_state  2

/*****************************   Constants   *******************************/

/*INT16U min_in 0;
INT16U max_in 4095;
INT16U min_out 100;
INT16U max_out 1000; */

/*****************************   Variables   *******************************/

extern INT8U scalefactor, offset, count;
extern QueueHandle_t xQueue_key, xQueue_lcd;
extern SemaphoreHandle_t xSemaphore_key, xSemaphore_lcd, xSemaphore_scoff;

/*****************************   Functions   *******************************/

char change_int_to_char1(INT8U number){
    switch(number){
        case 0:
            return '0';
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        case 9:
            return '9';
    }
}

void UI_task(void *pvParameters){

    INT8U setup_lcd;
    INT8U button_clicked;
    INT8U sequence[128];
    INT64U adc_value;
    INT8U reset_LCD = 0xff;



    portTickType delay;

    int i = 0;
    INT64U j = 1;
    int increment = 0;
    INT64U scale = 1;
    INT64U offset = 0;

    INT8U cv_mod[16];
    INT8U input[16];
    INT8U number_detected = 0;
    INT8U pressed = 0;

    const char* scale_str = "New scale: ";
    const char* Press_str = "Press # to save";

    const char* offset_str = "New offset: ";

    while(1){
        button_clicked = get_keyboard();
        switch(increment){
            case 0:
                adc_value = get_adc() * scale + offset;


                for(i = 0; i < 16; i++){
                    cv_mod[i] = (adc_value/(1*j)) % 10;
                    input[i] = change_int_to_char1(cv_mod[i]);
                    j *= 10;
                }
                j = 1;

                for(i = 15; i >= 0; i--){

                    if (input[i] != '0'){
                        number_detected = 1;
                    }
                    if (number_detected){
                        xQueueSend( xQueue_lcd, &input[i], portMAX_DELAY);
                    }

                }
                number_detected = 0;
                move_LCD(0,0);

                if(button_clicked == '#'){
                    increment++;
                    scale = 0;
                    offset = 0;
                }
                break;
            case 1:

                xQueueSend( xQueue_lcd, &reset_LCD, portMAX_DELAY);

                for(i = 0; i < 11; i++){
                    xQueueSend( xQueue_lcd, &scale_str[i], portMAX_DELAY);
                }
                move_LCD(0,1);
                for(i = 0; i < 15; i++){
                    xQueueSend( xQueue_lcd, &Press_str[i], portMAX_DELAY);
                }



                increment++;
                break;
            case 2:

                if(button_clicked == '#'){
                    increment++;
                    pressed = 0;
                }
                else if(button_clicked != 0){
                    move_LCD(10+pressed, 0);
                    if( xSemaphoreTake( xSemaphore_lcd, portMAX_DELAY)){ //( TickType_t ) 100 ) == pdTRUE )
                        if(pressed <= 6){
                            xQueueSend( xQueue_lcd, &button_clicked, portMAX_DELAY);
                            pressed++;
                            scale = scale*10 + button_clicked - '0';
                        }
                        xSemaphoreGive( xSemaphore_lcd );
                    }
                }
                break;
            case 3:

                xQueueSend( xQueue_lcd, &reset_LCD, portMAX_DELAY);

                for(i = 0; i < 12; i++){
                    xQueueSend( xQueue_lcd, &offset_str[i], portMAX_DELAY);
                }
                move_LCD(0,1);
                for(i = 0; i < 15; i++){
                    xQueueSend( xQueue_lcd, &Press_str[i], portMAX_DELAY);
                }



                increment++;
                break;
            case 4:

                if(button_clicked == '#'){
                    j = 1;
                    increment = 0;
                    clr_LCD();
                    move_LCD(0,0);
                    pressed = 0;
                }
                else if(button_clicked != 0){
                    move_LCD(11+pressed, 0);
                    if( xSemaphoreTake( xSemaphore_lcd, portMAX_DELAY)){ //( TickType_t ) 100 ) == pdTRUE )
                        if(pressed <= 5){
                            xQueueSend( xQueue_lcd, &button_clicked, portMAX_DELAY);
                            pressed++;
                            offset = offset*10 + button_clicked - '0';
                        }
                        xSemaphoreGive( xSemaphore_lcd );
                    }
                }
                break;
            default:
                break;
        }
    }


    /*
    INT8U setup_lcd;
    INT16U adc_value;

    char input0;
    char input1;
    char input2;
    char input3;
    while(1){
        adc_value = get_adc();

        int cv_mod0 = adc_value % 10;
        int cv_mod10 = adc_value/10 % 10;
        int cv_mod100 = adc_value/100 % 10;
        int cv_mod1000 = adc_value/1000 % 10;

        input0 = change_int_to_char(cv_mod1000);
        xQueueSend( xQueue_lcd, &input0, portMAX_DELAY);
        input1 = change_int_to_char(cv_mod100);
        xQueueSend( xQueue_lcd, &input1, portMAX_DELAY);
        input2 = change_int_to_char(cv_mod10);
        xQueueSend( xQueue_lcd, &input2, portMAX_DELAY);
        input3 = change_int_to_char(cv_mod0);
        xQueueSend( xQueue_lcd, &input3, portMAX_DELAY);
        move_LCD(0,0);
    }*/

}

