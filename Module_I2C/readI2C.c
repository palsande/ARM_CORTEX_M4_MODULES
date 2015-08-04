/*
 * readI2C.c
 *
 *  Created on: Jul 23, 2015
 *      Author: Sandeep
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "initI2C.h"
#include "tm4c123gh6pm.h"
uint8_t masterReceiveData;

uint8_t readI2C(uint8_t dataControl) //Master receive data
{

	I2C1_MCS_R   = dataControl;//Control the transmit data
	return masterReceiveData = I2C1_MDR_R;//Receive the data

}

