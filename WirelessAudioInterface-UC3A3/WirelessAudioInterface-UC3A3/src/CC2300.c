/*
 * CC2300.c
 *
 * Created: 4/3/2013 10:12:23 PM
 *  Author: Eva
 */ 

#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "spi_master.h"
#include "task_SPI.h"
#include "task_clock.h"
#include "CS2300.h"

// The lowest of low level 

void CS2300_send_data(uint8_t* data, uint8_t length, uint8_t start_reg) {
	uint8_t i;
	
	spi_select_device(SPI_CLOCK, &spi_device_clock);
	
	spi_write_single(SPI_CLOCK, CS2300_ADDRESS);
	
	if (length > 1)
	{
		//If writing to multiple registers, need incr bit
		spi_write_single(SPI_CLOCK, start_reg | (1 << 7));
	} 
	else
	{
		spi_write_single(SPI_CLOCK, start_reg);
	}
	
	for (i=0; i<length; i++) {
		spi_write_single(SPI_CLOCK, data[i]);
	}
	
	spi_deselect_device(SPI_CLOCK, &spi_device_clock);
}

// Getting a bit higher!

void CS2300_set_ratio(uint32_t ratio) {
	ratio_t ratio_packet = {
		.b = ratio
	};
	CS2300_send_data(ratio_packet.c, 4, MAP_RATIO_MSB);
}

// Because laziness

uint32_t CS2300_calculate_ratio(uint32_t freq_in_hz, uint32_t freq_out_hz) {
	//Ratio = clk_out/clk_in * 2^20
	return (uint32_t)( ((uint64_t)freq_out_hz * 1048576)/ ((uint64_t)freq_in_hz) );
}

void CS2300_set_MCLK(uint32_t mclk_hz) {
	CS2300_set_ratio(CS2300_calculate_ratio(BOARD_OSC0_HZ, mclk_hz));
	return;
}

// Here's the good stuff

void CS2300_config(uint32_t fs) {
	uint32_t mclk_hz;
	
	if ((fs % 44100) == 0) mclk_hz = 22579200;
	else if ((fs % 48000) == 0) mclk_hz = 24576000;
	else return;//error
	
	// Lock PLL, disable aux and output
	device_control_t control = {
		{.unlock = 0,
		.aux_output_disable = 1,
		.clock_out_disable = 1}
	};
	
	CS2300_send_data(&control.b, 1, MAP_DEV_CTRL);
	
	// Set R_mod to x1, aux is pll lock signal
	device_config_t device_config = {
		{.r_mod_sel = R_MOD_X1,
		.aux_src_sel = PLL_LOCK,
		.enable_config = 1}
	};
	
	CS2300_send_data(&device_config.b, 1, MAP_DEV_CFG);
	
	// Update configs
	global_config_t global_config = {
		{.freeze = 0,
		.enable_config = 1}
	};
	
	CS2300_send_data(&global_config.b, 1, MAP_GLOBAL_CFG);
	
	// Set ratio
	CS2300_set_MCLK(mclk_hz);
	
	// Clock skip is enabled, unlock = high
	function_config1_t cfg1 = {
		{.clock_skip_enable = 1,
		.aux_lock_config = 0,
		.enable_config = 1}
	};
	
	CS2300_send_data(&cfg1.b, 1, MAP_FUNC_CFG1);
	
	// Clock is low when PLL is unlocked, LF ratio is high accuracy 
	function_config2_t cfg2 = {
		{.clock_out_unlock = 0,
		.lf_ratio_config = HIGH_ACCURACY}
	};
	
	CS2300_send_data(&cfg2.b, 1, MAP_FUNC_CFG2);
	
	// Clock input bandwidth is 1Hz for low jitter
	function_config3_t cfg3 = {
		{.clkin_bw = MIN_BW_1HZ}
	};
	
	CS2300_send_data(&cfg3.b, 1, MAP_FUNC_CFG3);
	
	// Enable output clock
	control.unlock = 0;
	control.aux_output_disable = 1;
	control.clock_out_disable = 0;
	
	CS2300_send_data(&control.b, 1, MAP_DEV_CTRL);
	
	return;
}