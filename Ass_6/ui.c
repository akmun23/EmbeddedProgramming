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

void task_debugger()
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function : Prints the info from task on the terminal
 ******************************************************************************/
{
    int j = 0;
    int k = 0;
    uart0_putc('\r');                                                       // Reset cursor to start
    while (!uart0_tx_rdy())
        ;                                                                   // Wait for the reset to be written
    uart0_putc('\n');                                                       // Starts a new line
    while (!uart0_tx_rdy());                                                // Waits for the new line to be written

    for (j = 1; j <= MAX_TASKS; j++)                                        // Runs thorugh the maximum amount of task possible
    {
        get_task_info(taskbuffer, j);                                       // Gets info from task j
        namebuffer[0] = '\0';                                               // Reset namebuffer
        get_task_name(namebuffer, taskbuffer[0]);                           // Gets name for the current task

        char NextStrTask[128] = "TASK: ";
        char id[] = { '0' + taskbuffer[0] / 10, '0' + taskbuffer[0] % 10 }; // Writes id
        strcat(NextStrTask, id);                                            // Insert the id into the string

        strcat(NextStrTask, ", NAME: ");                                    // Start Name
        if (namebuffer[0] == '\0')                                          // If task is null
        {
            strcat(namebuffer, "                ");                         // Write empty string
        }

        while (strlen(namebuffer) < NAME_LENGTH)                            // If name is less than 16 char
        {
            strcat(namebuffer, " ");                                        // Fill it with space until it is 16 char long
        }

        strcat(NextStrTask, namebuffer);                                    // Insert the name that was found

        strcat(NextStrTask, ", CONDITION:");                                // Start Condition
        switch (taskbuffer[1])                                              // Find which condition the current task is in
        {
        case 1:
            strcat(NextStrTask, " READY  , ");                              // Condition is ready
            strcat(NextStrTask, "                     ");                   // Fill in blank where Timer and Sem should be
            break;
        case 2:
        case 4:
            strcat(NextStrTask, " WAITING, ");                              // Start Waiting
                if (taskbuffer[5])                                          // If it is waiting for Sem
                {
                    strcat(NextStrTask, " SEM: ");                          // Start Sem
                    char SEM[] = { '0' + taskbuffer[5] / 10, '0'
                                                               + taskbuffer[5] % 10,
                                                       ',', '\0' };         // Find Sem
                    strcat(NextStrTask, SEM);                               // Insert Sem into string
                }else{
                    strcat(NextStrTask, "         ");                       // Insert blank space

                }


                if (taskbuffer[6])                                          // If it is waiting for Timer
                {
                    strcat(NextStrTask, " TIM: ");                          // Start Tim

                    char TIM[] = { '0', '0', '0' + (taskbuffer[6] % 1000) / 100, '0'
                                           + (taskbuffer[6] % 100) / 10,
                                   '0' + taskbuffer[6] % 10, ',', '\0' };   // Find value of Tim
                    strcat(NextStrTask, TIM);                               // Insert value into string
                }else{
                    strcat(NextStrTask, "            ");                    // Insert blank space

                }
                break;
        default:
            strcat(NextStrTask, " DEAD   , ");                              // Set task condition to DEAD
            strcat(NextStrTask, "                     ");                   // Insert blank spaces
            break;

        }

        strcat(NextStrTask, " STATE: ");                                    // Start State
        char STATE[] = { '0' + taskbuffer[3] / 10, '0' + taskbuffer[3] % 10 }; // Find value of State
        strcat(NextStrTask, STATE);                                         //Insert value into string

        for (k = 0; k < 128; k++)                                           // Goes for the length of InBuf
        {
            uart0_putc(NextStrTask[k]);                                     // Writes one char at a time
            while (!uart0_tx_rdy());                                        // Waits for UART to be ready for the next char

        }
        uart0_putc('\r');                                                   // Resets the UART
        while (!uart0_tx_rdy());                                            // Waits for UART to be ready

        uart0_putc('\n');                                                   // Starts a new line
        while (!uart0_tx_rdy());                                            // Waits for UART to be ready
    }
}

void queue_debugger()
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function : Prints the info from queues on the terminal
 ******************************************************************************/
{
    int j = 0;
    int k = 0;
    uart0_putc('\r');                                                       // Reset cursor to start
    while (!uart0_tx_rdy())
        ;                                                                   // Wait for the reset to be written
    uart0_putc('\n');                                                       // Starts a new line
    while (!uart0_tx_rdy());                                                // Waits for the new line to be written
        ;
    for (j = 0; j <= MAX_QUEUES; j++)                                       // Goes for the maximum amount of Queues possible
    {

        get_queue_info(queuebuffer, j);                                     // Gets info for queue j
        namebuffer[0] = '\0';                                               // Resets namebuffer
        get_queue_name(namebuffer, j);                                      // Gets name of the Queue

        char NextStrQueue[128] = "QUEUE: ";                                 // Starts Queue
        char id[] = { '0' + j / 10, '0' + j % 10 };                         // Gets id of Queue
        strcat(NextStrQueue, id);                                           // Inserts id into string

        strcat(NextStrQueue, ", NAME: ");                                   // Start Name
        if (namebuffer[0] == '\0')                                          // If Name is null
        {
            strcat(namebuffer, "                ");                         // Insert blanks

        }
        while (strlen(namebuffer) < NAME_LENGTH)                            // A name is 16 chars
        {
            strcat(namebuffer, " ");                                        // Fill in blank until length is 16 chars
        }
        strcat(NextStrQueue, namebuffer);                                   // Insert name

        strcat(NextStrQueue, " HEAD: ");                                    // Start Head
        char head[] = { '0' + queuebuffer[0] / 100, '0'
                                + (queuebuffer[0] % 100) / 10,
                        '0' + queuebuffer[0] % 10 };                        // Find value of Head
        strcat(NextStrQueue, head);                                         // Insert value into string

        strcat(NextStrQueue, " TAIL: ");                                    // Start Tail
        char tail[] = { '0' + queuebuffer[1] / 100, '0'
                                + (queuebuffer[1] % 100) / 10,
                        '0' + queuebuffer[1] % 10, '\0' };                  // Find value of Tail
        strcat(NextStrQueue, tail);                                         // Insert value into string

        strcat(NextStrQueue, " NOT_FULL: ");                                // Start Not_full
        char not_full[] = { '0' + queuebuffer[2] % 10, '\0' };              // Find value of Not_full
        strcat(NextStrQueue, not_full);                                     // Insert value into string

        strcat(NextStrQueue, " NOT_EMPTY: ");                               // Start Not_Empty
        char not_empty[] = { '0' + (queuebuffer[3] % 100) / 10, '0'
                                     + queuebuffer[3] % 10,
                             '\0' };                                        // Find value of Not_empty
        strcat(NextStrQueue, not_empty);                                    // Insert into string

        strcat(NextStrQueue, " NUM_ELEMENTS: ");                            // Start Num_elements
        int elements = 0;                                                   // Initialize variable
        if (queuebuffer[0] >= queuebuffer[1])                               // If Head > Tail
        {
            elements = queuebuffer[0] - queuebuffer[1];                     // Elements = Head - Tail
            char elements_str[] = { '0' + elements / 100, '0'
                                            + (elements % 100) / 10,
                                    '0' + elements % 10, '\0' };            // Find value of Elements
            strcat(NextStrQueue, elements_str);                             // Insert into string
        }
        else                                                                // If Tail > Head
        {
            elements = QUEUE_SIZE - queuebuffer[1] + queuebuffer[0];        // ELEMENTS = QUEUE_SIZE - Tail + Head
            char elements_str[] = { '0' + elements / 100, '0'
                                            + (elements % 100) / 10,
                                    '0' + elements % 10, '\0' };            // Find value of Elements
            strcat(NextStrQueue, elements_str);                             // Insert into string
        }

        for (k = 0; k < 128; k++)                                           // Goes for the length of InBuf
        {
            uart0_putc(NextStrQueue[k]);                                    // Writes one char at a time
            while (!uart0_tx_rdy());                                        // Waits for UART to be ready for the next char

        }
        uart0_putc('\r');                                                   // Resets the UART
        while (!uart0_tx_rdy());                                            // Waits for UART to be ready

        uart0_putc('\n');                                                   // Starts a new line
        while (!uart0_tx_rdy());                                            // Waits for UART to be ready
    }
}

void ui_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
/*****************************************************************************
 *   Input    : INT8U, INT8U, INT8U, INT8U
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
/*****************************************************************************
 *   Input    : char*, INTU8
 *   Output   :
 *   Function : Get the name of the task based on the id
 ******************************************************************************/
{

    switch (id)                         // Switch case to find the Task name that fits the id
    {
    case TASK_RTC:
        strcpy(buf, "TASK_RTC");        // If id = 1, Task = Task_RTC
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
/*****************************************************************************
 *   Input    : char*, INTU8
 *   Output   :
 *   Function : Get the name of the queue based on the id
 ******************************************************************************/
{

    switch (id)                     // Switch case to find the Queue name that fits the id
    {
    case Q_UART_TX:                 // If id = 0
        strcpy(buf, "Q_UART_TX");   // Queue name = Q_UART_TX
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

