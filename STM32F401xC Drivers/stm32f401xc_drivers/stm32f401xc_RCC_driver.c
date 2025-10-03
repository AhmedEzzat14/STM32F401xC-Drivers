/*
 * stm32f401xc_RCC_driver.c
 *
 * Created on: Aug 26, 2025
 * Author: Ahmed Ezzat
 */

#include "stm32f401xc_RCC_driver.h"

void MCAL_RCC_Init(void){

#if RCC_SYS_CLK == HSE_CLK

    // Choose Bypass
#if HSE_BYPASS == RC_CLK
    RCC->CR |= (1 << 18);

#elif HSE_BYPASS == MECHANICAL_CLK
    RCC->CR &= ~(1 << 18);
#endif
    // HSE on
    RCC->CR |= (1 << 16);

    // Wait Till Flag is Set
    while(GET_BIT(RCC->CR,17) == 0);

    // Switch to HSE
    RCC->CFGR &= ~(1 << 1);
    RCC->CFGR |= (1 << 0);

    // CSSON enable/disable

#elif RCC_SYS_CLK == HSI_CLK
    // HSI on
    RCC->CR |= (1 << 0);

    // Wait Till Flag is Set
    while(GET_BIT(RCC->CR,1) == 0);

    // Switch to HSI
    RCC->CFGR &= ~(1 << 1);
    RCC->CFGR &= ~(1 << 0);

#elif RCC_SYS_CLK == PLL_CLK


#else
#error "Invalid option"
#endif
}
