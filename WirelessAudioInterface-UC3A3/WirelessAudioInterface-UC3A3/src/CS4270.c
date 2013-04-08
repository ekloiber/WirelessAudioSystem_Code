/*
 * CS4270.c
 *
 * Created: 4/4/2013 12:11:07 AM
 *  Author: Eva
 */ 

#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "spi_master.h"
#include "task_SPI.h"
#include "task_clock.h"
#include "CS4270.h"

void CS4270_send_data(uint8_t* data, uint8_t length, uint8_t start_reg) {
	uint8_t i;
	
	spi_select_device(SPI_CODEC, &spi_device_codec);
	
	spi_write_single(SPI_CODEC, CS4270_ADDRESS_WRITE);
	
	if (length > 1)
	{
		//If writing to multiple registers, need incr bit
		spi_write_single(SPI_CODEC, start_reg | (1 << 7));
	}
	else
	{
		spi_write_single(SPI_CODEC, start_reg);
	}
	
	for (i=0; i<length; i++) {
		spi_write_single(SPI_CODEC, data[i]);
	}
	
	spi_deselect_device(SPI_CODEC, &spi_device_codec);
}

uint8_t CS4270_read_data(void) {
	
	uint8_t data;
	
	spi_select_device(SPI_CODEC, &spi_device_codec);
	
	spi_write_single(SPI_CODEC, CS4270_ADDRESS_READ);
	
	spi_read_single(SPI_CODEC, &data);
	
	spi_deselect_device(SPI_CODEC, &spi_device_codec);
	
	return data;
}

void CS4270_set_vol(uint8_t db_times_two) {
	uint8_t volume = db_times_two;
	CS4270_send_data(&volume, 1, MAP_DAC_A_VOL);
	CS4270_send_data(&volume, 1, MAP_DAC_B_VOL);
	return;
}

void CS4270_headphone_amp_mode(bool enable) {
	adc_dac_ctrl_t adc_dac_ctrl;
	
	CS4270_send_data(NULL, 0, MAP_ADC_DAC_CTRL);
	
	adc_dac_ctrl.b = CS4270_read_data();
	
	adc_dac_ctrl.dig_loopback = enable;
	
	CS4270_send_data(&adc_dac_ctrl.b, 1, MAP_ADC_DAC_CTRL);
	
	return;
}

void CS4270_power_down(void) {
	pwr_ctrl_t pwr_ctrl = {
		{	.freeze = 0,
			.power_down_adc = 1,
			.power_down_dac = 1,
			.power_down = 1	}
	};
	CS4270_send_data(&pwr_ctrl.b, 1, MAP_PWR_CTRL);
	
	return;
}

void CS4270_config(void) {
	// Freeze registers, power down adc and dac
	pwr_ctrl_t pwr_ctrl = {
		{	.freeze = 1,
			.power_down_adc = 1,
			.power_down_dac = 1,
			.power_down = 0	}
	};
	CS4270_send_data(&pwr_ctrl.b, 1, MAP_PWR_CTRL);
	
	// I2S slave, MCLK = 24.576MHz or 22.5792MHz
	mode_ctrl_t mode_ctrl = {
		{	.func_mode = SLAVE_MODE,
			.ratio_sel = RATIO_DIV2,
			.popguard = 0	}
	};
	CS4270_send_data(&mode_ctrl.b, 1, MAP_MODE_CTRL);
	
	// I2S 24b mode
	adc_dac_ctrl_t adc_dac_ctrl = {
		{	.adc_hpf_freeze_a = 0,
			.adc_hpf_freeze_b = 0,
			.dig_loopback = 0,
			.dac_dig_format = I2S_24b,
			.adc_dig_format = I2S_24b	}
	};
	CS4270_send_data(&adc_dac_ctrl.b, 1, MAP_ADC_DAC_CTRL);
	
	// Soft ramping and zero crossing enabled
	trans_ctrl_t trans_ctrl = {
		{	.dac_single_vol = 0,
			.dac_soft_ramp = 1,
			.dac_zero_cross = 1,
			.adc_inv_pol_b = 0,
			.adc_inv_pol_a = 0,
			.dac_inv_pol_b = 0,
			.dac_inv_pol_a = 0,
			.de_emphasis = 0	}
	};
	CS4270_send_data(&trans_ctrl.b, 1, MAP_TRANS_CTRL);
	
	// Nothing muted
	mute_ctrl_t mute_ctrl = {
		{	.auto_mute = 0,
			.mute_adc_b = 0,
			.mute_adc_a = 0,
			.mute_dac_b = 0,
			.mute_dac_a = 0	}
	};
	CS4270_send_data(&mute_ctrl.b, 1, MAP_MUTE_CTRL);
	
	// Volume at 0dB
	CS4270_set_vol(0);
	
	// Freeze off, power on adc, dac
	pwr_ctrl.freeze = 0;
	pwr_ctrl.power_down_adc = 0;
	pwr_ctrl.power_down_dac = 0;
	pwr_ctrl.power_down = 0;
	
	CS4270_send_data(&pwr_ctrl.b, 1, MAP_PWR_CTRL);
	
	return;
}