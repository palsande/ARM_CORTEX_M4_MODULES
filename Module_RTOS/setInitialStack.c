/*
 * setInitialStack.c
 *
 *  Created on: Aug 26, 2015
 *      Author: Sandeep
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "subroutines_RTOS.h"

void setInitialStack(uint32_t i)
{
    tcbs[i].sp = &stacks[i][STACKSIZE-16];//thread stack pointer software based
    stacks[i][STACKSIZE-1] = 0x01000000;//Thumb Bit (PSR)
    stacks[i][STACKSIZE-3] = 0x14141414;//R14
    stacks[i][STACKSIZE-4] = 0x12121212;//R12
    stacks[i][STACKSIZE-5] = 0x03030303;//R3
    stacks[i][STACKSIZE-6] = 0x02020202;//R2
    stacks[i][STACKSIZE-7] = 0x01010101;//R1
    stacks[i][STACKSIZE-8] = 0x00000000;//R0
    stacks[i][STACKSIZE-9] = 0x11111111;//R11
    stacks[i][STACKSIZE-10]= 0x10101010;//R10
    stacks[i][STACKSIZE-11]= 0x09090909;//R9
    stacks[i][STACKSIZE-12]= 0x08080808;//R8
    stacks[i][STACKSIZE-13]= 0x07070707;//R7
    stacks[i][STACKSIZE-14]= 0x06060606;//R6
    stacks[i][STACKSIZE-15]= 0x05050505;//R5
    stacks[i][STACKSIZE-16]= 0x04040404;//R4
}
