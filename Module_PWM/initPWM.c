/*
 * initPWM.c
 *
 *  Created on: Aug 6, 2015
 *      Author: Sandeep
 */
#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>
void initPWM()
{
	SYSCTL_RCGCPWM_R   |= SYSCTL_RCGCPWM_R0;//Enable and Clock PWM Module0
	SYSCTL_RCGC2_R     |= SYSCTL_RCGC2_GPIOB;//Enable and Clock GPIOB Module
	//GPIO_PORTB_AFSEL_R |= 0xC0;// GPIOF working as alternate function
	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
	GPIO_PORTB_AFSEL_R |= 0x40;
	//GPIO_PORTB_PCTL_R  |= GPIO_PCTL_PB6_M0PWM0|GPIO_PCTL_PB7_M0PWM1;//Port Control for PB6 and PB7
	GPIO_PORTB_PCTL_R  |= GPIO_PCTL_PB6_M0PWM0;//PWM Module0 selected
	GPIO_PORTB_AMSEL_R &= ~0x40;//Disable Analog to PB6
	GPIO_PORTB_DEN_R   |= 0x40;//PB6 as Digital
	SYSCTL_RCC_R       |= SYSCTL_RCC_PWMDIV_M;//Enable PWM Clock Divisor
	SYSCTL_RCC_R       |= SYSCTL_RCC_PWMDIV_8;//PWM Clock divide by 8
	PWM0_0_CTL_R         = 0;//Enable PWM in Reloading Mode
	PWM0_0_GENA_R      |=0x000000C6;//High when load values and low when match values
	//PWM0_0_GENB_R      |=0x0000080C;//
	//PWM0_0_LOAD_R      |=0x0000018F;
	PWM0_0_LOAD_R      = (50000-1);//
	PWM0_0_CMPA_R      = (12500-1);//
	/*int i;
	for(i=100;i<3999;i+=20)
	{
		PWM0_0_CMPA_R = i;
	}*/
	//PWM0_0_CMPA_R      |=0x0000012B;//
	//PWM0_0_CMPB_R      |=0x00000063;//
	PWM0_0_CTL_R         |=0x00000001;//
	PWM0_ENABLE_R      |=0x00000001;//
}

