/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: ui.h
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
* 050128  KA    Module created.
*
*****************************************************************************/

#ifndef _UI_H
  #define _UI_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

void ui_task(INT8U, INT8U, INT8U, INT8U);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Test function
******************************************************************************/
void ui_key_task(INT8U, INT8U, INT8U, INT8U);
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Test function
******************************************************************************/
void get_task_name(char *buf, INT8U id);
void get_queue_name(char *buf, INT8U id);
void task_debugger();
void queue_debugger();


/****************************** End Of Module *******************************/
#endif

