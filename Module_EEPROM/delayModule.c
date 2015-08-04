/*
 * delayModule.c
 *
 *  Created on: Jul 30, 2015
 *      Author: Sandeep
 */
#include "tm4c123gh6pm.h"
#include "initEEPROM.h"
#include <stdint.h>
#include <stdbool.h>
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


