#include "bsp\key.h"
#include "bsp\led.h"

//变量创建区
__IO uint32_t uwTick_Key_Set_Point = 0;//控制Key_Proc的执行速度

//*按键扫描专用变量
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
	if((uwTick -  uwTick_Key_Set_Point)<100)	return;//减速函数   uwTick每1ms自增1
	uwTick_Key_Set_Point = uwTick;

	
	//分成几种情况
	//情况1：100ms两次扫描，按键得到的结果从0（都没按下）到B4按下，产生了下降沿。
	//ucKey_Val = 4(0000 0100)
	//unKey_Down = 0000 0100 & ( 0000 0000 ^ 0000 0100) = 0000 0100 & 0000 0100 = 0000 0100 (4)
	//ucKey_Up = 1111 1011 & 0000 0100 = 0000 0000 
	//ucKey_Old = 4
	
	//情况2：B4产生了下降沿后，按键一直按着
	//ucKey_Val = 4(0000 0100)
	//unKey_Down = 0000 0100 & ( 0000 0100 ^ 0000 0100) = 0000 0100 & 0000 0000 = 0000 0000 (0)
	//ucKey_Up = 1111 1011 & 0000 0000 = 0000 0000 
	//ucKey_Old = 4	
	
	//情况3：B4按键一直按着随后弹起
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

