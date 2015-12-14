#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"

#define RED_LED      (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4)))
#define BLUE_LED     (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4)))
#define GREEN_LED    (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))
#define PUSH_BUTTON  (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 4*4)))

void initHW()
{
	// Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
	SYSCTL_RCC_R = SYSCTL_RCC_USESYSDIV|SYSCTL_RCC_XTAL_16MHZ|SYSCTL_RCC_OSCSRC_MAIN|(4 << SYSCTL_RCC_SYSDIV_S);

	// Set GPIO ports to use APB (not needed since default configuration -- for clarity)
	// Note UART on port A must use APB
    SYSCTL_GPIOHBCTL_R  = 0;//use of APB bus
    SYSCTL_RCGC2_R      = SYSCTL_RCGC2_GPIOF | SYSCTL_RCGC2_GPIOA;//Enable port A and port F
    GPIO_PORTF_DIR_R    = 0x0E;//port F pins 1,2,3 are outputs and others are input
    GPIO_PORTF_DR2R_R   = 0x0E;//pins 1,2,3 has 2mA current drive strength
    GPIO_PORTF_PUR_R    = 0x10;// enable internal pull-up for push button on pin 4
    GPIO_PORTF_DEN_R    = 0x1E;//enable(or make LED's and push buttons digital) LEDs and pushbuttons

    //UART0 Module Initialization
    GPIO_PORTA_DEN_R = 0x03;			//PORT A enable
   	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
   	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
   	GPIO_PORTA_AFSEL_R|= 0x3;
   	GPIO_PORTA_PCTL_R  = GPIO_PCTL_PA0_U0RX | GPIO_PCTL_PA1_U0TX;
   	//Configure UART0 to 115200 baud, 8N1 format (must be 3 clocks from clock enable and config writes)
   	UART0_CTL_R  = 0;
   	UART0_CC_R   = UART_CC_CS_SYSCLK;
   	UART0_IBRD_R = 21;
   	UART0_FBRD_R = 45;
   	UART0_LCRH_R = UART_LCRH_WLEN_8|UART_LCRH_FEN;
   	UART0_CTL_R  = UART_CTL_UARTEN|UART_CTL_RXE|UART_CTL_TXE;

}

// Blocking function that writes a serial character when the UART buffer is not full
void putcUart0(char c)
{
    while(UART0_FR_R & UART_FR_TXFF);
    UART0_DR_R = c;
}

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(char* str)
{
    int i;
    for(i = 0; i < strlen(str); i++)
      putcUart0(str[i]);
}

// Blocking function that returns with serial data once the buffer is not empty
char getcUart0()
{
    while(UART0_FR_R & UART_FR_RXFE);
    return UART0_DR_R & 0xFF;
}

// Blocking function that returns only when SW1 is pressed
void waitPbPress()
{
    while(PUSH_BUTTON);
}

int main(void)
{
	// Initialize hardware
	initHW();

    // Display greeting
    putsUart0("Serial Example\r\n");
    putsUart0("Press '0' or '1'\r\n");
    putcUart0('a');
    BLUE_LED = 1;

    // Wait for PB press
    waitPbPress();

    // For each received character, toggle the red LED
    // For each received "1", set the green LED
    // For each received "0", clear the green LED
    while(1)
    {
        char c = getcUart0();
        RED_LED ^= 1;
        if (c == '1')
            GREEN_LED = 1;
        if (c == '0')
            GREEN_LED = 0;
    }

}
