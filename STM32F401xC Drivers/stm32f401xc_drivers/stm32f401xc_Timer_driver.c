/*
 * stm32f401xc_Timer_driver.c
 *
 * Created on: Sep 1, 2025
 * Author: Ahmed Ezzat
 */

//------------------------------
// Includes
//------------------------------
#include "stm32f401xc_Timer_driver.h"

//============================================================

/*
 * =======================================================
 * 		    Generic Variables
 * =======================================================
 */
volatile TIMER_TypeDef *G_Timer;
Timer_Config_t *G_Timer_config = NULL;

static uint32_t Encoder_Counts;

volatile uint8_t delay_flag;
uint32_t delay;

//============================================================

/*
 * =======================================================
 * 		   Generic Functions
 * =======================================================
 *
 */
static void MCAL_Timer_Start(Timer_Config_t *Timer_Config){
    // CEN
    Timer_Config->BaseConfig.TIMERx->CR1 |= (1 << 0);
}

static void MCAL_Timer_Stop(Timer_Config_t *Timer_Config){
    // CEN = 0
    Timer_Config->BaseConfig.TIMERx->CR1 &= ~(1 << 0);
}

static void MCAL_Timer_PWM_Start(Timer_Config_t *Timer_Config){
    MCAL_Timer_Start(Timer_Config);
}

static void MCAL_Timer_PWM_Stop(Timer_Config_t *Timer_Config){
    Timer_Config->BaseConfig.TIMERx->CCER &= ~((1 << 0) | (1 << 4) | (1 << 8) | (1 << 12));
    MCAL_Timer_Stop(Timer_Config);
}

//============================================================

/*
 * ===================================================
 * 	 APIs Supported by "MCAL Timer DRIVER"
 * ===================================================
 */
void MCAL_Timer_Init(Timer_Config_t *Timer_Config){
    TIMER_TypeDef *L_TIMERx = Timer_Config->BaseConfig.TIMERx;
    G_Timer_config = Timer_Config;

    if(TIMER1 == (Timer_Config->BaseConfig.TIMERx)){
	RCC_TIMER1_CLK_EN();
    }
    else if(TIMER2 == (Timer_Config->BaseConfig.TIMERx)){
	RCC_TIMER2_CLK_EN();
    }
    else if(TIMER3 == (Timer_Config->BaseConfig.TIMERx)){
	RCC_TIMER3_CLK_EN();
    }
    else if(TIMER4 == (Timer_Config->BaseConfig.TIMERx)){
	RCC_TIMER4_CLK_EN();
    }
    else if(TIMER5 == (Timer_Config->BaseConfig.TIMERx)){
	RCC_TIMER5_CLK_EN();
    }
    L_TIMERx->CR1 = 0;

    // Set The Prescaler & AutoReload Value
    L_TIMERx->ARR = Timer_Config->BaseConfig.TIMER_AutoReload;
    L_TIMERx->PSC = Timer_Config->BaseConfig.TIMER_Prescaler;

    // Set Counter Mode
    L_TIMERx->CR1 |= Timer_Config->BaseConfig.TIMER_CounterMode;

    // Set Clock
    L_TIMERx->CR1 |= Timer_Config->BaseConfig.TIMER_CLKDivision;

    // Set Repetition Counter if Timer1
    if(TIMER1 == (Timer_Config->BaseConfig.TIMERx)){
	L_TIMERx->RCR = Timer_Config->BaseConfig.TIMER_RepetitionCounter;
    }

    if(TIMER_IRQ_Mode_None != (Timer_Config->TIMER_IRQ_Enable)){
	L_TIMERx->DIER |= TIMER_IRQ_Mode_UIE;
	if(L_TIMERx == TIMER1){
	    NVIC_IRQ_TIMER1_UP_EN();
	}
	else if(L_TIMERx == TIMER2){
	    NVIC_IRQ_TIMER2_EN();
	}
	else if(L_TIMERx == TIMER3){
	    NVIC_IRQ_TIMER3_EN();
	}
	else if(L_TIMERx == TIMER4){
	    NVIC_IRQ_TIMER4_EN();
	}
	else if(L_TIMERx == TIMER5){
	    NVIC_IRQ_TIMER5_EN();
	}
    }
}

void MCAL_Timer_DeInit(Timer_Config_t *Timer_Config){
    TIMER_TypeDef *L_TIMERx = Timer_Config->BaseConfig.TIMERx;

    if(L_TIMERx == TIMER1){
	RCC_TIMER1_CLK_RST_SET();
	NVIC_IRQ_TIMER1_UP_DIS();
    }
    else if(L_TIMERx == TIMER2){
	RCC_TIMER2_CLK_RST_SET();
	NVIC_IRQ_TIMER2_DIS();
    }
    else if(L_TIMERx == TIMER3){
	RCC_TIMER3_CLK_RST_SET();
	NVIC_IRQ_TIMER3_DIS();
    }
    else if(L_TIMERx == TIMER4){
	RCC_TIMER4_CLK_RST_SET();
	NVIC_IRQ_TIMER4_DIS();
    }
    else if(L_TIMERx == TIMER5){
	RCC_TIMER5_CLK_RST_SET();
	NVIC_IRQ_TIMER5_DIS();
    }
}

void MCAL_Timer_PWM_Init(Timer_Config_t *Timer_Config, uint32_t DutyCycle){
    TIMER_TypeDef *L_TIMERx = Timer_Config->BaseConfig.TIMERx;

    G_Timer_config = Timer_Config;

    /* Enable timer clock */
    if(TIMER1 == (Timer_Config->BaseConfig.TIMERx)){
	RCC_TIMER1_CLK_EN();
    }
    else if(TIMER2 == (Timer_Config->BaseConfig.TIMERx)){
	RCC_TIMER2_CLK_EN();
    }
    else if(TIMER3 == (Timer_Config->BaseConfig.TIMERx)){
	RCC_TIMER3_CLK_EN();
    }
    else if(TIMER4 == (Timer_Config->BaseConfig.TIMERx)){
	RCC_TIMER4_CLK_EN();
    }
    else if(TIMER5 == (Timer_Config->BaseConfig.TIMERx)){
	RCC_TIMER5_CLK_EN();
    }

    L_TIMERx->PSC = 15;

    switch (Timer_Config->OCConfig->TIMER_Channel){
    case TIMER_Channel_1:
	L_TIMERx->CCMR1 &= ~(0x7 << 4);
	L_TIMERx->CCMR1 |= TIMER_OCMODE_PWM1;
	L_TIMERx->CCMR1 |= TIMER_OC1PE;
	L_TIMERx->CCER  |= (1 << 0);
	break;

    case TIMER_Channel_2:
	L_TIMERx->CCMR1 &= ~(0x7 << 12);
	L_TIMERx->CCMR1 |= (TIMER_OCMODE_PWM1 << 8);
	L_TIMERx->CCMR1 |= TIMER_OC2PE;
	L_TIMERx->CCER  |= (1 << 4);
	break;

    case TIMER_Channel_3:
	L_TIMERx->CCMR2 &= ~(0x7 << 4);
	L_TIMERx->CCMR2 |= TIMER_OCMODE_PWM1;
	L_TIMERx->CCMR2 |= TIMER_OC3PE;
	L_TIMERx->CCER  |= (1 << 8);
	break;

    case TIMER_Channel_4:
	L_TIMERx->CCMR2 &= ~(0x7 << 12);
	L_TIMERx->CCMR2 |= (TIMER_OCMODE_PWM1 << 8);
	L_TIMERx->CCMR2 |= TIMER_OC4PE;
	L_TIMERx->CCER  |= (1 << 12);
	break;
    }

    // Enable Timer
    L_TIMERx->CR1 |= (1 << 7);
    L_TIMERx->EGR |= (1 << 0);
    MCAL_Timer_PWM_Start(Timer_Config);

    if(TIMER_IRQ_Mode_None != (Timer_Config->TIMER_IRQ_Enable)){
	L_TIMERx->DIER |= TIMER_IRQ_Mode_UIE;
	if(L_TIMERx == TIMER1){
	    NVIC_IRQ_TIMER1_UP_EN();
	}
	else if(L_TIMERx == TIMER2){
	    NVIC_IRQ_TIMER2_EN();
	}
	else if(L_TIMERx == TIMER3){
	    NVIC_IRQ_TIMER3_EN();
	}
	else if(L_TIMERx == TIMER4){
	    NVIC_IRQ_TIMER4_EN();
	}
	else if(L_TIMERx == TIMER5){
	    NVIC_IRQ_TIMER5_EN();
	}
    }
}

void MCAL_Timer_PWM_SetDuty(Timer_Config_t *cfg, uint32_t DutyCycle){
    uint32_t period = (1000000 / cfg->OCConfig->TIMER_Frequency) - 1;
    cfg->BaseConfig.TIMERx->ARR = period;

    uint32_t ARR = cfg->BaseConfig.TIMERx->ARR;
    if (DutyCycle > 100) DutyCycle = 100;

    uint32_t CCR = (DutyCycle * (ARR + 1)) / 100;

    switch (cfg->OCConfig->TIMER_Channel){
    case TIMER_Channel_1:
	cfg->BaseConfig.TIMERx->CCR1 = CCR; break;
    case TIMER_Channel_2:
	cfg->BaseConfig.TIMERx->CCR2 = CCR; break;
    case TIMER_Channel_3:
	cfg->BaseConfig.TIMERx->CCR3 = CCR; break;
    case TIMER_Channel_4:
	cfg->BaseConfig.TIMERx->CCR4 = CCR; break;
    }

    cfg->BaseConfig.TIMERx->EGR |= (1 << 0);
}

void MCAL_Timer_Encoder_Init(TIMER_TypeDef *TIMERx){
    if(TIMER1 == TIMERx){
	RCC_TIMER1_CLK_EN();
    }
    else if(TIMER2 == TIMERx){
	RCC_TIMER2_CLK_EN();
    }
    else if(TIMER3 == TIMERx){
	RCC_TIMER2_CLK_EN();
    }
    else if(TIMER4 == TIMERx){
	RCC_TIMER2_CLK_EN();
    }
    else if(TIMER5 == TIMERx){
	RCC_TIMER2_CLK_EN();
    }

    // Setting the Autoreload Register to Maximum Value for the Overflow 0-65535
    TIMERx->ARR = 0xFFFF;

    // Configuring Channel 1 & Channel 2 as Input on their Trigger
    TIMERx->CCMR1 |= ((1U << 0) | (1U << 8));

    //choosing active high trigger on channel 1 and channel 2
    TIMERx->CCER &= ~((1U << 1) | (1U << 5));
    TIMERx->CCER |=  ((1U << 0) | (1U << 4));

    // Choosing Slave Mode Selection Which Allows Counter to Counts up/down for Both Channels Depending on the Inputs
    TIMERx->SMCR |=  ((1U << 0) | (1U << 1));

    // Enabling Counter from Timer Control Register
    TIMERx->CR1 |= (1U << 0);
}

sint16_t MCAL_Timer_Encoder_GetCounts(TIMER_TypeDef *TIMERx){
    if((TIMER5 == TIMERx) || ((TIMER2 == TIMERx))){
	Encoder_Counts += (sint32_t)TIMERx->CNT;
    }
    else{
	Encoder_Counts += (sint16_t)TIMERx->CNT;
    }
    return Encoder_Counts;
}

void MCAL_Timer_Encoder_SetCounts(TIMER_TypeDef *TIMERx, uint16_t Counts){
    if((TIMER5 == TIMERx) || ((TIMER2 == TIMERx))){
	TIMERx->CNT = (sint32_t)Counts;
    }
    else{
	TIMERx->CNT = Counts;
    }
}

void MCAL_Timer_Delay(TIMER_TypeDef *Timer, float32 time, uint8_t uint_type){
    G_Timer = Timer;

    // Disable timer completely first
    Timer->CR1 = 0;
    Timer->DIER = 0;
    Timer->SR = 0;

    if(Timer == TIMER1){
        RCC_TIMER1_CLK_EN();
        NVIC_IRQ_TIMER1_UP_EN();
    }
    else if(Timer == TIMER2){
        RCC_TIMER2_CLK_EN();
        NVIC_IRQ_TIMER2_EN();
    }
    else if(Timer == TIMER3){
        RCC_TIMER3_CLK_EN();
        NVIC_IRQ_TIMER3_EN();
    }
    else if(Timer == TIMER4){
        RCC_TIMER4_CLK_EN();
        NVIC_IRQ_TIMER4_EN();
    }
    else if(Timer == TIMER5){
        RCC_TIMER5_CLK_EN();
        NVIC_IRQ_TIMER5_EN();
    }

    // Timer off
    Timer->CR1 &= ~(1<<0);

    uint32_t timer_clock = 25000000; // 25 MHz - adjust to your actual clock
    uint32_t ticks_needed;
    uint32_t user_pre = 1;
    uint32_t user_top = 0;

    // Calculate required ticks based on unit
    if(uint_type == 0){
        // Milliseconds
        ticks_needed = (uint32_t)((timer_clock / 1000.0f) * time);
    }
    else{
        // Microseconds
        ticks_needed = (uint32_t)((timer_clock / 1000000.0f) * time);
    }

    // Find optimal prescaler and reload value
    // ARR max is 65535 (16-bit) or 0xFFFFFFFF (32-bit)
    user_pre = 1;

    while(1){
        user_top = ticks_needed / user_pre;

        if(user_top <= 65535){
            break;
        }

        if(user_pre >= 65535){
            user_pre = 65535;
            user_top = ticks_needed / user_pre;
            break;
        }

        user_pre++;
    }

    // Clear update flag
    Timer->SR = 0;

    // Only Counter Overflow generates Update
    Timer->CR1 |= (1<<2);

    // Update Interrupt Enabled
    Timer->DIER |= (1<<0);

    // Set reload value (period)
    Timer->ARR = user_top - 1; // -1 because counter is 0-indexed

    // Set Prescaler
    Timer->PSC = (user_pre - 1);

    // Generate update event to load prescaler
    Timer->EGR |= (1<<0);

    // Clear the update flag that was set by EGR
    Timer->SR = 0;

    // Set delay flag
    delay_flag = 1;

    // Enable The Timer
    Timer->CR1 |= (1<<0);

    // Wait for timer interrupt to clear flag
    while(delay_flag);
}

void TIM2_IRQHandler(){
    // Clear Update Interrupt Flag
    if(G_Timer->SR & (1<<0)){
        G_Timer->SR &= ~(1<<0);

        // Clear delay flag
        delay_flag = 0;

        // Timer off
        G_Timer->CR1 &= ~(1<<0);

        // Disable Interrupt
        NVIC_IRQ_TIMER2_DIS();
    }
}

void TIM3_IRQHandler(){
    G_Timer->SR &= ~(1<<0);  // Clear interrupt flag
    delay_flag = 0;
    NVIC_IRQ_TIMER3_DIS();
    G_Timer->CR1 &= ~(1<<0); // Timer off
}

void TIM4_IRQHandler(){
    G_Timer->SR &= ~(1<<0);
    delay_flag = 0;
    NVIC_IRQ_TIMER4_DIS();
    G_Timer->CR1 &= ~(1<<0);
}

void TIM5_IRQHandler(){
    G_Timer->SR &= ~(1<<0);
    delay_flag = 0;
    NVIC_IRQ_TIMER5_DIS();
    G_Timer->CR1 &= ~(1<<0);
}
