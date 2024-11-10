#include "main.h"
#include "adc.h"
#include "tim.h"
#include "uart.h"
#include "key_led.h"
#include "lcd.h"
#include "rcc.h"

/************* 定义区 **************/
#define Auto	1
#define Manu	2
#define SLEEP	3
#define DATA	4
#define Yes		5
#define No		6

/************* 子函数声明区 **************/
void Key_Proc(void);
void Uart_Proc(void);
void Lcd_Proc(void);
void Led_Proc(void);
uint8_t Key_Timer(void);
uint8_t Led_Timer(void);

/************* 变量声明区 **************/
//*********************** 用户自定义
uint8_t Mode =Auto;
uint8_t Screen =DATA;
float TEMP;
uint8_t GEAR;
uint8_t Uart_Receive =No;
uint16_t Freq_Count;
uint16_t Duty_Count;
float Freq;
float Duty;

//*********************** Key_led
uint8_t Key_Value;
__IO uint32_t uwTick_Key_Point =0;
__IO uint32_t uwTick_Led_Point =0;
uint8_t ucled;

//*********************** lcd
unsigned char LCD_Show_Strings[21];

//*********************** uart
unsigned char rxbuffer[2];
unsigned char txbuffer[10];

//*********************** tim



/************* 主函数 **************/
int main(void)
{
//*********************** 初始化	
	HAL_Init();
	SystemClock_Config();
	Key_led_Init();
	ADC2_Init();
	UART_Init();
	TIM2_Init();
	TIM3_Init();
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
//*********************** 外设基本配置
	HAL_UART_Receive_IT(&huart1, rxbuffer, 2);
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);

//*********************** 主循环	
  while (1)
  {
	  Key_Proc();
	  Lcd_Proc();
	  Uart_Proc();
	  Led_Proc();
  }
}



/************* 子函数 **************/
uint8_t Key_Timer(void)
{
	if(uwTick -uwTick_Key_Point <5000)	return 0;
	else 	return 1;	
}




void Key_Proc(void)
{
	Key_Value =Key_Scan();
	if(Key_Timer() ==1 && Screen ==DATA)
	{
		Screen =SLEEP;
		LCD_Clear(Black);
	}
	
	if(Key_Value ==1)
	{
		if(Screen ==DATA && Mode ==Auto)
		{
			Mode =Manu;
		}
		
		else if(Screen ==DATA && Mode ==Manu)
		{
			Mode =Auto;
		}
		
		else if(Screen ==SLEEP)
		{
			Screen =DATA;
			LCD_Clear(Black);
		}
		
		uwTick_Key_Point =uwTick;
	}
	
	if(Key_Value ==2)
	{
		if(Screen ==DATA && Mode ==Manu)
		{
			GEAR ++;
			if(GEAR ==4)
				GEAR =1;
		}
		
		else if(Screen ==SLEEP)
		{
			Screen =DATA;
			LCD_Clear(Black);
		}
		
		uwTick_Key_Point =uwTick;
	}
	
	if(Key_Value ==3)
	{
		if(Screen ==DATA && Mode ==Manu)
		{
			GEAR --;
			if(GEAR ==0)
				GEAR =3;
		}
		
		else if(Screen ==SLEEP)
		{
			Screen =DATA;
			LCD_Clear(Black);
		}
		
		uwTick_Key_Point =uwTick;
	}
	
	Key_Value =0;
}


uint8_t Led_Timer(void)
{
	if(uwTick -uwTick_Key_Point <3000)	return 0;
	else 	return 1;	
}


void Led_Proc(void)
{
	if(Mode ==Auto)
	{
		ucled |=0x80;
	}
	
	if(Mode ==Manu)
	{
		ucled &=(~0x80);
	}
	
	if(GEAR ==1)
	{
		ucled &=(~0x06);
		ucled |=0x01;
	}
	
	if(GEAR ==2)
	{
		ucled &=(~0x05);
		ucled |=0x02;
	}
	
	if(GEAR ==3)
	{
		ucled &=(~0x03);
		ucled |=0x04;
	}
	
	if(Uart_Receive ==Yes)
	{
		ucled |=0x08;
		if(Led_Timer() ==1)
		{
			ucled &=(~0x08);
		}
	}
	
	LED_Disp(ucled);
}



void Uart_Proc(void)
{
	if(rxbuffer[0] =='B' && rxbuffer[1] =='1')
	{
		rxbuffer[0] =NULL;
		rxbuffer[1] =NULL;
		uwTick_Led_Point =uwTick;
		Uart_Receive =Yes;
		
		if(Screen ==DATA && Mode ==Auto)
		{
			Mode =Manu;
		}
		
		else if(Screen ==DATA && Mode ==Manu)
		{
			Mode =Auto;
		}
		
		else if(Screen ==SLEEP)
		{
			Screen =DATA;
			LCD_Clear(Black);
		}
		
		uwTick_Key_Point =uwTick;
		
	}
	
	else if(rxbuffer[0] =='B' && rxbuffer[1] =='2')
	{
		rxbuffer[0] =NULL;
		rxbuffer[1] =NULL;
		uwTick_Led_Point =uwTick;
		Uart_Receive =Yes;
		
		if(Screen ==DATA && Mode ==Manu)
		{
			GEAR ++;
			if(GEAR ==4)
				GEAR =1;
		}
		
		else if(Screen ==SLEEP)
		{
			Screen =DATA;
			LCD_Clear(Black);
		}
		
		uwTick_Key_Point =uwTick;
	}
	
	else if(rxbuffer[0] =='B' && rxbuffer[1] =='3')
	{
		rxbuffer[0] =NULL;
		rxbuffer[1] =NULL;
		uwTick_Led_Point =uwTick;
		Uart_Receive =Yes;
		
		if(Screen ==DATA && Mode ==Manu)
		{
			GEAR --;
			if(GEAR ==0)
				GEAR =3;
		}
		
		else if(Screen ==SLEEP)
		{
			Screen =DATA;
			LCD_Clear(Black);
		}
		
		uwTick_Key_Point =uwTick;
	}
	
	else if(rxbuffer[0] !=NULL && rxbuffer[1] !=NULL && (rxbuffer[0] !='B' || rxbuffer[1] !='1' || rxbuffer[1] !='2' || rxbuffer[1] !='3'))
	{
		rxbuffer[0] =NULL;
		rxbuffer[1] =NULL;
		
		sprintf((char *)txbuffer, "NULL");
		HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 50);
	}
}




void Lcd_Proc(void)
{
	TEMP =(((float)ADC2_Get_Num()/4096*3.3)+1)/0.1;
			
	if(Mode ==Auto)
	{
		if(TEMP <25)
		{
			GEAR =1;
		}
		if(TEMP <=30 && TEMP >=25)
		{
			GEAR =2;
		}
		if(TEMP >30)
		{
			GEAR =3;
		}	
	}
	
	if(GEAR ==1)
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 50);		//10%
	if(GEAR ==2)
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 200);		//40%
	if(GEAR ==3)
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 400);		//80%
	
	if(Screen ==DATA)
	{
		if(Mode ==Auto)
		{
			
			sprintf((char *)LCD_Show_Strings, "        DATA");
			LCD_DisplayStringLine(Line1, LCD_Show_Strings);
			
			sprintf((char *)LCD_Show_Strings, "     TEMP:%.1f", TEMP);
			LCD_DisplayStringLine(Line3, LCD_Show_Strings);
			
			sprintf((char *)LCD_Show_Strings, "     MODE:Auto");
			LCD_DisplayStringLine(Line4, LCD_Show_Strings);
			
			sprintf((char *)LCD_Show_Strings, "     GEAR:%d", GEAR);
			LCD_DisplayStringLine(Line5, LCD_Show_Strings);
		}
		
		
		else if(Mode ==Manu)
		{
			sprintf((char *)LCD_Show_Strings, "        DATA");
			LCD_DisplayStringLine(Line1, LCD_Show_Strings);
			
			sprintf((char *)LCD_Show_Strings, "     TEMP:%.1f", TEMP);
			LCD_DisplayStringLine(Line3, LCD_Show_Strings);
			
			sprintf((char *)LCD_Show_Strings, "     MODE:Manu");
			LCD_DisplayStringLine(Line4, LCD_Show_Strings);
			
			sprintf((char *)LCD_Show_Strings, "     GEAR:%d", GEAR);
			LCD_DisplayStringLine(Line5, LCD_Show_Strings);
		}
		
	}

	
	else if(Screen ==SLEEP)
	{
		sprintf((char *)LCD_Show_Strings, "     SLEEPING");
		LCD_DisplayStringLine(Line4, LCD_Show_Strings);
		
		sprintf((char *)LCD_Show_Strings, "     TEMP:%.1f", TEMP);
		LCD_DisplayStringLine(Line5, LCD_Show_Strings);
		
		sprintf((char *)LCD_Show_Strings, " %.1fHZ,%.1f%%",Freq, Duty);
		LCD_DisplayStringLine(Line6, LCD_Show_Strings);
		sprintf((char *)LCD_Show_Strings, " %dHZ,%d%%",(unsigned int)Freq, (unsigned int)Duty);
		LCD_DisplayStringLine(Line7, LCD_Show_Strings);
	}
}





/************* 中断函数 **************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart1, rxbuffer, 2);
}



void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim ->Instance ==TIM3)
	{
		if(htim ->Channel ==HAL_TIM_ACTIVE_CHANNEL_2)
		{
			Freq_Count =HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_2)+1;
			Freq =10000.0/(float)Freq_Count;
			Duty =((float)Duty_Count/(float)Freq_Count)*100;
		}
		
		if(htim ->Channel ==HAL_TIM_ACTIVE_CHANNEL_1)
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
