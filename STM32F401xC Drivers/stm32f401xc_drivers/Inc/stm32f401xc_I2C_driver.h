/*
 * stm32f401xc_I2C_driver.h
 *
 * Created on: Sep 19, 2025
 * Author: Ahmed Ezzat
 */

#ifndef INC_STM32F401XC_I2C_DRIVER_H_
#define INC_STM32F401XC_I2C_DRIVER_H_

//------------------------------
// Includes
//------------------------------
#include "stm32f401xc.h"
#include "stm32f401xc_gpio_driver.h"

//============================================================

//------------------------------------------
// User Type Definitions (Structures)
//------------------------------------------
typedef enum{ Master_EV_SB, Master_EV_ADDR_Sent, Master_EV_ADD10_Sent, Master_EV_RxNE, Master_EV_TxE, Master_ERR_ARLO, Master_ERR_AF }Master_State;

typedef enum{ Slave_EV_STOPF, Slave_EV_ADDR_Sent, Slave_EV_ADD10_Sent, Slave_EV_RxNE, Slave_EV_TxE, Slave_ERR_AF }Slave_State;

typedef enum{ Without_Stop, With_Stop }Stop_Condition;

typedef enum{ Start, Repeated_Start }Start_Type;

typedef enum {I2C_Direction_Transmitter, I2C_Direction_Recieve}Direction;

typedef enum{ StateDisable, StateEnable }Function_States;

typedef enum{ RST, SET }Flag_Status;

typedef enum{ BUSY_FLAG, EV5_FLAG, EV6_FLAG, EV7_FLAG, EV8_FLAG, EV8_1_FLAG, EVENT_MASTER_BYTE_TRANSMITTING = ((uint32_t)0x00070080) }Status;

typedef struct{
    uint16_t	I2C_Dual_Address_EN;
    uint16_t	I2C_Primary_Slave_Address;
    uint16_t	I2C_Secondary_Slave_Address;
    uint32_t 	I2C_Addressing_Slave_Mode; 		// @ref I2C_Addressing_Slave_define
}I2C_Slave_Device_Address;

typedef struct{
    uint32_t						I2C_CLK_Speed			 	; 	    // Specifies the clock frequency.
    // This parameter must be set to @I2C_SCLK_define

    uint32_t 						I2C_ACK_Control			 	;  		// Enables or disables the acknowledgement.
    // This parameter can be a value of @ref I2C_Ack_define

    uint32_t						I2C_CLK_StretchMode		 	;		// Specifies the Stretch Mode
    // @ref I2C_CLK_StretchMode_define

    uint32_t 						I2C_General_Call_Address 	;		// Set the general address for devices
    // @ref I2C_General_Address_define

    uint32_t 						I2C_Mode					;		// Specifies the I2C mode.
    // ref I2C_Mode_define

    I2C_Slave_Device_Address I2C_Slave_Address					;

    void(* P_Master_Event_CallBack)(Master_State state)			;		// Set the C Function() which will be called once the IRQ  Happen (Master)
    void(* P_Slave_Event_CallBack)(Slave_State state)			;		// Set the C Function() which will be called once the IRQ  Happen (Slave)

}I2C_InitTypedef;

//============================================================

/*
 * =======================================================
 * 					  Generic Macros
 * =======================================================
 */

#define I2C_EVENT_MASTER_BYTE_TRANSMITTING                 ((uint32_t)0x00070080) /* TRA, BUSY, MSL, TXE flags */

//============================================================

//-------------------------
// Reference Macros
//-------------------------

// @ref I2C_SCLK_define
// – Standard Speed (up to 100 kHz)
// – Fast Speed (up to 400 kHz)
// • Configure Clock before enable the peripheral
// ---I2C_CR2.FREQ[5:0]: Peripheral clock frequency
// • Configure the clock control registers
//	 		 Thigh = CCR * TPCLK1
//			 SM or FM
// • Configure the rise time register I2C_TRISE
#define I2C_SCLK_SM_50K								(50000U)
#define I2C_SCLK_SM_100K							(100000U)
#define I2C_SCLK_FM_200K							(200000U)
#define I2C_SCLK_FM_400K							(400000U)

// @ref I2C_Ack_define
/* I2C_CR1 */
// Bit 10 ACK: Acknowledge enable
//	0: No acknowledge returned
//	1: Acknowledge returned after a byte is received (matched address or data)
#define I2C_Ack_DIS									(uint16_t)(0)
#define I2C_Ack_EN									I2C_CR1_ACK

// @ref I2C_CLK_StretchMode_define
/* I2C_CR1 */
// Bit 7 NOSTRETCH: Clock stretching disable (Slave mode)
//	0: Clock stretching enabled
//	1: Clock stretching disabled
#define I2C_CLK_StretchMode_DIS						(uint16_t)(0)
#define I2C_CLK_StretchMode_EN						I2C_CR1_NOSTRETCH

// @ref I2C_General_Addres_define
/* I2C_CR1 */
// Bit 6 ENGC: General call enable
//	0: General call disabled. Address 00h is NACKed.
//	1: General call enabled. Address 00h is ACKed.
#define I2C_General_Addres_DIS						(uint16_t)(0)
#define I2C_General_Addres_EN						I2C_CR1_ENGC

// ref I2C_Mode_define
/* I2C_CR1 */
// Bit 1 SMBUS: SMBus mode
//	0: I2C mode
//	1: SMBus mode
#define I2C_Mode_I2C								(uint16_t)(0)
#define I2C_Mode_SMBUS								I2C_CR1_SMBUS

// @ref I2C_Addressing_Slave_define
/* I2C_OAR1 */
// Bit 15 ADDMODE Addressing mode (slave mode)
//	0: 7-bit slave address (10-bit address not acknowledged)
//	1: 10-bit slave address (7-bit address not acknowledged)
#define I2C_Addressing_Slave_7Bits					(uint16_t)(0)
#define I2C_Addressing_Slave_10Bits					I2C_OAR1_ADDMODE

//============================================================

/*
 * =================================================
 * 		   APIs Supported by "I2C DRIVER"
 * =================================================
 */
void MCAL_I2C_Init(I2C_TypeDef * I2Cx, I2C_InitTypedef *I2C_Config);
void MCAL_I2C_DeInit(I2C_TypeDef * I2Cx);

void MCAL_I2C_Master_Tx(I2C_TypeDef * I2Cx, uint16_t DevAdd, uint8_t *Data, uint32_t DataLen, Stop_Condition Stop, Start_Type Start);
void MCAL_I2C_Master_Rx(I2C_TypeDef * I2Cx, uint16_t DevAdd, uint8_t *Data, uint32_t DataLen, Stop_Condition Stop, Start_Type Start);

void MCAL_I2C_SlaveSendData(I2C_TypeDef* I2Cx,uint8_t data);
uint8_t MCAL_I2C_SlaveReceiveData(I2C_TypeDef* I2Cx);

#endif /* INC_STM32F401XC_I2C_DRIVER_H_ */
