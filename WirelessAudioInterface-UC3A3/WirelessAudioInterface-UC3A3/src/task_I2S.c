/*
 * task_I2S.c
 *
 * Created: 4/3/2013 12:05:23 AM
 *  Author: Eva
 */ 

#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "ssc_i2s.h"
#include "task_I2S.h"

//! Initializes the I2S function
void task_I2S_init(void) {
	
	static const gpio_map_t SSC_GPIO_MAP =
	{
		{PIN_SSC_RX_DATA, FUNC_SSC_RX_DATA},
		{PIN_SSC_RX_FSYNC, FUNC_SSC_RX_FSYNC},
		{PIN_SSC_RX_CLOCK, FUNC_SSC_RX_CLOCK},
		{PIN_SSC_TX_DATA, FUNC_SSC_TX_DATA},
		{PIN_SSC_TX_FSYNC, FUNC_SSC_TX_FSYNC},
		{PIN_SSC_TX_CLOCK, FUNC_SSC_TX_CLOCK}
	};
	
	gpio_enable_module(SSC_GPIO_MAP, sizeof(SSC_GPIO_MAP)/sizeof(SSC_GPIO_MAP[0]));
	
	ssc_i2s_init(&AVR32_SSC, INITIAL_BITRATE, INITIAL_BITDEPTH, 32, SSC_I2S_MODE_STEREO_OUT_STEREO_IN, FPBA_HZ);
	
	return;
}	

void task_I2S_set_mclk(uint32_t fs) {
	
	switch (fs)
	{
		case 44100:
		
		break;
		
		case 48000:
		
		break;
		
		case 88200:
		
		break;
		
		case 96000:
		
		break;
		
		case 132300:
		
		break;
		
		case 196000:
		
		break;
		
		default:
		//bad fs
		break;
	}
	return;
}