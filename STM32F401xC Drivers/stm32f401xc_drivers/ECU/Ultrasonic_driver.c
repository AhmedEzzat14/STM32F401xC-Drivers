/*
 * Ultrasonic_driver.c
 *
 * Created on: Oct 3, 2025
 * Author: Ahmed Ezzat
 */

#include "Ultrasonic_driver.h"

volatile uint8_t isr_flag = 0;
volatile uint32_t elapsed_ticks = 0;
volatile uint8_t measurement_ready = 0;
volatile uint32_t isr_count = 0;  // Add this counter

void Echo_ISR(void){
    isr_count++;  // Increment every time ISR is called

    if(0 == isr_flag){
        // Rising edge - start measurement
        MCAL_SysTicK_StartTimer(0x00FFFFFF);
        isr_flag = 1;
        measurement_ready = 0;
    }
    else if(1 == isr_flag){
        // Falling edge - stop measurement
        MCAL_SysTicK_StopTimer();
        elapsed_ticks = MCAL_SysTicK_GetElapsedTime_SingleShot();
        isr_flag = 0;
        measurement_ready = 1;
    }
}

void HAL_Ultrasonic_Trigger(GPIO_TypeDef *GPIOx, uint16_t PinNumber){
    static uint8_t trigger_initialized = 0;

    // Initialize trigger pin only once
    if(!trigger_initialized){
        GPIO_PinConfig_t trigger_cfg;
        trigger_cfg.GPIO_PinNumber = PinNumber;
        trigger_cfg.GPIO_MODE = GPIO_MODE_OP;
        trigger_cfg.GPIO_TYPE = GPIO_TYPE_PP;
        trigger_cfg.GPIO_Output_Speed = GPIO_SPEED_MEDIUM;
        trigger_cfg.GPIO_PU_PD = GPIO_PU_PD_NONE;
        MCAL_GPIO_Init(GPIOx, &trigger_cfg);
        trigger_initialized = 1;
    }

    // Reset measurement flag
    measurement_ready = 0;

    // Generate 10us trigger pulse
    MCAL_GPIO_WritePin(GPIOx, PinNumber, 0);
    Timer_Delay(TIMER3, 2, Unit_ms);  // Small delay to ensure pin is low

    MCAL_GPIO_WritePin(GPIOx, PinNumber, 1);
    Timer_Delay(TIMER3, 10, Unit_us);  // 10us high pulse

    MCAL_GPIO_WritePin(GPIOx, PinNumber, 0);
}

void HAL_Ultrasonic_Echo(GPIO_TypeDef *GPIOx, EXTI_GPIO_Mapping_t Exti_Number){
    EXTI_PinConfig_t echo_cfg;

    echo_cfg.EXTI_PIN = Exti_Number;
    echo_cfg.EXTI_Enable = EXTI_IRQ_EN;
    echo_cfg.EXTI_EdgeSelect = EXTI_TRIGGER_RISING_FALLING;
    echo_cfg.P_IRQ_CallBack = Echo_ISR;
    MCAL_EXTI_GPIO_Init(&echo_cfg);
}

uint32_t HAL_Ultrasonic_GetDistance(void){
    // Check if measurement is ready
    if(!measurement_ready){
        return 0;  // No valid measurement yet
    }

    // Calculate time in microseconds
    // SysTick is running at AHB/8 = 25MHz/8 = 3.125MHz
    // Each tick = 1/3.125MHz = 0.32 microseconds
    float time_us = (float)elapsed_ticks * 0.32f;

    // Distance = (time * speed_of_sound) / 2
    // Speed of sound = 343 m/s = 0.0343 cm/μs
    // Formula: distance (cm) = (time_us * 0.0343) / 2
    uint32_t distance_cm = (uint32_t)((time_us * 0.0343f) / 2.0f);

    // Sanity check - HC-SR04 max range is about 400cm
    if(distance_cm > 400){
        distance_cm = 0;  // Invalid reading
    }

    return distance_cm;
}

// Alternative function if you want to wait for measurement with timeout
uint32_t HAL_Ultrasonic_GetDistance_Blocking(uint32_t timeout_ms){
    uint32_t start_time = 0;  // You'd need a millisecond counter here

    // Wait for measurement to complete or timeout
    while(!measurement_ready){
        // Add timeout check here if you have a millisecond timer
        // if(ms_counter - start_time > timeout_ms) return 0;
    }

    return HAL_Ultrasonic_GetDistance();
}
