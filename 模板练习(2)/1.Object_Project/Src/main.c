#include "main.h"
#include "adc.h"
#include "rtc.h"
#include "tim.h"
#include "uart.h"
#include "key_led.h"
#include "rcc.h"
#include "lcd.h"
#include "i2c.h"

/********** 函数声明区 ***********/
void Key_Proc(void);
void Lcd_Proc(void);
void Uart_Proc(void);
void EEPROM_Test(void);
void MSP4017_Test(void);


/********** 变量声明区 ***********/
//************************** key_led专属变量
__IO uint32_t uwTick_Key_Point =0;
uint8_t Key_Value;
uint8_t ucled;

//************************** lcd专属变量
unsigned char Lcd_Show_Strings[21];

//************************** adc专属变量

//************************** i2c专属变量
uint16_t EEPROM_0[4] ={0x10, 0x11, 0x12, 0x13};
uint16_t EEPROM_1[4] ={0};
uint16_t MSP4017;

//************************** uart专属变量
unsigned char rxbufferr[4];
unsigned char txbufferr[4];
uint8_t ucNum =3;

//************************** rtc专属变量
RTC_TimeTypeDef Time;
RTC_DateTypeDef Date;


//************************** tim专属变量
uint16_t Freq_Count;
uint16_t Duty_Count;
float Freq;
float Duty;

/********** 主函数 ***********/
int main(void)
{
//************************** 初始化配置
	HAL_Init();
	SystemClock_Config();
	Key_led_Init();
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	ADC2_Init();
	UART_Init();
	I2CInit();
	TIM2_Init();
	TIM3_Init();
	TIM6_Init();
	TIM15_Init();
	RTC_Init();

//************************** 基本外设配置
	//HAL_UART_Receive_IT(&huart1, rxbufferr, 4);
	EEPROM_Test();
	MSP4017_Test();
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start(&htim6);
	HAL_TIM_Base_Start(&htim15);
	
	HAL_TIM_Base_Start_IT(&htim6);
//	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
//	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	//HAL_TIM_OC_Start_IT(&htim15, TIM_CHANNEL_1);
	

//************************** 主循环
  while (1)
  {
	  Key_Proc();
	  Lcd_Proc();
	  Uart_Proc();
  }
}

uint8_t Key_Timer(void)
{
	if(uwTick -uwTick_Key_Point <100)	return 0;
	uwTick_Key_Point =uwTick;	return 1;
}


/********** 子函数 ***********/
void Key_Proc(void)
{
	Key_Value =Key_Scan();
	
	if(Key_Value ==1)
	{
		ucled |=0x01;
	}
	
	if(Key_Value ==2)
	{
		ucled &=(~0x01);
	}
	if(Key_Value ==3)
	{
		if(Key_Timer() ==1)
			ucled ^=0x01;
	}
	
	Key_Value =0;
	Led_Disp(ucled);
}


void Lcd_Proc(void)
{
	sprintf((char *)Lcd_Show_Strings, "        DATA");
	LCD_DisplayStringLine(Line0,Lcd_Show_Strings );
	
	sprintf((char *)Lcd_Show_Strings, "       R37:%4.1f", (float)ADC2_Get_Num()/4096.0*3.3);
	LCD_DisplayStringLine(Line1,Lcd_Show_Strings );
	
	sprintf((char *)Lcd_Show_Strings, "  EEPROM:%d,%x,%x,%x", EEPROM_0[0], EEPROM_0[1], EEPROM_0[2], EEPROM_0[3] );
	LCD_DisplayStringLine(Line2,Lcd_Show_Strings );
	
	sprintf((char *)Lcd_Show_Strings, "  MSP4017:%4.1fK", (float)MSP4017*0.7874);
	LCD_DisplayStringLine(Line3,Lcd_Show_Strings );
	
	sprintf((char *)Lcd_Show_Strings, "  Duty:%.1f%%", Duty);
	LCD_DisplayStringLine(Line4,Lcd_Show_Strings );
	
	sprintf((char *)Lcd_Show_Strings, "  Freq:%.1fHZ", Freq);
	LCD_DisplayStringLine(Line5,Lcd_Show_Strings );
	
	HAL_RTC_GetTime(&hrtc, &Time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &Date, RTC_FORMAT_BIN);
	
	sprintf((char *)Lcd_Show_Strings, "  Time:%d-%d-%d", Time.Hours, Time.Minutes, Time.Seconds);
	LCD_DisplayStringLine(Line6,Lcd_Show_Strings );
	
	sprintf((char *)Lcd_Show_Strings, "  Date:2024-%d-%d", Date.Month, Date.Date);
	LCD_DisplayStringLine(Line7,Lcd_Show_Strings );
}


void Uart_Proc(void)
{
	if(rxbufferr[0] =='Y' && rxbufferr[1] =='e' && rxbufferr[2] =='s')
	{
		rxbufferr[0]=NULL;
		rxbufferr[1]=NULL;
		rxbufferr[2]=NULL;
		
		sprintf((char *)txbufferr, "Yes\r\n");
		HAL_UART_Transmit(&huart1, txbufferr, 3, 10);
	}
}


void EEPROM_Test(void)
{
	iic24c02_Write(EEPROM_0, 3, 4);
	HAL_Delay(10);
	iic24c02_Read(EEPROM_1, 3, 4);
}

void MSP4017_Test(void)
{
	msp4017_Write(0x7F);
	HAL_Delay(10);
	MSP4017 =msp4017_Read();
}

/********** 中断函数 ***********/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart1, rxbufferr, 4);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance ==TIM6)
	{
		sprintf((char *)txbufferr, "Hi\r\n");
		HAL_UART_Transmit(&huart1, txbufferr, strlen(txbufferr), 50);
	}
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance ==TIM15)
	{
		if(htim->Channel ==HAL_TIM_ACTIVE_CHANNEL_1)
		{
			__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, __HAL_TIM_GetCounter(htim));
		}
	}
}




void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance ==TIM3)
	{
		if(htim->Channel ==HAL_TIM_ACTIVE_CHANNEL_2)
		{
			Freq_Count =HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_2)+1;
			Freq =1000000.0/(float)Freq_Count;
			Duty =((float)Duty_Count/(float)Freq_Count)*100;
		}
		
		if(htim->Channel ==HAL_TIM_ACTIVE_CHANNEL_1)
		{
			Duty_Count =HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1)+1;
		}
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

