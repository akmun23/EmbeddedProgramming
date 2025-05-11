/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: status led.h
*
* PROJECT....: EMP
*
* DESCRIPTION: Test.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 100408  KHA    Module created.
*
*****************************************************************************/

#ifndef _LEDS_H
  #define _LEDS_H

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "emp_type.h"
//#include "glob_def.h"
//#include "status_led.h"
/*****************************    Defines    *******************************/
#define DOOR_CLOSED 0x00
#define DOOR_OPEN  0x01
#define DOOR_OPENING 0x02
#define DOOR_CLOSING 0x03
#define ELEVATOR_ACCELERATING 0x04
#define ELEVATOR_DECELERATING 0x05
#define ERROR 0x06

#define LED_TASK_DELAY 100 / portTICK_RATE_MS // 1000 ms delay for LED tasks

typedef struct{
    INT8U led_state;               // Elevator state
} Led_controller;

Led_controller led_controller;


/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/


void red_led_task(void *pvParameters);
void yellow_led_task(void *pvParameters);
void green_led_task(void *pvParameters);

void elevator_led_task(void *pvParameters);
void elevatorMoving_ledControl(void);

/****************************** End Of Module *******************************/
#endif

