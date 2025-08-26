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

void HAL_LedMatrix_Init(GPIO_TypeDef *GPIOx_rows, GPIO_TypeDef *GPIOx_cols, GPIO_PinConfig_t *Rows, uint8_t row_number, GPIO_PinConfig_t *Cols, uint8_t Col_number);
void HAL_LedMatrix_DisplayFrame(uint8_t frame[], uint32_t FrameDelay);

#define ScanTime		2.5  // 50Hz / 20

#endif /* ECU_INC_LED_MATRIX_DRIVER_H_ */
