/*
 * CS2300.h
 *
 * Created: 4/3/2013 10:12:36 PM
 *  Author: Eva
 */ 


#ifndef CS2300_H_
#define CS2300_H_

#define CS2300_ADDRESS	0x9E	//0b10011110

// Memory address pointers
#define MAP_CLOCK_ID		0x01
#define MAP_DEV_CTRL	0x02
#define MAP_DEV_CFG		0x03
#define MAP_GLOBAL_CFG	0x05
#define MAP_RATIO_MSB	0x06
#define MAP_RATIO_2SB	0x07
#define MAP_RATIO_3SB	0x08
#define MAP_RATIO_LSB	0x09
#define MAP_FUNC_CFG1	0x16
#define MAP_FUNC_CFG2	0x17
#define MAP_FUNC_CFG3	0x1E

// Values for device_config.r_mod_sel
#define R_MOD_X1	0x0
#define R_MOD_X2	0x1
#define R_MOD_X4	0x2
#define R_MOD_X8	0x3
#define R_MOD_DIV2	0x4
#define R_MOD_DIV4	0x5
#define R_MOD_DIV8	0x6
#define R_MOD_DIV16	0x7

// Values for device_config.aux_src_sel
#define CLK_IN		0x1
#define CLK_OUT		0x2
#define PLL_LOCK	0x3

// Values for function_config1.aux_lock_config
#define ACTIVE_HIGH	0x0
#define ACTIVE_LOW	0x1

// Values for function_config2.lf_ratio_config
#define HIGH_MULTIPLIER	0x0
#define HIGH_ACCURACY	0x1

// Values for function_config3.clkin_bw
#define MIN_BW_1HZ		0x0
#define MIN_BW_2HZ		0x1
#define MIN_BW_4HZ		0x2
#define MIN_BW_8HZ		0x3
#define MIN_BW_16HZ		0x4
#define MIN_BW_32HZ		0x5
#define MIN_BW_64HZ		0x6
#define MIN_BW_128HZ	0x7

typedef union{
	struct {
		uint8_t device:5;
		uint8_t revision:3;
	};
	uint8_t b;
}device_id_t;

typedef union{
	struct {
		uint8_t unlock:1;
		uint8_t :5;
		uint8_t aux_output_disable:1;
		uint8_t clock_out_disable:1;
	};
	uint8_t b;
}device_control_t;

typedef union{
	struct {
		uint8_t r_mod_sel:3;
		uint8_t :2;
		uint8_t aux_src_sel:2;
		uint8_t enable_config:1;
	};
	uint8_t b;
}device_config_t;

typedef union{
	struct {
		uint8_t :4;
		uint8_t freeze:1;
		uint8_t :2;
		uint8_t enable_config:1;
	};
	uint8_t b;
}global_config_t;


typedef union {
	struct {
		uint8_t ratio_msb;
		uint8_t ratio_2sb;
		uint8_t ratio_3sb;
		uint8_t ratio_lsb;
	};
	uint32_t b;
	uint8_t c[4];
}ratio_t;

typedef union{
	struct {
		uint8_t clock_skip_enable:1;
		uint8_t aux_lock_config:1;
		uint8_t :1;	
		uint8_t enable_config:1;
		uint8_t :4;
	};	
	uint8_t b;
}function_config1_t;

typedef union{
	struct {
		uint8_t :3;
		uint8_t clock_out_unlock:1;
		uint8_t lf_ratio_config:1;
		uint8_t :3;
	};
	uint8_t b;
}function_config2_t;

typedef union{
	struct {
		uint8_t :1;
		uint8_t clkin_bw:3;
		uint8_t :4;
	};
	uint8_t b;
}function_config3_t;


// Function prototypes
extern void CS2300_send_data(uint8_t* data, uint8_t length, uint8_t start_reg);
extern void CS2300_set_ratio(uint32_t ratio);
extern uint32_t CS2300_calculate_ratio(uint32_t freq_in_hz, uint32_t freq_out_hz);
extern void CS2300_set_MCLK(uint32_t mclk_hz);
extern void CS2300_config(uint32_t fs);

#endif /* CS2300_H_ */