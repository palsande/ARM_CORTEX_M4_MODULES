//Study of Graphic LCD Module using SSI protocol

//-----------------------------------------------------------------------------
// Device include
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "header_SSI.h"
#include "init_LCD.h"

#define RED_BL_LED   (*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 5*4)))
#define GREEN_BL_LED (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 5*4)))
#define BLUE_BL_LED  (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 4*4)))


//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
    // Initialize hardware
    initHw();
    init_SSI0();

    // Turn-on all LEDs to create white backlight
    RED_BL_LED   = 1;
    GREEN_BL_LED = 1;
    BLUE_BL_LED  = 1;

    // Initialize graphics LCD
    initGraphicsLcd();

    // Display First name and last name on the screen.
    setGraphicsLcdTextPosition(4, 0);
    uint16_t a = 500;
    char buffer[20];
    sprintf(buffer,"%d",a);
    putsGraphicsLcd("ADC Value is:");
    setGraphicsLcdTextPosition(100, 0);
    putsGraphicsLcd(buffer);
    //putcGraphicsLcd(c);

    //putsGraphicsLcd("Pal:");

    while(1)
    {}

}

