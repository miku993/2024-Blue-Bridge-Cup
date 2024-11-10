#include "main.h"
#include "adc.h"
#include "tim.h"
#include "key_led.h"
#include "rcc.h"
#include "lcd.h"

/********* 定义区 ************/
#define Reset	0
#define DATA	1
#define PARA	2
#define RECD	3
#define High	4
#define Low	    5
#define R	    6
#define K	    7
#define Yes	    8
#define No	    9
#define Rising	    10
#define Falling	    11


/********** 函数声明区 ************/
void Key_Proc(void);
void Lcd_Proc(void);
void Tim_Proc(void);
void Led_Proc(void);
void Speed_Proc(void);
uint8_t Tim_Timer(void);
uint8_t Led_Timer(void);
uint8_t Speed_Timer(void);

/********** 变量声明区 ************/

//*********************** 用户自定义变量
uint8_t Flag_Mode =1;
uint8_t Mode_DATA =Low;
uint8_t Mode_PARA =R;
uint8_t Data_R =1;
uint8_t Data_K =1;
uint8_t Lock =No;
uint32_t PWM_T_Count =0;
float PWM_D_Count =0;
float Duty_Output =0;
float Duty_Input =0;
uint32_t Freq_Output =4000;
uint32_t Freq_Input =0;
float Voltage;
uint8_t Change_Mode =Low;
uint8_t Change_Flag =No;
uint8_t count =0;
uint8_t Out_Freq_Change_Times =0;
float V_Speed[5]={0, 0, 0, 0, 0};
__IO uint32_t uwTick_Speed_Point =0;
uint8_t Speed_Count =0;
float V;
float Speed_H_Max;
float Speed_L_Max;

//*********************** key_led变量
__IO uint32_t uwTick_LED_Point =0;
uint8_t Key_value;
uint16_t ucled =0x01;

//*********************** tim变量
__IO uint32_t uwTick_Tim_Point =0;

//*********************** adc变量

//*********************** lcd变量
uint8_t Lcd_Show_Strings[21];


/********** 主函数 ************/
int main(void)
{
//*********************** 初始化 
	HAL_Init();
	SystemClock_Config();
	Key_led_Init();
	TIM2_Init();
	TIM3_Init();
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	ADC2_Init();


//*********************** 外设基本配置 
	HAL_TIM_Base_Start(&htim3);		// 启动定时器
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
	__HAL_TIM_SET_AUTORELOAD(&htim2, (uint16_t)((1000000.0/Freq_Output)-1));
	
//*********************** 主循环
  while (1)
  {
	  Key_Proc();
	  Lcd_Proc();
	  Tim_Proc();
	  Led_Proc();
	  Speed_Proc();
  }
}





/********** 子函数 ************/

//*********************** Key子函数
void Key_Proc(void)
{
	
	Key_value =Key_Scan();
	
	if(Key_value ==1)		// 界面切换
	{
		LCD_Clear(Black);	// 清屏
		Flag_Mode +=1;
		if(Flag_Mode ==4)
			Flag_Mode =1;
	}
	
	if(Key_value ==2)		// 选择按键
	{
		if(Flag_Mode ==DATA)	// 高低频切换
		{
			if(Mode_DATA ==Low)
			{
				Change_Mode =Rising;
				Change_Flag =Yes;
			}	
			else if(Mode_DATA ==High)		// 主义if与else if的使用逻辑
			{
				Change_Mode =Falling;
				Change_Flag =Yes;
			}	
		}
		
		if(Flag_Mode ==PARA)		// 调节切换
		{
			if(Mode_PARA ==R)
				Mode_PARA =K;
			else if(Mode_PARA ==K)
				Mode_PARA =R;
		}
	}
	
	if(Key_value ==3)		// 加
	{
		if(Flag_Mode ==PARA)
		{
			if(Mode_PARA ==R)
			{
				Data_R +=1;
				if(Data_R ==11)
					Data_R =1;
			}
			else if(Mode_PARA ==K)
			{
				Data_K +=1;
				if(Data_K ==11)
					Data_K =1;
			}
		}	
	}
	
	if(Key_value ==4)		// 减
	{
		if(Flag_Mode ==PARA)
		{
			if(Mode_PARA ==R)
			{
				Data_R -=1;
				if(Data_R ==0)
					Data_R =10;
			}
			else if(Mode_PARA ==K)
			{
				Data_K -=1;
				if(Data_K ==0)
					Data_K =10;
			}
		}
		
		if(Flag_Mode ==DATA)
		{
			Lock =No;		// 解锁	
		}
	}
	
	if(Key_value ==5)		// 锁定
	{
		if(Flag_Mode ==DATA)
		{
			Lock =Yes;		
		}
	}
	
	Key_value =0;
}

//*********************** Lcd子函数
void Lcd_Proc(void)
{
	if(Flag_Mode ==DATA)
	{
		sprintf((char *)Lcd_Show_Strings, "        DATA        ");
		LCD_DisplayStringLine(Line1, Lcd_Show_Strings);
		
		if(Mode_DATA ==High)
			sprintf((char *)Lcd_Show_Strings, "     M=H     ");
			
		if(Mode_DATA ==Low)
			sprintf((char *)Lcd_Show_Strings, "     M=L     ");
		LCD_DisplayStringLine(Line3, Lcd_Show_Strings);
		
		sprintf((char *)Lcd_Show_Strings, "     P=%d%%     ",(unsigned int)Duty_Output);
		LCD_DisplayStringLine(Line4, Lcd_Show_Strings);
		
		sprintf((char *)Lcd_Show_Strings, "     V=%4.1f     ",V);
		LCD_DisplayStringLine(Line5, Lcd_Show_Strings);
	}
	
	else if(Flag_Mode ==PARA)
	{
		sprintf((char *)Lcd_Show_Strings, "        PARA        ");
		LCD_DisplayStringLine(Line1, Lcd_Show_Strings);
		
		sprintf((char *)Lcd_Show_Strings, "     R=%d     ", Data_R);
		LCD_DisplayStringLine(Line3, Lcd_Show_Strings);
		
		sprintf((char *)Lcd_Show_Strings, "     K=%d     ", Data_K);
		LCD_DisplayStringLine(Line4, Lcd_Show_Strings);
	}
	
	else if(Flag_Mode ==RECD)
	{
		sprintf((char *)Lcd_Show_Strings, "        RECD        ");
		LCD_DisplayStringLine(Line1, Lcd_Show_Strings);
		
		sprintf((char *)Lcd_Show_Strings, "     N=%d     ",count );
		LCD_DisplayStringLine(Line3, Lcd_Show_Strings);
		
		sprintf((char *)Lcd_Show_Strings, "     MH=%4.1f     ",Speed_H_Max);
		LCD_DisplayStringLine(Line4, Lcd_Show_Strings);
		
		sprintf((char *)Lcd_Show_Strings, "     ML=%4.1f     ",Speed_L_Max);
		LCD_DisplayStringLine(Line5, Lcd_Show_Strings);
	}
}


uint8_t Speed_Timer(void)
{
	if(uwTick -uwTick_Speed_Point < 2000)	return 0;
	uwTick_Speed_Point =uwTick;		return 1;
}


//*********************** Speed子函数
void Speed_Proc(void)
{
	V =0.0628*((float)Data_R/(float)Data_K)*(float)Freq_Input;
	
	V_Speed[0] =V_Speed[1];
	V_Speed[1] =V_Speed[2];
	V_Speed[2] =V_Speed[3];
	V_Speed[3] =V_Speed[4];
	V_Speed[4] =V;

	if(Speed_Timer() ==1)		// 2s
	{
		if((V_Speed[0] ==V) && (V_Speed[1] ==V) && (V_Speed[2] ==V) && (V_Speed[3] ==V) && (V_Speed[4] ==V))	// 2s内保持该速度
		{
			if(Mode_DATA ==High)
				Speed_H_Max =V;
			else if(Mode_DATA ==Low)
				Speed_L_Max =V;
		}
	}		
	
	
}

uint8_t Tim_Timer(void)
{
	if(uwTick -uwTick_Tim_Point < 200)	return 0;
	uwTick_Tim_Point =uwTick;		return 1;
}


//*********************** Tim子函数
void Tim_Proc(void)
{

	//*********************** 调节占空比
	Voltage =((float)ADC2_Get_Value()/4096)*3.3;	// 转换电压单位
	
	if(Lock ==No)		// 没锁
	{
		if(Voltage <1)
			Duty_Output =10;
		else if(Voltage >3)
			Duty_Output =85;
		else
			Duty_Output =(float)(Voltage *37.5 -27.5);		// y=kx+b
		
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, ((1000000/Freq_Output)*Duty_Output*0.01)-1);
	}
	
	//*********************** 频率转换
	if(Change_Mode ==Rising)		// L->H
	{
		if(Tim_Timer() ==1)
		{
			Freq_Output +=160;		// 每200ms频率升高160hz
			Out_Freq_Change_Times ++;
			if(Out_Freq_Change_Times == 25)
			{
				Change_Mode =Reset;
				Change_Flag =No;
				Mode_DATA =High;
				Out_Freq_Change_Times =0;
				count ++;
			}
		}
		__HAL_TIM_SET_AUTORELOAD(&htim2, (1000000/Freq_Output)-1);		// 设置ARR ->改变频率
		HAL_Delay(10);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, (1000000/Freq_Output)*Duty_Output-1);	// 保持Duty不变
		
		
	}
	
	if(Change_Mode ==Falling)		// H->L
	{
		if(Tim_Timer() ==1)
		{
			Freq_Output -=160;		// 每200ms频率升高160hz
			Out_Freq_Change_Times ++;
			if(Out_Freq_Change_Times == 25)
			{
				Change_Mode =Reset;
				Change_Flag =No;
				Mode_DATA =Low;
				Out_Freq_Change_Times =0;
				count ++;
			}
		}
		__HAL_TIM_SET_AUTORELOAD(&htim2, (1000000/Freq_Output)-1);		// 设置ARR ->改变频率
		HAL_Delay(10);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, (1000000/Freq_Output)*Duty_Output-1);	// 保持Duty不变
		
		
	}
	
}

uint8_t Led_Timer(void)
{
	if(uwTick -uwTick_LED_Point < 100)	return 0;
	uwTick_LED_Point =uwTick;		return 1;
}


//*********************** led子函数
void Led_Proc(void)
{

	
	if(Flag_Mode ==DATA)
	{
		ucled |= 0x01;		// 亮LED1
	}
	
	if(Change_Flag ==Yes)
	{
		ucled |= 0x04;		// 亮LED3
	}
	
	if(Change_Flag ==No)
	{
		ucled &= (~0x04);		// 亮LED3
	}
	
	if(Lock ==Yes)
	{
		if(Led_Timer() ==1)
		{
			ucled ^= 0x02;		// 闪亮LED2
		}
		
	}
	
	if(Lock ==No)
	{
		if(Led_Timer() ==1)
		{
			ucled &= (~0x02);		// 灭LED2
		}
	}
	
	Led_Disp(ucled);
}


/********** 中断函数 ************/

//*********************** IC中断函数
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim ->Instance ==TIM3)
	{
		if(htim ->Channel ==HAL_TIM_ACTIVE_CHANNEL_2)		// CH2计频率
		{
			PWM_T_Count =HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2)+1;
			Freq_Input =1000000/PWM_T_Count;
			Duty_Input =PWM_D_Count/PWM_T_Count*100;		// 在周期的最后上升沿再计算占空比
		}
		
		if(htim ->Channel ==HAL_TIM_ACTIVE_CHANNEL_1)		// CH1计占空比
		{
			PWM_D_Count =HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1)+1;
			
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
