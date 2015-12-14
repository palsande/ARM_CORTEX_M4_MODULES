/*
 * initTimer.c
 *
 *  Created on: Aug 20, 2015
 *      Author: Sandeep
 */
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
void initTimer()
{
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;//Timer0 Module enabled
	SYSCTL_RCGC1_R     |= SYSCTL_RCGC1_TIMER0;//Timer0 Module enabled for legacy register support
    //  one-shot/periodic timer mode
	TIMER0_CTL_R   |= 0x00000000;//TimerA of Module Timer0 is disabled TIMER_CTL_TAEN
	TIMER0_CFG_R   |= 0x4;//Timer0 Module working as 16-bit not 32-bit
	TIMER0_TAMR_R  |= 0x2;//Timer0 Module working as one-shot
	TIMER0_TAILR_R = (63000-1);//Load value in the timer
	TIMER0_CTL_R   |= TIMER_CTL_TAEN;//TimerA of Module Timer0 is enabled TIMER_CTL_TAEN
}


