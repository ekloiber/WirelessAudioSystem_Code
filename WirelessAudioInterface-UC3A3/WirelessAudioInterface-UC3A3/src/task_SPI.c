/*
 * task_SPI.c
 *
 * Created: 4/2/2013 11:47:51 PM
 *  Author: Eva
 */ 

#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "spi_master.h"
#include "task_SPI.h"

//! Initializes the SPI function
void task_spi_init(void) {
	
	spi_master_init(&AVR32_SPI0);
	
	static const gpio_map_t SPI_GPIO_MAP =
	{
		{PIN_SCLK0,	FUNC_SCLK0},
		{PIN_MISO0,	FUNC_MISO0},
		{PIN_MOSI0,	FUNC_MOSI0},
		{PIN_SCLK1,	FUNC_SCLK1},
		{PIN_MISO1,	FUNC_MISO1},
		{PIN_MOSI1,	FUNC_MOSI1},
		{PIN_CC8530_nCS, FUNC_CC8530_nCS},
		{PIN_CODEC_nCS, FUNC_CODEC_nCS},
		{PIN_CLOCK_nCS, FUNC_CLOCK_nCS},
		{PIN_TOUCH_nCS, FUNC_TOUCH_nCS},
	};
	
	gpio_enable_module(SPI_GPIO_MAP, sizeof(SPI_GPIO_MAP)/sizeof(SPI_GPIO_MAP[0]));
		
	spi_device_cc8530.id = ID_CC8530_nCS;
	spi_device_codec.id = ID_CODEC_nCS;
	spi_device_clock.id = ID_CLOCK_nCS;
		
	// CC8530: fmax=20MHz, 2M bytes/s, CPOL=0, CPHA=0
	spi_master_setup_device(&AVR32_SPI0, &spi_device_cc8530, SPI_MODE_0, 12000000, 0);
	// CODEC: fmax=6MHz, CPOL=0, CPHA=0
	spi_master_setup_device(&AVR32_SPI0, &spi_device_codec, SPI_MODE_0, 4000000, 0);
	// Clock: fmax=6MHz, CPOL=0, CPHA=0
	spi_master_setup_device(&AVR32_SPI0, &spi_device_clock, SPI_MODE_0, 4000000, 0);
		
		
	spi_master_init(&AVR32_SPI1);
		
	spi_device_touch.id = ID_TOUCH_nCS;
		
	// AT42QT1110: fmax=1.5MHz, CPOL=1, CPHA=1
	spi_master_setup_device(&AVR32_SPI1, &spi_device_touch, SPI_MODE_3, 1000000, 0);
	
	return;
}

//! Starts SPI on both channels
void task_spi_start(void) {
	spi_enable(&AVR32_SPI0);
	spi_enable(&AVR32_SPI1);
}