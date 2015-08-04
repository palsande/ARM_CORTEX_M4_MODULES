/*
 * initInterrupt.c
 *
 *  Created on: Jul 26, 2015
 *      Author: Sandeep
 */
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "initInterrupt.h"

void initInterrupt()
{
	/* Interrupt Configuration */
	// enable interrupts for Port F
	GPIO_PORTF_IM_R |= 0x10;
	// enable interrupt for PortF(IRQ 30) at NVIC
	NVIC_EN0_R |= 0x40000000;
	NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF0FFFFF) | 0x00600000; // here PortF is given to priority 3
	GPIO_PORTF_IS_R  &= ~0x10;// PF4 pin has falling-edge trigger
	GPIO_PORTF_IBE_R &= ~0x10;
	GPIO_PORTF_IEV_R &= ~0x10;// falling-edge interrupt

}

