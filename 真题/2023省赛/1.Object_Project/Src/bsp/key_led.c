#include "key_led.h"


__IO uint32_t uwTick_Key_Start =0;
__IO uint32_t uwTick_Key_End =0;


void Key_led_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_8
												|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_8
												|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  //**�����еĵ�Ϩ��
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_8
												|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);		
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}


void Led_Disp(uint8_t ucLed)
{
	//**�����еĵ�Ϩ��
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_8
												|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);		
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);

	//����ucLed����ֵ������Ӧ�ĵ�
	HAL_GPIO_WritePin(GPIOC, ucLed<<8, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);		
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);	
}


uint8_t Key_Scan(void)
{
	uint8_t Key_value;
	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == 0)		//B1
	{
		HAL_Delay(10);
		Key_value =1;
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == 0);
	}
	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 0)		//B2
	{
		HAL_Delay(10);
		Key_value =2;
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 0);
	}
	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == 0)		//B3
	{
		HAL_Delay(10);
		Key_value =3;
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == 0);
	}
	
	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == RESET)
	{
		HAL_Delay(10);
		uwTick_Key_Start =uwTick;	// �����£���ʼ��¼��ǰ���ֵ
		
		while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == RESET)
		{
			uwTick_Key_End =uwTick;		// ����ˢ��uwTick_Key_End��ֵ
		}
		
		if(uwTick_Key_End -uwTick_Key_Start >=2000)		// ����ֵ����2s
		{
			Key_value =5;
		}
		else
			Key_value =4;
	}
	
	return Key_value;
}
