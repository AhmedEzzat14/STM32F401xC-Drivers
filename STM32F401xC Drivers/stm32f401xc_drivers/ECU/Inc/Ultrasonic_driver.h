/*
 * Ultrasonic_driver.h
 *
 * Created on: Oct 3, 2025
 * Author: Ahmed Ezzat
 */

#ifndef ECU_INC_ULTRASONIC_DRIVER_H_
#define ECU_INC_ULTRASONIC_DRIVER_H_

#include "stm32f401xc_gpio_driver.h"
#include "stm32f401xc_SysTick_driver.h"
#include "stm32f401xc_Timer_driver.h"
#include "stm32f401xc_EXTI_driver.h"


void HAL_Ultrasonic_Trigger(GPIO_TypeDef *GPIOx, uint16_t PinNumber);
void HAL_Ultrasonic_Echo(GPIO_TypeDef *GPIOx, EXTI_GPIO_Mapping_t Exti_Number);
uint32_t HAL_Ultrasonic_GetDistance(void);

#endif /* ECU_INC_ULTRASONIC_DRIVER_H_ */
