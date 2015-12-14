/*
 * main.c
 */
#define RED_LED      (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4))) // on pin PF2

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "initTimer.h"

int main(void)
{
	// Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
	SYSCTL_RCC_R = SYSCTL_RCC_USESYSDIV|SYSCTL_RCC_XTAL_16MHZ|SYSCTL_RCC_OSCSRC_MAIN|(4 << SYSCTL_RCC_SYSDIV_S);

	// Set GPIO ports to use APB (not needed since default configuration -- for clarity)
	// Note UART on port A must use APB
    SYSCTL_GPIOHBCTL_R  = 0;//use of APB bus

    //------------------------------------------------------------------------
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;//Enable PortF
    // Configure LED and pushbutton pins
    GPIO_PORTF_DIR_R = 0x0A;  // bits 1 and 3 are outputs, other pins are inputs
    GPIO_PORTF_DR2R_R = 0x0A; // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTF_DEN_R = 0x1A;  // enable LEDs and pushbuttons
    GPIO_PORTF_PUR_R = 0x10;  // enable internal pull-up for push button
    //------------------------------------------------------------------------------
    initTimer();

    //RED_LED=0;
	while(1)
	{
	    while((TIMER0_RIS_R & 0x1) == 0x0);
	    TIMER0_ICR_R |= 0x1;
		RED_LED^=1;
	}
}
