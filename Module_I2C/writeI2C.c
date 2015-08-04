/*
 * writeI2C.c
 *
 *  Created on: Jul 23, 2015
 *      Author: Sandeep
 */
#include <stdint.h>
#include <stdbool.h>
#include "initI2C.h"
#include "tm4c123gh6pm.h"

void writeI2C(uint8_t masterSendData,uint8_t dataControl) //Master send data
{
	I2C1_MDR_R   = masterSendData;//Transmit the data
	I2C1_MCS_R   = dataControl;//

	while((I2C1_MCS_R & I2C_MCS_BUSY) != 0);

	if((I2C1_MCS_R & I2C_MCS_ERROR) != 0)
	{
		if((I2C1_MCS_R & I2C_MCS_ARBLST) == 1)
		{

		}
		else
		{
			I2C1_MCS_R |= I2C_MCS_STOP;
			while((I2C1_MCS_R & I2C_MCS_BUSY) != 0);
		}
	}

}

