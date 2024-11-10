#include "main.h"


void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
	
