/*
 * nrf24l01_driver.c
 *
 * Created on: Oct 29, 2025
 * Author: Ahmed Ezzat
 */


/* nrf.c
 * Implements:
 *   - HAL_NRF24L01_Init
 *   - HAL_NRF24L01_Device
 *   - HAL_NRF24L01_Read
 *   - HAL_NRF24L01_Write
 *   - HAL_NRF24L01_Transmit
 *   - HAL_NRF24L01_Receive
 *   - helpers: CE/CSN control, SPI transceive wrapper, send_payload, flush...
 */

//------------------------------
// Includes
//------------------------------
#include "nrf24l01_driver.h"

/*
 * nrf24l01_driver.c
 *
 * Created on: Nov 15, 2025
 * Author: Ahmed Ezzat
 */

//------------------------------
// Includes
//------------------------------
#include "nrf24l01_driver.h"
#include "stm32f401xc_gpio_driver.h" // For CE/CSN pins

//============================================================

/*
 * =======================================================
 * Generic Variables
 * =======================================================
 */

// Stores the current operating mode (PTX, PRX, etc.)
static uint8_t G_Current_Mode = DEVICE_NOT_INITIALIZED;

//============================================================

/*
 * =======================================================
 * Generic Macros
 * =======================================================
 */

// Hardcoded CE and CSN pins, similar to your SPI driver's pin setup
// Note: This assumes SPI1 is used, so CSN is on PA4 (SPI1_NSS)
#define NRF_CSN_PORT		GPIOA
#define NRF_CSN_PIN			GPIO_PIN_4

// CE pin can be any free GPIO. Let's use PA3.
#define NRF_CE_PORT			GPIOA
#define NRF_CE_PIN			GPIO_PIN_3

//============================================================

/*
 * =======================================================
 * Generic Functions
 * =======================================================
 */

/**
 * @brief  Selects the NRF chip (CSN = LOW)
 */
static void NRF_CSN_Select(void)
    {
    MCAL_GPIO_WritePin(NRF_CSN_PORT, NRF_CSN_PIN, GPIO_PIN_RST);
    }

/**
 * @brief  Deselects the NRF chip (CSN = HIGH)
 */
static void NRF_CSN_Unselect(void)
    {
    MCAL_GPIO_WritePin(NRF_CSN_PORT, NRF_CSN_PIN, GPIO_PIN_SET);
    }

/**
 * @brief  Enables the NRF chip (CE = HIGH)
 */
static void NRF_CE_Enable(void)
    {
    MCAL_GPIO_WritePin(NRF_CE_PORT, NRF_CE_PIN, GPIO_PIN_SET);
    }

/**
 * @brief  Disables the NRF chip (CE = LOW)
 */
static void NRF_CE_Disable(void)
    {
    MCAL_GPIO_WritePin(NRF_CE_PORT, NRF_CE_PIN, GPIO_PIN_RST);
    }

/**
 * @brief  Transmits and receives a single byte over SPI using your driver
 * @retval The byte received from the SPI bus
 */
static uint8_t NRF_SPI_TransceiveByte(uint8_t data)
    {
    uint8_t buffer = data;
    // Use your SPI driver's Transceive function
    // This function relies on the G_SPIx global var set by MCAL_SPI_Init
    MCAL_SPI_Transceive(&buffer, Polling_Enable);
    return buffer;
    }

/**
 * @brief  Initializes the GPIO pins for CE and CSN
 * @note   This is the "select device" init part.
 */
static void HAL_NRF24L01_SelectDevice_Init(void)
    {
    GPIO_PinConfig_t Pin_Config;

    // 1. Configure CSN pin (PA4) as GPIO Output
    // Your MCAL_SPI_Init configures this as AF, we must override it
    // to be a software-controlled GPIO Output.
    Pin_Config.GPIO_PinNumber = NRF_CSN_PIN;
    Pin_Config.GPIO_MODE = GPIO_MODE_OP; // General Purpose Output
    Pin_Config.GPIO_TYPE = GPIO_TYPE_PP; // Push-Pull
    Pin_Config.GPIO_Output_Speed = GPIO_SPEED_HIGH;
    Pin_Config.GPIO_PU_PD = GPIO_PU_PD_PU; // Pull-up
    MCAL_GPIO_Init(NRF_CSN_PORT, &Pin_Config);

    // 2. Configure CE pin (PA3) as GPIO Output
    Pin_Config.GPIO_PinNumber = NRF_CE_PIN;
    Pin_Config.GPIO_MODE = GPIO_MODE_OP;
    Pin_Config.GPIO_TYPE = GPIO_TYPE_PP;
    Pin_Config.GPIO_Output_Speed = GPIO_SPEED_HIGH;
    Pin_Config.GPIO_PU_PD = GPIO_PU_PD_NONE; // No pull
    MCAL_GPIO_Init(NRF_CE_PORT, &Pin_Config);

    // 3. Set initial states
    NRF_CE_Disable();
    NRF_CSN_Unselect();
    }

/**
 * @brief  Writes a single-byte command to the NRF
 */
static void HAL_NRF24L01_Write_Cmd(uint8_t cmd)
    {
    NRF_CSN_Select();
    NRF_SPI_TransceiveByte(cmd);
    NRF_CSN_Unselect();
    }

/**
 * @brief  Resets the NRF registers to their default values
 */
static void HAL_NRF24L01_Reset(void)
    {
    uint8_t temp;

    NRF_CE_Disable();
    NRF_CSN_Unselect();

    // Reset all registers to default (values from reference code)
    temp = 0x08; // CONFIG_REGISTER_DEFAULT
    HAL_NRF24L01_Write(CONFIG, &temp, 1, CLOSE);
    temp = 0x3F; // EN_AA_REGISTER_DEFAULT
    HAL_NRF24L01_Write(EN_AA, &temp, 1, CLOSE);
    temp = 0x00; // EN_RXADDR_REGISTER_DEFAULT
    HAL_NRF24L01_Write(EN_RXADDR, &temp, 1, CLOSE);
    temp = 0x03; // SETUP_AW_REGISTER_DEFAULT
    HAL_NRF24L01_Write(SETUP_AW, &temp, 1, CLOSE);
    temp = 0x02; // RF_CH_REGISTER_DEFAULT
    HAL_NRF24L01_Write(RF_CH, &temp, 1, CLOSE);
    temp = 0x0E; // RF_SETUP_REGISTER_DEFAULT
    HAL_NRF24L01_Write(RF_SETUP, &temp, 1, CLOSE);
    temp = 0x0E; // STATUS_REGISTER_DEFAULT
    HAL_NRF24L01_Write(STATUS, &temp, 1, CLOSE);

    // Clear status flags by writing 1 to them
    temp = (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT);
    HAL_NRF24L01_Write(STATUS, &temp, 1, CLOSE);

    // Flush FIFOs
    HAL_NRF24L01_Write_Cmd(FLUSH_TX);
    HAL_NRF24L01_Write_Cmd(FLUSH_RX);
    }

//============================================================

/*
 * =================================================
 * APIs Supported by "NRF24L01 DRIVER"
 * =================================================
 */

/**================================================================
 * @Fn				- HAL_NRF24L01_Read (Your "read" function)
 * @brief 			- Reads bytes from an NRF24L01 register
 * @param [in]  	- address: The register address (from nrf24l01_driver.h)
 * @param [out] 	- data: Pointer to the buffer to store read data
 * @param [in] 	- datalength: Number of bytes to read
 * @param [in] 	- spi_state: CLOSE (1) to release CSN, OPEN (0) to keep it selected
 * @retval 		- none
 */
void HAL_NRF24L01_Read(uint8_t address, uint8_t *data, uint8_t datalength, uint8_t spi_state)
    {
    uint8_t cmd = R_REGISTER | address;
    uint8_t i;

    NRF_CSN_Select();
    NRF_SPI_TransceiveByte(cmd); // Send Read command + register address

    for(i = 0; i < datalength; i++)
	{
	data[i] = NRF_SPI_TransceiveByte(NOP); // Clock in data by sending NOPs
	}

    if(spi_state == CLOSE)
	{
	NRF_CSN_Unselect();
	}
    }

/**================================================================
 * @Fn				- HAL_NRF24L01_Write (Your "write" function)
 * @brief 			- Writes bytes to an NRF24L01 register
 * @param [in]  	- address: The register address (from nrf24l01_driver.h)
 * @param [in] 	- data: Pointer to the data to write
 * @param [in] 	- datalength: Number of bytes to write
 * @param [in] 	- spi_state: CLOSE (1) to release CSN, OPEN (0) to keep it selected
 * @retval 		- none
 */
void HAL_NRF24L01_Write(uint8_t address, uint8_t *data, uint8_t datalength, uint8_t spi_state)
    {
    uint8_t cmd = W_REGISTER | address;
    uint8_t i;

    NRF_CSN_Select();
    NRF_SPI_TransceiveByte(cmd); // Send Write command + register address

    for(i = 0; i < datalength; i++)
	{
	NRF_SPI_TransceiveByte(data[i]); // Clock out data
	}

    if(spi_state == CLOSE)
	{
	NRF_CSN_Unselect();
	}
    }

/**================================================================
 * @Fn				- HAL_NRF24L01_Init (Your "init" function)
 * @brief 			- Initializes the SPI peripheral and GPIOs for NRF24L01
 * @param [in]  	- SPIx: which SPI peripheral (SPI1, SPI2, etc.)
 * @param [in] 	- SPI_Config: The SPI configuration structure from your driver
 * @retval 		- none
 * @note			- This function calls your MCAL_SPI_Init and then
 * initializes CE/CSN pins.
 */
void HAL_NRF24L01_Init(SPI_Typedef *SPIx, SPI_PinConfig_t *SPI_Config)
    {
    // 1. Initialize the SPI peripheral using your driver
    // This will set up MOSI, MISO, SCK, and set the G_SPIx global var
    MCAL_SPI_Init(SPIx, SPI_Config);

    // 2. Initialize CE and CSN pins (the "select device" pins)
    // This will re-configure CSN (NSS) as a GPIO Output
    HAL_NRF24L01_SelectDevice_Init();

    // 3. Basic startup delay for NRF to power on
    MCAL_SysTicK_SetDelay_ms(StartUp_Delay);
    }

/**================================================================
 * @Fn				- HAL_NRF24L01_Device
 * @brief 			- Configures the NRF24L01 for a specific mode (TX/RX)
 * @param [in]  	- DeviceMode: TRANSMITTER (0) or RECEIVER (1)
 * @param [in] 	- ResetState: RESET (1) or NO_RESET (0)
 * @retval 		- none
 */
void HAL_NRF24L01_Device(uint8_t DeviceMode, uint8_t ResetState)
    {
    uint8_t config_val;

    if (ResetState == RESET)
	{
	HAL_NRF24L01_Reset();
	}

    MCAL_SysTicK_SetDelay_ms(PWR_Down_Delay);

    // Read current config
    HAL_NRF24L01_Read(CONFIG, &config_val, 1, CLOSE);

    // Set PWR_UP bit to enter Standby-I mode
    config_val |= (1 << PWR_UP);

    if (DeviceMode == TRANSMITTER)
	{
	// Set as PTX (PRIM_RX = 0)
	config_val &= ~(1 << PRIM_RX);
	G_Current_Mode = PTX;
	NRF_CE_Disable(); // CE is pulsed high only when transmitting
	}
    else // RECEIVER
	{
	// Set as PRX (PRIM_RX = 1)
	config_val |= (1 << PRIM_RX);
	G_Current_Mode = PRX;
	NRF_CE_Enable(); // CE stays high to listen
	}

    // Write new config
    HAL_NRF24L01_Write(CONFIG, &config_val, 1, CLOSE);
    G_Current_Mode = DeviceMode;

    // Wait for oscillator to stabilize (Tpd2stby)
    MCAL_SysTicK_SetDelay_ms(StandBy_Delay);
    }

/**================================================================
 * @Fn				- HAL_NRF24L01_Transmit (Your "tx" function)
 * @brief 			- Transmits a payload in PTX mode (blocking)
 * @param [in]  	- payload: Pointer to the data buffer
 * @param [in] 	- payload_width: Number of bytes to send (1-32)
 * @param [in] 	- ack_mode: ACK_MODE (0) or NO_ACK_MODE (1)
 * @retval 		- uint8_t: TRANSMIT_DONE, TRANSMIT_FAILED
 */
uint8_t HAL_NRF24L01_Transmit(uint8_t *payload, uint8_t payload_width, uint8_t ack_mode){
    uint8_t status;
    uint8_t cmd;
    uint8_t i;

    if (G_Current_Mode != TRANSMITTER)
	return TRANSMIT_FAIL; // Must be in PTX mode

    // 1. Check if TX FIFO is full (using the new unique name)
    HAL_NRF24L01_Read(STATUS, &status, 1, CLOSE);
    if (status & (1 << STATUS_TX_FULL))
	{
	return TRANSMIT_FAIL; // TX FIFO is full, can't send
	}

    // 2. Select payload command (with or without ACK)
    if (ack_mode == NO_ACK_MODE)
	{
	cmd = W_TX_PAYLOAD_NOACK;
	}
    else
	{
	cmd = W_TX_PAYLOAD;
	}

    // 3. Load payload into TX FIFO
    NRF_CSN_Select();
    NRF_SPI_TransceiveByte(cmd); // Send TX Payload command
    for (i = 0; i < payload_width; i++)
	{
	NRF_SPI_TransceiveByte(payload[i]); // Send payload bytes
	}
    NRF_CSN_Unselect();

    // 4. Pulse CE high for > 10us to start transmission
    NRF_CE_Enable();
    MCAL_SysTicK_SetDelay_ms(1);
    NRF_CE_Disable();

    // 5. Wait for transmission to complete (blocking poll)
    while(1)
	{
	HAL_NRF24L01_Read(STATUS, &status, 1, CLOSE);

	if (status & (1 << TX_DS))
	    {
	    // Success! Clear the flag
	    status |= (1 << TX_DS);
	    HAL_NRF24L01_Write(STATUS, &status, 1, CLOSE);
	    return TRANSMIT_DONE;
	    }

	if (status & (1 << MAX_RT))
	    {
	    // Failed (Max Retransmits). Clear the flag.
	    status |= (1 << MAX_RT);
	    HAL_NRF24L01_Write(STATUS, &status, 1, CLOSE);
	    HAL_NRF24L01_Write_Cmd(FLUSH_TX); // Flush TX FIFO
	    return TRANSMIT_FAILED;
	    }
	}
}

/**================================================================
 * @Fn				- HAL_NRF24L01_Receive (Your "rx" function)
 * @brief 			- Receives a payload if available in PRX mode
 * @param [out] 	- payload: Pointer to a buffer to store the data
 * @param [in] 	- payload_width: Static payload width you configured
 * @retval 		- uint8_t: OPERATION_DONE (1) if data received,
 * RECEIVE_FIFO_EMPTY (2) if not
 */
uint8_t HAL_NRF24L01_Receive(uint8_t *payload, uint8_t payload_width)
    {
    uint8_t status, i;

    if (G_Current_Mode != RECEIVER)
	return OPERATION_ERROR; // Must be in PRX mode

    // 1. Check STATUS register for RX_DR (Data Ready) flag
    HAL_NRF24L01_Read(STATUS, &status, 1, CLOSE);

    if (status & (1 << RX_DR))
	{
	// Data is available!
	// 2. Read payload from RX FIFO
	NRF_CSN_Select();
	NRF_SPI_TransceiveByte(R_RX_PAYLOAD); // Send Read RX Payload command
	for (i = 0; i < payload_width; i++)
	    {
	    payload[i] = NRF_SPI_TransceiveByte(NOP); // Read payload bytes
	    }
	NRF_CSN_Unselect();

	// 3. Clear the RX_DR flag by writing 1 to it
	status |= (1 << RX_DR);
	HAL_NRF24L01_Write(STATUS, &status, 1, CLOSE);

	// 4. Flush RX FIFO if it's somehow full (optional, but good practice)
	HAL_NRF24L01_Read(FIFO_STATUS, &status, 1, CLOSE);
	if (status & (1 << RX_FULL))
	    {
	    HAL_NRF24L01_Write_Cmd(FLUSH_RX);
	    }

	return OPERATION_DONE;
	}
    else
	{
	// No data available
	return RECEIVE_FIFO_EMPTY;
	}
    }
