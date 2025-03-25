/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: ui.c
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
* 050128  KA    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"
#include "systick.h"
#include "tmodel.h"
#include "systick.h"
#include "ui.h"
#include "rtc.h"
#include "rtcs.h"
#include "string.h"
/*****************************    Defines    *******************************/


/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
INT8U i;
INT8U InBuf[128];

INT16U buffer[7] = {0};
char namebuffer[128] = {0};

/*****************************   Functions   *******************************/


void ui_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  INT8U ch;

  //if( get_queue( Q_UART_RX, &ch, WAIT_FOREVER ))
  if( get_file( COM1, &ch ))                                                                // if char received from uart (COM1)
  {
    if( i < 128 )                                                                           // if our buffer is not full
      InBuf[i++] = ch;                                                                      // store char in buffer
    put_file( COM1, ch );                                                                   // put char back to uart (write char to terminal)
    if( ch == '\r' )                                                                        // if the char is 'Enter' (Return)
    {
      if(( InBuf[0] == '1' )){                                                 // if the first char in buffer is '1' and there are at least 8 chars
          if(i >= 8){
            set_hour( (InBuf[1]-'0')*10+InBuf[2]-'0');                                        // use the chars in buffer to set the clock
            set_min( (InBuf[3]-'0')*10+InBuf[4]-'0');
            set_sec( (InBuf[5]-'0')*10+InBuf[6]-'0');
            gfprintf( COM1, "Clock changed!\r\n");

          }
          gfprintf( COM1, "%c - %02d:%02d:%02d\r\n", InBuf[0], get_hour(), get_min(), get_sec() );   // print the current time to uart
      } else if((( InBuf[0] == '2' ) && ( i == 3 ))){
          get_info(buffer ,InBuf[1]);
          get_task_name(namebuffer, buffer[0]);
          gfprintf( COM1,   "%c: \r\n"
                            "ID - %d\r\n"
                            "Condition - %d\r\n"
                            "Name - %c\r\n"
                            "State - %d\r\n"
                            "Event - %d\r\n"
                            "Semaphore - %d\r\n",
                            "Timer - %d\r\n",
                            InBuf[0], buffer[0],buffer[1], namebuffer[0], buffer[3], buffer[4], buffer[5], buffer[6] );
      }
      i = 0;

    }

  }
}

void get_task_name(char *buf, INT8U id){

    switch (id){
        case 0:
                buf[0] = 'T';
                buf[1] = 'A';
                buf[2] = 'S';
                buf[3] = 'K';
                buf[4] = '_';
                buf[5] = 'R';
                buf[6] = 'T';
                buf[7] = 'C';
                        // buf = "TASK_RTC";
            break;
        case 1:
               buf[0] = 'T';
               buf[1] = 'E';
               buf[2] = 'S';
               buf[3] = 'T';
               buf[4] = '_';
               buf[5] = 'R';
               buf[6] = 'T';
               buf[7] = 'C';

            break;

    }
    return;
}

void ui_key_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  INT8U ch;
  static INT8U Buf[6];

  if( get_file( COM3, &ch ))                            // if input received from keypad (COM3)
  {
    switch( my_state )
    {
    case 0:
      Buf[0] = ch;                                      // add key press to buffer
      gfprintf( COM2, "%c%c%c", 0x1B, 0xC4, ch );       // print the key to the display (COM2) on the second line
      set_state( 1 );
      break;
    case 1:
      Buf[1] = ch;                                      // repeat previous step for all digits of the clock
      gfprintf( COM2, "%c%c%c:", 0x1B, 0xC5, ch );
      set_state( 2 );
      break;
    case 2:
      Buf[2] = ch;
      gfprintf( COM2, "%c%c%c", 0x1B, 0xC7, ch );
      set_state( 3 );
      break;
    case 3:
      Buf[3] = ch;
      gfprintf( COM2, "%c%c%c:", 0x1B, 0xC8, ch );
      set_state( 4 );
      break;
    case 4:
      Buf[4] = ch;
      gfprintf( COM2, "%c%c%c", 0x1B, 0xCA, ch );
      set_state( 5 );
      break;
    case 5:
      Buf[5] = ch;                                      // when all digits received, set the clock
      set_hour( (Buf[0]-'0')*10+Buf[1]-'0');
      set_min( (Buf[2]-'0')*10+Buf[3]-'0');
      set_sec( (Buf[4]-'0')*10+Buf[5]-'0');
      gfprintf( COM2, "%c%c        ", 0x1B, 0xC4 );
      set_state( 0 );
      break;
    }
  }
}



/****************************** End Of Module *******************************/












