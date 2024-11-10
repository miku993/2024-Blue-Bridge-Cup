#include "bsp\uart.h"
#include "bsp\led.h"

//变量创建区
__IO uint32_t uwTick_Uart_Set_Point = 0;//控制Uart_Proc的执行速度
UART_HandleTypeDef huart1;

//*串口专用变量
int counter = 0;
char str[40];
unsigned char rx_buffer;


void Uart1_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  
  HAL_UART_Receive_IT(&huart1, &rx_buffer, 1);    //串口中断打开
}


void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)  //不用动，别的地方要调用它的
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
	  
	
  }
}


void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10                                                                                                                                                                                                                                                                                                                   );

    HAL_NVIC_DisableIRQ(USART1_IRQn);
  }
}

void Uart_Proc(void)
{
	if((uwTick -  uwTick_Uart_Set_Point)<2000)	return;//减速函数
	uwTick_Uart_Set_Point = uwTick;
	
	sprintf(str, "%04d:Hello,world.\r\n", counter);
	HAL_UART_Transmit(&huart1,(unsigned char *)str, strlen(str), 50);
	
	if(++counter == 10000)
		counter = 0;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	LED_Disp(0xff);
	HAL_Delay(300);		// Delay函数的实现是通过滴答定时器，若滴答的抢占优先级比串口中断低，则将会卡死在这
	LED_Disp(0x00);	
	
	HAL_UART_Receive_IT(&huart1, &rx_buffer, 1);
}

