#ifndef __I2C_H
#define __I2C_H

#include "main.h"


void I2CStart(void);
void I2CStop(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);
void I2CInit(void);
void iic_24c02_write(unsigned char *pucBuf, unsigned char ucAddr, unsigned char ucNum);
void iic_24c02_read(unsigned char *pucBuf, unsigned char ucAddr, unsigned char ucNum);
void write_resistor(uint8_t value);
uint8_t read_resistor(void);

#endif
