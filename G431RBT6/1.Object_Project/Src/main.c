#include "main.h"
#include "rcc.h"
#include "key_led.h"
#include "lcd.h"
#include "uart.h"
#include "i2c.h"
#include "adc.h"
#include "tim.h"
#include "rtc.h"
#include "exti.h"

/********** 定义区 ***********/	

/********** 函数声明区 ***********/	
void Lcd_Proc(void);
void Key_Proc(void);
void Uart_Proc(void);
void iic_24c02_test(void);
void resistor_test(void);
void Led_Proc(void);
	
/********** 变量声明区 ***********/	

//************************ 用户自定义变量

//************************ key_led专属变量
__IO uint32_t uwTick_Key_Set_Point = 0;
__IO uint32_t uwTick_Led_Set_Point = 0;
uint8_t ucLed;
uint8_t Key_Value;

//************************ lcd专属变量
__IO uint32_t uwTick_Lcd_Set_Point = 0;
unsigned char Lcd_Disp_String[21];//最多显示20个字符

//************************ tim专属变量
uint8_t num =0;
uint16_t PWM_T_Count;
uint16_t PWM_D_Count;
float Duty;
float Freq;

//************************ adc专属变量


//************************ i2c专属变量
uint8_t EEPROM_String_1[5] = {1, 2, 3, 4 ,5};
uint8_t EEPROM_String_2[5] = {0};
uint8_t RES_4017;

//************************ rtc专属变量
RTC_TimeTypeDef H_M_S_Time;
RTC_DateTypeDef Y_M_D_Date;

//************************ uart专属变量
__IO uint32_t uwTick_Usart_Set_Point = 0;
uint16_t counter = 0;	// 行数计时器
unsigned char rx_buffer[4];		// 接收缓存区
unsigned char tx_buffer[4];		// 发送缓存区



/********** 主函数 ***********/	
int main(void)
{

/********** 初始化配置 ***********/	
  HAL_Init();
  SystemClock_Config();
  Key_LED_Init();
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);	
  UART_Init();
  I2CInit();
  ADC1_Init();
  ADC2_Init();
  TIM6_Init();
  TIM2_Init();
  TIM3_Init();
  TIM17_Init();
  TIM15_Init();
  RTC_Init();  
	
/********** 外设使用基本配置 ***********/	
//*EEPROM测试
  iic_24c02_test();	
	
//*MCP4017测试
  resistor_test();
  
//*Time_Base启动
  HAL_TIM_Base_Start_IT(&htim6);//每100ms触发一次中断
  
//*IC启动  
  HAL_TIM_Base_Start(&htim2);    //启动定时器
  HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);    //启动定时器通道输入捕获并开启中断
  HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);	
  
//*OC启动 （方波）
  HAL_TIM_OC_Start_IT(&htim15,TIM_CHANNEL_1);
	
//*OC启动 （PWM波）
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);	// 30%
  HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);	// 60%

//*串口接收开启
  HAL_UART_Receive_IT(&huart1, rx_buffer, 1);		// 这句必须有

/*********** 主循环 ***********/	
  while (1)
  {
	Key_Proc();
	Led_Proc();
	Lcd_Proc();
	//Uart_Proc();
  }
}


/*********** 子函数 ***********/	

//************************************** LCD扫描子函数
void Lcd_Proc(void)
{
	if((uwTick -  uwTick_Lcd_Set_Point)<100)	return;//减速函数
		uwTick_Lcd_Set_Point = uwTick;
	
	//*显示测试
	sprintf((char *)Lcd_Disp_String, "        DATA      ");
	LCD_DisplayStringLine(Line0, Lcd_Disp_String);
	
	//*串口接收测试
	sprintf((char *)Lcd_Disp_String, "%x", *rx_buffer);
	LCD_DisplayStringLine(Line1, Lcd_Disp_String);

	//*EEPROM测试
	sprintf((char *)Lcd_Disp_String, "EE:%x %x %x %x %x R:%x",EEPROM_String_2[0],
		EEPROM_String_2[1],EEPROM_String_2[2],EEPROM_String_2[3],EEPROM_String_2[4], RES_4017);
	LCD_DisplayStringLine(Line2, Lcd_Disp_String);
	sprintf((char *)Lcd_Disp_String, "RES_K:%5.2fK",0.7874*RES_4017);
	LCD_DisplayStringLine(Line3, Lcd_Disp_String);	
	sprintf((char *)Lcd_Disp_String, "VOLTAGE:%6.3fV",3.3*((0.7874*RES_4017)/(0.7874*RES_4017+10)));
	LCD_DisplayStringLine(Line4, Lcd_Disp_String);	

	//*ADC测试
	sprintf((char *)Lcd_Disp_String, "R38_Vol:%6.3fV",(float)getADC1()/4096*3.3);
	LCD_DisplayStringLine(Line5, Lcd_Disp_String);	
	sprintf((char *)Lcd_Disp_String, "R37_Vol:%6.3fV",(float)getADC2()/4096*3.3);
	LCD_DisplayStringLine(Line6, Lcd_Disp_String);	
	
	//*PWM输入捕获测试，测量占空比和频率
	sprintf((char *)Lcd_Disp_String, "R40P:%05dHz,%3.1f%%",(unsigned int)Freq, Duty);		// 1MHZ -> f=1M/PWM_T_Count
	LCD_DisplayStringLine(Line7, Lcd_Disp_String);		
	
	//*RTC内容显示
	HAL_RTC_GetTime(&hrtc, &H_M_S_Time, RTC_FORMAT_BIN);//读取日期和时间必须同时使用
	HAL_RTC_GetDate(&hrtc, &Y_M_D_Date, RTC_FORMAT_BIN);
	sprintf((char *)Lcd_Disp_String, "Monday-%02d-%02d-2024",(unsigned int)Y_M_D_Date.Month,(unsigned int)Y_M_D_Date.Date);
	LCD_DisplayStringLine(Line8, Lcd_Disp_String);	
	sprintf((char *)Lcd_Disp_String, "Time:%02d-%02d-%02d",(unsigned int)H_M_S_Time.Hours,(unsigned int)H_M_S_Time.Minutes,(unsigned int)H_M_S_Time.Seconds);
	LCD_DisplayStringLine(Line9, Lcd_Disp_String);		
	

}


//*************************************** 按键扫描子函数
void Key_Proc(void)
{
	if((uwTick -  uwTick_Key_Set_Point)<50)	return;//减速函数
		uwTick_Key_Set_Point = uwTick;

	Key_Value = Key_Scan();
	
	if(Key_Value == 1)
	{
		ucLed |= 0x88;	// 亮指定灯
	}
	if(Key_Value == 2)
	{
		ucLed &= (~0x88);  // 灭指定灯
	}	
	if(Key_Value == 3)
	{
		ucLed ^= 0x88;  // 取反指定灯
	}	
	Key_Value =0;
}

//**************************** Uart扫描子函数
void Uart_Proc(void)
{
	if((uwTick -  uwTick_Usart_Set_Point)<1000)	return;//减速函数
	uwTick_Usart_Set_Point = uwTick;
	
	sprintf((char *)tx_buffer, "%04d:Hello,world.\r\n", counter);
	HAL_UART_Transmit(&huart1, tx_buffer, strlen(tx_buffer), 50);
	
	if(++counter == 10000)
		counter = 0;
}


//*********************************** 24C02读写测试
void iic_24c02_test(void)
{
	iic_24c02_write(EEPROM_String_1, 0, 5);
	HAL_Delay(10);		// 延时>=5
	iic_24c02_read(EEPROM_String_2, 0, 5);
}


//*********************************** MCP4017读写测试
void resistor_test(void)
{
	write_resistor(0x11);
    RES_4017 = read_resistor();
}

//*************************************** LED扫描子函数
void Led_Proc(void)
{
	if((uwTick -  uwTick_Led_Set_Point)<200)	return;//减速函数
		uwTick_Led_Set_Point = uwTick;

	LED_Disp(ucLed);
}





/*********** 中断回调函数 ***********/	

//*************************************** EXTI回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	LED_Disp(0x01);
}


//**************************** 串口接收中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(*rx_buffer =='?')
	{
		*rx_buffer ='0';
		sprintf((char *)tx_buffer, "Got it!");
		HAL_UART_Transmit(&huart1, tx_buffer, strlen(tx_buffer), 50);
	}
}


//************************************* Time_Base中断回调
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM6)
	{
//		if(++num == 10)		// 1s	
//		{
//			num = 0;
//			sprintf((char *)tx_buffer, "Hello,world.\r\n");
//			HAL_UART_Transmit(&huart1,(unsigned char *)tx_buffer,
//				strlen((char *)tx_buffer), 50);	
//		}
	}
}


//************************************* IC中断回调
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM2)
	{	
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)		// CH1计频率
		{
			PWM_T_Count =  HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_1)+1;			// CCR1 -> PWM_T_Count
			Freq = 1000000.0/(float)PWM_T_Count;
			Duty = (float)PWM_D_Count/(float)PWM_T_Count*100;
			
		}
		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)		// CH2计占空比
		{
			PWM_D_Count =  HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_2)+1;		// CCR2 -> PWM_D_Count
		}			
	}	
}

// 假设 PWM_D_Count 是 2，PWM_T_Count 是 5。
//如果这两个值都是整数类型，那么执行 PWM_D_Count / PWM_T_Count 将会得到 0，
//因为它们都是整数，结果将会被截断为整数。但实际上占空比应该是 2/5，即 0.4。


//************************************* 方波输出回调函数
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM15)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{			
			 __HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_1,(__HAL_TIM_GetCounter(htim)+100));	//5Khz
		}
	}
}




/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

