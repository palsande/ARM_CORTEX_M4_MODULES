/*
 * subroutines_RTOS.h
 *
 *  Created on: Aug 26, 2015
 *      Author: Sandeep
 */

#ifndef SUBROUTINES_RTOS_H_
#define SUBROUTINES_RTOS_H_

//-----------------------------Thread Control Block Unit-----------------------------
#define NUMTHREADS 3     //Maximum Number of threads for this project
#define STACKSIZE  100   //no of 32-bit words stack

struct tcb
{
    uint32_t   *sp;    //Software based Stack pointer for thread
    struct tcb *nextThread;//linked list pointer to other 2 threads(nodes)
};
typedef struct tcb tcbDetails; //Description of thread unit of each threads
extern tcbDetails tcbs[NUMTHREADS];
extern tcbDetails *currentThread;
//extern tcbDetails stacks[NUMTHREADS][STACKSIZE];
extern uint32_t stacks[NUMTHREADS][STACKSIZE];
extern uint32_t *SP;
//---------------------------------------------------------------------------------------
void setInitialStack(uint32_t i);
int  rtos_AddThreads (void(*task0)(void),void(*task1)(void),void(*task2)(void));//Add Active Threads or ready to run threads
void rtos_Init(void);
void rtos_Launch(uint32_t timeSlice);
void start_RTOS(void);

#endif /* SUBROUTINES_RTOS_H_ */
