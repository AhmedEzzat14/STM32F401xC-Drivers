#include "stm32f401xc.h"
#include "stm32f401xc_gpio_driver.h"
#include "stm32f401xc_SPI_driver.h"
#include "stm32f401xc_RCC_driver.h"
#include "stm32f401xc_FPU_driver.h"
#include "nrf24l01_driver.h"

void Clock_Init(void) {
    RCC_GPIOA_CLK_EN();
    RCC_GPIOB_CLK_EN();
    RCC_SYSCFG_CLK_EN();
}

// --- Test Defines ---
#define TEST_LED_PORT		GPIOA
#define TEST_LED_PIN		GPIO_PIN_0

// Simple blocking delay helper
void HAL_Delay_ms(volatile uint32_t ms)
{
	for(volatile uint32_t i = 0; i < ms; i++)
	{
		for(volatile uint32_t j = 0; j < 2000; j++);
	}
}

int main(void)
{
	// 1. Enable Clocks
	RCC_GPIOA_CLK_EN();
	RCC_SPI1_CLK_EN();

	// 2. Init Test LED (PA0)
	GPIO_PinConfig_t led_config;
	led_config.GPIO_PinNumber = TEST_LED_PIN;
	led_config.GPIO_MODE = GPIO_MODE_OP;
	led_config.GPIO_TYPE = GPIO_TYPE_PP;
	led_config.GPIO_Output_Speed = GPIO_SPEED_LOW;
	led_config.GPIO_PU_PD = GPIO_PU_PD_NONE;
	MCAL_GPIO_Init(TEST_LED_PORT, &led_config);
	MCAL_GPIO_WritePin(TEST_LED_PORT, TEST_LED_PIN, GPIO_PIN_RST);

	// 3. Configure SPI
	SPI_PinConfig_t spi_config;
	spi_config.SPI_Mode = SPI_MODE_Master;
	spi_config.SPI_Communication_Mode = SPI_Communication_MODE_2Lines;
	spi_config.SPI_DataSize = SPI_MODE_8Bit;
	spi_config.SPI_FrameFormat = SPI_FrameFormat_MSB;
	spi_config.SPI_BaudRatePrescaler = SPI_BaudRate_Prescaler_8;
	spi_config.SPI_CPOL = SPI_CPOL_Idle_0;
	spi_config.SPI_CPHA = SPI_CPHA_FirstEdge;
	spi_config.SPI_NSS = SPI_NSS_SW_SSI_EN;
	spi_config.SPI_IRQ_Enable = SPI_IRQ_EN_None;
	spi_config.P_IRQ_CallBack = NULL;

	// 4. Init nRF Driver
	HAL_NRF24L01_Init(SPI1, &spi_config);

	// 5. Configure nRF Settings
	uint8_t address[] = "omar1";
	uint8_t payload_width = 32;
	uint8_t rf_channel = 76;

	// --- FIX 1: DATA RATE (1Mbps) ---
	uint8_t rf_setup_val = 0x00;
	HAL_NRF24L01_Write(RF_SETUP, &rf_setup_val, 1, CLOSE);

	// --- FIX 2: CRC MATCHING (2 Bytes) ---
	// Read current CONFIG
	uint8_t config_val = 0;
	HAL_NRF24L01_Read(CONFIG, &config_val, 1, CLOSE);
	// Set Bit 2 (CRCO) to 1 for 2-byte CRC
	config_val |= (1 << 2);
	HAL_NRF24L01_Write(CONFIG, &config_val, 1, CLOSE);

	// Standard Config
	HAL_NRF24L01_Write(TX_ADDR, address, 5, CLOSE);
	HAL_NRF24L01_Write(RX_ADDR_P0, address, 5, CLOSE);
	HAL_NRF24L01_Write(RX_PW_P0, &payload_width, 1, CLOSE);
	HAL_NRF24L01_Write(RF_CH, &rf_channel, 1, CLOSE);

	uint8_t en_bits = 1;
	HAL_NRF24L01_Write(EN_RXADDR, &en_bits, 1, CLOSE);
	HAL_NRF24L01_Write(EN_AA, &en_bits, 1, CLOSE);

	// 6. Payload
	uint8_t tx_payload[32] = "Hello from STM32! (CRC Fixed)";

	// 7. TX Mode
	HAL_NRF24L01_Device(TRANSMITTER, NO_RESET);

	// 8. Loop
	while(1)
	{
		uint8_t tx_status = HAL_NRF24L01_Transmit(tx_payload, payload_width, ACK_MODE);

		if (tx_status == TRANSMIT_DONE)
		{
			// Blink Fast Twice on Success
			MCAL_GPIO_TogglePin(TEST_LED_PORT, TEST_LED_PIN);
			HAL_Delay_ms(50);
			MCAL_GPIO_TogglePin(TEST_LED_PORT, TEST_LED_PIN);
			HAL_Delay_ms(50);
			MCAL_GPIO_TogglePin(TEST_LED_PORT, TEST_LED_PIN);
			HAL_Delay_ms(50);
			MCAL_GPIO_TogglePin(TEST_LED_PORT, TEST_LED_PIN);
		}

		HAL_Delay_ms(1000);
	}
}
