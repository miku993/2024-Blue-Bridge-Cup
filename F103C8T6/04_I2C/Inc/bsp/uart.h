#include "main.h"

//需要在Drivers/STM32F1xx_HAL_Driver中添加uart.c 
//在stm32f1xx_hal_conf.h 中解除 UART_ENABLED 的注释

void Uart1_Init(void);
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle);  //不用动，别的地方要调用它的
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle);
void Uart_Proc(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);




