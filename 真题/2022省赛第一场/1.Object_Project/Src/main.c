#include "main.h"
#include "tim.h"
#include "uart.h"
#include "key_led.h"
#include "rcc.h"
#include "lcd.h"


/*************** 定义区 ***************/
#define PSD		1
#define STA		2
#define Reset	100



/*************** 变量声明区 ***************/
//*************************用户自定义
uint8_t Screen =PSD;
uint8_t B1 =Reset;
uint8_t B2 =Reset;
uint8_t B3 =Reset;
uint8_t PASSWORD =123;
uint8_t Password_Input;
uint8_t Error_count =0;
uint32_t Freq_Count;
uint32_t Duty_Count;
float Freq;
float Duty;

//*************************tim

//*************************key_led
uint8_t Key_Value;
uint8_t ucled;
__IO uint32_t uwTick_Led_Point =0;
__IO uint32_t uwTick_Led_Shan_Point =0;


//*************************lcd
unsigned char Lcd_Show_String[21];

//*************************uart
uint8_t rxbuffer[7];


/*************** 子函数声明区 ***************/
void Key_Proc(void);
void Lcd_Proc(void);
uint8_t Led_Timer(void);
void Led_Proc(void);
uint8_t Led_ShanTimer(void);
void Uart_Proc(void);

/*************** 主函数 ***************/

int main(void)
{
//************************* 初始化
	HAL_Init();
	SystemClock_Config();
	Key_led_Init();
	UART_Init();
	TIM2_Init();
	TIM3_Init();
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
//************************* 外设基本配置
	HAL_TIM_Base_Start(&htim2);
//	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
	HAL_UART_Receive_IT(&huart1, rxbuffer, 7);
	
//************************* 主循环
  while (1)
  {
	  Key_Proc();
	  Lcd_Proc();
	  Led_Proc();
	  Uart_Proc();
  }
}




/*************** 子函数 ***************/




void Key_Proc(void)
{
	Key_Value =Key_Scan();
	
	
	if(Key_Value ==1)
	{
		if(Screen ==PSD && B1 ==Reset)
		{
			B1 =0;
		}	
		else
		{
			B1++;
			if(B1 ==10)
			{
				B1 =0;
			}
		}
	}
	
	if(Key_Value ==2)
	{
		if(Screen ==PSD && B2 ==Reset)
		{
			B2 =0;
		}	
		else
		{
			B2++;
			if(B2 ==10)
			{
				B2 =0;
			}
		}
	}
	
	if(Key_Value ==3)
	{
		if(Screen ==PSD && B3 ==Reset)
		{
			B3 =0;
		}	
		else
		{
			B3++;
			if(B3 ==10)
			{
				B3 =0;
			}
		}
	}
	
	if(Key_Value ==4)
	{
		Password_Input =B1*100+B2*10+B3;
		if(Screen ==PSD && Password_Input ==PASSWORD)
		{
			Screen =STA;
			LCD_Clear(Black);
			Error_count =0;
			uwTick_Led_Point =uwTick;
		}	
		else
		{
			B1 =Reset;
			B2 =Reset;
			B3 =Reset;
			Error_count ++;
			if(Error_count >=3)
			{
				uwTick_Led_Point =uwTick;
			}
			
		}
	}
	
	Key_Value =0;
}


uint8_t Led_Timer(void)
{
	if(uwTick -uwTick_Led_Point <5000)			return 0;
	else if(uwTick -uwTick_Led_Point >=5000)		return 1;

}

uint8_t Led_ShanTimer(void)
{
	if(uwTick -uwTick_Led_Shan_Point <100)			return 0;
	else if(uwTick -uwTick_Led_Shan_Point >=100)		return 1;

}

void Led_Proc(void)
{
	if(Password_Input ==PASSWORD)
	{
		ucled |=0x01;
		__HAL_TIM_SetAutoreload(&htim2, 499);		// 2kHZ
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 50);	// 10%
		if(Led_Timer() ==1)
		{
			ucled =0x00;
			__HAL_TIM_SetAutoreload(&htim2, 999);		// 1kHZ
			__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 500);	// 50%
			Screen =PSD;
			B1 =Reset;
			B2 =Reset;
			B3 =Reset;
			LCD_Clear(Black);
			Password_Input =0;
		}
		
	}
	
	if(Error_count >=3 && Led_Timer() !=1)
	{
		if(Led_ShanTimer() ==1)
		{
			ucled ^=0x02;
			uwTick_Led_Shan_Point =uwTick;
		}
	}
	
	
	
	Led_Disp(ucled);
}




void Lcd_Proc(void)
{
	if(Screen ==PSD)
	{
		sprintf((char*)Lcd_Show_String, "       PSD");
		LCD_DisplayStringLine(Line1, Lcd_Show_String);
		
		if(B1 ==Reset)
		{
			sprintf((char*)Lcd_Show_String, "    B1:@");
			LCD_DisplayStringLine(Line3, Lcd_Show_String);
		}
		else
		{
			sprintf((char*)Lcd_Show_String, "    B1:%d", B1);
			LCD_DisplayStringLine(Line3, Lcd_Show_String);
		}
		
		if(B2 ==Reset)
		{
			sprintf((char*)Lcd_Show_String, "    B2:@");
			LCD_DisplayStringLine(Line4, Lcd_Show_String);
		}
		else
		{
			sprintf((char*)Lcd_Show_String, "    B2:%d", B2);
			LCD_DisplayStringLine(Line4, Lcd_Show_String);
		}
		
		if(B3 ==Reset)
		{
			sprintf((char*)Lcd_Show_String, "    B3:@");
			LCD_DisplayStringLine(Line5, Lcd_Show_String);
		}
		else
		{
			sprintf((char*)Lcd_Show_String, "    B3:%d", B3);
			LCD_DisplayStringLine(Line5, Lcd_Show_String);
		}	
	}
	
	if(Screen ==STA)
	{
		sprintf((char*)Lcd_Show_String, "       STA");
		LCD_DisplayStringLine(Line1, Lcd_Show_String);
		
		sprintf((char*)Lcd_Show_String, "    F:%dHz",(unsigned int)Freq);
		LCD_DisplayStringLine(Line3, Lcd_Show_String);
		
		sprintf((char*)Lcd_Show_String, "    D:%d%%",(unsigned int)Duty);
		LCD_DisplayStringLine(Line4, Lcd_Show_String);
	}
	
}


void Uart_Proc(void)
{
	if((rxbuffer[0]-'0')*100+(rxbuffer[1]-'0')*10+(rxbuffer[2]-'0') ==PASSWORD  && rxbuffer[3] =='-')	// 密码正确
	{
		PASSWORD =(rxbuffer[4]-'0')*100+(rxbuffer[5]-'0')*10+(rxbuffer[6]-'0');
		rxbuffer[0] =rxbuffer[1]=rxbuffer[2]=rxbuffer[3]=rxbuffer[4]
			=rxbuffer[5]=rxbuffer[6]=NULL;
	}

}




/*************** 中断函数 ***************/

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


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	HAL_UART_Receive_IT(&huart1, rxbuffer, 7);
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
