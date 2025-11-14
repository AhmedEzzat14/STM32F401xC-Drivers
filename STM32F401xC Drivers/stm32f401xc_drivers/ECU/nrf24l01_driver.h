/*
 * nrf24l01_driver.h
 *
 * Created on: Oct 29, 2025
 * Author: Ahmed Ezzat
 */

#ifndef ECU_NRF24L01_DRIVER_H_
#define ECU_NRF24L01_DRIVER_H_

//------------------------------
// Includes
//------------------------------
#include "stm32f401xc_SPI_driver.h"

typedef struct{
    uint8_t mode;
    uint8_t channel;
    uint8_t address_width;
    uint8_t power;
    uint8_t datarate;
}NRF24L01_Config_t;

//============================================================

/*
 * =======================================================
 * 		    Commands Definition
 * =======================================================
 */
#define R_REGISTER		0x00		/* Read  command and status registers. AAAAA = 5 bit Register Map Address */
#define W_REGISTER		0x20		/* Write command and status registers. AAAAA = 5 bit Register Map Address */
#define R_RX_PAYLOAD		0x61		/* Read  RX-payload: 1 – 32 bytes */
#define W_TX_PAYLOAD		0xA0		/* Write TX-payload: 1 – 32 bytes */
#define FLUSH_TX		0xE1		/* Flush TX FIFO, used in TX mode */
#define FLUSH_RX		0xE2		/* Flush RX FIFO, used in RX mode */
#define REUSE_TX_PL		0xE3		/* Used for a PTX device Reuse last transmitted payload */
#define ACTIVATE		0x50		/* This write command followed by data 0x73 activate some feautres */
#define R_RX_PL_WID		0x60		/* Read RX-payload width for the top R_RX_PAYLOAD in the RX FIFO */
#define W_ACK_PAYLOAD		0xA8		/* Used in RX mode. Write Payload to be transmitted together with ACK packet on PIPE PPP */
#define W_TX_PAYLOAD_NOACK	0x58		/* Used in TX mode. Disables AUTOACK on this specific packe */
#define NOP			0xFF		/* No Operation. Might be used to read the STATUS register */

//============================================================

/*
 * =======================================================
 * 		     Register Map Table
 * =======================================================
 */
#define CONFIG			0X00
#define EN_AA			0X01
#define EN_RXADDR		0X02
#define SETUP_AW		0X03
#define SETUP_RETR		0X04
#define RF_CH			0X05
#define RF_SETUP		0X06
#define STATUS			0X07
#define OBSERVE_TX		0X08
#define RPD_REG			0X09
#define RX_ADDR_P0		0X0A
#define RX_ADDR_P1		0X0B
#define RX_ADDR_P2		0X0C
#define RX_ADDR_P3		0X0D
#define RX_ADDR_P4		0X0E
#define RX_ADDR_P5		0X0F
#define TX_ADDR			0X10
#define RX_PW_P0		0X11
#define RX_PW_P1		0X12
#define RX_PW_P2		0X13
#define RX_PW_P3		0X14
#define RX_PW_P4		0X15
#define RX_PW_P5		0X16
#define FIFO_STATUS		0X17
#define DYNPD           	0X1C
#define FEATURE	                0X1D

//============================================================

/*
 * =======================================================
 * 		       Bits Definition
 * =======================================================
 */
#define MASK_RX_DR          6               /*mask interrupt caused by RX_DR: 1 interrupt not reflected on IRQ pin (IRQ is active low), inside CONFIG register*/
#define MASK_TX_DS          5               /*mask interrupt caused by TX_DS: 1 interrupt not reflected on IRQ pin (IRQ is active low), inside CONFIG register*/
#define MASK_MAX_RT         4               /*mask interrupt caused by MAX_RT means maximum number of retransmissions reached: 1 interrupt not reflected on IRQ pin (IRQ is active low), inside CONFIG register*/
#define EN_CRC              3               /*enale CRC, forced high if one of the bits in EN_AA is high, inside CONFIG register*/
#define CRCO                2               /*CRC encoding scheme, 0 is 1 byte, 1 is 2 bytes, inside CONFIG register*/
#define PWR_UP              1               /*1 is power up, inside CONFIG register*/
#define PRIM_RX             0               /*RX/TX control, 1: PRX, inside CONFIG register*/
#define ENAA_P5             5               /*enable auto acknowledgement data pipe 5*/
#define ENAA_P4             4
#define ENAA_P3             3
#define ENAA_P2             2
#define ENAA_P1             1
#define ENAA_P0             0
#define ERX_P5              5               /*part of EN_RXADDR, enable data pipe 5*/
#define ERX_P4              4
#define ERX_P3              3
#define ERX_P2              2
#define ERX_P1              1
#define ERX_P0              0
#define AW_1                1               /*RX/TX address field width, 00 illegal, 01 3 bytes, 10 4 bytes, 11 5 bytes*/
#define AW_0                0
#define ARD_3               7               /*auto retransmit delay, 0000 250us, 0001 500us ...> 1111 4000us*/
#define ARD_2               6
#define ARD_1               5
#define ARD_0               4
#define ARC_3               3               /*auto retransmit count, 0000 retransmit deisabled, 1111 up to 15 retransmit on failure of AA. (inside SETUP_RETR register)*/
#define ARC_2               2
#define ARC_1               1
#define ARC_0               0
#define RF_CH_6             6               /*sets the frequencvy channel nRF24L01+ operates on*/
#define RF_CH_5             5
#define RF_CH_4             4
#define RF_CH_3             3
#define RF_CH_2             2
#define RF_CH_1             1
#define RF_CH_0             0
#define CONT_WAVE           7               /*enables continuous carrier transmit when high*/
#define RF_DR_LOW           5               /*sets the RF data rate to 250kbps*/
#define PLL_LOCK            4               /*force PLL lock signal. used for testing ONLY*/
#define RF_DR_HIGH          3               /*select between high speed data rates and works ONLY when RF_DR_LOW is 0. 0 for 1Mbps, 1 for 2Mbps*/
#define RF_PWR_1            2
#define RF_PWR_0            1
#define RX_DR               6               /*IRQ for new packet in RX FIFO (newly received)*/
#define TX_DS               5               /*IRQ for ACK received in TX mode*/
#define MAX_RT              4
#define RX_P_NO_2           3
#define RX_P_NO_1           2
#define RX_P_NO_0           1
#define TX_FULL             0
#define PLOS_CNT_3          7               /*inside OBSERVE_TX register, counts the total number of retransmissions since last channel change. reset by writing to RF_CH*/
#define PLOS_CNT_2          6
#define PLOS_CNT_1          5
#define PLOS_CNT_0          4
#define ARC_CNT_3           3               /*inside OBSERVE_TX register, counts the number of retransmissions for current transaction. reset by initiating new transaction*/
#define ARC_CNT_2           2
#define ARC_CNT_1           1
#define ARC_CNT_0           0
#define RPD                 0               /*received power detector, if received power is less than -64dbm, RPD = 0*/
#define TX_REUSE            6
#define TX_FULL             5
#define TX_EMPTY            4
#define RX_FULL             1
#define RX_EMPTY            0
#define DPL_P5              5
#define DPL_P4              4
#define DPL_P3              3
#define DPL_P2              2
#define DPL_P1              1
#define DPL_P0              0                 /*must be set on PTX in dynamic payload length mode*/
#define EN_DPL              2                 /*set to enable dynamic payload length*/
#define EN_ACK_PAY          1                 /*used to enable auto acknowledgement with payload in PRX (inside FEATURE register)*/
#define EN_DYN_ACK          0                 /**/

//============================================================

/*
 * =======================================================
 * 			Generic Macros
 * =======================================================
 */

// All Delays in ms
#define StartUp_Delay		150
#define PWR_Down_Delay		2
#define StandBy_Delay		2
#define PRX_Mode_Delay		100

// All Addresses in Bytes
#define Address_Width		5
#define RF_Channel		32
#define RF_DataRate		1000
#define RF_PWR			6
#define Static_PayLoad_Width	1
#define Number_DataPipes	1
#define ReTransmit_Count	2
#define ReTransmit_Delay	500

#define OPEN                          1
#define CLOSE                         0
#define ENABLE                        1
#define DISABLE                       0
#define SPI_OFF                       1
#define SPI_ON                        0
#define CE_OFF                        0
#define CE_ON                         1

#define CONFIG_REGISTER_DEFAULT       0X08
#define EN_AA_REGISTER_DEFAULT        0X3F
#define EN_RXADDR_REGISTER_DEFAULT    0X00
#define SETUP_AW_REGISTER_DEFAULT     0X03
#define SETUP_RETR_REGISTER_DEFAULT   0X03
#define RF_CH_REGISTER_DEFAULT        0X02
#define RF_SETUP_REGISTER_DEFAULT     0X0E
#define STATUS_REGISTER_DEFAULT       0X0E
#define MAXIMUM_NUMBER_OF_DATAPIPES   6

#define POWER_DOWN                    0X00
#define STANDBYI                      0X01
#define STANDBYII                     0X02
#define PTX                           0X03
#define PRX                           0X04
#define DEVICE_NOT_INITIALIZED        0X05

#define TRANSMITTER                   0X00
#define RECEIVER                      0X01
#define POWER_SAVING                  0X02
#define TURN_OFF                      0X03

#define RESET                         1
#define NO_RESET                      0
#define NO_ACK_MODE                   1
#define ACK_MODE                      0
#define TRANSMIT_BEGIN                1
#define TRANSMIT_FAIL                 0
#define TRANSMIT_IN_PROGRESS          0
#define TRANSMIT_DONE                 1
#define TRANSMIT_FAILED               0XFF
#define OPERATION_DONE                1
#define OPERATION_ERROR               0
#define RECEIVE_FIFO_EMPTY            2
#define TX_BUFFER                     1
#define RX_BUFFER                     0

//============================================================

/*
 * =================================================
 * 	     APIs Supported by "SPI DRIVER"
 * =================================================
 */
void HAL_NRF24L01_Init(SPI_Typedef *SPIx, NRF24L01_Config_t nrf24l01_config);
void HAL_NRF24L01_Device(uint8_t DeviceMode, uint8_t ResetState);
void HAL_NRF24L01_Read(uint8_t address, uint8_t *data, uint8_t datalength, uint8_t spi_state);
void HAL_NRF24L01_Write(uint8_t address, uint8_t *data, uint8_t datalength, uint8_t spi_state);

#endif /* ECU_NRF24L01_DRIVER_H_ */
