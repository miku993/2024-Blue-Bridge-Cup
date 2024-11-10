#include "bsp\key.h"
#include "bsp\led.h"

//����������
__IO uint32_t uwTick_Key_Set_Point = 0;//����Key_Proc��ִ���ٶ�

//*����ɨ��ר�ñ���
unsigned char ucKey_Val, unKey_Down, ucKey_Up, ucKey_Old;



void Key_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  /*Configure GPIO pins : PB0 PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}



uint16_t Key_Scan(void)
{
	unsigned char unKey_Val = 0;
	
	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)
		unKey_Val = 1;

	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET)
		unKey_Val = 2;
	
	return unKey_Val;
}


void Key_Proc(void)
{
	if((uwTick -  uwTick_Key_Set_Point)<100)	return;//���ٺ���   uwTickÿ1ms����1
	uwTick_Key_Set_Point = uwTick;

	
	//�ֳɼ������
	//���1��100ms����ɨ�裬�����õ��Ľ����0����û���£���B4���£��������½��ء�
	//ucKey_Val = 4(0000 0100)
	//unKey_Down = 0000 0100 & ( 0000 0000 ^ 0000 0100) = 0000 0100 & 0000 0100 = 0000 0100 (4)
	//ucKey_Up = 1111 1011 & 0000 0100 = 0000 0000 
	//ucKey_Old = 4
	
	//���2��B4�������½��غ󣬰���һֱ����
	//ucKey_Val = 4(0000 0100)
	//unKey_Down = 0000 0100 & ( 0000 0100 ^ 0000 0100) = 0000 0100 & 0000 0000 = 0000 0000 (0)
	//ucKey_Up = 1111 1011 & 0000 0000 = 0000 0000 
	//ucKey_Old = 4	
	
	//���3��B4����һֱ���������
	//ucKey_Val = 0(0000 0000)
	//unKey_Down = 0000 0000 & ( 0000 0100 ^ 0000 0000) = 0000 0000 & 0000 0100 = 0000 0000 (0)
	//ucKey_Up = 1111 1111 & 0000 0100 = 0000 0100 (4)
	//ucKey_Old = 0		
	
	ucKey_Val = Key_Scan();
	unKey_Down = ucKey_Val & (ucKey_Old ^ ucKey_Val); 
	ucKey_Up = ~ucKey_Val & (ucKey_Old ^ ucKey_Val);	
	ucKey_Old = ucKey_Val;
	
	if(unKey_Down == 1)
	{
		LED_Disp(0x01);
	}
	if(unKey_Down == 2)
	{
		LED_Disp(0x02);
	}	
}

