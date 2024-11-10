#include "main.h"

void TIM2_Init(void);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);





