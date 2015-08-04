/*
 * slaveSelectI2C.c
 *
 *  Created on: Jul 23, 2015
 *      Author: Sandeep
 */
#include "initI2C.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>

void slaveSelectI2C(uint8_t slave) //Select slave for I2C Master
{
	//I2C1_MSA_R   = (slave << 1);//Slave as Microchip's IO Expander
	I2C1_MSA_R   = slave;//Slave as HTU21DF Sensor
}

