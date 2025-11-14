/*
 * TFT_driver.h
 *
 * Created on: Sep 6, 2025
 * Author: Ahmed Ezzat
 */

#ifndef ECU_INC_TFT_DRIVER_H_
#define ECU_INC_TFT_DRIVER_H_

#include "stm32f401xc_gpio_driver.h"
#include "stm32f401xc_SysTick_driver.h"
#include "stm32f401xc_SPI_driver.h"
#include "TFT_BitMap.h"

void HAL_TFT_Init(GPIO_TypeDef *GPIOx_PIN, GPIO_TypeDef *GPIOx_RST, GPIO_PinConfig_t *Pin_Config, GPIO_PinConfig_t *RST_Config, SPI_Typedef *SPIx, SPI_PinConfig_t *SPI_Config);

void HAL_TFT_ShowImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *image);
void HAL_TFT_ShowString(uint16_t x, uint16_t y, const char *str, uint16_t textColor, uint16_t bgColor, uint8_t size);

void HAL_TFT_SetXPos(uint16_t X_Start, uint16_t X_End);
void HAL_TFT_SetYPos(uint16_t Y_Start, uint16_t Y_End);

void HAL_TFT_FillBackground(uint16_t Background_Color);
void HAL_TFT_FillRectangle(uint16_t Rectangle_Color);

#endif /* ECU_INC_TFT_DRIVER_H_ */
