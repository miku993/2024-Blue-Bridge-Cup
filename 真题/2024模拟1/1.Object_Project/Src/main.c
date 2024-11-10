#include "main.h"
#include "adc.h"
#include "uart.h"
#include "key_led.h"
#include "lcd.h"
#include "rcc.h"

/************** 定义区 **************/
#define S 	1
#define L 	2
#define R 	3
#define DATA 	4
#define WARN 	5
#define Yes 	6
#define No 		7

/************** 子函数声明区 **************/
void Key_Proc(void);
void Lcd_Proc(void);
void Led_Proc(void);
void Uart_Proc(void);
void ADC_Proc(void);
uint8_t Led_Timer(void);

/************** 变量声明区 **************/
//*********************** 用户自定义变量
uint8_t Mode =DATA;
uint8_t Way =S;
float Distance;
uint8_t Wait_5s =No;
uint8_t Changing =No;

//*********************** uart专属变量
unsigned char rxbuffer[1]={0};
unsigned char txbuffer[1];

//*********************** key_led专属变量
uint8_t Key_Value;
__IO uint32_t uwTick_Key_Change_Point=0;
__IO uint32_t uwTick_Led_Point=0;
uint8_t ucled =0x00;
uint8_t ShanShuo =No;

//*********************** lcd专属变量
unsigned char Lcd_Show_Strings[21];

//*********************** adc专属变量
float Voltage;


/************** 主函数 **************/
int main(void)
{
//************************ 初始化
	HAL_Init();
	SystemClock_Config();
	Key_led_Init();
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	UART_Init();
	ADC2_Init();
	
//************************ 基本外设配置
	HAL_UART_Receive_IT(&huart1, rxbuffer, 1);
	
//************************ 主循环
  while (1)
  {
	  Key_Proc();
	  Lcd_Proc();
	  Uart_Proc();
	  ADC_Proc();
	  Led_Proc();
  }
}



/************** 子函数 **************/

void Key_Proc(void)
{
	Key_Value =Key_Scan();
	
	if(Key_Value ==1 && Mode ==WARN)
	{
		Mode =DATA;
		Way =S;
		sprintf((char *)txbuffer, "Success\r\n");
		HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 5);
	}
	
	else if(Key_Value ==3 && Changing ==No && Mode ==DATA)
	{
		sprintf((char *)txbuffer, "Warn\r\n");
		HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 5);
	}
	
	else if(Key_Value ==4 && Changing ==No && Mode ==DATA)
	{
		sprintf((char *)txbuffer, "Warn\r\n");
		HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 5);
	}
	
	else if(Mode ==DATA)
	{
		if(Way ==L)
		{
			if(Wait_5s ==No)		// 首次进入
			{
				uwTick_Key_Change_Point =uwTick;
				Wait_5s =Yes;
			}
			
			else if(Key_Value ==3)	// 左转键按下
			{
				Way =S;		
				Changing =No;
				Wait_5s =No;
				sprintf((char *)txbuffer, "Success\r\n");
				HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 5);
			}
			else if(uwTick -uwTick_Key_Change_Point >=5000)		// 5s已到
			{
				Mode =WARN;
				Way =S;
				Wait_5s =No;
				Changing =No;
				sprintf((char *)txbuffer, "Warn\r\n");
				HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 5);
				LCD_Clear(Black);
			}
		}
		
		if(Way ==R)
		{
			if(Wait_5s ==No)		// 首次进入
			{
				Wait_5s =Yes;
				uwTick_Key_Change_Point =uwTick;
			}
			
			else if(Key_Value ==4)	// 右转键按下
			{
				Way =S;			
				Changing =No;
				Wait_5s =No;
				sprintf((char *)txbuffer, "Success\r\n");
				HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 5);
			}
	
			else if(uwTick -uwTick_Key_Change_Point >=5000)		// 5s已到
			{
				Mode =WARN;
				Way =S;
				Wait_5s =No;
				Changing =No;
				sprintf((char *)txbuffer, "Warn\r\n");
				HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 5);
				LCD_Clear(Black);
			}
				
		}
	}
	
	Key_Value =0;
}


uint8_t Led_Timer(void)
{
	if(uwTick -uwTick_Led_Point <100)	return 0;
	uwTick_Led_Point =uwTick;		return 1;
		
}


void Led_Proc(void)
{
	
	if(Way ==L)
	{
		if(Led_Timer() ==1)
		{
			ucled &= (~0xFE);
			ucled ^=0x01;
		}
	}
		
	else if(Way ==R)
	{
		if(Led_Timer() ==1)
		{
			ucled &= (~0xFD);
			ucled ^=0x02;
		}
	}
		
	else if(Mode ==WARN)
	{
		ucled =0x00;
		ucled |=0x80;
	}

	else
	{
		ucled =0x00;
	}
		
	
	Led_Disp(ucled);
	
}


void Lcd_Proc(void)
{
	if(Mode ==DATA) 
	{
		sprintf((char *)Lcd_Show_Strings, "        DATA");
		LCD_DisplayStringLine(Line1, Lcd_Show_Strings);
		
		if(Way ==S)
		{
			sprintf((char *)Lcd_Show_Strings, "       N:S");
			LCD_DisplayStringLine(Line3, Lcd_Show_Strings);
		}
		
		else if(Way ==L)
		{
			sprintf((char *)Lcd_Show_Strings, "       N:L");
			LCD_DisplayStringLine(Line3, Lcd_Show_Strings);
		}
		
		else if(Way ==R)
		{
			sprintf((char *)Lcd_Show_Strings, "       N:R");
			LCD_DisplayStringLine(Line3, Lcd_Show_Strings);
		}
		
		
		sprintf((char *)Lcd_Show_Strings, "       D:%4.1f", Distance);
		LCD_DisplayStringLine(Line4, Lcd_Show_Strings);
	}		
	
	if(Mode ==WARN) 
	{
		sprintf((char *)Lcd_Show_Strings, "        WARN");
		LCD_DisplayStringLine(Line4, Lcd_Show_Strings);
	}
}


void Uart_Proc(void)
{
	if(Mode ==DATA)
	{
		if(*rxbuffer == 'L' && Changing ==No && Mode !=WARN)		// 非正在变换状态下
		{
			Way =L;
			Changing =Yes;
			*rxbuffer =NULL;
		}
		
		else if(*rxbuffer == 'R' && Changing ==No && Mode !=WARN)
		{
			Way =R;
			Changing =Yes;
			*rxbuffer =NULL;
		}
		
		else if(*rxbuffer == 'L' && (Changing ==Yes || Mode ==WARN))		// 正在切换
		{
			*rxbuffer =NULL;
		}
		
		else if(*rxbuffer == 'R' && (Changing ==Yes || Mode ==WARN))
		{
			*rxbuffer =NULL;
		}
		
		else if(*rxbuffer != NULL && *rxbuffer != 'L' && *rxbuffer != 'R')
		{
			sprintf((char *)txbuffer, "ERROR\r\n");
			HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 5);
			*rxbuffer =NULL;
		}

	}
	
	if(Mode ==WARN)
	{
		if(*rxbuffer !=NULL)
		{
			*rxbuffer =NULL;
			sprintf((char *)txbuffer, "WAIT\r\n");
			HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 5);
		}
	}
	
}

void ADC_Proc(void)
{
	Voltage= (float)ADC_Get_Num()/4096.0*3.3;
	
	if(Voltage >=0 && Voltage <=3)
	{
		Distance =100*Voltage;
	}
	
	else
		Distance =300;
}


/************** 中断函数 **************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart1, rxbuffer, 1);
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

