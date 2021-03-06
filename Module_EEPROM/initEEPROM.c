/*
 * initEEPROM.c
 *
 *  Created on: Jul 30, 2015
 *      Author: Sandeep
 */
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "initEEPROM.h"
#include "initDisplay.h"

void initEEPROM()
{
	SYSCTL_RCGCEEPROM_R &= ~SYSCTL_SREEPROM_R0;//Disable and Clock EEPROM Module
	SYSCTL_RCGCEEPROM_R |= SYSCTL_RCGCEEPROM_R0;//Enable and Clock EEPROM Module
	//6 cycles delay
	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
	__asm(" NOP");
	while(EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING)//Poll WORKING bit of EEDONE Register
	{}
	if((EEPROM_EESUPP_R & EEPROM_EESUPP_PRETRY) == 0x00000008)
	{
		//Put Error Message on UART
	    putsUart0("EEPROM init Error\r\n");
	}
	else if((EEPROM_EESUPP_R & EEPROM_EESUPP_PRETRY) == 0x00000004)
	{
		//Put Error Message on UART
		putsUart0("EEPROM init Error\r\n");
	}
	else
	{
		//putsUart0("EEPROM init Success\r\n");
	}
	//EEPROM_EEDBGME_R |=0xE37B0001;





//	SYSCTL_SREEPROM_R |= SYSCTL_SREEPROM_R0;//Reset SREEPROM Module
//	__asm("NOP");//6 cycles delay
//	__asm("NOP");
//	__asm("NOP");
//	__asm("NOP");
//	__asm("NOP");
//	__asm("NOP");
//	while((EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING) == 0x00000000)//Poll WORKING bit of EEDONE Register
//	{}
//	if((EEPROM_EESUPP_R & EEPROM_EESUPP_PRETRY) == 0x00000008)
//		{
//			//Put Error Message on UART
//		    putsUart0("EEPROM init Error\r\n");
//		}
//		else if((EEPROM_EESUPP_R & EEPROM_EESUPP_PRETRY) == 0x00000004)
//		{
//			//Put Error Message on UART
//			putsUart0("EEPROM init Error\r\n");
//		}
//		else
//		{
//			putsUart0("EEPROM init Success\r\n");
//		}

}

