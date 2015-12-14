/*
 * rtos_AddThreads.c
 *
 *  Created on: Aug 26, 2015
 *      Author: Sandeep
 */
#include <stdint.h>
#include <stdbool.h>
#include "subroutines_RTOS.h"




int rtos_AddThreads (void(*task0)(void),void(*task1)(void),void(*task2)(void))
{
	//Using Circular Linked-List for chaining Threads
    tcbs[0].nextThread = &tcbs[1]; //Thread 0 points to Thread 1
    tcbs[1].nextThread = &tcbs[2]; //Thread 1 points to Thread 2
    tcbs[2].nextThread = &tcbs[0]; //Thread 2 points to Thread 0
    setInitialStack(0);//Set Software Stack for Thread 0
    stacks[0][STACKSIZE-2] = (uint32_t)(task0);//PC for Thread 0
    setInitialStack(1);//Set Software Stack for Thread 1
    stacks[1][STACKSIZE-2] = (uint32_t)(task1);//PC for Thread 1
    setInitialStack(2);//Set Software Stack for Thread 2
    stacks[2][STACKSIZE-2] = (uint32_t)(task2);//PC for Thread 2
    currentThread = &tcbs[0];//Thread 0 will run first
    return 1;
}

