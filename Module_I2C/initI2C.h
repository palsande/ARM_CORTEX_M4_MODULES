/*
 * initI2C.h
 *
 *  Created on: Jul 23, 2015
 *      Author: Sandeep
 */

#ifndef INITI2C_H_
#define INITI2C_H_
#include <stdio.h>
#include <stdint.h>

void initI2C();
void slaveSelectI2C(uint8_t slave);//Select slave for I2C Master
void writeI2C(uint8_t masterSendData,uint8_t dataControl);//Master send data
uint8_t readI2C(uint8_t dataControl);//Slave sends data
void displayUART();//Show message via UART
void putcUart0(char c);
void putsUart0(char* str);
char getcUart0();
void initHW();

#endif /* INITI2C_H_ */
