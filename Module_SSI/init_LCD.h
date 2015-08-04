/*
 * init_LCD.h
 *
 *  Created on: Jul 15, 2015
 *      Author: Sandeep
 */

#ifndef INIT_LCD_H_
#define INIT_LCD_H_

#include <stdint.h>

void initHw();
void waitMicrosecond(uint32_t us);
void sendGraphicsLcdCommand(uint8_t command);
void sendGraphicsLcdData(uint8_t data);
void setGraphicsLcdPage(uint8_t page);
void setGraphicsLcdColumn(uint8_t x);
void refreshGraphicsLcd();
void clearGraphicsLcd();
void initGraphicsLcd();
void setGraphicsLcdTextPosition(uint8_t x, uint8_t page);
void putcGraphicsLcd(char c);
void putsGraphicsLcd(char str[]);


#endif /* INIT_LCD_H_ */
