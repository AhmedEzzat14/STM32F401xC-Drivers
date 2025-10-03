/*
 * stm32f401xc_Timer_driver.h
 *
 * Created on: Sep 1, 2025
 * Author: Ahmed Ezzat
 */

#ifndef INC_STM32F401XC_TIMER_DRIVER_H_
#define INC_STM32F401XC_TIMER_DRIVER_H_

//------------------------------
// Includes
//------------------------------
#include "stm32f401xc_gpio_driver.h"

//============================================================

//------------------------------------------
// User Type Definitions (Structures)
//------------------------------------------
typedef struct{
    uint8_t TXE  	 : 1; // TX buffer empty interrupt
    uint8_t RXNE 	 : 1; // RX buffer not empty interrupt
    uint8_t TCE 	 : 1; // Transmission complete interrupt
    uint8_t Reserved : 5;
}S_TIMER_IRQ_SRC;

typedef struct{
    TIMER_TypeDef 		*TIMERx					 	;
    uint8_t		   	TIMER_CounterMode		 		;
    uint16_t	   	TIMER_Prescaler			 		;
    uint16_t	   	TIMER_AutoReload			 	;
    uint8_t		   	TIMER_CLKDivision		 		;
    uint8_t		   	TIMER_RepetitionCounter	 		;
}Timer_BaseConfig_t;

typedef struct{
    uint8_t 	TIMER_Channel			 		;
    uint8_t  	TIMER_OutputCompare_Mode	 	;
    uint32_t  	TIMER_OutputCompare_Pulse		;
    uint8_t  	TIMER_OutputCompare_Polarity	;
    uint8_t  	TIMER_Complement_Output	 		;
    uint32_t 	TIMER_Frequency					;
}Timer_OutputCompare_Config_t;

typedef struct{
    uint8_t 	TIMER_Channel			 		;
    uint8_t  	TIMER_InputCapture_Filter 		;
    uint8_t  	TIMER_InputPrescaler 	 		;
    uint8_t  	TIMER_InputPolarity 			;
    uint8_t 	TIMER_InputSelection			;
}Timer_InputCapture_Config_t;

typedef struct{
    uint8_t  	TIMER_Encoder_Mode 		 		;
    uint8_t  	TIMER_Input1Polarity 			;
    uint8_t  	TIMER_InputCapture1_Filter 		;
    uint8_t  	TIMER_Input2Polarity 			;
    uint8_t  	TIMER_InputCapture2_Filter 		;
}Timer_Encoder_Config_t;

typedef struct {
    Timer_BaseConfig_t 				BaseConfig		 ;
    Timer_OutputCompare_Config_t   	*OCConfig		 ;
    Timer_InputCapture_Config_t	   	*ICConfig		 ;
    Timer_Encoder_Config_t 			*EncoderConfig	 ;
    uint8_t  	   					TIMER_IRQ_Enable ;
    uint8_t 	   					TIMER_DMA_Enable ;
    void (*P_IRQ_Callback)(S_TIMER_IRQ_SRC irq_src)	 ;
} Timer_Config_t;

//============================================================

//-------------------------
// Reference Macros
//-------------------------

// @ref TIMER_OCxPE (Output Compare preload enable)
// Bit 3 OC1PE, Bit 11 OC2PE, Bit 3 OC3PE (CCMR2), Bit 11 OC4PE (CCMR2)
#define TIMER_OC1PE   (1 << 3)
#define TIMER_OC2PE   (1 << 11)
#define TIMER_OC3PE   (1 << 3)
#define TIMER_OC4PE   (1 << 11)

// @ref TIMER_Counter_Mode_define
// Bit 4 DIR: Direction
//	0: Counter used as upcounter
//	1: Counter used as downcounter
/* Note: This bit is read only when the timer is configured in Center-aligned mode or Encoder mode */

// Bits 6:5 CMS[1:0]: Center-aligned mode selection
//	00: Edge-aligned mode. The counter counts up or down depending on the direction bit (DIR).
//	01: Center-aligned mode 1. The counter counts up and down alternatively. Output compare interrupt flags of channels configured in output (CCxS=00 in TIMx_CCMRx register) are set only when the counter is counting down.
//	10: Center-aligned mode 2. The counter counts up and down alternatively. Output compare interrupt flags of channels configured in output (CCxS=00 in TIMx_CCMRx register) are set only when the counter is counting up.
//	11: Center-aligned mode 3. The counter counts up and down alternatively. Output compare interrupt flags of channels configured in output (CCxS=00 in TIMx_CCMRx register) are set both when the counter is counting up or down.
/* It is not allowed to switch from edge-aligned mode to center-aligned mode as long as the counter is enabled (CEN=1) */
#define TIMER_Counter_Mode_CounterUp			(uint32_t)(0)
#define TIMER_Counter_Mode_CounterDown			(uint32_t)(1 << 4)
#define TIMER_Counter_Mode_CenterAligned1		(uint32_t)(1 << 5)
#define TIMER_Counter_Mode_CenterAligned2		(uint32_t)(2 << 5)
#define TIMER_Counter_Mode_CenterAligned3		(uint32_t)(3 << 5)

// @ref TIMER_CLKDivision_define
// Bits 9:8 CKD[1:0]: Clock division
//	00: tDTS=tCK_INT
//	01: tDTS=2*tCK_INT
//	10: tDTS=4*tCK_INT
//	11: Reserved, do not program this value
/* This bit-field indicates the division ratio between the timer clock (CK_INT) frequency and the
dead-time and sampling clock (tDTS)used by the dead-time generators and the digital filters
(ETR, TIx) */
#define TIMER_CLKDivision1						(uint32_t)(0)
#define TIMER_CLKDivision2						(uint32_t)(1 << 8)
#define TIMER_CLKDivision4						(uint32_t)(2 << 8)

// @ref TIMER_OCMODE_define
// Bits 6:4 OC1M: Output Compare 1 mode
//	000: Frozen - The comparison between the output compare register TIMx_CCR1 and the counter TIMx_CNT has no effect on the outputs.(this mode is used to generate a timing base).
//  001: Set channel 1 to active level on match. OC1REF signal is forced high when the counter TIMx_CNT matches the capture/compare register 1 (TIMx_CCR1).
//	010: Set channel 1 to inactive level on match. OC1REF signal is forced low when the counter TIMx_CNT matches the capture/compare register 1 (TIMx_CCR1).
//	011: Toggle - OC1REF toggles when TIMx_CNT=TIMx_CCR1.
//	100: Force inactive level - OC1REF is forced low.
//	101: Force active level - OC1REF is forced high.
//	110: PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1 else inactive. In downcounting, channel 1 is inactive (OC1REF=‘0’) as long as TIMx_CNT>TIMx_CCR1 else active (OC1REF=’1’).
//	111: PWM mode 2 - In upcounting, channel 1 is inactive as long as TIMx_CNT<TIMx_CCR1 else active. In downcounting, channel 1 is active as long as TIMx_CNT>TIMx_CCR1 else inactive.
/* These bits define the behavior of the output reference signal OC1REF from which OC1 and
OC1N are derived. OC1REF is active high whereas OC1 and OC1N active level depends
on CC1P and CC1NP bits */
/* Note: 1: These bits can not be modified as long as LOCK level 3 has been programmed
(LOCK bits in TIMx_BDTR register) and CC1S=’00’ (the channel is configured in
output).
2: In PWM mode 1 or 2, the OCREF level changes only when the result of the
comparison changes or when the output compare mode switches from “frozen” mode
to “PWM” mode.
3: On channels having a complementary output, this bit field is preloaded. If the CCPC
bit is set in the TIMx_CR2 register then the OC1M active bits take the new value from
the preloaded bits only when a COM event is generated. */
#define TIMER_OCMODE_Frozen						(uint32_t)(0)
#define TIMER_OCMODE_Active						(uint32_t)(1 << 4)
#define TIMER_OCMODE_InActive					(uint32_t)(2 << 4)
#define TIMER_OCMODE_Toggle						(uint32_t)(3 << 4)
#define TIMER_OCMODE_Force_InActive				(uint32_t)(4 << 4)
#define TIMER_OCMODE_Force_Active				(uint32_t)(5 << 4)
#define TIMER_OCMODE_PWM1						(uint32_t)(6 << 4)
#define TIMER_OCMODE_PWM2						(uint32_t)(7 << 4)

// @ref TIMER_Channel_define
#define TIMER_Channel_1							(uint32_t)(1)
#define TIMER_Channel_2							(uint32_t)(2)
#define TIMER_Channel_3							(uint32_t)(3)
#define TIMER_Channel_4							(uint32_t)(4)

// @ref TIMER_OutputCompare_Polarity
// Bit 1 CC1P: Capture/Compare 1 output polarity
// Bit 5 CC2P: Capture/Compare 2 output polarity
// Bit 9 CC3P: Capture/Compare 3 output polarity
// Bit 13 CC4P: Capture/Compare 4 output polarity
//	0: OC1 active high
//	1: OC1 active low
#define TIMER_OutputCompare_Polarity_High		(uint32_t)(0)
#define TIMER_OutputCompare_Polarity_Low_CH1	(uint32_t)(1 << 1)
#define TIMER_OutputCompare_Polarity_Low_CH2	(uint32_t)(1 << 5)
#define TIMER_OutputCompare_Polarity_Low_CH3	(uint32_t)(1 << 9)
#define TIMER_OutputCompare_Polarity_Low_CH4	(uint32_t)(1 << 13)

// @ref TIMER_InputPolarity
// CC1NP/CC1P bits select the active polarity of TI1FP1 and TI2FP1 for trigger or capture operations.
//	00: non-inverted/rising edge
//	01: inverted/falling edge
//	10: reserved, do not use this configuration.
//	11: non-inverted/both edges
#define TTIMER_InputPolarity_Rising			(uint32_t)(0)
#define TIMER_InputPolarity_Falling			(uint32_t)(1 << 1)
#define TIMER_InputPolarity_Rising_Falling	(uint32_t)((1 << 1) | (1 << 3))

// @ref TIMER_Encoder_Mode_define
// Bits 2:0 SMS: Slave mode selection
//	000: Slave mode disabled
//	001: Encoder mode 1
//	010: Encoder mode 2
//	011: Encoder mode 3
//	100: Reset Mode - Rising edge of the selected trigger input (TRGI) reinitializes the counter
//	101: Gated Mode - The counter clock is enabled when the trigger input (TRGI) is high. The
//	110: Trigger Mode - The counter starts at a rising edge of the trigger TRGI (but it is notS
//	111: External Clock Mode 1 - Rising edges of the selected trigger (TRGI) clock the counter.
#define TIMER_Encoder_Mode_DIS				(uint32_t)(0)
#define TIMER_Encoder_Mode1					(uint32_t)(1 << 0)
#define TIMER_Encoder_Mode2					(uint32_t)(2 << 0)
#define TIMER_Encoder_Mode3					(uint32_t)(3 << 0)

// @ref TIMER_Encoder_Mode_define
// Bit 7 BIE: Break interrupt enable
//	0: Break interrupt disabled
//	1: Break interrupt enabled

// Bit 6 TIE: Trigger interrupt enable
//	0: Trigger interrupt disabled
//	1: Trigger interrupt enabled

//Bit 4 CC4IE: Capture/Compare 4 interrupt enable
//	0: CC4 interrupt disabled
//	1: CC4 interrupt enabled

// Bit 3 CC3IE: Capture/Compare 3 interrupt enable
//	0: CC3 interrupt disabled
//	1: CC3 interrupt enabled

// Bit 2 CC2IE: Capture/Compare 2 interrupt enable
//	0: CC2 interrupt disabled
//	1: CC2 interrupt enabled

// Bit 1 CC1IE: Capture/Compare 1 interrupt enable
//	0: CC1 interrupt disabled
//	1: CC1 interrupt enabled

// Bit 0 UIE: Update interrupt enable
//	0: Update interrupt disabled
//	1: Update interrupt enabled
#define TIMER_IRQ_Mode_None					(uint32_t)(0)
#define TIMER_IRQ_Mode_UIE					(uint32_t)(1 << 0)
#define TIMER_IRQ_Mode_CC1IE					(uint32_t)(1 << 1)
#define TIMER_IRQ_Mode_CC2IE					(uint32_t)(1 << 2)
#define TIMER_IRQ_Mode_CC3IE					(uint32_t)(1 << 3)
#define TIMER_IRQ_Mode_CC4IE					(uint32_t)(1 << 4)
#define TIMER_IRQ_Mode_TIE					(uint32_t)(1 << 6)
#define TIMER_IRQ_Mode_BIE					(uint32_t)(1 << 7)

//============================================================

/*
 * ===================================================
 * 		 APIs Supported by "MCAL Timer DRIVER"
 * ===================================================
 */
void MCAL_Timer_Init(Timer_Config_t *Timer_Config);
void MCAL_Timer_DeInit(Timer_Config_t *Timer_Config);

void MCAL_Timer_PWM_Init(Timer_Config_t *Timer_Config, uint32_t DutyCycle);
void MCAL_Timer_PWM_SetDuty(Timer_Config_t *cfg, uint32_t DutyCycle);

void MCAL_Timer_Encoder_Init(Timer_Config_t *Timer_Config);
sint16_t MCAL_Timer_Encoder_GetCounts(Timer_Config_t *Timer_Config);
void MCAL_Timer_Encoder_SetCounts(Timer_Config_t *Timer_Config, uint16_t Counts);

void Timer_Delay(TIMER_TypeDef *Timer, float32 time, uint8_t uint);

/*
 * =======================================================
 * 		   Generic Macros
 * =======================================================
 *
 */
#define Unit_ms					0
#define Unit_us					1


//============================================================

#endif /* INC_STM32F401XC_TIMER_DRIVER_H_ */
