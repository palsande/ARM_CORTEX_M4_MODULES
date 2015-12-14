/*
 * main.c
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "subroutines_RTOS.h"

#define RED_LED      (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4)))
#define BLUE_LED     (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4)))
#define GREEN_LED    (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))

void task0(void)
{
	while(1)
	{
		RED_LED ^= 1;//toggle RED LED
	}
}

void task1(void)
{
	while(1)
	{
		BLUE_LED ^= 1;//toggle BLUE LED
	}
}

void task2(void)
{
	while(1)
	{
		GREEN_LED ^= 1;//toggle GREEN LED
	}
}

void SysTick_Handler(void)
{
	NVIC_EN0_R |= 0x00000000;//Disable Interrupt
//    __asm(" PUSH R0"); //GPR
//    __asm(" PUSH R1"); //GPR
//    __asm(" PUSH R2"); //GPR
//    __asm(" PUSH R3"); //GPR
//    __asm(" PUSH R12");//GPR
//    __asm(" PUSH R14");//LR
//    __asm(" PUSH R15");//PC
//    __asm(" PUSH R16");//PSR
    __asm(" PUSH {R4}"); //GPR
    __asm(" PUSH {R5}"); //GPR
    __asm(" PUSH {R6}"); //GPR
    __asm(" PUSH {R7}"); //GPR
    __asm(" PUSH {R8}"); //GPR
    __asm(" PUSH {R9}"); //GPR
    __asm(" PUSH {R10}");//GPR
    __asm(" PUSH {R11}");//GPR
    SP = 0x00000000;//Main Hardware Stack
    currentThread->sp = SP;//Stack of current thread goes into Main hardware Stack
    currentThread = currentThread->nextThread;//New thread in now current thread
    SP = currentThread->sp;//Main hardware stack is pointing to new current threads stack
    __asm(" POP {R11}"); //GPR
    __asm(" POP {R10}"); //GPR
    __asm(" POP {R9}");  //GPR
    __asm(" POP {R8}");  //GPR
    __asm(" POP {R7}");  //GPR
    __asm(" POP {R6}");  //GPR
    __asm(" POP {R5}");  //GPR
    __asm(" POP {R4}");  //GPR
//    __asm(" POP R16"); //PSR
//    __asm(" POP R15"); //PC
//    __asm(" POP R14"); //LR
//    __asm(" POP R12"); //GPR
//    __asm(" POP R3");  //GPR
//    __asm(" POP R2");  //GPR
//    __asm(" POP R1");  //GPR
//    __asm(" POP R0");  //GPR
    NVIC_EN0_R |= 0x00008000;//Enable Interrupt
}

int main(void)
{
	//--------------------------System Clock Unit----------------------------------------
	// Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
	SYSCTL_RCC_R = SYSCTL_RCC_USESYSDIV|SYSCTL_RCC_XTAL_16MHZ|SYSCTL_RCC_OSCSRC_MAIN|(4 << SYSCTL_RCC_SYSDIV_S);

	// Set GPIO ports to use APB (not needed since default configuration -- for clarity)
	// Note UART on port A must use APB
    SYSCTL_GPIOHBCTL_R  = 0;//use of APB bus
    //--------------------------------------------------------------------------------------
    //----------------LED Control Unit------------------------------------------------------
    SYSCTL_RCGC2_R      = SYSCTL_RCGC2_GPIOF;//Enable port A and port F
    GPIO_PORTF_DIR_R    = 0x0E;//port F pins 1,2,3 are outputs and others are input
    GPIO_PORTF_DR2R_R   = 0x0E;//pins 1,2,3 has 2mA current drive strength
    GPIO_PORTF_PUR_R    = 0x10;// enable internal pull-up for push button on pin 4
    GPIO_PORTF_DEN_R    = 0x1E;//enable(or make LED's and push buttons digital) LEDs and pushbuttons
    //----------------------------------------------------------------------------------------------
    rtos_Init();
    rtos_AddThreads(&task0,&task1,&task2);
    rtos_Launch(4000);

	while(1)
	{

	}
}
