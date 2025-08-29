/*
 * DAC_driver.c
 *
 * Created on: Aug 27, 2025
 * Author: Ahmed Ezzat
 */

#include "DAC_driver.h"

GPIO_TypeDef *G_GPIOx;

void HAL_DAC_Init(GPIO_TypeDef *GPIOx, GPIO_PinConfig_t sound_pins[], uint8_t pins_number){
	G_GPIOx = GPIOx;

	for(uint8_t i = 0; i < pins_number; i++){
		MCAL_GPIO_Init(GPIOx, &sound_pins[i]);
	}

	SysTick_Config_t SysTick_cfg;

	SysTick_cfg.SysTick_InterruptEnable = SysTick_Interrupt_EN;
	SysTick_cfg.SysTick_CLKSource = SysTick_CLK_AHB_8;

	MCAL_SysTicK_Init(&SysTick_cfg);
}

void HAL_DAC_SendAudioSample(const uint8_t audio_arr[], uint32_t index){
	for(uint8_t i = 0; i < SamplingPins; i++){
		MCAL_GPIO_WritePin(G_GPIOx, i, GET_BIT(audio_arr[index], i));
	}
}


