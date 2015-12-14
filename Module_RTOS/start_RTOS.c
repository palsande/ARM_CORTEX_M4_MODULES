/*
 * start_RTOS.c
 *
 *  Created on: Aug 26, 2015
 *      Author: Sandeep
 */
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "subroutines_RTOS.h"

void start_RTOS(void)
{
	SP = 0x00000000;//Main Hardware Stack
	SP = currentThread->sp;//Main hardware stack is pointing to  current threads stack
	__asm(" POP {R11}"); //GPR
	__asm(" POP {R10}"); //GPR
	__asm(" POP {R9}");  //GPR
	__asm(" POP {R8}");  //GPR
	__asm(" POP {R7}");  //GPR
	__asm(" POP {R6}");  //GPR
	__asm(" POP {R5}");  //GPR
	__asm(" POP {R4}");  //GPR
    __asm(" POP {R3}");  //GPR
	__asm(" POP {R2}");  //GPR
	__asm(" POP {R1}");  //GPR
	__asm(" POP {R0}");  //GPR
	__asm(" POP {R12}"); //PSR
	__asm(" POP {R14}"); //PC
	//__asm(" POP {R16}"); //LR
	__asm(" POP {R15}"); //GPR
	NVIC_EN0_R |= 0x00008000;//Enable Interrupt
}

