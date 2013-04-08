/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include "task_SPI.h"
#include "task_I2S.h"
#include "task_LEDs.h"
#include "CS2300.h"
#include "CS4270.h"
#include "AT42QT1110.h"
#ifdef DEBUG
	#include "print_funcs.h"
#endif

int main (void)
{
	pm_enable_osc0_crystal(&AVR32_PM, BOARD_XOSC_HZ);
	pm_enable_osc1_ext_clock(&AVR32_PM);
	
	pm_pll_setup(&AVR32_PM, 0, CONFIG_PLL0_MUL, CONFIG_PLL0_DIV, CONFIG_PLL0_SRC, CONFIG_PLL0_LOCK);
	pm_pll_setup(&AVR32_PM, 1, CONFIG_PLL1_MUL, CONFIG_PLL1_DIV, CONFIG_PLL1_SRC, CONFIG_PLL1_LOCK);
	
	sysclk_init();
	
	board_init();
	task_spi_init();
	task_I2S_init();
	task_leds_init();
	#ifdef DEBUG
		init_dbg_rs232(FPBA_HZ);
	#endif
	delay_init(FCPU_HZ);
}
