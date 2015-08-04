/*
 * displayUART.c
 *
 *  Created on: Jul 23, 2015
 *      Author: Sandeep
 */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "initI2C.h"
#include "tm4c123gh6pm.h"

void initHW()
{
    //UART0 Module Initialization
    GPIO_PORTA_DEN_R |= 0x03;			//PORT A enable
   	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
   	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
   	GPIO_PORTA_AFSEL_R|= 0x3;
   	GPIO_PORTA_PCTL_R  |= GPIO_PCTL_PA0_U0RX | GPIO_PCTL_PA1_U0TX;
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

