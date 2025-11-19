/*
 * JoyStick_driver.c
 *
 * Created on: Nov 19, 2025
 * Author: Ahmed Ezzat
 */

#include "JoyStick_driver.h"

#define ADC_CHANNEL_X 0
#define ADC_CHANNEL_Y 1

// DMA configuration for ADC1 on STM32F401
#define ADC_DMA_STREAM 0
#define ADC_DMA_CHANNEL 0

static volatile uint16_t s_adc_dma_buf[2];

void HAL_Joystick_DMA_Init(Joystick_Config_t *joystick_pin){

    // init both pins as analog
    ADC_Init(joystick_pin->X_PORT, joystick_pin->X_PIN);
    ADC_Init(joystick_pin->Y_PORT, joystick_pin->Y_PIN);

    // init DMA once for ADC1 (use single stream for scan of two channels)
    MCAL_DMA_Init(DMA2, ADC_DMA_STREAM, ADC_DMA_CHANNEL);

    // Set Stream Parameters:
    // Source: ADC1 Data Register (Peripheral)
    // Dest: s_adc_dma_buf (Memory)
    // PSize/MSize: Half-word (16-bit)
    // BlockSize: 2 (2 conversions per sequence)
    // Threshold: Full FIFO
    MCAL_DMA_SetStream(DMA2, ADC_DMA_STREAM,
        (uint32_t *)&(ADC1->DR), // Peripheral Address (Source)
        (uint32_t *)s_adc_dma_buf, // Memory Address (Destination)
	HalfWord, HalfWord,
        2, FULL_FIFO);

    // --- Configure ADC1 ---
    // a. CR1 Register
    // Bit 25: RES[1:0] = 00 (12-bit resolution) - default
    // Bit 24: DISCEN = 0 (Discontinuous mode disabled) - default
    // Bit 11: SCAN = 1 (Scan Mode enabled for multiple channels)
    ADC1->CR1 |= (1 << 8); // ADC_CR1_SCAN

    // CR2 Register
    // Bit 10: CONT = 1 (Continuous Conversion Mode)
    // Bit 9: EOCS = 0 (EOC flag after sequence is complete, not each conversion)
    // Bit 8: DMA = 1 (DMA request enabled)
    // Bit 1: CONT = 1, Bit 8: DMA = 1
    ADC1->CR2 |= (1 << 1) | (1 << 8);

    // Sampling Time (SMPR2)
    // Set sampling time for IN0 and IN1 (e.g., 480 cycles for stability) - 0b111
    ADC1->SMPR2 = (0b111 << (0 * 3)) | (0b111 << (1 * 3));

    // Sequence Length (SQR1)
    // L[3:0] = (2 conversions - 1) = 1
    ADC1->SQR1 &= ~(0xF << 20); // Clear L[3:0]
    ADC1->SQR1 |= (1 << 20);    // Set L = 1 (2 conversions)

    // Sequence Ranks (SQR3)
    // SQ1 = Channel 0 (PA0), SQ2 = Channel 1 (PA1)
    ADC1->SQR3 = (ADC_CHANNEL_X << 0) | (ADC_CHANNEL_Y << 5);

    // Small delay to ensure ADC is ready before starting the DMA stream
    for (volatile int i = 0; i < 1000; i++);

    // Enable DMA Stream
    MCAL_DMA_EnableStream(ADC_DMA_STREAM);
}

void HAL_Joystick_DMA_Start(void){
    ADC1->CR2 |= (1 << 30);
}

Joystick_Data HAL_Joystick_DMA_GetData(void){
    Joystick_Data current_data;

    current_data.x = s_adc_dma_buf[0];
    current_data.y = s_adc_dma_buf[1];

    return current_data;
}
