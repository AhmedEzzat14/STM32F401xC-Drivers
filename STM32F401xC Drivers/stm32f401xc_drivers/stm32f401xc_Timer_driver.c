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
Timer_Config_t *G_Timer_config = NULL;

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

    // Update
    cfg->BaseConfig.TIMERx->EGR |= (1 << 0);
}

void MCAL_Timer_Encoder_Init(Timer_Config_t *Timer_Config){
	/* TODO: implement encoder mode init (not implemented yet) */
}

sint16_t MCAL_Timer_Encoder_GetCounts(Timer_Config_t *Timer_Config){
	/* TODO: implement */
	return 0;
}

void MCAL_Timer_Encoder_SetCounts(Timer_Config_t *Timer_Config, uint16_t Counts){
	/* TODO: implement */
}


