/*
 * CS4270.h
 *
 * Created: 4/4/2013 12:11:24 AM
 *  Author: Eva
 */ 


#ifndef CS4270_H_
#define CS4270_H_

#define CS4270_ADDRESS_WRITE	0x9E	//0b10011110
#define CS4270_ADDRESS_READ	0x9F	//0b10011111

#define MAP_CODEC_ID	0x01
#define MAP_PWR_CTRL	0x02
#define MAP_MODE_CTRL	0x03
#define MAP_ADC_DAC_CTRL	0x04
#define MAP_TRANS_CTRL	0x05
#define MAP_MUTE_CTRL	0x06
#define MAP_DAC_A_VOL	0x07
#define MAP_DAC_B_VOL	0x08

// mode_ctrl_t.func_mode values
#define SINGLE_SPEED	0x0		//fs = 4..54kHz
#define DOUBLE_SPEED	0x1		//fs = 50..108kHz
#define QUAD_SPEED		0x2		//fs = 100..216kHz
#define SLAVE_MODE		0x3

// mode_ctrl_t.ratio_sel values
#define RATIO_DIV1	0x0
#define RATIO_DIV1p5	0x1
#define RATIO_DIV2	0x2
#define RATIO_DIV3	0x3
#define RATIO_DIV4	0x4

// adc_dac_ctrl_t.xxx_dig_format values
#define LJF_24b		0x0
#define I2S_24b		0x1
#define RJF_16b		0x3
#define RJF_24b		0x4



typedef union{
	struct {
		uint8_t device:4;
		uint8_t revision:4;
	};
	uint8_t b;
}codec_id_t;

typedef union{
	struct {
		uint8_t freeze:1;
		uint8_t :1;
		uint8_t power_down_adc:1;
		uint8_t :3;
		uint8_t power_down_dac:1;
		uint8_t power_down:1;
	};
	uint8_t b;
}pwr_ctrl_t;

typedef union{
	struct {
		uint8_t :2;
		uint8_t func_mode:2;
		uint8_t ratio_sel:3;
		uint8_t popguard:1;
	};
	uint8_t b;
}mode_ctrl_t;

typedef union{
	struct {
		uint8_t adc_hpf_freeze_a:1;
		uint8_t adc_hpf_freeze_b:1;
		uint8_t dig_loopback:1;
		uint8_t dac_dig_format:2;
		uint8_t :2;
		uint8_t adc_dig_format:1;
	};
	uint8_t b;
}adc_dac_ctrl_t;

typedef union{
	struct {
		uint8_t dac_single_vol:1;
		uint8_t dac_soft_ramp:1;
		uint8_t dac_zero_cross:1;
		uint8_t adc_inv_pol_b:1;
		uint8_t adc_inv_pol_a:1;
		uint8_t dac_inv_pol_b:1;
		uint8_t dac_inv_pol_a:1;
		uint8_t de_emphasis:1;
	};
	uint8_t b;
}trans_ctrl_t;

typedef union{
	struct {
		uint8_t :2;
		uint8_t auto_mute:1;
		uint8_t mute_adc_b:1;
		uint8_t mute_adc_a:1;
		uint8_t mute_pol:1;
		uint8_t mute_dac_b:1;
		uint8_t mute_dac_a:1;
	};
	uint8_t b;
}mute_ctrl_t;

extern void CS4270_send_data(uint8_t* data, uint8_t length, uint8_t start_reg);
extern uint8_t CS4270_read_data(void);
extern void CS4270_set_vol(uint8_t db_times_two);
extern void CS4270_headphone_amp_mode(bool enable);
extern void CS4270_power_down(void);
extern void CS4270_config(void);

#endif /* CS4270_H_ */