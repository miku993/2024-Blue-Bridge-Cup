#ifndef __I2C_HAL_H
#define __I2C_HAL_H

#include "stm32g4xx_hal.h"

void I2CStart(void);
void I2CStop(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);
void I2CInit(void);
void iic24c02_Write(uint16_t *pc, uint16_t Addr, uint16_t ucNum);
void iic24c02_Read(uint16_t *pc, uint16_t Addr, uint16_t ucNum);
void msp4017_Write(uint16_t value);
uint16_t msp4017_Read();

#endif
