#include "main.h"

//��Ҫ��Drivers/STM32F1xx_HAL_Driver�����uart.c 
//��stm32f1xx_hal_conf.h �н�� UART_ENABLED ��ע��

void Uart1_Init(void);
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle);  //���ö�����ĵط�Ҫ��������
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle);
void Uart_Proc(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);




