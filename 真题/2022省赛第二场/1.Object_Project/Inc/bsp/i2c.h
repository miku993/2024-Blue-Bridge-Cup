#ifndef __I2C_H
#define __I2C_H

#include "stm32g4xx_hal.h"

void I2CStart(void);
void I2CStop(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);
void I2CInit(void);
//*********************************** 24C02д
void i2c_24c02_write(uint8_t *pucBuf, uint8_t ucAddr, uint8_t ucNum);
void i2c_24c02_read(uint8_t *pucBuf, uint8_t ucAddr, uint8_t ucNum);
#endif
