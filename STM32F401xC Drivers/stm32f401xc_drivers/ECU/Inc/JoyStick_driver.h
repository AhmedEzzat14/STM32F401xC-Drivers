/*
 * JoyStick_driver.h
 *
 * Created on: Nov 19, 2025
 * Author: Ahmed Ezzat
 */

#ifndef ECU_INC_JOYSTICK_DRIVER_H_
#define ECU_INC_JOYSTICK_DRIVER_H_

#include "stm32f401xc_ADC_driver.h"

typedef struct{
    GPIO_TypeDef *X_PORT;
    GPIO_TypeDef *Y_PORT;

    uint16_t X_PIN;
    uint16_t Y_PIN;
}Joystick_Config_t;

typedef struct{
    uint16_t x;
    uint16_t y;
}Joystick_Data;

void HAL_Joystick_DMA_Init(Joystick_Config_t *joystick_pin);
void HAL_Joystick_DMA_Start(void);
Joystick_Data HAL_Joystick_DMA_GetData(void);

void ADC_DMA_TC_Callback(void);

#endif /* ECU_INC_JOYSTICK_DRIVER_H_ */
