//Study of I2C on TM4C123GH6PM Microcontroller

//-----------------------------------------------------
//Device includes
//-----------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "tm4c123gh6pm.h"
#include "initI2C.h"

//--------------------------------------------------------
//Main
//--------------------------------------------------------

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

int main(void)
{
    // Initialize I2C
	initI2C();
	// Initialize I2C
	//initHW();

	//slaveSelectI2C(0x20);//Select IO Expander
	slaveSelectI2C(0x80);//Select Sensor with write
    writeI2C(0xFE,(I2C_MCS_RUN | I2C_MCS_STOP | I2C_MCS_START));//Soft Reset
    slaveSelectI2C(0x80);//Again Select Sensor
    writeI2C(0xE3,(I2C_MCS_RUN | I2C_MCS_START));//Measure Temperature;
    slaveSelectI2C(0x81);//
    uint16_t data = readI2C(I2C_MCS_RUN | I2C_MCS_START| I2C_MCS_ACK);
    data = data << 8;
    data |= readI2C(I2C_MCS_RUN | I2C_MCS_STOP);
    putsUart0("Serial Example\r\n");
//      putsUart0("Press '0' or '1'\r\n");
//      putcUart0('a\r\n');
//      uint16_t a = data;
      char buffer[20];
      sprintf(buffer,"%d",data);
      putsUart0(buffer);


    while(1)
    {
    // Display greeting
//    putsUart0("Serial Example\r\n");
//    putsUart0("Press '0' or '1'\r\n");
//    putcUart0('a\r\n');
//    uint16_t a = data;
//    char buffer[20];
//    sprintf(buffer,"%d",a);
//    putsUart0(buffer);
    //writeI2C(0x09,(I2C_MCS_RUN | I2C_MCS_START));//Select GPIO register
    //waitMicrosecond(2000000);
    //writeI2C(0xFF,(I2C_MCS_RUN | I2C_MCS_STOP));//Select all LED's on GPIO
    //waitMicrosecond(2000000);
    }

}
