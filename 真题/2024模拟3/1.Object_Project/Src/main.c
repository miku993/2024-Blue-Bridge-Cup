#include "main.h"
#include "adc.h"
#include "uart.h"
#include "key_led.h"
#include "rcc.h"
#include "lcd.h"

/*********** 定义区 ***********/
#define GOODS	1
#define STANDARD 	2
#define PASS  	3
#define Yes  	4
#define No  	5
#define Reset  	6
#define R37_H  	7
#define R37_L 	8
#define R38_H 	9
#define R38_L 	10

/*********** 函数声明区 ***********/
void Key_Proc(void);
void Lcd_Proc(void);
uint8_t LED_Timer_1(void);
uint8_t LED_Timer_2(void);
void Led_Proc(void);
void Uart_Proc(void);

/*********** 变量声明区 ***********/
//*********************** 用户自定义
uint8_t Mode =GOODS;
uint8_t R37_Check_Num =0;
uint8_t R38_Check_Num =0;
float R37_Low =1.2;
float R37_High =2.2;
float R38_Low =1.4;
float R38_High =3.0;
uint8_t Choose =R37_H;
float R37_PAAS_OUT =0;
float R38_PAAS_OUT =0;
uint8_t R37_Pass_Num =0;
uint8_t R38_Pass_Num =0;
uint8_t R37_PASSING =No;
uint8_t R38_PASSING =No;
uint8_t Clear =Yes;

//*********************** Key_led专属变量
uint8_t Key_Value;
uint8_t ucled;
__IO uint32_t uwTick_Led1_Point =0;
__IO uint32_t uwTick_Led2_Point =0;

//*********************** adc专属变量

//*********************** lcd专属变量
unsigned char LCD_Show_Strings[21];

//*********************** uart专属变量
unsigned char rxbuffer[4];
unsigned char txbuffer[50];


/*********** 主函数 ***********/
int main(void)
{
//*********************** 初始化
	HAL_Init();
	SystemClock_Config();
	Key_led_Init();
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	ADC1_Init();
	ADC2_Init();
	UART_Init();
	
//*********************** 外设基本配置
	HAL_UART_Receive_IT(&huart1, rxbuffer, 3);
	
//*********************** 主循环
  while (1)
  {
	  Key_Proc();
	  Lcd_Proc();
	  Led_Proc();
	  Uart_Proc();
  }
}





/*********** 子函数 ***********/

void Key_Proc(void)
{
	R37_PASSING =Reset;
	R38_PASSING =Reset;
	Key_Value =Key_Scan();
	
	if(Key_Value ==1)
	{
		Mode ++;
		if(Mode ==4)
		{
			Mode =GOODS;
		}
		LCD_Clear(Black);
		
	}
	
	if(Key_Value ==2)
	{
		if(Mode ==GOODS)
		{
			R37_Check_Num +=1;
			Clear =No;
			if( (((float)ADC2_Get_Num()/4096*3.3) >=R37_Low) && (((float)ADC2_Get_Num()/4096*3.3) <=R37_High))
			{
				R37_Pass_Num +=1;
				R37_PASSING =Yes;
			}
		}
		
		if(Mode ==STANDARD)
		{
			Choose +=1;
			if(Choose ==11)
			{
				Choose =R37_H;
			}
		}
	}
	
	if(Key_Value ==3)
	{
		if(Mode ==GOODS)
		{
			R38_Check_Num +=1;
			Clear =No;
			if( (((float)ADC1_Get_Num()/4096*3.3) >=R38_Low) && (((float)ADC1_Get_Num()/4096*3.3) <=R38_High))
			{
				R38_Pass_Num +=1;
				R38_PASSING =Yes;
			}
		}
		
		if(Mode ==STANDARD)
		{
			if(Choose ==R37_H)
			{
				R37_High +=0.2;
				if(R37_High >=3.2)
				{
					R37_High =2.2;
				}
			}
			else if(Choose ==R37_L)
			{
				R37_Low +=0.2;
				if(R37_Low >=2.2)
				{
					R37_Low =1.2;
				}
			}
			
			else if(Choose ==R38_H)
			{
				R38_High +=0.2;
				if(R38_High >=3.2)
				{
					R38_High =2.2;
				}
			}
			else if(Choose ==R38_L)
			{
				R38_Low +=0.2;
				if(R38_Low >=2.2)
				{
					R38_Low =1.2;
				}
			}
		}
	}
	
	
	if(Key_Value ==4)
	{
		if(Mode ==STANDARD)
		{
			if(Choose ==R37_H)
			{
				R37_High -=0.2;
				if(R37_High <=2.0)
				{
					R37_High =3.0;
				}
			}
			else if(Choose ==R37_L)
			{
				R37_Low -=0.2;
				if(R37_Low <=1.0)
				{
					R37_Low =2.0;
				}
			}
			else if(Choose ==R38_H)
			{
				R38_High -=0.2;
				if(R38_High <=2.0)
				{
					R38_High =3.0;
				}
			}
			else if(Choose ==R38_L)
			{
				R38_Low -=0.2;
				if(R38_Low <=1.0)
				{
					R38_Low =2.0;
				}
			}
		}
		
		if(Mode ==PASS)
		{
			LCD_Clear(Black);
			Clear =Yes;
			R37_PAAS_OUT =0;
			R38_PAAS_OUT =0;
			R37_Pass_Num =0;
			R38_Pass_Num =0;
			R37_Check_Num =0;
			R38_Check_Num =0;
		}		
		
	}
	
	
	Key_Value =0;
}



void Uart_Proc(void)
{
	if(rxbuffer[0] =='R' && rxbuffer[1] =='3' && rxbuffer[2] =='7')
	{
		rxbuffer[0] ='0';
		rxbuffer[1] ='0';
		rxbuffer[2] ='0';
		
		if(Clear ==Yes)
		{
			sprintf((char *)txbuffer, "R37:%d,%d,0.0%%\r\n", R37_Check_Num, R37_Pass_Num);
			HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 70);
		}
		else
		{
			sprintf((char *)txbuffer, "R37:%d,%d,%.1f%%\r\n", R37_Check_Num, R37_Pass_Num, R37_PAAS_OUT);
			HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 70);
		}
	}
	
	if(rxbuffer[0] =='R' && rxbuffer[1] =='3' && rxbuffer[2] =='8')
	{
		rxbuffer[0] ='0';
		rxbuffer[1] ='0';
		rxbuffer[2] ='0';
		
		if(Clear ==Yes)
		{
			sprintf((char *)txbuffer, "R38:%d,%d,0.0%%\r\n", R38_Check_Num, R38_Pass_Num);
			HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 70);
		}
		else
		{
			sprintf((char *)txbuffer, "R38:%d,%d,%.1f%%\r\n", R38_Check_Num, R38_Pass_Num, R38_PAAS_OUT);
			HAL_UART_Transmit(&huart1, txbuffer, strlen(txbuffer), 70);
		}
	}
}




void Lcd_Proc(void)
{
	
	R37_PAAS_OUT =(float)R37_Pass_Num/(float)R37_Check_Num*100;
	R38_PAAS_OUT =(float)R38_Pass_Num/(float)R38_Check_Num*100;
	
	if(Mode ==GOODS)
	{
		sprintf((char *)LCD_Show_Strings, "       GOODS");
		LCD_DisplayStringLine(Line1, LCD_Show_Strings);
		
		sprintf((char *)LCD_Show_Strings, "     R37:%4.2fV", (float)ADC2_Get_Num()/4096*3.3);
		LCD_DisplayStringLine(Line3, LCD_Show_Strings);
		
		sprintf((char *)LCD_Show_Strings, "     R38:%4.2fV", (float)ADC1_Get_Num()/4096*3.3);
		LCD_DisplayStringLine(Line4, LCD_Show_Strings);
	}
	
	if(Mode ==STANDARD)
	{
		sprintf((char *)LCD_Show_Strings, "      STANDARD");
		LCD_DisplayStringLine(Line1, LCD_Show_Strings);
	
		sprintf((char *)LCD_Show_Strings, "    SR37:%3.1f-%3.1f", R37_Low, R37_High);
		LCD_DisplayStringLine(Line3, LCD_Show_Strings);
		
		sprintf((char *)LCD_Show_Strings, "    SR38:%3.1f-%3.1f", R38_Low, R38_High);
		LCD_DisplayStringLine(Line4, LCD_Show_Strings);
	}
	
	if(Mode ==PASS)
	{
		sprintf((char *)LCD_Show_Strings, "        PASS");
		LCD_DisplayStringLine(Line1, LCD_Show_Strings);
		
		if(Clear ==Yes)
		{
			sprintf((char *)LCD_Show_Strings, "     PR37:0.0%%");
			LCD_DisplayStringLine(Line3, LCD_Show_Strings);
		}
		else
		{
			sprintf((char *)LCD_Show_Strings, "     PR37:%.1f%%", R37_PAAS_OUT);
			LCD_DisplayStringLine(Line3, LCD_Show_Strings);
		}
		
		if(Clear ==Yes)
		{
			sprintf((char *)LCD_Show_Strings, "     PR38:0.0%%");
			LCD_DisplayStringLine(Line4, LCD_Show_Strings);
		}
		else
		{
			sprintf((char *)LCD_Show_Strings, "     PR38:%.1f%%", R38_PAAS_OUT);
			LCD_DisplayStringLine(Line4, LCD_Show_Strings);
		}
	}
}


uint8_t LED_Timer_1(void)
{
	if(uwTick -uwTick_Led1_Point <1000)		return 0;
	if(uwTick -uwTick_Led1_Point >=1000)	return 1;
}


uint8_t LED_Timer_2(void)
{
	if(uwTick -uwTick_Led2_Point <1000)		return 0;
	if(uwTick -uwTick_Led2_Point >=1000)	return 1;
}

void Led_Proc(void)
{
	if(R37_PASSING ==Yes)
	{
		uwTick_Led1_Point =uwTick;
		ucled |=0x01;
	}
	
	if(LED_Timer_1() ==1)
	{
		ucled &=(~0x01);
	}
	
	if(R38_PASSING ==Yes)
	{
		uwTick_Led2_Point =uwTick;
		ucled |=0x02;
	}
	
	if(LED_Timer_2() ==1)
	{
		ucled &=(~0x02);
	}
	
	if(Mode ==GOODS)		//LD3
	{
		ucled &=(~0xF8);	// 灭4 5
		ucled |=0x04;
	}
	
	if(Mode ==STANDARD)		//LD4
	{
		ucled &=(~0xF4);	// 灭3 5
		ucled |=0x08;
	}
	
	if(Mode ==PASS)		//LD5
	{
		ucled &=(~0xFC);	// 灭3 4
		ucled |=0x10;
	}
	
	Led_Disp(ucled);
}

/*********** 中断函数 ***********/


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart1, rxbuffer, 3);

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
