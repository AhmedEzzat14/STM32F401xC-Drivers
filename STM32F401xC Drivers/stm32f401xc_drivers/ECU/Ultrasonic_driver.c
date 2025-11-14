/*
 * Ultrasonic_driver.c
 *
 * Created on: Oct 3, 2025
 * Author: Ahmed Ezzat
 */

#include "Ultrasonic_driver.h"

volatile uint8_t isr_flag = 0;
volatile uint32_t elapsed_ticks = 0;
volatile uint32_t isr_count = 0;

void Echo_ISR(void){
    isr_count++;

    if(0 == isr_flag){
        MCAL_SysTicK_StartTimer(0x00FFFFFF);
        isr_flag = 1;
    }
    else if(1 == isr_flag){
        MCAL_SysTicK_StopTimer();
        elapsed_ticks = MCAL_SysTicK_GetElapsedTime_SingleShot();
        isr_flag = 0;
    }
}

void HAL_Ultrasonic_Trigger(GPIO_TypeDef *GPIOx, uint16_t PinNumber){
    GPIO_PinConfig_t trigger_cfg;
    trigger_cfg.GPIO_PinNumber = PinNumber;
    trigger_cfg.GPIO_MODE = GPIO_MODE_OP;
    trigger_cfg.GPIO_TYPE = GPIO_TYPE_PP;
    trigger_cfg.GPIO_Output_Speed = GPIO_SPEED_MEDIUM;
    trigger_cfg.GPIO_PU_PD = GPIO_PU_PD_NONE;
    MCAL_GPIO_Init(GPIOx, &trigger_cfg);

    MCAL_GPIO_WritePin(GPIOx, PinNumber, 1);
    Timer_Delay(TIMER3, 10, Unit_ms);
    MCAL_GPIO_WritePin(GPIOx, PinNumber, 0);
}

void HAL_Ultrasonic_Echo(EXTI_GPIO_Mapping_t Exti_Number){
    EXTI_PinConfig_t echo_cfg;

    echo_cfg.EXTI_PIN = Exti_Number;
    echo_cfg.EXTI_Enable = EXTI_IRQ_EN;
    echo_cfg.EXTI_EdgeSelect = EXTI_TRIGGER_RISING_FALLING;
    echo_cfg.P_IRQ_CallBack = Echo_ISR;
    MCAL_EXTI_GPIO_Init(&echo_cfg);
}

uint32_t HAL_Ultrasonic_GetDistance(void){
    float time_us = (float)elapsed_ticks * 0.32f;

    // Distance = (time * speed_of_sound) / 2
    // Speed of sound = 343 m/s = 0.0343 cm/μs
    // Formula: distance (cm) = (time_us * 0.0343) / 2
    uint32_t distance_cm = (uint32_t)((time_us * 0.0343f) / 2.0f);

    return distance_cm;
}
