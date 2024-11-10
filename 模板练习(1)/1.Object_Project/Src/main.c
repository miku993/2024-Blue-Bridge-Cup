#include "main.h"
#include "adc.h"
#include "rtc.h"
#include "tim.h"
#include "uart.h"
#include "i2c.h"
#include "lcd.h"
#include "key_led.h"
#include "rcc.h"

/************* 定义区 *************/

/************* 函数声明区 *************/
void Key_Proc(void);
void Uart_Proc(void);
uint16_t ADC1_Get_Num(void);
void Lcd_Proc(void);
void iic_EEPROM_Test(void);
void iic_4017_Test(void);

/************* 变量声明区 *************/

//************************* Key_led专属变量
__IO uint32_t uwTick_Key_Point =0;
uint8_t Key_Value =0;
uint8_t ucled;

//************************* tim专属变量
float Freq;
float Duty;
uint16_t Duty_Count;
uint16_t Freq_Count;

//************************* lcd专属变量
__IO uint32_t uwTick_Lcd_Point =0; 
unsigned char Lcd_Show_Strings[21];

//************************* iic专属变量
uint8_t EEPROM_Strings_0[4] ={1, 2, 3, 4};
uint8_t EEPROM_Strings_1[4] ={0};
uint8_t MSP4017;

//************************* rtc专属变量
RTC_TimeTypeDef H_M_S_Time;
RTC_DateTypeDef Y_M_D_Date;

//************************* uart专属变量
__IO uint32_t uwTick_Uart_Point =0;
unsigned char txbuff[4];
unsigned char rxbuff[4];


/************* 主函数 *************/
int main(void)
{
//************************* 初始化配置
	HAL_Init();
	SystemClock_Config();
	Key_led_Init();
	UART_Init();
	ADC1_Init();
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	I2CInit();
	TIM2_Init();
	TIM3_Init();
	TIM15_Init();
	TIM6_Init();
	RTC_Init();

//************************* 外设基本配置
	UART_Start_Receive_IT(&huart1, rxbuff, 4);
	iic_EEPROM_Test();
	iic_4017_Test();
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start(&htim15);
	HAL_TIM_Base_Start(&htim6);
	
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
	HAL_TIM_OC_Start_IT(&htim15, TIM_CHANNEL_1);
	
//************************* 主循环
  while (1)
  {
	  Key_Proc();
	  //Uart_Proc();
	  Lcd_Proc();
	  
  }
}




/************* 子函数 *************/

void Key_Proc(void)
{
	if(uwTick -uwTick_Key_Point <100)  return;
		uwTick_Key_Point =uwTick;
	
	Key_Value =Key_Scan();
	
	if(Key_Value ==1)
	{
		ucled |= 0x02;
	}
	
	if(Key_Value ==2)
	{
		ucled &= (~0x02);
	}
	
	if(Key_Value ==3)
	{
		ucled ^= 0x02;
	}
	
	LED_Disp(ucled);
	Key_Value =0;
}




void Uart_Proc(void)
{
	if(uwTick -uwTick_Uart_Point <1000)  return;
		uwTick_Uart_Point =uwTick;

	sprintf((char *)txbuff, "Hello World!\r\n");
	HAL_UART_Transmit(&huart1, txbuff, strlen(txbuff), 50);

}


void Lcd_Proc(void)
{
	if(uwTick -uwTick_Lcd_Point <100)  return;
		uwTick_Lcd_Point =uwTick;
	
	sprintf((char *)Lcd_Show_Strings, "       Hello       ");
	LCD_DisplayStringLine(Line0, Lcd_Show_Strings);
	
	sprintf((char *)Lcd_Show_Strings, "       ADC:%4.1f       ",(float)ADC1_Get_Num()/4096*3.3);
	LCD_DisplayStringLine(Line1, Lcd_Show_Strings);
	
	sprintf((char *)Lcd_Show_Strings, "    EEPROM:%d,%d,%d,%d      ", EEPROM_Strings_0[0],EEPROM_Strings_0[1],EEPROM_Strings_0[2],EEPROM_Strings_0[3]);
	LCD_DisplayStringLine(Line2, Lcd_Show_Strings);
	
	sprintf((char *)Lcd_Show_Strings, "     4017:%4.1fK      ",0.7874* (float)MSP4017);
	LCD_DisplayStringLine(Line3, Lcd_Show_Strings);
	
	sprintf((char *)Lcd_Show_Strings, "     Freq:%dHz      ",(unsigned int)Freq);
	LCD_DisplayStringLine(Line5, Lcd_Show_Strings);
	
	sprintf((char *)Lcd_Show_Strings, "     Duty:%d%%      ",(unsigned int)Duty);
	LCD_DisplayStringLine(Line6, Lcd_Show_Strings);
	
	HAL_RTC_GetTime(&hrtc, &H_M_S_Time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &Y_M_D_Date, RTC_FORMAT_BIN);
	sprintf((char *)Lcd_Show_Strings, "     Time:%d:%d:%d      ",H_M_S_Time.Hours, H_M_S_Time.Minutes, H_M_S_Time.Seconds);
	LCD_DisplayStringLine(Line7, Lcd_Show_Strings);
	
	sprintf((char *)Lcd_Show_Strings, "     Date:2024-%d-%d      ",Y_M_D_Date.Month, Y_M_D_Date.Date);
	LCD_DisplayStringLine(Line8, Lcd_Show_Strings);
}


uint16_t ADC1_Get_Num(void)
{
	uint16_t adc1;
	HAL_ADC_Start(&hadc1);
	adc1 =HAL_ADC_GetValue(&hadc1);
	return adc1;
}


void iic_EEPROM_Test(void)
{
	iic24c02_Write(EEPROM_Strings_0, 10, 5);
	HAL_Delay(10);
	iic24c02_Read(EEPROM_Strings_1, 10, 5);
}

void iic_4017_Test(void)
{
	msp4017_Write(0x01);
	HAL_Delay(10);
	MSP4017 =msp4017_Read();
}
	
/************* 中断函数 *************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart1, rxbuff, 1);
	 
	if(*rxbuff == '?')
	{
		*rxbuff ='0';
		sprintf((char *)txbuff, "Got it!");
		HAL_UART_Transmit(&huart1, txbuff, strlen(txbuff), 50);
	}
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim ->Instance == TIM3)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			Freq_Count =HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_2)+1;
			Freq =1000000.0/(float)Freq_Count;
			Duty =((float)Duty_Count/(float)Freq_Count)*100;
		}
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			Duty_Count =HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1)+1;
		}
	}
}


void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance ==TIM15)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			__HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1,(__HAL_TIM_GetCounter(htim)+200) );
		}
	}
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim ->Instance ==TIM6)
	{
		sprintf((char *)txbuff, "Yes!");
		HAL_UART_Transmit(&huart1, txbuff, strlen(txbuff), 50);
	}
}



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
