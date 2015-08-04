/*
 * initI2C.c
 *
 *  Created on: Jul 23, 2015
 *      Author: Sandeep
 */
#include <stdint.h>
#include <stdbool.h>
#include "initI2C.h"
#include "tm4c123gh6pm.h"

void initI2C()
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
	SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

	// Set GPIO ports to use APB (not needed since default configuration -- for clarity)
	// Note UART on port A must use APB
	SYSCTL_GPIOHBCTL_R = 0;

	//Initialize I2C Module

	//SYSCTL_RCGCI2C_R   |= SYSCTL_RCGCI2C_R1;//Enable and clock I2C1
	SYSCTL_RCGC1_R     |= SYSCTL_RCGC1_I2C1;//Enable Legacy I2C1 Module Register
	SYSCTL_RCGCGPIO_R  |= SYSCTL_RCGCGPIO_R0;//Port A provides I2C1 Module
	GPIO_PORTA_AFSEL_R |= 0xC0;//Port A pins 6,7 working as Alternate pins
	GPIO_PORTA_DEN_R   |= 0xC0;//Port A pins 6,7 Digitally enabled
	GPIO_PORTA_ODR_R   |= 0x80;//PortA pin7 as Open Drain
	GPIO_PORTA_PCTL_R  |= 0x33000000;//Device Signal for Port A pins 6,7


	//Configure I2C1 Module
    I2C1_MCR_R  |= I2C_MCR_MFE;//I2C1 as Master Device
    I2C1_MTPR_R  = 0x13;//I2C1 Master Set Clock

}

