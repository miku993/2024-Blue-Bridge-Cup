#ifndef __I2C_HAL_H
#define __I2C_HAL_H

#include "main.h"
#include "stm32g4xx_hal.h"

void I2CStart(void);
void I2CStop(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);
void I2CInit(void);
void iic_24c02_write(uint8_t *pucBuf, uint8_t ucAddr, uint8_t ucNum);
void iic_24c02_read(uint8_t *pucBuf, uint8_t ucAddr, uint8_t ucNum);
void write_resistor(uint8_t value);
uint8_t read_resistor(void);


#endif
