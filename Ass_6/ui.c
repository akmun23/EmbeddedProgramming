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

INT16U taskbuffer[7] = { 0 };
INT16U queuebuffer[5] = { 0 };
char namebuffer[128] = { 0 };

/*****************************   Functions   *******************************/

void delay(int count)
{
    while (count--)
        ;
    return;
}

void task_debugger()
{
    int j = 1;
    int k = 0;
    uart0_putc('\r');
    while (!uart0_tx_rdy())
        ;
    uart0_putc('\n');
    while (!uart0_tx_rdy())
        ;
    for (j = 0; j <= 15; j++)
    {
        get_task_info(taskbuffer, j + 1);
        namebuffer[0] = '\0';
        get_task_name(namebuffer, taskbuffer[0]);

        char NextStrTask[128] = "TASK: ";
        char id[] = { '0' + j / 10, '0' + j % 10 };
        strcat(NextStrTask, id);

        if (namebuffer[0] == '\0')
        {
            strcat(namebuffer, "                ");

        }
        while (strlen(namebuffer) < 16)
        {
            strcat(namebuffer, " ");
        }

        strcat(NextStrTask, ", NAME: ");
        strcat(NextStrTask, namebuffer);

        strcat(NextStrTask, ", CONDITION: ");
        switch (taskbuffer[1])
        {
        case 1:
            strcat(NextStrTask, " READY  , ");
            strcat(NextStrTask, "                     ");
            break;
        case 2:
        case 4:
            strcat(NextStrTask, " WAITING, ");

            if (taskbuffer[5])
            {
                strcat(NextStrTask, " SEM: ");
                if (taskbuffer[5] > 64)
                {
                    taskbuffer[5] = 0;
                }

                if (!taskbuffer[6])
                {
                    char SEM[] = { '0' + taskbuffer[5] / 10, '0'
                                           + taskbuffer[5] % 10,
                                   ',', ' ', ' ', ' ' };
                    strcat(NextStrTask, SEM);
                    strcat(NextStrTask, "         ");
                }
                else
                {
                    char SEM[] = { '0' + taskbuffer[5] / 10, '0'
                                           + taskbuffer[5] % 10,
                                   ',', '\0' };
                    strcat(NextStrTask, SEM);
                }

            }
            if (taskbuffer[6])
            {
                if (taskbuffer[5])
                {
                    strcat(NextStrTask, " TIM: ");

                }
                else
                {
                    strcat(NextStrTask, "          TIM: ");

                }
                char TIM[] = { '0', '0', '0' + (taskbuffer[6] % 1000) / 100, '0'
                                       + (taskbuffer[6] % 100) / 10,
                               '0' + taskbuffer[6] % 10, ',' };
                strcat(NextStrTask, TIM);
            }
            break;
        default:
            strcat(NextStrTask, " DEAD   , ");
            strcat(NextStrTask, "                     ");
            break;

        }

        strcat(NextStrTask, " STATE: ");
        char STATE[] = { '0' + taskbuffer[3] / 10, '0' + taskbuffer[3] % 10 };
        strcat(NextStrTask, STATE);

        for (k = 0; k < 100; k++)
        {
            uart0_putc(NextStrTask[k]);
            while (!uart0_tx_rdy())
                ;
        }
        uart0_putc('\r');
        while (!uart0_tx_rdy())
            ;
        uart0_putc('\n');
        while (!uart0_tx_rdy())
            ;

    }

}
void queue_debugger()
{
    int j = 1;
    int k = 0;
    uart0_putc( '\r' );
    while(!uart0_tx_rdy());
    uart0_putc( '\n' );
    while(!uart0_tx_rdy());
    for (j = 0; j <= 15; j++)
    {

        get_queue_info(queuebuffer, j + 1);
        namebuffer[0] = '\0';
        get_queue_name(namebuffer, j);

        char NextStrQueue[128] = "QUEUE: ";
        char id[] = { '0' + j / 10, '0' + j % 10 };
        strcat(NextStrQueue, id);

        if (namebuffer[0] == '\0')
                {
                    strcat(namebuffer, "                ");

                }
                while (strlen(namebuffer) < 16)
                {
                    strcat(namebuffer, " ");
                }

                strcat(NextStrQueue, ", NAME: ");
                strcat(NextStrQueue, namebuffer);

        strcat(NextStrQueue, " HEAD: ");
        char head[] = { '0'+queuebuffer[0]/100,'0' + (queuebuffer[0]%100) / 10, '0' + queuebuffer[0] % 10 };
        strcat(NextStrQueue, head);

        strcat(NextStrQueue, " TAIL: ");
        char tail[] = { '0'+queuebuffer[1]/100,'0' + (queuebuffer[1]%100) / 10, '0' + queuebuffer[1] % 10 , '\0'};
        strcat(NextStrQueue, tail);

        strcat(NextStrQueue, " NOT_FULL: ");
        char not_full[] = {'0' + queuebuffer[2] % 10, '\0'};
        strcat(NextStrQueue, not_full);

        strcat(NextStrQueue, " NOT_EMPTY: ");
        char not_empty[] = {'0' + (queuebuffer[3]%100) / 10, '0' + queuebuffer[3] % 10, '\0'};
        strcat(NextStrQueue, not_empty);

        strcat(NextStrQueue, " NUM_ELEMENTS: ");
        int elements = 0;
        if(queuebuffer[0] >= queuebuffer[1]){
            elements = queuebuffer[0] - queuebuffer[1];
            char elements_str[] = { '0'+elements/100,'0' + (elements%100) / 10, '0' + elements % 10, '\0'};
            strcat(NextStrQueue, elements_str);
        }else {
            elements = QUEUE_SIZE - queuebuffer[1] + queuebuffer[0];
            char elements_str[] = { '0'+elements/100,'0' + (elements%100) / 10, '0' + elements % 10, '\0'};
            strcat(NextStrQueue, elements_str);
        }





        for (k = 0; k < 128; k++)
        {
            uart0_putc(NextStrQueue[k]);
            while (!uart0_tx_rdy())
                ;
        }
        uart0_putc('\r');
        while (!uart0_tx_rdy())
            ;
        uart0_putc('\n');
        while (!uart0_tx_rdy())
            ;
    }
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
    if (get_file( COM1, &ch))               // if char received from uart (COM1)
    {
        if (i < 128)                                // if our buffer is not full
            InBuf[i++] = ch;                             // store char in buffer
        put_file( COM1, ch);   // put char back to uart (write char to terminal)
        if (ch == '\r')                       // if the char is 'Enter' (Return)
        {
            if ((InBuf[0] == '1'))
            { // if the first char in buffer is '1' and there are at least 8 chars
                if (i >= 8)
                {
                    set_hour((InBuf[1] - '0') * 10 + InBuf[2] - '0'); // use the chars in buffer to set the clock
                    set_min((InBuf[3] - '0') * 10 + InBuf[4] - '0');
                    set_sec((InBuf[5] - '0') * 10 + InBuf[6] - '0');
                    gfprintf( COM1, "Clock changed!\r\n");

                }
                gfprintf( COM1, "%c - %02d:%02d:%02d\r\n", InBuf[0], get_hour(),
                         get_min(), get_sec()); // print the current time to uart
            }
            else if (((InBuf[0] == '2') && (i == 2)))
            {
                task_debugger();
            }
            else if (((InBuf[0] == '3') && (i == 2)))
            {
                queue_debugger();
            }
            i = 0;
        }
    }
}

void get_task_name(char *buf, INT8U id)
{

    switch (id)
    {
    case TASK_RTC:
        strcpy(buf, "TASK_RTC");
        break;

    case TASK_DISPLAY_RTC:
        strcpy(buf, "TASK_DISPLAY_RTC");
        break;
    case TASK_LCD:
        strcpy(buf, "TASK_LCD");
        break;
    case TASK_UART_RX:
        strcpy(buf, "TASK_UART_RX");
        break;
    case TASK_UI:
        strcpy(buf, "TASK_UI");
        break;
    case TASK_UART_TX:
        strcpy(buf, "TASK_UART_TX");
        break;
    case TASK_KEY:
        strcpy(buf, "TASK_KEY");
        break;
    case TASK_UI_KEY:
        strcpy(buf, "TASK_UI_KEY");
        break;

    }
    return;
}

void get_queue_name(char *buf, INT8U id)
{

    switch (id)
    {
    case Q_UART_TX:
        strcpy(buf, "Q_UART_TX");
        break;
    case Q_UART_RX:
        strcpy(buf, "Q_UART_RX");
        break;
    case Q_LCD:
        strcpy(buf, "Q_LCD");
        break;
    case Q_KEY:
        strcpy(buf, "Q_KEY");
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

    if (get_file( COM3, &ch))            // if input received from keypad (COM3)
    {
        switch (my_state)
        {
        case 0:
            Buf[0] = ch;                              // add key press to buffer
            gfprintf( COM2, "%c%c%c", 0x1B, 0xC4, ch); // print the key to the display (COM2) on the second line
            set_state(1);
            break;
        case 1:
            Buf[1] = ch;     // repeat previous step for all digits of the clock
            gfprintf( COM2, "%c%c%c:", 0x1B, 0xC5, ch);
            set_state(2);
            break;
        case 2:
            Buf[2] = ch;
            gfprintf( COM2, "%c%c%c", 0x1B, 0xC7, ch);
            set_state(3);
            break;
        case 3:
            Buf[3] = ch;
            gfprintf( COM2, "%c%c%c:", 0x1B, 0xC8, ch);
            set_state(4);
            break;
        case 4:
            Buf[4] = ch;
            gfprintf( COM2, "%c%c%c", 0x1B, 0xCA, ch);
            set_state(5);
            break;
        case 5:
            Buf[5] = ch;              // when all digits received, set the clock
            set_hour((Buf[0] - '0') * 10 + Buf[1] - '0');
            set_min((Buf[2] - '0') * 10 + Buf[3] - '0');
            set_sec((Buf[4] - '0') * 10 + Buf[5] - '0');
            gfprintf( COM2, "%c%c        ", 0x1B, 0xC4);
            set_state(0);
            break;
        }
    }
}

/****************************** End Of Module *******************************/

