/**
 * \file
 *
 * \brief User board initialization template
 *
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	
	// Configure pin directions of GPIO
	gpio_configure_pin(PIN_CC8530_nRESET, GPIO_DIR_OUTPUT);
	gpio_configure_pin(PIN_CC8530_nPAIR, GPIO_DIR_OUTPUT);
	
	gpio_configure_pin(PIN_PHONE_SENSE, GPIO_DIR_INPUT);
	gpio_configure_pin(PIN_LINEIN_SENSE, GPIO_DIR_INPUT);
	
	gpio_configure_pin(PIN_SD_nIN, GPIO_DIR_INPUT | GPIO_PULL_UP);
	
	gpio_configure_pin(PIN_TOUCH_nCHANGE, GPIO_DIR_INPUT | GPIO_PULL_UP);

	gpio_configure_pin(PIN_CODEC_nMUTE_L, GPIO_DIR_OUTPUT);
	gpio_configure_pin(PIN_CODEC_nMUTE_R, GPIO_DIR_OUTPUT);
	gpio_configure_pin(PIN_CODEC_nRESET, GPIO_DIR_OUTPUT);
	
	gpio_configure_pin(PIN_nLED0, GPIO_DIR_OUTPUT);
	gpio_configure_pin(PIN_nLED1, GPIO_DIR_OUTPUT);
	gpio_configure_pin(PIN_nGPIO, GPIO_DIR_INPUT | GPIO_PULL_UP);
	
	// Default pin states
	gpio_set_pin_low(PIN_CC8530_nRESET);
	gpio_set_pin_high(PIN_CC8530_nPAIR);
	gpio_set_pin_low(PIN_CODEC_nRESET);
	gpio_set_pin_high(PIN_CODEC_nMUTE_L);
	gpio_set_pin_high(PIN_CODEC_nMUTE_R);
}
