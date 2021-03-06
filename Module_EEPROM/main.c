/*
 * main.c
 */
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "initDisplay.h"
#include "initEEPROM.h"
uint32_t a[6];
int main(void)
{

	// Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
	SYSCTL_RCC_R = SYSCTL_RCC_USESYSDIV|SYSCTL_RCC_XTAL_16MHZ|SYSCTL_RCC_OSCSRC_MAIN|(4 << SYSCTL_RCC_SYSDIV_S);

	// Set GPIO ports to use APB (not needed since default configuration -- for clarity)
	// Note UART on port A must use APB
    SYSCTL_GPIOHBCTL_R  = 0;//use of APB bus

    //EEPROM_EESUPP_R  = EEPROM_EESUPP_START|EEPROM_EESUPP_EREQ;//Erase Copy Buffer
    //EEPROM_EEDBGME_R |=0x1;
    //EEPROM_EEPROT_R |= 0x0;//No Protection to blocks

    initEEPROM();
    //initUART0();
    //int i;
    EEPROM_EEBLOCK_R   = EEPROM_EEBLOCK_BLOCK_S;  //Current Block
    EEPROM_EEOFFSET_R  = 0x0;//Current Address offset
    //int i;
    //for(i=10;i<=15;i++)
    //{
    EEPROM_EERDWRINC_R = 11;//Write to EEPROM
    while(EEPROM_EEDONE_R & EEPROM_EEDONE_WRBUSY)//Check if Write was successful
    {}

    EEPROM_EERDWRINC_R = 16;//Write to EEPROM
        while(EEPROM_EEDONE_R & EEPROM_EEDONE_WRBUSY)//Check if Write was successful
        {}
        EEPROM_EERDWRINC_R = 12;//Write to EEPROM
            while(EEPROM_EEDONE_R & EEPROM_EEDONE_WRBUSY)//Check if Write was successful
            {}
            EEPROM_EERDWRINC_R = 13;//Write to EEPROM
                while(EEPROM_EEDONE_R & EEPROM_EEDONE_WRBUSY)//Check if Write was successful
                {}
                EEPROM_EERDWRINC_R = 14;//Write to EEPROM
                    while(EEPROM_EEDONE_R & EEPROM_EEDONE_WRBUSY)//Check if Write was successful
                    {}
    EEPROM_EEBLOCK_R   = EEPROM_EEBLOCK_BLOCK_S;  //Current Block
    EEPROM_EEOFFSET_R  = 0x0;//Current Address offset
    int j;
    for(j=0;j<=5;j++)
    {
       a[j]= EEPROM_EERDWRINC_R;//Read EEPROM
    }

    while(1)
    {

    }
}
