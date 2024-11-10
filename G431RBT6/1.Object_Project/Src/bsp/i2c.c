/**
 * CT117E-M4 / GPIO - I2C
*/

#include "i2c.h"

#define DELAY_TIME	20

//
void SDA_Input_Mode()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = GPIO_PIN_7;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//
void SDA_Output_Mode()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = GPIO_PIN_7;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//
void SDA_Output( uint16_t val )
{
    if ( val )
    {
        GPIOB->BSRR |= GPIO_PIN_7;
    }
    else
    {
        GPIOB->BRR |= GPIO_PIN_7;
    }
}

//
void SCL_Output( uint16_t val )
{
    if ( val )
    {
        GPIOB->BSRR |= GPIO_PIN_6;
    }
    else
    {
        GPIOB->BRR |= GPIO_PIN_6;
    }
}

//
uint8_t SDA_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_SET){
		return 1;
	}else{
		return 0;
	}
}

//
static void delay1(unsigned int n)
{
    uint32_t i;
    for ( i = 0; i < n; ++i);
}

//
void I2CStart(void)
{
    SDA_Output(1);
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SDA_Output(0);
    delay1(DELAY_TIME);
    SCL_Output(0);
    delay1(DELAY_TIME);
}

//
void I2CStop(void)
{
    SCL_Output(0);
    delay1(DELAY_TIME);
    SDA_Output(0);
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SDA_Output(1);
    delay1(DELAY_TIME);

}

//
unsigned char I2CWaitAck(void)
{
    unsigned short cErrTime = 5;
    SDA_Input_Mode();
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    while(SDA_Input())
    {
        cErrTime--;
        delay1(DELAY_TIME);
        if (0 == cErrTime)
        {
            SDA_Output_Mode();
            I2CStop();
            return ERROR;
        }
    }
    SCL_Output(0);
    SDA_Output_Mode();
    delay1(DELAY_TIME);
    return SUCCESS;
}

//
void I2CSendAck(void)
{
    SDA_Output(0);
    delay1(DELAY_TIME);
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SCL_Output(0);
    delay1(DELAY_TIME);

}

//
void I2CSendNotAck(void)
{
    SDA_Output(1);
    delay1(DELAY_TIME);
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SCL_Output(0);
    delay1(DELAY_TIME);

}

//
void I2CSendByte(unsigned char cSendByte)
{
    unsigned char  i = 8;
    while (i--)
    {
        SCL_Output(0);
        delay1(DELAY_TIME);
        SDA_Output(cSendByte & 0x80);
        delay1(DELAY_TIME);
        cSendByte += cSendByte;
        delay1(DELAY_TIME);
        SCL_Output(1);
        delay1(DELAY_TIME);
    }
    SCL_Output(0);
    delay1(DELAY_TIME);
}

//
unsigned char I2CReceiveByte(void)
{
    unsigned char i = 8;
    unsigned char cR_Byte = 0;
    SDA_Input_Mode();
    while (i--)
    {
        cR_Byte += cR_Byte;
        SCL_Output(0);
        delay1(DELAY_TIME);
        delay1(DELAY_TIME);
        SCL_Output(1);
        delay1(DELAY_TIME);
        cR_Byte |=  SDA_Input();
    }
    SCL_Output(0);
    delay1(DELAY_TIME);
    SDA_Output_Mode();
    return cR_Byte;
}

//
void I2CInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = GPIO_PIN_7 | GPIO_PIN_6;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//*********************************** 24C02д
void iic_24c02_write(uint8_t *pucBuf, uint8_t ucAddr, uint8_t ucNum)
{
	/******* дҳ��ʱ�� *******/
	
	/******* ���� *******/
	I2CStart();		// ��ʼ�ź�
	I2CSendByte(0xa0);		// �ӻ���ַ -> 1010 xxxx
	I2CWaitAck();		// �����ȴ�Ӧ��
	
	/******* ���ʾ���λ�� *******/
	I2CSendByte(ucAddr);		// ��Ҫ���ʴӻ����ĸ���ַ
	I2CWaitAck();		// �����ȴ�Ӧ��
	
	while(ucNum--)
	{
		I2CSendByte(*pucBuf++);		// ��������
		I2CWaitAck();		// �����ȴ�Ӧ��
	}
	I2CStop();		// ֹͣ�ź�
	HAL_Delay(500);	
}

//*********************************** 24C02��
void iic_24c02_read(uint8_t *pucBuf, uint8_t ucAddr, uint8_t ucNum)
{
	/******* �����ַ��дʱ�� *******/
	
	/******* ���� *******/
	I2CStart();		// ��ʼ�ź�
	I2CSendByte(0xa0);		// �ӻ���ַ -> 1010 xxxx
	I2CWaitAck();		// �����ȴ�Ӧ��
	
	/******* ���ʾ���λ�� *******/
	I2CSendByte(ucAddr);		// ��Ҫ���ʴӻ����ĸ���ַ
	I2CWaitAck();		// �����ȴ�Ӧ��
	
	/******* �� *******/
	I2CStart();
	I2CSendByte(0xa1);		// ������ -> 1010 0001
	I2CWaitAck();
	
	while(ucNum--)
	{
		*pucBuf++ = I2CReceiveByte();
		if(ucNum)
			I2CSendAck();	
		else
			I2CSendNotAck();
	}
	I2CStop();		// ֹͣ�ź�
}


//*********************************** MCP4017д
void write_resistor(uint8_t value)
{
	I2CStart();
	I2CSendByte(0x5E);		// д���� -> 0101 1110
	I2CWaitAck();
	
	I2CSendByte(value);
	I2CWaitAck();	
	I2CStop();	
}


//*********************************** MCP4017��
uint8_t read_resistor(void)
{
	uint8_t value;
	I2CStart();
	I2CSendByte(0x5F);		// ������ -> 0101 1111
	I2CWaitAck();
	
	value = I2CReceiveByte();
	I2CSendNotAck();	
	I2CStop();	
	
	return value;
}
