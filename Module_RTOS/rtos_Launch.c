/*
 * rtos_Launch.c
 *
 *  Created on: Aug 26, 2015
 *      Author: Sandeep
 */
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "subroutines_RTOS.h"
void rtos_Launch(uint32_t timeSlice)
{
    NVIC_ST_RELOAD_R   = timeSlice-1;   // reload value
    NVIC_ST_CTRL_R     |= 0x07;      // enable SysTick with core clock and interrupts
    NVIC_EN0_R         |= 0x00008000;//Enable IRQ-15
    start_RTOS();
}



