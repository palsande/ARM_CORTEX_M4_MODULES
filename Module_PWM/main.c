/*
 * main.c
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "initPWM.h"
int main(void)
{
	// Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
	SYSCTL_RCC_R = SYSCTL_RCC_USESYSDIV|SYSCTL_RCC_XTAL_16MHZ|SYSCTL_RCC_OSCSRC_MAIN|(4 << SYSCTL_RCC_SYSDIV_S);

	// Set GPIO ports to use APB (not needed since default configuration -- for clarity)
	// Note UART on port A must use APB
    SYSCTL_GPIOHBCTL_R  = 0;//use of APB bus

    initPWM();

    SYSCTL_RCGC2_R      |= SYSCTL_RCGC2_GPIOF;//Enable and Clock GPIOF port
    GPIO_PORTF_DIR_R    |= 0x08;//PF3 as output
    GPIO_PORTF_DR2R_R   |= 0x08;//2mA current drive strength to PF3
    GPIO_PORTF_DEN_R    |= 0x08;//PF3 as digital
	while(1)
	{
		    int i;
			for(i=10000;i<50000;i+=1000)
			{
				PWM0_0_CMPA_R = (i-1);
			}

	}
}
