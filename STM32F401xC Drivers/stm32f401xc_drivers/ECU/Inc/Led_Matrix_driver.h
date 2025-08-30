/*
 * Led_Matrix_driver.h
 *
 * Created on: Aug 26, 2025
 * Author: Ahmed Ezzat
 */

#ifndef ECU_INC_LED_MATRIX_DRIVER_H_
#define ECU_INC_LED_MATRIX_DRIVER_H_

#include "stm32f401xc_SysTick_driver.h"
#include "stm32f401xc_gpio_driver.h"
#include "Serial2Parallel_drivers.h"

void HAL_LedMatrix_Init(GPIO_TypeDef *GPIOx_rows, GPIO_TypeDef *GPIOx_cols, GPIO_PinConfig_t *Rows, uint8_t row_number, GPIO_PinConfig_t *Cols, uint8_t Col_number);
void HAL_LedMatrix_DirectDisplayFrame(uint8_t frame[], uint32_t FrameDelay);

void HAL_LedMatrix_S2P_Init(GPIO_TypeDef *GPIOx_data, GPIO_TypeDef *GPIOx_shift, GPIO_TypeDef *GPIOx_latch, S2P_PinConfig_t *S2P_pin);
void HAL_LedMatrix_S2P_DisplayFrame(S2P_PinConfig_t *S2P_pin, uint8_t frame[], uint32_t FrameDelay);

#define ScanTime		2.5  // 50Hz / 20

#endif /* ECU_INC_LED_MATRIX_DRIVER_H_ */
