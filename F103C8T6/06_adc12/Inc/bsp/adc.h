#include "main.h"

void ADC1_Init(void);
void ADC2_Init(void);
void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle);
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle);
uint16_t getADC1(void);
uint16_t getADC2(void);



