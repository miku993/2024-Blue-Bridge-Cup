#include "main.h"
#include "adc.h"
#include "tim.h"
#include "key_led.h"
#include "rcc.h"
#include "lcd.h"

/********* ������ ************/
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


/********** ���������� ************/
void Key_Proc(void);
void Lcd_Proc(void);
void Tim_Proc(void);
void Led_Proc(void);
void Speed_Proc(void);
uint8_t Tim_Timer(void);
uint8_t Led_Timer(void);
uint8_t Speed_Timer(void);

/********** ���������� ************/

//*********************** �û��Զ������
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

//*********************** key_led����
__IO uint32_t uwTick_LED_Point =0;
uint8_t Key_value;
uint16_t ucled =0x01;

//*********************** tim����
__IO uint32_t uwTick_Tim_Point =0;

//*********************** adc����

//*********************** lcd����
uint8_t Lcd_Show_Strings[21];


/********** ������ ************/
int main(void)
{
//*********************** ��ʼ�� 
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


//*********************** ����������� 
	HAL_TIM_Base_Start(&htim3);		// ������ʱ��
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
	__HAL_TIM_SET_AUTORELOAD(&htim2, (uint16_t)((1000000.0/Freq_Output)-1));
	
//*********************** ��ѭ��
  while (1)
  {
	  Key_Proc();
	  Lcd_Proc();
	  Tim_Proc();
	  Led_Proc();
	  Speed_Proc();
  }
}





/********** �Ӻ��� ************/

//*********************** Key�Ӻ���
void Key_Proc(void)
{
	
	Key_value =Key_Scan();
	
	if(Key_value ==1)		// �����л�
	{
		LCD_Clear(Black);	// ����
		Flag_Mode +=1;
		if(Flag_Mode ==4)
			Flag_Mode =1;
	}
	
	if(Key_value ==2)		// ѡ�񰴼�
	{
		if(Flag_Mode ==DATA)	// �ߵ�Ƶ�л�
		{
			if(Mode_DATA ==Low)
			{
				Change_Mode =Rising;
				Change_Flag =Yes;
			}	
			else if(Mode_DATA ==High)		// ����if��else if��ʹ���߼�
			{
				Change_Mode =Falling;
				Change_Flag =Yes;
			}	
		}
		
		if(Flag_Mode ==PARA)		// �����л�
		{
			if(Mode_PARA ==R)
				Mode_PARA =K;
			else if(Mode_PARA ==K)
				Mode_PARA =R;
		}
	}
	
	if(Key_value ==3)		// ��
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
	
	if(Key_value ==4)		// ��
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
			Lock =No;		// ����	
		}
	}
	
	if(Key_value ==5)		// ����
	{
		if(Flag_Mode ==DATA)
		{
			Lock =Yes;		
		}
	}
	
	Key_value =0;
}

//*********************** Lcd�Ӻ���
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


//*********************** Speed�Ӻ���
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
		if((V_Speed[0] ==V) && (V_Speed[1] ==V) && (V_Speed[2] ==V) && (V_Speed[3] ==V) && (V_Speed[4] ==V))	// 2s�ڱ��ָ��ٶ�
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


//*********************** Tim�Ӻ���
void Tim_Proc(void)
{

	//*********************** ����ռ�ձ�
	Voltage =((float)ADC2_Get_Value()/4096)*3.3;	// ת����ѹ��λ
	
	if(Lock ==No)		// û��
	{
		if(Voltage <1)
			Duty_Output =10;
		else if(Voltage >3)
			Duty_Output =85;
		else
			Duty_Output =(float)(Voltage *37.5 -27.5);		// y=kx+b
		
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, ((1000000/Freq_Output)*Duty_Output*0.01)-1);
	}
	
	//*********************** Ƶ��ת��
	if(Change_Mode ==Rising)		// L->H
	{
		if(Tim_Timer() ==1)
		{
			Freq_Output +=160;		// ÿ200msƵ������160hz
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
		__HAL_TIM_SET_AUTORELOAD(&htim2, (1000000/Freq_Output)-1);		// ����ARR ->�ı�Ƶ��
		HAL_Delay(10);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, (1000000/Freq_Output)*Duty_Output-1);	// ����Duty����
		
		
	}
	
	if(Change_Mode ==Falling)		// H->L
	{
		if(Tim_Timer() ==1)
		{
			Freq_Output -=160;		// ÿ200msƵ������160hz
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
		__HAL_TIM_SET_AUTORELOAD(&htim2, (1000000/Freq_Output)-1);		// ����ARR ->�ı�Ƶ��
		HAL_Delay(10);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, (1000000/Freq_Output)*Duty_Output-1);	// ����Duty����
		
		
	}
	
}

uint8_t Led_Timer(void)
{
	if(uwTick -uwTick_LED_Point < 100)	return 0;
	uwTick_LED_Point =uwTick;		return 1;
}


//*********************** led�Ӻ���
void Led_Proc(void)
{

	
	if(Flag_Mode ==DATA)
	{
		ucled |= 0x01;		// ��LED1
	}
	
	if(Change_Flag ==Yes)
	{
		ucled |= 0x04;		// ��LED3
	}
	
	if(Change_Flag ==No)
	{
		ucled &= (~0x04);		// ��LED3
	}
	
	if(Lock ==Yes)
	{
		if(Led_Timer() ==1)
		{
			ucled ^= 0x02;		// ����LED2
		}
		
	}
	
	if(Lock ==No)
	{
		if(Led_Timer() ==1)
		{
			ucled &= (~0x02);		// ��LED2
		}
	}
	
	Led_Disp(ucled);
}


/********** �жϺ��� ************/

//*********************** IC�жϺ���
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim ->Instance ==TIM3)
	{
		if(htim ->Channel ==HAL_TIM_ACTIVE_CHANNEL_2)		// CH2��Ƶ��
		{
			PWM_T_Count =HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2)+1;
			Freq_Input =1000000/PWM_T_Count;
			Duty_Input =PWM_D_Count/PWM_T_Count*100;		// �����ڵ�����������ټ���ռ�ձ�
		}
		
		if(htim ->Channel ==HAL_TIM_ACTIVE_CHANNEL_1)		// CH1��ռ�ձ�
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
