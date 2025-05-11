/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: uart.h
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
* 150228  MoH   Module created.
*
*****************************************************************************/

#ifndef _UART_H
  #define _UART_H

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FreeRTOS.h"
#include "FRT_Files/inc/Task.h"
#include "FRT_Files/inc/queue.h"
#include "FRT_Files/inc/semphr.h"
#include "glob_def.h"
#include "elevator.h"

/*****************************    Defines    *******************************/
extern QueueHandle_t xQueue_UART_TX;
extern SemaphoreHandle_t xSemaphore_UART_TX;
extern QueueHandle_t xQueue_UART_RX;
extern SemaphoreHandle_t xSemaphore_UART_RX;

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

extern BOOLEAN uart0_rx_rdy();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Character ready at uart0 RX
******************************************************************************/

extern INT8U uart0_getc();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Get character from uart0 RX
******************************************************************************/

extern BOOLEAN uart0_tx_rdy();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : uart0 TX buffer ready
******************************************************************************/

extern void uart0_putc( INT8U );
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Put character to uart0 TX
******************************************************************************/

extern void uart0_init( INT32U, INT8U, INT8U, INT8U );
/*****************************************************************************
*   Input    : INT32U baud_rate, INT8U databits, INT8U stopbits, INT8U parity
*   Output   : -
*   Function : Initialize uart 0
******************************************************************************/
void UART_RX_task(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Receive task
******************************************************************************/

void UART_TX_task(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Transmit task
******************************************************************************/

void UART_debug_task(void *pvParameters);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Command interpreter task
******************************************************************************/

void getCommand(INT8U *buf, INT8U length);
/*****************************************************************************
*   Input    : Int8U *buf, INT8U length
*   Output   : -
*   Function : get command from the buffer
******************************************************************************/

void getHelp(void);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Send Help string to UART
******************************************************************************/

void sendString(const char *str);
/*****************************************************************************
*   Input    : const char *str
*   Output   : -
*   Function : send string to the UART
******************************************************************************/
void newLine(void);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : send new line to the UART
******************************************************************************/

/****************************** End Of Module *******************************/
#endif

