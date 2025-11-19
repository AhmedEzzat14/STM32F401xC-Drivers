/*
 * main.c
 * Calibrated joystick demo (ADC polling, averaging, calibration)
 * Updated by ChatGPT for user
 */

#include "stm32f401xc_FPU_driver.h"
#include "stm32f401xc_FMI_driver.h"
#include "stm32f401xc_SysTick_driver.h"
#include "stm32f401xc_DMA_driver.h"

#include "stm32f401xc_RCC_driver.h"
#include "stm32f401xc_gpio_driver.h"
#include "stm32f401xc_EXTI_driver.h"

#include "stm32f401xc_USART_driver.h"
#include "stm32f401xc_SPI_driver.h"
#include "stm32f401xc_I2C_driver.h"
#include "stm32f401xc_Timer_driver.h"
#include "stm32f401xc_ADC_driver.h"

#include "nrf24l01_driver.h"
#include "Ultrasonic_driver.h"
#include "JoyStick_driver.h"

#include <stdio.h>

#define JOYSTICK_X_PORT GPIOA
#define JOYSTICK_X_PIN  0
#define JOYSTICK_Y_PORT GPIOA
#define JOYSTICK_Y_PIN  1

void Clock_Init(void) {
    RCC_GPIOA_CLK_EN();
    RCC_GPIOB_CLK_EN();
    RCC_SYSCFG_CLK_EN();
}

void delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < (ms * 1000); i++);
}

/* Helper conversion / map functions */
static inline uint16_t to10bit(uint16_t raw12) {
    return (uint16_t)(raw12 >> 2);
}

static inline uint16_t map_clamp_uint16(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
    if (in_max <= in_min) return out_min;
    if (x <= in_min) return out_min;
    if (x >= in_max) return out_max;
    uint32_t num = (uint32_t)(x - in_min) * (out_max - out_min);
    return (uint16_t)(num / (in_max - in_min) + out_min);
}

/* read_raw12: returns 0..4095 by using ADC_READ and G_ADC_pin */
uint16_t read_raw12(uint8_t ch) {
    G_ADC_pin = ch;
    return ADC_READ();
}

/* read averaged raw 12-bit value */
uint16_t read_raw12_avg(uint8_t ch, int samples) {
    uint32_t sum = 0;
    for (int i = 0; i < samples; ++i) {
        sum += read_raw12(ch);
        for (volatile int d = 0; d < 50; ++d) __asm volatile ("nop");
    }
    return (uint16_t)(sum / samples);
}

/* calibration data storage */
static uint16_t cal_min[2];
static uint16_t cal_max[2];

/* calibrate_dynamic: sample both axes while user moves joystick around
   seconds: how long to sample, samples_per_sec: sampling frequency target */
void calibrate_dynamic(int seconds, int samples_per_sec) {
    /* initialize */
    cal_min[0] = cal_min[1] = 0xFFFFU;
    cal_max[0] = cal_max[1] = 0;

    int total = seconds * samples_per_sec;
    if (total <= 0) total = 1;

    /* approximate delay per sample (not precise) */
    uint32_t delay_loops = 1000000 / (samples_per_sec > 0 ? samples_per_sec : 10);

    for (int i = 0; i < total; ++i) {
        for (uint8_t ch = 0; ch < 2; ++ch) {
            uint16_t raw = read_raw12(ch);
            if (raw < cal_min[ch]) cal_min[ch] = raw;
            if (raw > cal_max[ch]) cal_max[ch] = raw;
        }
        for (volatile uint32_t d = 0; d < delay_loops; ++d) __asm volatile ("nop");
    }

    /* fallback if no movement detected */
    for (uint8_t ch = 0; ch < 2; ++ch) {
        if (cal_min[ch] >= cal_max[ch]) {
            uint16_t mid = 2048U;
            cal_min[ch] = (mid > 1500U) ? (mid - 1500U) : 0U;
            cal_max[ch] = (mid + 1500U < 4095U) ? (mid + 1500U) : 4095U;
        }
    }
}

/* read calibrated 10-bit value (0..1023) */
uint16_t read_calibrated_10bit(uint8_t ch) {
    uint16_t raw12 = read_raw12_avg(ch, 4); /* average 4 for speed */
    /* clamp to calibration range */
    if (raw12 < cal_min[ch]) raw12 = cal_min[ch];
    if (raw12 > cal_max[ch]) raw12 = cal_max[ch];

    /* map to full 12-bit range then to 10-bit */
    uint16_t mapped12 = map_clamp_uint16(raw12, cal_min[ch], cal_max[ch], 0, 4095);
    return to10bit(mapped12);
}

int main(void){
    MCAL_FPU_Enable();
    MCAL_RCC_Init();
    Clock_Init();

    /* Init ADC pins */
    ADC_Init(GPIOA, 0); // PA0
    ADC_Init(GPIOA, 1); // PA1

    /* long sample time for channels 0 and 1 (480 cycles) */
    ADC1->SMPR2 &= ~((0x7U << (0 * 3)) | (0x7U << (1 * 3)));
    ADC1->SMPR2 |=  ((0x7U << (0 * 3)) | (0x7U << (1 * 3)));

    /* Optional: small startup delay */
    //for (volatile int i = 0; i < 2000; ++i) __asm volatile ("nop");

    /* Ask user to move joystick to extremes during calibration or do it quietly here */
    /* You can prompt via UART if you wish; here we do automatic short calibration (3s, 20 samples/sec) */
    calibrate_dynamic(3, 20);

    /* Main loop: read calibrated 10-bit values */
    while (1) {
        // 1) raw averaged 12-bit
        uint16_t raw_x = read_raw12_avg(0, 4);
        uint16_t raw_y = read_raw12_avg(1, 4);

        // 2) mapped from calibration (12-bit)
        uint16_t mapped12_x = map_clamp_uint16(raw_x, cal_min[0], cal_max[0], 0, 4095);
        uint16_t mapped12_y = map_clamp_uint16(raw_y, cal_min[1], cal_max[1], 0, 4095);

        // 3) final 10-bit outputs
        uint16_t out10_x = to10bit(mapped12_x);
        uint16_t out10_y = to10bit(mapped12_y);

        // debug-visible copies (set break-point here)
        volatile uint16_t dbg_raw_x     = raw_x;
        volatile uint16_t dbg_raw_y     = raw_y;
        volatile uint16_t dbg_calmin_x  = cal_min[0];
        volatile uint16_t dbg_calmax_x  = cal_max[0];
        volatile uint16_t dbg_calmin_y  = cal_min[1];
        volatile uint16_t dbg_calmax_y  = cal_max[1];
        volatile uint16_t dbg_map12_x   = mapped12_x;
        volatile uint16_t dbg_map12_y   = mapped12_y;
        volatile uint16_t dbg_out10_x   = out10_x;
        volatile uint16_t dbg_out10_y   = out10_y;

        delay_ms(200);
    }

    return 0;
}
