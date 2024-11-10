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
void iic24c02_Write(uint8_t *pc, uint8_t Addr, uint8_t ucNum);
void iic24c02_Read(uint8_t *pc, uint8_t Addr, uint8_t ucNum);
void msp4017_Write(uint8_t value);
uint8_t msp4017_Read(void);

