/*
 * DAC_driver.h
 *
 * Created on: Aug 27, 2025
 * Author: Ahmed Ezzat
 */

#ifndef ECU_INC_DAC_DRIVER_H_
#define ECU_INC_DAC_DRIVER_H_

#include "stm32f401xc_SysTick_driver.h"
#include "stm32f401xc_gpio_driver.h"

void HAL_DAC_Init(GPIO_TypeDef *GPIOx, GPIO_PinConfig_t sound_pins[], uint8_t pins_number);
void HAL_DAC_SendAudioSample(const uint8_t audio_arr[], uint32_t index);

#define SamplingPins			8

#endif /* ECU_INC_DAC_DRIVER_H_ */
