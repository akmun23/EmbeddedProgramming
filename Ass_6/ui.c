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
#include <stdint.h>
#include <stdbool.h>


/*****************************    Defines    *******************************/


/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
INT8U i;
INT8U InBuf[128];

INT16U buffer[7] = {0};
char namebuffer[128] = {0};

/*****************************   Functions   *******************************/



void delay(int count){
    while(count--);
    return;
}
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
      } else if((( InBuf[0] == '2' ) && ( i == 2 ))){
          /*
           get_info(buffer ,InBuf[0]-'0');
           get_task_name(namebuffer, buffer[0]);
           gfprintf( COM1,   "You are calling the task with id --- %d\r\n"
                            "ID - %d\r\n"
                            "Condition - %d\r\n"
                            "Name - %20c\r\n"
                            "State - %d\r\n"
                            "Event - %d\r\n"
                            "Semaphore - %d\r\n"
                            "Timer - %d\r\n",
                            InBuf[1]-'0', buffer[0],buffer[1], namebuffer, buffer[3], buffer[4], buffer[5], buffer[6] );*/
          int j = 1;
          int k = 0;
          uart0_putc( '\r' );
          while(!uart0_tx_rdy());
          uart0_putc( '\n' );
          while(!uart0_tx_rdy());
          for(j = 0; j <= 15; j++){
              get_info(buffer, j+1);
              namebuffer[0] = '\0';
              get_task_name(namebuffer, buffer[0]);
              //gfprintf(COM1, "\rTASK: %02d, Condition: %d  \n", j - 1, buffer[1]);


              char NextStr[160] = "TASK: ";
              char id[] = {'0'+j/10, '0'+j%10};
              strcat(NextStr, id);

              if (namebuffer[0] == '\0'){
                  strcat(namebuffer, "                ");

              }
              while(strlen(namebuffer) < 16){
                  strcat(namebuffer, " ");
              }

              strcat(NextStr, ", NAME: ");
              strcat(NextStr, namebuffer);


              strcat(NextStr, ", CONDITION: ");
              switch(buffer[1]){
                  case 1:
                      strcat(NextStr, " READY  , ");
                      strcat(NextStr, "                     ");
                      break;
                  case 2:
                  case 4:
                      strcat(NextStr, " WAITING, ");

                      if(buffer[5]){
                          strcat(NextStr, " SEM: ");
                              if(buffer[5] > 64){
                                  buffer[5] = 0;
                          }

                          if(!buffer[6]){
                              char SEM[] = {'0'+buffer[5]/10, '0'+buffer[5]%10, ',', ' ', ' ',' '};
                              strcat(NextStr, SEM);
                              strcat(NextStr, "         ");
                          }else{
                              char SEM[] = {'0'+buffer[5]/10, '0'+buffer[5]%10, ','};
                              strcat(NextStr, SEM);
                          }


                      }
                      if(buffer[6]){
                          if(buffer[5]){
                              strcat(NextStr, "TIM: ");

                          }else{
                              strcat(NextStr, "          TIM: ");

                          }
                          char TIM[] = {'0', '0', '0'+(buffer[6]%1000)/100, '0'+(buffer[6]%100)/10, '0'+buffer[6]%10, ','};
                          strcat(NextStr, TIM);
                      }
                      break;
                  default:
                      strcat(NextStr, " DEAD   , ");
                      strcat(NextStr, "                     ");
                      break;

              }

              strcat(NextStr, " STATE: ");
              if(buffer[3] > 33){
                  buffer[3] = 0;
              }
              char STATE[] = {'0'+buffer[1]/10, '0'+buffer[1]%10};
              strcat(NextStr, STATE);

              for(k = 0; k < 100; k++){
                    uart0_putc( NextStr[k]);
                    while(!uart0_tx_rdy());
              }
              uart0_putc( '\r' );
              while(!uart0_tx_rdy());
              uart0_putc( '\n' );
              while(!uart0_tx_rdy());


              //gfprintf(COM1, "%d", j/j);


              //uart0_putc( 'kadsads' );
              //while(!uart0_tx_rdy());
              //uart0_putc( '\r' );
              //while(!uart0_tx_rdy());
              //uart0_putc( '\n' );
              //while(!uart0_tx_rdy());

              //uart0_put_q('k');
          }


      }
      i = 0;
    }
  }
}



void get_task_name(char *buf, INT8U id){

    switch (id){
        case TASK_RTC:
                strcpy(buf,"TASK_RTC");
            break;

        case TASK_DISPLAY_RTC:
                strcpy(buf,"TASK_DISPLAY_RTC");
            break;
        case TASK_LCD:
                strcpy(buf,"TASK_LCD");
            break;
        case TASK_UART_RX:
                strcpy(buf,"TASK_UART_RX");
            break;
        case TASK_UI:
                strcpy(buf,"TASK_UI");
            break;
        case TASK_UART_TX:
                strcpy(buf,"TASK_UART_TX");
            break;
        case TASK_KEY:
                strcpy(buf,"TASK_KEY");
            break;
        case TASK_UI_KEY:
                strcpy(buf,"TASK_UI_KEY");
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












