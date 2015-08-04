//Study of Analog to digital converter on TM4C123GH6PM Microcontroller

//-----------------------------------------------------
//Device includes,defines and assembler directives
//-----------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "tm4c123gh6pm.h"
#include "header_SSI.h"
#include "init_LCD.h"
#include "initAdc.h"
#include "conversionAdc.h"

//--------------------------------------------------------
//Main
//--------------------------------------------------------
int main(void)
{
    // Initialize hardware
	initHw();
	init_SSI0();
	// Initialize graphics LCD
	initGraphicsLcd();
	// Initialize ADC
	initAdc();
	// Display ADC Value on the screen.
	setGraphicsLcdTextPosition(4, 0);
	double a;
	a = conversionAdc();
	char buffer[20];
	sprintf(buffer,"%.2f",a);
	putsGraphicsLcd("ADC Value is:");
	setGraphicsLcdTextPosition(100, 0);
	putsGraphicsLcd(buffer);
	while(1)
	{}
}
