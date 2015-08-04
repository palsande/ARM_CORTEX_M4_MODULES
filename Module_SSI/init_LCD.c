// Graphics LCD Example
// Sandeep Pal

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL with LCD/Keyboard Interface
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

//-------------------------------------------------------------------------------
// Hardware configuration:
//-------------------------------------------------------------------------------

// Red Backlight LED:
//   PB5 drives an NPN transistor that powers the red LED
// Green Backlight LED:
//   PE5 drives an NPN transistor that powers the green LED
// Blue Backlight LED:
//   PE4 drives an NPN transistor that powers the blue LED
// ST7565R Graphics LCD Display Interface:
//   MOSI (SSI2Tx) on PA5
//   MISO (SSI2Rx) is not used by the LCD display but the pin is used for GPIO for A0
//   SCLK (SSI2Clk) on PA2
//   A0 connected to PA4
//   ~CS connected to PA6
//   SSI0Fss connected to PA3

//-----------------------------------------------------------------------------
// Device include
//-----------------------------------------------------------------------------

#include "init_LCD.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

#define CS_NOT       (*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 1*4)))
#define A0           (*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 6*4)))

	// Set pixel arguments
	#define CLEAR  0
	#define SET    1
	#define INVERT 2

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

	uint8_t     pixelMap[1024];
	uint16_t    txtIndex = 0;
	// 96 character 5x7 bitmaps based on ISO-646 (BCT IRV extensions)
	const uint8_t charGen[100][5] = {
		// Codes 32-127
		// Space ! " % $ % & ' ( ) * + , - . /
		{0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x4F, 0x00, 0x00},
		{0x00, 0x07, 0x00, 0x07, 0x00},
		{0x14, 0x7F, 0x14, 0x7F, 0x14},
		{0x24, 0x2A, 0x7F, 0x2A, 0x12},
		{0x23, 0x13, 0x08, 0x64, 0x62},
		{0x36, 0x49, 0x55, 0x22, 0x40},
		{0x00, 0x05, 0x03, 0x00, 0x00},
		{0x00, 0x1C, 0x22, 0x41, 0x00},
		{0x00, 0x41, 0x22, 0x1C, 0x00},
		{0x14, 0x08, 0x3E, 0x08, 0x14},
		{0x08, 0x08, 0x3E, 0x08, 0x08},
		{0x00, 0x50, 0x30, 0x00, 0x00},
		{0x08, 0x08, 0x08, 0x08, 0x08},
		{0x00, 0x60, 0x60, 0x00, 0x00},
		{0x20, 0x10, 0x08, 0x04, 0x02},
		// 0-9
		{0x3E, 0x51, 0x49, 0x45, 0x3E},
		{0x00, 0x42, 0x7F, 0x40, 0x00},
		{0x42, 0x61, 0x51, 0x49, 0x46},
		{0x21, 0x41, 0x45, 0x4B, 0x31},
		{0x18, 0x14, 0x12, 0x7F, 0x10},
		{0x27, 0x45, 0x45, 0x45, 0x39},
		{0x3C, 0x4A, 0x49, 0x49, 0x30},
		{0x01, 0x71, 0x09, 0x05, 0x03},
		{0x36, 0x49, 0x49, 0x49, 0x36},
		{0x06, 0x49, 0x49, 0x29, 0x1E},
		// : ; < = > ? @
		{0x00, 0x36, 0x36, 0x00, 0x00},
		{0x00, 0x56, 0x36, 0x00, 0x00},
		{0x08, 0x14, 0x22, 0x41, 0x00},
		{0x14, 0x14, 0x14, 0x14, 0x14},
		{0x00, 0x41, 0x22, 0x14, 0x08},
		{0x02, 0x01, 0x51, 0x09, 0x3E},
		{0x32, 0x49, 0x79, 0x41, 0x3E},
		// A-Z
		{0x7E, 0x11, 0x11, 0x11, 0x7E},
		{0x7F, 0x49, 0x49, 0x49, 0x36},
		{0x3E, 0x41, 0x41, 0x41, 0x22},
		{0x7F, 0x41, 0x41, 0x22, 0x1C},
		{0x7F, 0x49, 0x49, 0x49, 0x41},
		{0x7F, 0x09, 0x09, 0x09, 0x01},
		{0x3E, 0x41, 0x49, 0x49, 0x3A},
		{0x7F, 0x08, 0x08, 0x08, 0x7F},
		{0x00, 0x41, 0x7F, 0x41, 0x00},
		{0x20, 0x40, 0x41, 0x3F, 0x01},
		{0x7F, 0x08, 0x14, 0x22, 0x41},
		{0x7F, 0x40, 0x40, 0x40, 0x40},
		{0x7F, 0x02, 0x0C, 0x02, 0x7F},
		{0x7F, 0x04, 0x08, 0x10, 0x7F},
		{0x3E, 0x41, 0x41, 0x41, 0x3E},
		{0x7F, 0x09, 0x09, 0x09, 0x06},
		{0x3E, 0x41, 0x51, 0x21, 0x5E},
		{0x7F, 0x09, 0x19, 0x29, 0x46},
		{0x46, 0x49, 0x49, 0x49, 0x31},
		{0x01, 0x01, 0x7F, 0x01, 0x01},
		{0x3F, 0x40, 0x40, 0x40, 0x3F},
		{0x1F, 0x20, 0x40, 0x20, 0x1F},
		{0x3F, 0x40, 0x70, 0x40, 0x3F},
		{0x63, 0x14, 0x08, 0x14, 0x63},
		{0x07, 0x08, 0x70, 0x08, 0x07},
		{0x61, 0x51, 0x49, 0x45, 0x43},
		// [ \ ] ^ _ `
		{0x00, 0x7F, 0x41, 0x41, 0x00},
		{0x02, 0x04, 0x08, 0x10, 0x20},
		{0x00, 0x41, 0x41, 0x7F, 0x00},
		{0x04, 0x02, 0x01, 0x02, 0x04},
		{0x40, 0x40, 0x40, 0x40, 0x40},
		{0x00, 0x01, 0x02, 0x04, 0x00},
		// a-z
		{0x20, 0x54, 0x54, 0x54, 0x78},
		{0x7F, 0x44, 0x44, 0x44, 0x38},
		{0x38, 0x44, 0x44, 0x44, 0x20},
		{0x38, 0x44, 0x44, 0x48, 0x7F},
		{0x38, 0x54, 0x54, 0x54, 0x18},
		{0x08, 0x7E, 0x09, 0x01, 0x02},
		{0x0C, 0x52, 0x52, 0x52, 0x3E},
		{0x7F, 0x08, 0x04, 0x04, 0x78},
		{0x00, 0x44, 0x7D, 0x40, 0x00},
		{0x20, 0x40, 0x44, 0x3D, 0x00},
		{0x7F, 0x10, 0x28, 0x44, 0x00},
		{0x00, 0x41, 0x7F, 0x40, 0x00},
		{0x7C, 0x04, 0x18, 0x04, 0x78},
		{0x7C, 0x08, 0x04, 0x04, 0x78},
		{0x38, 0x44, 0x44, 0x44, 0x38},
		{0x7C, 0x14, 0x14, 0x14, 0x08},
		{0x08, 0x14, 0x14, 0x18, 0x7C},
		{0x7C, 0x08, 0x04, 0x04, 0x08},
		{0x48, 0x54, 0x54, 0x54, 0x20},
		{0x04, 0x3F, 0x44, 0x40, 0x20},
		{0x3C, 0x40, 0x40, 0x20, 0x7C},
		{0x1C, 0x20, 0x40, 0x20, 0x1C},
		{0x3C, 0x40, 0x20, 0x40, 0x3C},
		{0x44, 0x28, 0x10, 0x28, 0x44},
		{0x0C, 0x50, 0x50, 0x50, 0x3C},
		{0x44, 0x64, 0x54, 0x4C, 0x44},
		// { | } ~ cc
		{0x00, 0x08, 0x36, 0x41, 0x00},
		{0x00, 0x00, 0x7F, 0x00, 0x00},
		{0x00, 0x41, 0x36, 0x08, 0x00},
		{0x0C, 0x04, 0x1C, 0x10, 0x18},
		{0x00, 0x00, 0x00, 0x00, 0x00},
		// Custom assignments beyond ISO646
		// Codes 128+: right arrow, left arrow, degree sign
		{0x08, 0x08, 0x2A, 0x1C, 0x08},
		{0x08, 0x1C, 0x2A, 0x08, 0x08},
		{0x07, 0x05, 0x07, 0x00, 0x00},
	};

	const uint8_t charGen1[1][4] = {0x1E, 0x05, 0x05, 0x1E};


//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware
void initHw()
{
	// Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

    // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
    // Note UART on port A must use APB
    SYSCTL_GPIOHBCTL_R = 0;

    // Enable GPIO port B and E peripherals for RGB backlights
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOB | SYSCTL_RCGC2_GPIOE | SYSCTL_RCGC2_GPIOA;

    // Configure three backlight LEDs
    GPIO_PORTB_DIR_R    |= 0x20;   // make bit5 an output
    GPIO_PORTB_DR2R_R   |= 0x20;   // set drive strength to 2mA
    GPIO_PORTB_DEN_R    |= 0x20;   // enable bit5 for digital
    GPIO_PORTE_DIR_R    |= 0x30;   // make bits 4 and 5 outputs
    GPIO_PORTE_DR2R_R   |= 0x30;   // set drive strength to 2mA
    GPIO_PORTE_DEN_R    |= 0x30;   // enable bits 4 and 5 for digital

    GPIO_PORTB_DIR_R   |= 0x42;   // make bits 1 and 6 outputs
    GPIO_PORTB_DR2R_R  |= 0x42;   // set drive strength to 2mA
    GPIO_PORTB_DEN_R   |= 0x42;   // enable bits 1 and 6 for digital

}

// Approximate busy waiting (in units of microseconds), given a 40 MHz system clock
void waitMicrosecond(uint32_t us)
{
	                                            // Approx clocks per us
	__asm("WMS_LOOP0:   MOV  R1, #6");          // 1
    __asm("WMS_LOOP1:   SUB  R1, #1");          // 6
    __asm("             CBZ  R1, WMS_DONE1");   // 5+1*3
    __asm("             NOP");                  // 5
    __asm("             B    WMS_LOOP1");       // 5*3
    __asm("WMS_DONE1:   SUB  R0, #1");          // 1
    __asm("             CBZ  R0, WMS_DONE0");   // 1
    __asm("             B    WMS_LOOP0");       // 1*3
    __asm("WMS_DONE0:");                        // ---
                                                // 40 clocks/us + error
}

// Blocking function that writes data to the SPI bus and waits for the data to complete transmission
void sendGraphicsLcdCommand(uint8_t command)
{
	CS_NOT = 0;                        // assert chip select
	__asm (" NOP");                    // allow line to settle
	__asm (" NOP");
	__asm (" NOP");
	__asm (" NOP");
	A0 = 0;                            // clear A0 for commands
	SSI0_DR_R = command;               // write command
	while (SSI0_SR_R & SSI_SR_BSY);
	CS_NOT = 1;                        // de-assert chip select
}

// Blocking function that writes data to the SPI bus and waits for the data to complete transmission
void sendGraphicsLcdData(uint8_t data)
{
	CS_NOT = 0;                        // assert chip select
	__asm (" NOP");                    // allow line to settle
  	__asm (" NOP");
    __asm (" NOP");
	__asm (" NOP");
	A0 = 1;                            // set A0 for data
	SSI0_DR_R = data;                  // write data
	while (SSI0_SR_R & SSI_SR_BSY);    // wait for transmission to stop
	CS_NOT = 1;                        // de-assert chip select
}

void setGraphicsLcdPage(uint8_t page)
{
  sendGraphicsLcdCommand(0xB0 | page);
}

void setGraphicsLcdColumn(uint8_t x)
{
  sendGraphicsLcdCommand(0x10 | ((x >> 4) & 0x0F));
  sendGraphicsLcdCommand(0x00 | (x & 0x0F));
}

void refreshGraphicsLcd()
{
    uint8_t x, page;
    uint16_t i = 0;
    for (page = 0; page < 8; page ++)
    {
    	setGraphicsLcdPage(page);
        setGraphicsLcdColumn(0);
        for (x = 0; x < 128; x++)
    	    sendGraphicsLcdData(pixelMap[i++]);
    }
}

void clearGraphicsLcd()
{
    uint16_t i;
    // clear data memory pixel map
    for (i = 0; i < 1024; i++)
        pixelMap[i] = 0;
    // copy to display
    refreshGraphicsLcd();
}
//Single Power Supply

void initGraphicsLcd()
{
    sendGraphicsLcdCommand(0x40); // set start line to 0
    sendGraphicsLcdCommand(0xA1); // reverse horizontal order
    sendGraphicsLcdCommand(0xC0); // normal vertical order
    sendGraphicsLcdCommand(0xA6); // normal pixel polarity
    sendGraphicsLcdCommand(0xA2); // set led bias to 1/9 (should be A2)
    sendGraphicsLcdCommand(0x2F); // turn on voltage booster and regulator
    sendGraphicsLcdCommand(0xF8); // set internal volt booster to 4x Vdd
    sendGraphicsLcdCommand(0x00);
    sendGraphicsLcdCommand(0x27); // set contrast
    sendGraphicsLcdCommand(0x81); // set LCD drive voltage
    sendGraphicsLcdCommand(0x16);
    sendGraphicsLcdCommand(0xAC); // no flashing indicator
    sendGraphicsLcdCommand(0x00);
    clearGraphicsLcd();           // clear display
    sendGraphicsLcdCommand(0xAF); // display on
}

void setGraphicsLcdTextPosition(uint8_t x, uint8_t page)
{
    txtIndex = (page << 7) + x;
    setGraphicsLcdPage(page);
    setGraphicsLcdColumn(x);
}

void putcGraphicsLcd(char c)
{
    uint8_t i, val;
    uint8_t uc;
    // convert to unsigned to access characters > 127
    uc = (uint8_t) c;
    for (i = 0; i < 5; i++)
    {
        val = charGen[uc-' '][i];
        pixelMap[txtIndex++] = val;
        sendGraphicsLcdData(val);
    }
    pixelMap[txtIndex++] = 0;
    sendGraphicsLcdData(0);
}

void putsGraphicsLcd(char str[])
{
    uint8_t i = 0;
    while (str[i] != 0)
        putcGraphicsLcd(str[i++]);
}
