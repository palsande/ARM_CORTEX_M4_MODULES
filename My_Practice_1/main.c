#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "tm4c123gh6pm.h"

#define RED_LED     (*((volatile uint32_t*)(0x42000000 + (0x400253FC-0x40000000)*32 +1*4)))
#define GREEN_LED   (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))
#define BLUE_LED   (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4)))
#define PUSH_BUTTON (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 4*4)))

void init_hw()
{
	//Configure HW to work with 16MHZ XTAL, PLL enabled, system clock of 40MHZ
	SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

	// Set GPIO ports to use APB (not needed since default configuration -- for clarity)
	// Note UART on port A must use APB
	SYSCTL_GPIOHBCTL_R = 0;

	// Enable GPIO port A and F peripherals
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOF;

	//Configure LED and Push Button pins
	GPIO_PORTF_DIR_R = 0x0E;  // bits 1,2 & 3 are outputs, other pins are inputs
	GPIO_PORTF_DR2R_R = 0x0E; // set drive strength to 2mA (not needed since default configuration -- for clarity)
	GPIO_PORTF_PUR_R = 0x10;  // enable internal pull-up for push button
	GPIO_PORTF_DEN_R = 0x1E;  // enable LEDs and pushbuttons


	//Configure UART0 pins
	GPIO_PORTA_DEN_R |= 0x3;											//Digital enable PA0 and PA1
	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0; 							//Enable UART0 module
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
	GPIO_PORTA_AFSEL_R |= 0x3;											//Select alternate function at PA0 and PA1
	GPIO_PORTA_PCTL_R = GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX;		//Select specific peripheral signal for each GPIO pin when using AFSEL

}

void init_UART()
{
	//Configure UART0 module for baud 115200, 8N1
	UART0_CTL_R &= ~UART_CTL_UARTEN;					//Disable UART0 for safe programming
	UART0_IBRD_R = 21;									//integer value from BRD = BRDI+BRDF = UARTSysClk / (ClkDiv * BaudRate)
	UART0_FBRD_R = 45;									//Fraction value from UARTFBRD[DIVFRAC] = integer(BRDF * 64 + 0.5)
	UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;	//Configure 8N1 with 16 level FIFO enabled
	UART0_CC_R = UART_CC_CS_SYSCLK;						//UART clock source is system clock
	UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;  //Enable receive, transmit and UART0 module

}

//Delay in us
void waitMicrosecond(uint32_t us)
{
	//__asm("             MOV R0, #2000000");     // Approx clocks per us
	    __asm("WMS_LOOP0:   MOV  R1, #6");          // 1
	    __asm("WMS_LOOP1:   SUB  R1, #1");          // 6
	    __asm("             CBZ  R1, WMS_DONE1");   // 5+1*3
	    __asm("             NOP");                    // 5
	    __asm("             NOP");                    // 5
	    __asm("             B    WMS_LOOP1");       // 5*2
	    __asm("WMS_DONE1:   SUB  R0, #1");          // 1
	    __asm("             CBZ  R0, WMS_DONE0");   // 1
	    __asm("             NOP");                    // 1
	    __asm("             B    WMS_LOOP0");       // 1*2
	    __asm("WMS_DONE0:");                        // ---
	                                                // 40 clocks/us + error
}

//Blocking function to transmit or write character when UART transmit buffer is not full
void serial_putc(char c)
{
	while (UART0_FR_R & UART_FR_TXFF);					//Check if transmit buffer is full or not
	UART0_DR_R = c;										//If the buffer is not full write character into the data register
}

//Blocking function to transmit or write a string
void serial_puts(char *str)
{
	int i;
	for (i=0 ; i<strlen(str) ; i++)
		serial_putc(str[i]);							//Transmit a string
}

//Blocking function to receive or read a character when receive buffer is not empty
unsigned char serial_getc()
{
	while (UART0_FR_R & UART_FR_RXFF);					//Check if receive buffer is full or not
	return UART0_DR_R & 0xFF;							//Read data from data register if buffer is full
}

int main(void)
{
	init_hw();				//Initialize hardware
	init_UART();			//Initialize UART module

	serial_puts("Hello everyone!\r\n");			//Display the string on Tera term
	serial_putc('a');
	BLUE_LED = 1;
	waitMicrosecond(2000000);
	BLUE_LED = 0;
	//while(1);
	while(1)
	{
	char c = serial_getc();
	GREEN_LED ^= 1;			//Toggle Green led(ex-or operation)
	if (c == '1')
		RED_LED = 1;
	if (c == '0')
		RED_LED = 0;
	}
}
