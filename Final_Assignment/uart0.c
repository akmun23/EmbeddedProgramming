/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: emp.c
*
* PROJECT....: EMP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 150228  MoH   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "uart0.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
BOOLEAN acc_input = FALSE;

/*****************************   Functions   *******************************/


INT32U lcrh_databits( INT8U antal_databits )
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : sets bit 5 and 6 according to the wanted number of data bits.
*   		    5: bit5 = 0, bit6 = 0.
*   		    6: bit5 = 1, bit6 = 0.
*   		    7: bit5 = 0, bit6 = 1.
*   		    8: bit5 = 1, bit6 = 1  (default).
*   		   all other bits are returned = 0
******************************************************************************/
{
  if(( antal_databits < 5 ) || ( antal_databits > 8 ))
	antal_databits = 8;
  return(( (INT32U)antal_databits - 5 ) << 5 );  // Control bit 5-6, WLEN
}

INT32U lcrh_stopbits( INT8U antal_stopbits )
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : sets bit 3 according to the wanted number of stop bits.
*   		    1 stpobit:  bit3 = 0 (default).
*   		    2 stopbits: bit3 = 1.
*   		   all other bits are returned = 0
******************************************************************************/
{
  if( antal_stopbits == 2 )
    return( 0x00000008 );  		// return bit 3 = 1
  else
	return( 0x00000000 );		// return all zeros
}

INT32U lcrh_parity( INT8U parity )
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : sets bit 1, 2 and 7 to the wanted parity.
*   		    'e':  00000110b.
*   		    'o':  00000010b.
*   		    '0':  10000110b.
*   		    '1':  10000010b.
*   		    'n':  00000000b.
*   		   all other bits are returned = 0
******************************************************************************/
{
  INT32U result;

  switch( parity )
  {
    case 'e':
      result = 0x00000006;
      break;
    case 'o':
      result = 0x00000002;
      break;
    case '0':
      result = 0x00000086;
      break;
    case '1':
      result = 0x00000082;
      break;
    case 'n':
    default:
      result = 0x00000000;
  }
  return( result );
}

void uart0_fifos_enable()
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : Enable the tx and rx fifos
******************************************************************************/
{
  UART0_LCRH_R  |= 0x00000010;
}

void uart0_fifos_disable()
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : Enable the tx and rx fifos
******************************************************************************/
{
  UART0_LCRH_R  &= 0xFFFFFFEF;
}


extern BOOLEAN uart0_rx_rdy()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  return( UART0_FR_R & UART_FR_RXFF );
}

extern INT8U uart0_getc()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  return ( UART0_DR_R );
}

extern BOOLEAN uart0_tx_rdy()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  return( UART0_FR_R & UART_FR_TXFE );
}

extern void uart0_putc( INT8U ch )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  UART0_DR_R = ch;
}

extern void uart0_init( INT32U baud_rate, INT8U databits, INT8U stopbits, INT8U parity )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  INT32U BRD;

  #ifndef E_PORTA
  #define E_PORTA
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;					// Enable clock for Port A
  #endif

  #ifndef E_UART0
  #define E_UART0
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0;					// Enable clock for UART 0
  #endif

  GPIO_PORTA_AFSEL_R |= 0x00000003;		// set PA0 og PA1 to alternativ function (uart0)
  GPIO_PORTA_DIR_R   |= 0x00000002;     // set PA1 (uart0 tx) to output
  GPIO_PORTA_DIR_R   &= 0xFFFFFFFE;     // set PA0 (uart0 rx) to input
  GPIO_PORTA_DEN_R   |= 0x00000003;		// enable digital operation of PA0 and PA1
  //GPIO_PORTA_PUR_R   |= 0x00000002;

  BRD = 64000000 / baud_rate;   	// X-sys*64/(16*baudrate) = 16M*4/baudrate
  UART0_IBRD_R = BRD / 64;
  UART0_FBRD_R = BRD & 0x0000003F;

  UART0_LCRH_R  = lcrh_databits( databits );
  UART0_LCRH_R += lcrh_stopbits( stopbits );
  UART0_LCRH_R += lcrh_parity( parity );

  uart0_fifos_disable();

  UART0_CTL_R  |= (UART_CTL_UARTEN | UART_CTL_TXE );  // Enable UART
}


void UART_RX_task(void *pvParameters) {
    INT8U key_in;
    while (1) {
        if (uart0_rx_rdy()) {
            key_in = uart0_getc();
            xQueueSend(xQueue_UART_RX, &key_in, portMAX_DELAY);
            if(key_in != '\r'){
                xQueueSend(xQueue_UART_TX, &key_in, portMAX_DELAY);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void UART_TX_task(void *pvParameters) {
    INT8U key_out;

    while (1) {
        if(xSemaphoreTake(xSemaphore_UART_TX, portMAX_DELAY) == pdTRUE) {
            // Wait for the UART TX queue to be empty
            while (xQueueReceive(xQueue_UART_TX, &key_out, 0) != pdTRUE) {
              // Send the character to the UART
              uart0_putc(key_out);  
            }
            // Give back the semaphore
            xSemaphoreGive(xSemaphore_UART_TX);
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void UART_debug_task(void *pvParameters) {
    // Read from the UART RX queue and react to the input when a command is received
    INT8U key_in;
    INT8U buf[16];
    INT8U i = 0;
    while (1) {
      // Take the semaphore for UART RX
      if (xSemaphoreTake(xSemaphore_UART_RX, portMAX_DELAY) == pdTRUE) {
      // Wait for a character to be available in the queue
        if (xQueueReceive(xQueue_UART_RX, &key_in, portMAX_DELAY) == pdTRUE) {
          // Process the received character
          // If the character is an enter key, process the command
          if(key_in == '\r'){

            // If setAcc the next character is the acceleration value
            if (acc_input) {
              // Check if the input is a single digit between 1-5
              if (i == 1 && buf[0] >= '1' && buf[0] <= '5') {
                  setAcc(FALSE, buf[0]);  // Pass the input to setAcc
                  acc_input = FALSE;      // Reset the flag
              } else {
                  setAcc(FALSE, '\0'); 
              }
          } else {
              // Process normal commands
              getCommand(buf, i);
          }

            // Reset the buffer
            memset(buf, 0, sizeof(buf));
            i = 0;

          } else {
              if (i < sizeof(buf) - 1) {
                buf[i++] = key_in;
              }
          }
        }
        // Give back the semaphore
        xSemaphoreGive(xSemaphore_UART_RX);    
      }
    }
}

void getCommand(INT8U *buf, INT8U length){
    // Function to get command from the buffer
    // This function will be called when a command is received

    // Call the function matching the command
    // getLog will be called if the command is "getLog"
    const char *getLog_str = "getLog";
    const char *help_str = "help";
    const char *setAcc_str = "setAcc";
    INT8U i;


    // Check if the command is getLog
    if(length == 6 && !memcmp(buf, getLog_str, 6)){
        getLog();
    } else if(length == 4 && !memcmp(buf, help_str, 4)){
        printHelp();
    } else if(length == 6 && !memcmp(buf, setAcc_str, 6)){
        acc_input = TRUE;
        setAcc(acc_input, 0);
    } else if(length == 0){
        newLine();
    } else {
        const char *msg = "\n\rUnknown command. Write 'help' to get a list \n\r";
        sendString(msg);
    }

}

void printHelp(void) {
    INT8U i;
    const char *helpMsg =
        "\n\r"
        "Available commands:\n\r"
        "getLog   - Show elevator trip log\n\r"
        "setAcc   - Set acceleration\n\r"
        "help     - Show this help message\n\r";
    sendString(helpMsg);
}

void sendString(const char *str) {
    INT8U i;
    for (i = 0; str[i] != '\0'; i++) {
        while (xQueueSend(xQueue_UART_TX, &str[i], 10) != pdTRUE) vTaskDelay(1);
    }
}

void newLine() {
    INT8U newline = '\n';
    while (xQueueSend(xQueue_UART_TX, &newline, 10) != pdTRUE) vTaskDelay(1);
    INT8U resetline = '\r';
    while (xQueueSend(xQueue_UART_TX, &resetline, 10) != pdTRUE) vTaskDelay(1);
}
/****************************** End Of Module *******************************/
