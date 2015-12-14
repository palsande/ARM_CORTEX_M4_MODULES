/*
 * rtos_Init.c
 *
 *  Created on: Aug 26, 2015
 *      Author: Sandeep
 */
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "subroutines_RTOS.h"
void rtos_Init(void)
{
	NVIC_ST_CTRL_R     = 0;         // disable SysTick during setup
    NVIC_ST_CURRENT_R  = 0;         // any write to current clears it
    NVIC_SYS_PRI3_R    = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
}




