#include "main.h"
#include "tim.h"
#include "uart.h"
#include "rcc.h"
#include "i2c.h"
#include "key_led.h"
#include "lcd.h"

/***************** ������ *****************/
#define SHOP 1
#define PRICE 2
#define REP 3
#define Yes 4
#define NO 5


/***************** ���������� *****************/
void Key_Proc(void);
void LED_Proc(void);
void Lcd_Proc(void);
void i2c_Proc(void);
void Uart_Proc(void);

/***************** ���������� *****************/

//*************************** �û��Զ���
uint8_t Flag_Mode =SHOP;
uint8_t Goods_SHOP_X =0;
uint8_t Goods_SHOP_Y =0;
float Goods_PRICE_X =1.0;
float Goods_PRICE_Y =1.0;
uint8_t X_Price_Ten = 10;
uint8_t Y_Price_Ten = 10;
uint8_t Goods_REP_X =10;
uint8_t Goods_REP_Y =10;
uint8_t Goods_Sum =0;
uint8_t Flag_write =NO;
uint8_t Flag_LED_ON =0;


//*************************** Key������
__IO uint32_t uwTick_Key_Point =0;
uint8_t Key_value;


//*************************** LED������
__IO uint32_t uwTick_LED_Point =0;
__IO uint32_t uwTick_LED_ON =0;
uint8_t ucLed;

//*************************** lcd������
__IO uint32_t uwTick_Lcd_Point =0;
unsigned char Lcd_Show_String_0[21];
unsigned char Lcd_Show_String_1[21];
unsigned char Lcd_Show_String_2[21];
unsigned char Lcd_Show_String_3[21];
unsigned char Lcd_Show_String_4[21];
unsigned char Lcd_Show_String_5[21];

//*************************** Uart������
__IO uint32_t uwTick_Uart_Point =0;
unsigned char str[40];
uint8_t rx_buffer;

/***************** ������ *****************/
int main(void)
{
  
//************************** ��ʼ��
  HAL_Init();
  SystemClock_Config();

  Key_LED_Init();
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);	
  I2CInit();
  UART_Init();
  TIM2_Init();
	
//************************** �����������	
  i2c_24c02_read(&X_Price_Ten, 0x00, 1);
  i2c_24c02_read(&Y_Price_Ten, 0x01, 1);
  i2c_24c02_read(&Goods_REP_X, 0x02, 1);
  i2c_24c02_read(&Goods_REP_Y, 0x03, 1);
  
  Goods_PRICE_X =X_Price_Ten*0.1;
  Goods_PRICE_Y =Y_Price_Ten*0.1;
  
  HAL_UART_Receive_IT(&huart1, &rx_buffer, 1);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);		//PA1
  
//************************** ��ѭ��
  while (1)
  {	
	  Lcd_Proc();
	  Key_Proc();
	  LED_Proc();
	  Uart_Proc();
  }
}


/***************** �Ӻ��� *****************/

//********************************************* �����Ӻ���
void Key_Proc(void)
{
	if(uwTick- uwTick_Key_Point < 50)	return;
		uwTick_Key_Point =uwTick;
	
	Key_value =Key_Scan(); 
	
	if(Key_value == 1)		// �����л�
	{
		Flag_Mode ++;
		if(Flag_Mode ==4)
			Flag_Mode =SHOP;

		Goods_SHOP_X =0;
		Goods_SHOP_Y =0;
			
		Key_value =0;
	}
	
	if(Key_value == 2)		// X
	{
		if(Flag_Mode == SHOP)
		{
			if(Goods_SHOP_X < Goods_REP_X)	// 0-REP
				Goods_SHOP_X +=1;
		}
		if(Flag_Mode == PRICE)
		{
			Goods_PRICE_X +=0.1;
			if(Goods_PRICE_X >= 2.1)	// 1.0-2.0
				Goods_PRICE_X =1.0;
			
			X_Price_Ten =(uint8_t)(Goods_PRICE_X*10);
		    i2c_24c02_write(&X_Price_Ten, 0x00, 1);
		}
		if(Flag_Mode == REP)
		{
			Goods_REP_X +=1;	// 10-����
			i2c_24c02_write(&Goods_REP_X, 0x02, 1);
			
		}
		
		Key_value =0;
	}
	
	if(Key_value == 3)		// Y
	{
		if(Flag_Mode == SHOP)
		{
			if(Goods_SHOP_Y < Goods_REP_Y)	// 0-REP
				Goods_SHOP_Y +=1;
				
		}
		if(Flag_Mode == PRICE)
		{
			Goods_PRICE_Y +=0.1;
			if(Goods_PRICE_Y >= 2.1)	// 1.0-2.0
				Goods_PRICE_Y =1.0;
			
			Y_Price_Ten =(uint8_t)(Goods_PRICE_Y*10);
		    i2c_24c02_write(&Y_Price_Ten, 0x01, 1);
		}
		if(Flag_Mode == REP)
		{
			Goods_REP_Y +=1;	// 10-����
			i2c_24c02_write(&Goods_REP_Y, 0x03, 1);
		}
		
		Key_value =0;
	}
	
	if(Key_value == 4)		// buy
	{
		if((Flag_Mode == SHOP) && (Goods_REP_X >= Goods_SHOP_X) && (Goods_REP_Y >= Goods_SHOP_Y) && ((Goods_SHOP_X != 0) || (Goods_SHOP_Y != 0)))	// ���ڹ�������ҹ���ɹ�
		{
			Goods_REP_X -= Goods_SHOP_X;
			Goods_REP_Y -= Goods_SHOP_Y;
			i2c_24c02_write(&Goods_REP_X, 0x02, 1);
		    i2c_24c02_write(&Goods_REP_Y, 0x03, 1);
			
			sprintf(str, "X:%d,Y:%d,Z:%.1f\r\n", Goods_SHOP_X, Goods_SHOP_Y, (Goods_SHOP_X*Goods_PRICE_X+Goods_SHOP_Y*Goods_PRICE_Y));
			HAL_UART_Transmit(&huart1, str, strlen(str), 50);
			
			Goods_SHOP_X =0;
			Goods_SHOP_Y =0;
			
			Flag_LED_ON =1;
			uwTick_LED_ON =uwTick;		// �����£���¼��ǰ����ֵ
			ucLed |= 0x01;	// ����һ����|
			
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 25*6);
		}
		
		Key_value =0;
	}
	
}



//********************************************* lcd�Ӻ���
void Lcd_Proc(void)
{
	if(uwTick- uwTick_Lcd_Point < 100)	return;
		uwTick_Lcd_Point =uwTick;
	
	if(Flag_Mode ==1)	// �������
	{
		sprintf((char *)Lcd_Show_String_1, "        SHOP          ");
		LCD_DisplayStringLine(Line1, Lcd_Show_String_1);
		
		sprintf((char *)Lcd_Show_String_3, "     X:%d        ", Goods_SHOP_X);
		LCD_DisplayStringLine(Line3, Lcd_Show_String_3);
		
		sprintf((char *)Lcd_Show_String_4, "     Y:%d        ", Goods_SHOP_Y);
		LCD_DisplayStringLine(Line4, Lcd_Show_String_4);
	}
	
	if(Flag_Mode ==2)	// �۸����
	{
		sprintf((char *)Lcd_Show_String_1, "        PRICE          ");
		LCD_DisplayStringLine(Line1, Lcd_Show_String_1);	
		
		sprintf((char *)Lcd_Show_String_3, "     X:%3.1f        ", Goods_PRICE_X);
		LCD_DisplayStringLine(Line3, Lcd_Show_String_3);
		
		sprintf((char *)Lcd_Show_String_4, "     Y:%3.1f        ", Goods_PRICE_Y);
		LCD_DisplayStringLine(Line4, Lcd_Show_String_4);
	}

	if(Flag_Mode ==3)	// ������
	{	
		sprintf((char *)Lcd_Show_String_1, "        REP          ");
		LCD_DisplayStringLine(Line1, Lcd_Show_String_1);
		
		sprintf((char *)Lcd_Show_String_3, "     X:%d        ", Goods_REP_X);
		LCD_DisplayStringLine(Line3, Lcd_Show_String_3);
		
		sprintf((char *)Lcd_Show_String_4, "     Y:%d        ", Goods_REP_Y);
		LCD_DisplayStringLine(Line4, Lcd_Show_String_4);
	}
	
}

//********************************************* led�Ӻ���
void LED_Proc(void)
{
	if(uwTick- uwTick_LED_Point < 100)	return;
		uwTick_LED_Point =uwTick;
	
	if(Flag_LED_ON ==1)		// ����
	{
		if((uwTick - uwTick_LED_ON)>= 5000)		// 5s��
		{
			ucLed &= (~0x01);	// ���һ����&		
			Flag_LED_ON = 0;
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 25);
		}
	}
		
	if((Goods_REP_X == 0)&&(Goods_REP_Y == 0))		// �޿��
	{
		ucLed ^= 0x02;		// ȡ��һ����^
	}
	else
	{
		ucLed &= (~0x02);	
	}
		
	LED_Disp(ucLed);
}


//********************************************* Uart�Ӻ���
void Uart_Proc(void)
{
	if(uwTick- uwTick_Uart_Point < 100)	return;
		uwTick_Uart_Point =uwTick;
	
	if(rx_buffer == '?')
	{
		rx_buffer ='0';
		sprintf(str, "X:%.1f,Y:%.1f\r\n", Goods_PRICE_X, Goods_PRICE_Y);
		HAL_UART_Transmit(&huart1, str, strlen(str), 50);
	}
}

//********************************************* Uart�����ж�
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart1, &rx_buffer, 1);
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
