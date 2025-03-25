/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: rtcs.c
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
* 150303  MoH   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "glob_def.h"
#include "rtcs.h"
#include "systick.h"

/*****************************    Defines    *******************************/

typedef struct                              // task control block
{
  INT8U  condition;
  INT8U  name;
  INT8U  state;
  INT8U  event;
  INT8U  sem;
  INT16U timer;
  void (*tf)(INT8U, INT8U, INT8U, INT8U);  // Pointer to task function
} tcb;

typedef struct
{
  INT8U  condition;
  INT8U  type;
  INT8U  count;
}scb;                                       // semaphore control block

typedef struct
{
    INT8U   head;                           // head pointer
    INT8U   tail;                           // tail pointer
    SEM     q_not_full;
    SEM     q_not_empty;
    INT8U buf[QUEUE_SIZE];
}qcb;                                       // queue control block

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
extern volatile INT16S ticks;

HANDLE current_task;

tcb pot[MAX_TASKS];             // Pool of tasks
scb pos[MAX_SEMAPHORES];        // Pool of semaphores
qcb poq[MAX_QUEUES];            // Pool of queues

/*****************************   Functions   *******************************/

HANDLE retrieve_id(void)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  static HANDLE next_id = 0;

  return( next_id++ );
}

void i_am_alive( INT8U my_id, INT8U my_state, INT8U event, INT8U data )
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  if( my_state == 0 )
  {
    // Turn on the LED.
    GPIO_PORTD_DATA_R |= 0x40;
    set_state( 1 );
  }
  else
  {
    // Turn off the LED.
    GPIO_PORTD_DATA_R &= ~(0x40);
    set_state( 0 );
  }
  wait( 200 );
}

extern void set_state( INT8U new_state )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  pot[current_task].state = new_state;
}

extern void wait( INT16U timeout )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  pot[current_task].timer     = timeout;
  pot[current_task].condition = TASK_WAIT_FOR_TIMEOUT;
}

extern BOOLEAN wait_sem( INT8U sem, INT16U timeout )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  BOOLEAN result = TRUE;

  if( pos[sem].count )
  {
    pos[sem].count--;
    pot[current_task].condition = TASK_READY;
    result = TRUE;
  }
  else
  {
    pot[current_task].sem       = sem;
    pot[current_task].condition = TASK_WAIT_FOR_SEMAPHORE;
    if( timeout )
    {
      pot[current_task].timer     = timeout;
      pot[current_task].condition |= TASK_WAIT_FOR_TIMEOUT;
    }
    result = FALSE;
  }
  return( result );
}

extern void signal( INT8U sem )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  if( sem < MAX_SEMAPHORES )
    pos[sem].count++;
}

extern void preset_sem( INT8U sem, INT8U signals )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  if( sem < MAX_SEMAPHORES )
    pos[sem].count = signals;
}

INT8S open_queue( id )
INT8U id;
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  INT8S result;

    if( id < MAX_QUEUES )
    {
      poq[id].head        = 0;
      poq[id].tail        = 0;
      poq[id].q_not_full  = id;
      poq[id].q_not_empty = MAX_QUEUES + id;
      preset_sem( poq[id].q_not_full, QUEUE_SIZE );
      result = id;
    }
    else
      result = -1;
    return( result );
}

BOOLEAN put_queue( id, ch, timeout )
INT8U  id;
INT8U  ch;
INT16U timeout;
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  BOOLEAN result = FALSE;

  if( id < MAX_QUEUES )
  {
    if( wait_sem( poq[id].q_not_full, timeout ))            // if the queue is not full (if full set the task to wait for timeout)
    {
      poq[id].buf[poq[id].head++] = ch;                     // put the char in the queue and move the header pointer
      poq[id].head &= 0x7F;                                 // if head pointer exceeds 127 it is set to 0
      signal( poq[id].q_not_empty );                        // signal the q_not_empty semaphore
      result = TRUE;
    }
  }
  return( result );
}

BOOLEAN get_queue( id, pch, timeout )
INT8U  id;
INT8U *pch;
INT16U timeout;
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  BOOLEAN result = FALSE;

  if( id < MAX_QUEUES )
  {
    if( wait_sem( poq[id].q_not_empty, timeout ))           // if the queue is not empty (if empty set the task to wait for timeout)
    {
      *pch = poq[id].buf[poq[id].tail++];                   // put the value pointed to by the tail pointer in the pch (char pointer). increment tail pointer
      poq[id].tail &= 0x7F;                                 // if the tail pointer exceeds 127 set it to 0
      signal( poq[id].q_not_full );                         // signal the q_not_full semaphore
      result = TRUE;
    }
  }
  return( result );
}


extern HANDLE start_task( INT8U name, void (*tf)(INT8U, INT8U, INT8U, INT8U) )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  HANDLE this_id;

  this_id = retrieve_id();
  if( this_id != ERROR_TASK )
  {
    //pot[this_id].id    = this_id;
    pot[this_id].condition = TASK_READY;
    pot[this_id].name      = name;
    pot[this_id].state     = 0;
    pot[this_id].event     = EVENT_RESET;
    pot[this_id].timer     = 0;
    pot[this_id].tf        = tf;
  }
  return( 0 );
}

extern INT8U init_rtcs()
{
  INT8U i;

  init_systick();
  for( i = 0; i < MAX_SEMAPHORES; i++ )
    pos[i].count = 0;

  for( i = 0; i < MAX_TASKS; i++ )                 // create an array of tasks with length MAX_TASKS
  {
      pot[i].condition = TASK_IDLE;                // set the condition of all the tasks to idle (these are empty tasks to be ignored be scheduler)
  }
  start_task( SYS_TASK+1, i_am_alive );            // start the i_am_alive tasks (blinking led)
  return( 1 );
}

void schedule()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{

  while(1)
  {
    while( !ticks );
    ticks--;
    current_task = 0;
    do
    {
      if( pot[current_task].condition & TASK_WAIT_FOR_SEMAPHORE )       // if the task is waiting for a semaphore
      {
          if( pos[pot[current_task].sem].count )                        // if that semaphore is now available
          {
            if( !( pot[current_task].sem < (2 * MAX_QUEUES )))          // if the semaphore is not reserved for queues (first 2*MAX_QUEUES are reserved)
                pos[pot[current_task].sem].count--;                     // take the semaphore
            pot[current_task].event     = EVENT_SIGNAL;                 // signal event
            pot[current_task].condition = TASK_READY;                   // set the task as ready
          }
      }
      if( pot[current_task].condition & TASK_WAIT_FOR_TIMEOUT )         // if the task is waiting for timeout (from wait for semaphore)
      {
          if( pot[current_task].timer )                                 // if the timer is not zero
          {
          pot[current_task].timer--;                                    // decrement the timer
          if( pot[current_task].timer == 0 )                            // if the timer is now zero
          {
            pot[current_task].event     = EVENT_TIMEOUT;                // the timer has run out
            pot[current_task].condition = TASK_READY;                   // and the task is ready again
          }
          }
      }

      if( pot[current_task].condition == TASK_READY )                   // if the task is ready
        pot[current_task].tf(current_task, pot[current_task].state, pot[current_task].event, 0);    // run the task function
      current_task++;                                                   // increment the task counter (go to next task)
      } while ( pot[current_task].condition != TASK_IDLE );
  }
}
/****************************** End Of Module *******************************/












