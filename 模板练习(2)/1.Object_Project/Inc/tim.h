#include "main.h"

extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim3;

extern TIM_HandleTypeDef htim6;

extern TIM_HandleTypeDef htim15;


void TIM2_Init(void);
void TIM3_Init(void);
void TIM6_Init(void);
void TIM15_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);


