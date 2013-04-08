/*
 * AT42QT1110.c
 *
 * Created: 4/5/2013 7:25:28 PM
 *  Author: Eva
 */ 

#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "spi_master.h"
#include "task_SPI.h"
#include "AT42QT1110.h"

uint8_t AT42QT1110_calc_crc(uint8_t* data, uint8_t length) {
	uint8_t index;
	uint8_t fb;
	uint8_t i;
	uint8_t crc = 0;
	
	for (i=0; i<length; i++) {
		index = 8;
		do {
			fb = (crc ^ data[i]) & 0x01;
			data[i] >>= 1;
			crc >>= 1;
			if (fb) crc ^= 0x8C;
		} while (--index);
	}
	
	return crc;
}

void AT42QT1110_spi_read_timeout(uint8_t* data, uint32_t timeout_us) {
	uint32_t t = 0;

	while (!spi_is_rx_full(SPI_TOUCH)) {
		delay_us(1);
		if(t++ > timeout_us) return;
	}
	t = 0;
	spi_read_single(&SPI_TOUCH, data);
	
	return;
}

void AT42QT1110_send_data(uint8_t* data, uint8_t length) {
	uint8_t i;
	
	spi_select_device(SPI_TOUCH, &spi_device_touch);
	
	for (i=0; i<length; i++) {
		delay_us(150);
		spi_write_single(SPI_TOUCH, data[i]);
	}
	
	spi_deselect_device(SPI_TOUCH, &spi_device_touch);
	
	return;
}

uint8_t AT42QT1110_send_cmd(uint8_t cmd) {
	uint8_t data;
	
	spi_select_device(SPI_TOUCH, &spi_device_touch);
	
	spi_write_single(SPI_TOUCH, cmd);
	
	data = AT42QT1110_spi_read_timeout(&data, 100);
	
	spi_deselect_device(SPI_TOUCH, &spi_device_touch);
	
	return data;
}

void AT42QT1110_get_data(uint8_t cmd, uint8_t* data, uint8_t length) {
	uint8_t idle;
	uint8_t i;
	
	spi_select_device(SPI_TOUCH, &spi_device_touch);
	
	idle = AT42QT1110_send_cmd(cmd);
	
	if (idle != 0x55) return;
	
	for (i=0; i<length; i++) {
		delay_us(150);
		spi_write_single(SPI_TOUCH, 0x00);
		AT42QT1110_spi_read_timeout(data, 100);
	}	
	
	spi_deselect_device(SPI_TOUCH, &spi_device_touch);
	
	return;
}

uint32_t AT42QT1110_get_key(uint32_t key) {
	all_keys_t all_keys;
	
	AT42QT1110_get_data(RPT_REQ_SEND_ALL_KEYS, all_keys.b, LENGTH_SEND_ALL_KEYS);
	
	return ((all_keys.c & (1 << key)) ? 1 : 0);
}

int32_t AT42QT1110_init(void) {
	setups.a = {
		.device_mode = {
			.key_ac = KEY_AC_SRC_TIMED,
			.mode = MODE_11_KEY,
			.signal_mode = SIGNAL_PARALLEL,
			.sync_trig = SYNC_TRIG_EDGE,
			.repeat_time = 2	//32ms
		},
		.guard_key_com_opts = {
			.guard_key = 0,
			.guard_key_en = 0,
			.quick_spi_en = 0,
			.chg_mode = CHG_MODE_DATA,
			.crc_en = 0
		},
		.dil_dht = {
			.dil = 3,
			.dht = 8
		},
		.pthr_physt = {
			.pthr = 4,
			.physt = 2	//25%
		},
		.pdrift = {
			.pdrift = 6	//960ms
		},
		.prd = 6,	//960ms
		.lbl = 18, 
		.aks_mask = {
			.c = 0x07FF	//All enabled
		},
		.detect_pwm[0] = {
			.b = 0x00
		},
		.detect_pwm[1] = {
			.b = 0x00
		},
		.detect_pwm[2] = {
			.b = 0x00
		},
		.detect_pwm[3] = {
			.b = 0x00
		},
		.detect_pwm[4] = {
			.b = 0x00
		},
		.detect_pwm[5] = {
			.b = 0x00
		},
		.detect_pwm[6] = {
			.b = 0x00
		},
		.led_det_hold_time = 0x00,
		.led_fade_key = {
			.b = 0x00
		},
		.led_latch = {
			.b = 0x00
		},
		.nthr_nhyst[0] = {
			.nthr = 10,
			.nhsyt = 2	//25%
		},
		.nthr_nhyst[1] = {
			.nthr = 10,
			.nhsyt = 2	//25%
		},
		.nthr_nhyst[2] = {
			.nthr = 10,
			.nhsyt = 2	//25%
		},
		.nthr_nhyst[3] = {
			.nthr = 10,
			.nhsyt = 2	//25%
		},
		.nthr_nhyst[4] = {
			.nthr = 10,
			.nhsyt = 2	//25%
		},
		.nthr_nhyst[5] = {
			.nthr = 10,
			.nhsyt = 2	//25%
		},
		.nthr_nhyst[6] = {
			.nthr = 10,
			.nhsyt = 2	//25%
		},
		.nthr_nhyst[7] = {
			.nthr = 10,
			.nhsyt = 2	//25%
		},
		.nthr_nhyst[8] = {
			.nthr = 10,
			.nhsyt = 2	//25%
		},
		.nthr_nhyst[9] = {
			.nthr = 10,
			.nhsyt = 2	//25%
		},
		.nthr_nhyst[10] = {
			.nthr = 10,
			.nhsyt = 2	//25%
		},
		.extend_pulse_time = {
			.high_time = 0,
			.low_time = 0
		},
		.ndrift_nrd[0] = {
			.ndrift = 7,	//2240ms
			.ndr = 10	//25.6s
		},
		.ndrift_nrd[1] = {
			.ndrift = 7,	//2240ms
			.ndr = 10	//25.6s
		},
		.ndrift_nrd[2] = {
			.ndrift = 7,	//2240ms
			.ndr = 10	//25.6s
		},
		.ndrift_nrd[3] = {
			.ndrift = 7,	//2240ms
			.ndr = 10	//25.6s
		},
		.ndrift_nrd[4] = {
			.ndrift = 7,	//2240ms
			.ndr = 10	//25.6s
		},
		.ndrift_nrd[5] = {
			.ndrift = 7,	//2240ms
			.ndr = 10	//25.6s
		},
		.ndrift_nrd[6] = {
			.ndrift = 7,	//2240ms
			.ndr = 10	//25.6s
		},
		.ndrift_nrd[7] = {
			.ndrift = 7,	//2240ms
			.ndr = 10	//25.6s
		},
		.ndrift_nrd[8] = {
			.ndrift = 7,	//2240ms
			.ndr = 10	//25.6s
		},
		.ndrift_nrd[9] = {
			.ndrift = 7,	//2240ms
			.ndr = 10	//25.6s
		},
		.ndrift_nrd[10] = {
			.ndrift = 7,	//2240ms
			.ndr = 10	//25.6s
		}
	};
	
	//uint8_t crc = AT42QT1110_calc_crc(setups.b, 42);
	
	AT42QT1110_send_cmd(CTRL_CMD_SEND_SETUPS);
	
	AT42QT1110_send_data(setups.b, 42);
	
	//if (AT42QT1110_send_cmd(crc) == crc)
		//return 0;
	//else
		//return -1;
	return 0;
}
