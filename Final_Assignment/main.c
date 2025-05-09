

/**
 * main.c
 */
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FRT_Files/TivaM4/systick_frt.h"
#include "FreeRTOS.h"
#include "FRT_Files/inc/task.h"
#include "status_led.h"
#include "tmodel.h"
#include "leds.h"
#include "adc.h"
#include "FRT_Files/inc/queue.h"
#include "FRT_Files/inc/semphr.h"
#include "file.h"
#include "lcd.h"
#include "uart0.h"
#include "UI_task.h"
#include "key.h"
#include "encoder.h"
#include "elevator.h"




#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3


#define QUEUE_LEN   128

QueueHandle_t xQueue_lcd;
SemaphoreHandle_t xSemaphore_lcd;
QueueHandle_t xQueue_key;
SemaphoreHandle_t xSemaphore_key;


static void setupHardware(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :
*****************************************************************************/
{
  // TODO: Put hardware configuration and initialisation in here

  // Warning: If you do not initialize the hardware clock, the timings will be inaccurate
  init_systick();
  status_led_init();
  init_adc();
  init_gpio();
  xQueue_lcd = xQueueCreate( QUEUE_LEN , sizeof( INT8U ));
  xSemaphore_lcd = xSemaphoreCreateMutex();
  uart0_init( 9600, 8, 1, 'n' );

  xQueue_key = xQueueCreate( QUEUE_LEN , sizeof( INT8U ));
  xSemaphore_key = xSemaphoreCreateMutex();
}


char change_int_to_char(INT8U number){
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

void UART_task(void *pvParameters){
    INT8U adc_value;
    INT8U button_clicked;
    INT8U sequence[128];

    portTickType delay;

    int i = 0;
    int j = 0;
    int increment = 0;
    while(1){
        button_clicked = 0;
        if(uart0_tx_rdy() && (button_clicked != 0 || increment != 0)){
            switch(increment){
                case 0:
                    if(button_clicked == '#' || i == 127){
                        increment++;
                    }
                    else{
                        sequence[i++] = button_clicked;
                    }
                    break;
                case 1:
                    if(j < i){
                        uart0_putc(sequence[j++]);
                    }else{
                        j = 0;
                        i = 0;
                        increment++;
                    }
                    break;
                case 2:
                    uart0_putc('\n');
                    increment++;
                    break;
                case 3:
                    uart0_putc('\r');
                    increment = 0;
                    break;
                default:
                    break;
            }
        }
    }
}

INT8U button_pushed()
{
  return( !(GPIO_PORTF_DATA_R & 0x10) );
}

void switch_task(void *pvParameters){

    while(1){
        if(button_pushed()){
            GPIO_PORTF_DATA_R |= 0x04;
            GPIO_PORTF_DATA_R &= ~(0x02);
        }
        else{
            GPIO_PORTF_DATA_R &= ~(0x04);
            GPIO_PORTF_DATA_R |= 0x02;
        }
    }
}

int main(void)
{
    setupHardware();
    xTaskCreate( status_led_task, "Status_led", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    //xTaskCreate( red_led_task,    "Red_led",    USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    //xTaskCreate( yellow_led_task, "Yellow_led", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    //xTaskCreate( green_led_task,  "Green_led",  USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );
    xTaskCreate( UART_task, "UART", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
    xTaskCreate( lcd_task, "LCD", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
    xTaskCreate( UI_task, "UI_task", USERTASK_STACK_SIZE, NULL, HIGH_PRIO, NULL );
    xTaskCreate(switch_task, "switch",USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
    xTaskCreate(key_task, "Keypad", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
    vTaskStartScheduler();
	return 0;
}
