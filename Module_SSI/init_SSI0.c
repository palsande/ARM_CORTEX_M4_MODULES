// Synchronous serial interface initialization
// Sandeep Pal

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:

//   SSI0Tx   on PA5
//   SSI0Rx   on
//   SSI0Clk  on PA2
//   SSI0Fss  on PA3


//-----------------------------------------------------------------------------
// Device include
//-----------------------------------------------------------------------------

#include "header_SSI.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

//-----------------------------------------------------------------------------
// Subroutine
//-----------------------------------------------------------------------------

// Initialize SSI0 Module

void init_SSI0()
{

	//Initialization of SSI0 Module
	SYSCTL_RCGCSSI_R   |= SYSCTL_RCGCSSI_R0;//Enable SSI Module using RCGCSSI Register
	SYSCTL_RCGC2_R     |= SYSCTL_RCGC2_GPIOA;//Enable clock to Port A
	GPIO_PORTA_DIR_R   |= 0x24;
	GPIO_PORTA_DR2R_R  |= 0x24;
	GPIO_PORTA_AFSEL_R |= 0x24;//Port A pins 2,3,4,5 working as Alternate functions
	GPIO_PORTA_PCTL_R  |= GPIO_PCTL_PA2_SSI0CLK | GPIO_PCTL_PA5_SSI0TX;//port A pins 2,3,4,5 assigned related SSI signals
	GPIO_PORTA_DEN_R   |= 0x24;//port A pins 2,3,4,5 enabled as digital
	//GPIO_PORTA_PUR_R   |= 0X04;//port A pin2(SSICLK) enabled as pull-up

    //Configuration of SSI0 Module
	SSI0_CR1_R  &= ~(SSI_CR1_SSE); //SSE bit is clear for SSICR1 Register
	SSI0_CR1_R   = 0x00000000; // SSI working as master slave mode
	SSI0_CC_R    = 0x0; // SSI Clock Source is system clock
	SSI0_CPSR_R  = 40;  //SSInClk=1/(CPSDVSR*(1+SCR)) will give digital data rate of 1MHz
	SSI0_CR0_R   = SSI_CR0_SPH|SSI_CR0_SPO|SSI_CR0_FRF_MOTO|SSI_CR0_DSS_8;//set SR=0, mode 3 (SPH=1, SPO=1), 8-bit
	SSI0_CR1_R  |= SSI_CR1_SSE;//SSE bit is set for SSICR1 Register
}

