
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "initAdc.h"
void initAdc()
{
	//Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
	SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);
	//ADC Module Initialization
	// Set GPIO ports to use APB (not needed since default configuration -- for clarity)
	// Note UART on port A must use APB
	SYSCTL_GPIOHBCTL_R  = 0;//use of APB bus

	SYSCTL_RCGC2_R     |= SYSCTL_RCGC2_GPIOE; //Enable and clock portE
	//SYSCTL_RCGCGPIO_R  |= SYSCTL_RCGCGPIO_R4; //Enable and clock portE
	//SYSCTL_RCGC0_R     |= 0x00010000;         //Select ADC0 and provide clock to it
	SYSCTL_RCGCADC_R   |= SYSCTL_RCGCADC_R0;  //Select ADC0 and provide clock to it
	GPIO_PORTE_DIR_R   &= ~0x08;              //Channel AIN0(PE3) as input
	GPIO_PORTE_AFSEL_R |= 0x08;               //No Alternate function on portE pins
	GPIO_PORTE_DEN_R    &= ~0x08;               //Make GPIO pin PE3 Analog
	GPIO_PORTE_AMSEL_R |= 0x08;               //Disable Analog isolation circuit on PE3
	//ADC0 Module Configuration
	ADC0_SSPRI_R	   |= 0x00;
	ADC0_ACTSS_R       &= ~0x01;               //Disable SS0 Sample Sequencer
	ADC0_EMUX_R        |= 0x00;                //Software triggered SS0
	ADC0_SSMUX0_R      &= ~0xFFFFFFFF;
	ADC0_SSMUX0_R      = 0;                //select 1st sample from SS0
	ADC0_SSCTL0_R      |= 0x02;                //flag set after sample 0 of SS0 done
	ADC0_ACTSS_R       |= 0x01;
}
