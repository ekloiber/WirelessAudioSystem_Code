/*
 * AT42QT1110.h
 *
 * Created: 4/5/2013 7:25:46 PM
 *  Author: Eva
 */ 


#ifndef AT42QT1110_H_
#define AT42QT1110_H_

// Keys
#define KEY_STOP	0
#define KEY_NEXT	1
#define KEY_BUTTON	2
#define KEY_VOL_UP	3
#define KEY_WIRELESS	4
#define KEY_VOL_DN	5
#define KEY_PREV	6
#define KEY_PLAY_PAUSE	7

// Control commands
#define CTRL_CMD_SEND_SETUPS	0x01
#define CTRL_CMD_CALIBRATE_ALL	0x03
#define CTRL_CMD_RESET			0x04
#define DELAY_RESET_MS			160

#define CTRL_CMD_SLEEP			0x05

#define CTRL_CMD_STORE_TO_EEPROM	0x0A
#define DELAY_STORE_EEPROM_MS	200

#define CTRL_CMD_RESTORE_FROM_EEPROM	0x0B
#define DELAY_RESTORE_EEPROM_MS	150

#define CTRL_CMD_ERASE_EEPROM	0x0C
#define DELAY_ERASE_EEPROM_MS	50

#define CTRL_CMD_RECOVER_EEPROM	0x0D
#define DELAY_RECOVER_EEPROM_MS	50

#define CTRL_CMD_CALIBRATE_KEY0	0x10
#define CTRL_CMD_CALIBRATE_KEY1	0x11
#define CTRL_CMD_CALIBRATE_KEY2	0x12
#define CTRL_CMD_CALIBRATE_KEY3	0x13
#define CTRL_CMD_CALIBRATE_KEY4	0x14
#define CTRL_CMD_CALIBRATE_KEY5 0x15
#define CTRL_CMD_CALIBRATE_KEY6	0x16
#define CTRL_CMD_CALIBRATE_KEY7 0x17
#define CTRL_CMD_CALIBRATE_KEY8	0x18
#define CTRL_CMD_CALIBRATE_KEY9	0x19
#define CTRL_CMD_CALIBRATE_KEY10	0x1A


// Report requests + return length
#define RPT_REQ_SEND_FIRST_KEY	0xC0
#define LENGTH_SEND_FIRST_KEY	1

#define RPT_REQ_SEND_ALL_KEYS	0xC1
#define LENGTH_SEND_ALL_KEYS	2

#define RPT_REQ_DEVICE_STATUS	0xC2
#define LENGTH_DEVICE_STATUS	1

#define RPT_REQ_EEPROM_CRC		0xC3
#define LENGTH_EEPROM_CRC		1

#define RPT_REQ_RAM_CRC			0xC4
#define LENGTH_RAM_CRC			1

#define RPT_REQ_ERROR_KEYS		0xC5
#define LENGTH_ERROR_KEYS		2

#define RPT_REQ_SIGNAL_KEY0		0x20
#define RPT_REQ_SIGNAL_KEY1		0x21
#define RPT_REQ_SIGNAL_KEY2		0x22
#define RPT_REQ_SIGNAL_KEY3		0x23
#define RPT_REQ_SIGNAL_KEY4		0x24
#define RPT_REQ_SIGNAL_KEY5		0x25
#define RPT_REQ_SIGNAL_KEY6		0x26
#define RPT_REQ_SIGNAL_KEY7		0x27
#define RPT_REQ_SIGNAL_KEY8		0x28
#define RPT_REQ_SIGNAL_KEY9		0x29
#define RPT_REQ_SIGNAL_KEY10	0x2A
#define LENGTH_SIGNAL_KEY		2

#define RPT_REQ_REF_KEY0		0x40
#define RPT_REQ_REF_KEY1		0x41
#define RPT_REQ_REF_KEY2		0x42
#define RPT_REQ_REF_KEY3		0x43
#define RPT_REQ_REF_KEY4		0x44
#define RPT_REQ_REF_KEY5		0x45
#define RPT_REQ_REF_KEY6		0x46
#define RPT_REQ_REF_KEY7		0x47
#define RPT_REQ_REF_KEY8		0x48
#define RPT_REQ_REF_KEY9		0x49
#define RPT_REQ_REF_KEY10		0x4A
#define LENGTH_REF_KEY			2

#define RPT_REQ_STATUS_KEY0		0x80
#define RPT_REQ_STATUS_KEY1		0x81
#define RPT_REQ_STATUS_KEY2		0x82
#define RPT_REQ_STATUS_KEY3		0x83
#define RPT_REQ_STATUS_KEY4		0x84
#define RPT_REQ_STATUS_KEY5		0x85
#define RPT_REQ_STATUS_KEY6		0x86
#define RPT_REQ_STATUS_KEY7		0x87
#define RPT_REQ_STATUS_KEY8		0x88
#define RPT_REQ_STATUS_KEY9		0x89
#define RPT_REQ_STATUS_KEY10	0x8A
#define LENGTH_STATUS_KEY		1

#define RPT_REQ_DETECT_OUTPUT_STATES	0xC6
#define LENGTH_DETECT_OUTPUT_STATES		1

#define RPT_REQ_LAST_CMD		0xC7
#define LENGTH_LAST_CMD			1

#define RPT_REQ_SETUPS			0xC8
#define LENGTH_SETUPS			42

#define RPT_REQ_DEVICE_ID		0xC9
#define LENGTH_DEVICE_ID		1

#define RPT_REQ_FIRMWARE_VERSION	0xCA
#define LENGTH_FIRMWARE_VERSION		1

typedef union {
	struct {
		uint8_t detect:1;
		uint8_t numkey:1;
		uint8_t error:1;
		uint8_t :1;
		uint8_t key_num:3;	
	};
	uint8_t b;
} first_key_t;

typedef union {
	struct {
		uint8_t :5;
		uint8_t key_10:1;
		uint8_t key_9:1;
		uint8_t key_8:1;
		uint8_t key_7:1;
		uint8_t key_6:1;
		uint8_t key_5:1;
		uint8_t key_4:1;
		uint8_t key_3:1;
		uint8_t key_2:1;
		uint8_t key_1:1;
		uint8_t key_0:1;
	};
	uint16_t c;
	uint8_t b[2];
} all_keys_t;

typedef union {
	struct {
		uint8_t :1;
		uint8_t detect:1;
		uint8_t cycle:1;
		uint8_t error:1;
		uint8_t change:1;
		uint8_t eeprom:1;
		uint8_t reset:1;
		uint8_t guard:1;
	};
	uint8_t b;
} device_status_t;

typedef union {
	struct {
		uint8_t :5;
		uint8_t key_10:1;
		uint8_t key_9:1;
		uint8_t key_8:1;
		uint8_t key_7:1;
		uint8_t key_6:1;
		uint8_t key_5:1;
		uint8_t key_4:1;
		uint8_t key_3:1;
		uint8_t key_2:1;
		uint8_t key_1:1;
		uint8_t key_0:1;
	};
	uint16_t c;
	uint8_t b[2];
} error_keys_t;

typedef union {
	uint16_t signal;
	uint8_t b[2];
} signal_key_t;

typedef union {
	uint16_t ref;
	uint8_t b[2];
} ref_key_t;

typedef union {
	struct  {
		uint8_t detect:1;
		uint8_t lbl_error:1;
		uint8_t mbl_error:1;
		uint8_t :2;
		uint8_t aks_enabled:1;
		uint8_t calibrating:1;
		uint8_t key_enabled:1;
	};
	uint8_t b;
} status_key_t;

typedef union {
	struct {
		uint8_t :1;
		uint8_t detect_6:1;
		uint8_t detect_5:1;
		uint8_t detect_4:1;
		uint8_t detect_3:1;
		uint8_t detect_2:1;
		uint8_t detect_1:1;
		uint8_t detect_0:1;
	};
	uint8_t b;
} output_states_t;

typedef union {
	struct {
		uint8_t maj_version:4;
		uint8_t min_version:4;	
	};
	uint8_t b;
} firmware_version_t;

// Setups + status info
#define SET_DEVICE_MODE	0x90
#define GET_DEVICE_MODE	0xD0
#define SET_GUARD_COM_OPTS	0x91
#define GET_GUARD_COM_OPTS	0xD1
#define SET_DI_DHT	0x92
#define GET_DI_DHT	0xD2
#define SET_PTHR_PHYST	0x93
#define GET_PTHR_PHYST	0xD3
#define SET_PDRIFT	0x94
#define GET_PDRIFT	0xD4
#define SET_PRD	0x95
#define GET_PRD	0xD5
#define SET_LBL	0x96
#define GET_LBL	0xD6
#define SET_AKS_MASK_8_10	0x97
#define GET_AKS_MASK_8_10	0xD7
#define SET_AKS_MASK_0_7	0x98
#define GET_AKS_MASK_0_7	0xD8
#define SET_DETECT0	0x99
#define GET_DETECT0	0xD9
#define SET_DETECT1	0x9A
#define GET_DETECT1	0xDA
#define SET_DETECT2	0x9B
#define GET_DETECT2	0xDB
#define SET_DETECT3	0x9C
#define GET_DETECT3	0xDC
#define SET_DETECT4	0x9D
#define GET_DETECT4	0xDD
#define SET_DETECT5	0x9E
#define GET_DETECT5	0xDE
#define SET_DETECT6	0x9F
#define GET_DETECT6	0xDF
#define SET_LED_HOLD_TIME	0xA0
#define GET_LED_HOLD_TIME	0xE0
#define SET_LED_FADE_KEY	0xA1
#define GET_LED_FADE_KEY	0xE1
#define SET_NTHR_NHYST_KEY0	0xA3
#define GET_NTHR_NHYST_KEY0	0xE3
#define SET_NTHR_NHYST_KEY1	0xA4
#define GET_NTHR_NHYST_KEY1	0xE4
#define SET_NTHR_NHYST_KEY2	0xA5
#define GET_NTHR_NHYST_KEY2	0xE5
#define SET_NTHR_NHYST_KEY3	0xA6
#define GET_NTHR_NHYST_KEY3	0xE6
#define SET_NTHR_NHYST_KEY4	0xA7
#define GET_NTHR_NHYST_KEY4	0xE7
#define SET_NTHR_NHYST_KEY5	0xA8
#define GET_NTHR_NHYST_KEY5	0xE8
#define SET_NTHR_NHYST_KEY6	0xA9
#define GET_NTHR_NHYST_KEY6	0xE9
#define SET_NTHR_NHYST_KEY7	0xAA
#define GET_NTHR_NHYST_KEY7	0xEA
#define SET_NTHR_NHYST_KEY8	0xAB
#define GET_NTHR_NHYST_KEY8	0xEB
#define SET_NTHR_NHYST_KEY9	0xAC
#define GET_NTHR_NHYST_KEY9	0xEC
#define SET_NTHR_NHYST_KEY10	0xAD
#define GET_NTHR_NHYST_KEY10	0xED
#define SET_EXTEND_PULSE_TIME	0xAE
#define GET_EXTEND_PULSE_TIME	0xEE
#define SET_NDRIFT_NRD_KEY0	0xB0
#define GET_NDRIFT_NRD_KEY0	0xF0
#define SET_NDRIFT_NRD_KEY1	0xB1
#define GET_NDRIFT_NRD_KEY1	0xF1
#define SET_NDRIFT_NRD_KEY2	0xB2
#define GET_NDRIFT_NRD_KEY2	0xF2
#define SET_NDRIFT_NRD_KEY3	0xB3
#define GET_NDRIFT_NRD_KEY3	0xF3
#define SET_NDRIFT_NRD_KEY4	0xB4
#define GET_NDRIFT_NRD_KEY4	0xF4
#define SET_NDRIFT_NRD_KEY5	0xB5
#define GET_NDRIFT_NRD_KEY5	0xF5
#define SET_NDRIFT_NRD_KEY6	0xB6
#define GET_NDRIFT_NRD_KEY6	0xF6
#define SET_NDRIFT_NRD_KEY7	0xB7
#define GET_NDRIFT_NRD_KEY7	0xF7
#define SET_NDRIFT_NRD_KEY8	0xB8
#define GET_NDRIFT_NRD_KEY8	0xF8
#define SET_NDRIFT_NRD_KEY9	0xB9
#define GET_NDRIFT_NRD_KEY9	0xF9
#define SET_NDRIFT_NRD_KEY10	0xBA
#define GET_NDRIFT_NRD_KEY10	0xFA



#define KEY_AC_SRC_SYNC		0
#define KEY_AC_SRC_TIMED	1
#define MODE_7_KEY	0
#define MODE_11_KEY	1
#define SIGNAL_SERIAL	0
#define SIGNAL_PARALLEL	1
#define SYNC_TRIG_LEVEL	0
#define SYNC_TRIG_EDGE	1
#define REPEAT_TIME_FREE_RUN	0x0

typedef union {
	struct {
		uint8_t key_ac:1;
		uint8_t mode:1;
		uint8_t signal_mode:1;
		uint8_t sync_trig:1;
		uint8_t repeat_time:4;	//Mult of 16ms
	};	
	uint8_t b;
} device_mode_t;

#define CHG_MODE_DATA	0
#define CHG_MODE_TOUCH	1

typedef union {
	struct {
		uint8_t guard_key:4;
		uint8_t guard_key_en:1;
		uint8_t quick_spi_en:1;
		uint8_t chg_mode:1;
		uint8_t crc_en:1;
	};
	uint8_t b;
} guard_key_com_opts_t;

typedef union {
	struct {
		uint8_t dil:4;
		uint8_t dht:4;
	};
	uint8_t b;
} dil_dht_t;

typedef union {
	struct {
		uint8_t pthr:6;
		uint8_t physt:2;	//Mult of 12.5% reduction
	};
	uint8_t b;
} pthr_physt_t;

typedef union {
	struct {
		uint8_t :1;
		uint8_t pdrift:7;	//Mult of 160ms
	};
	uint8_t b;
} pdrift_t;

typedef uint8_t prd_t;		//Mult of 160ms

typedef uint8_t lbl_t;

typedef union {
	struct {
		uint8_t :2;
		uint8_t aks_10_en:1;
		uint8_t aks_9_en:1;
		uint8_t aks_8_en:1;
		uint8_t aks_7_en:1;
		uint8_t aks_6_en:1;
		uint8_t aks_5_en:1;
		uint8_t aks_4_en:1;
		uint8_t aks_3_en:1;
		uint8_t aks_2_en:1;
		uint8_t aks_1_en:1;
		uint8_t aks_0_en:1;
	};
	uint8_t b[2];
	uint16_t c;
} aks_mask_t;

typedef union {
	struct {
		uint8_t in_detect:4;	//Mult of 12.5%
		uint8_t out_detect:4;
	};
	uint8_t b;
} detect_pwm_t;

typedef uint8_t led_det_hold_time_t;	//Mult of 16ms

typedef union {
	struct {
		uint8_t fade:1;
		uint8_t led_6:1;
		uint8_t led_5:1;
		uint8_t led_4:1;
		uint8_t led_3:1;
		uint8_t led_2:1;
		uint8_t led_1:1;
		uint8_t led_0:1;
	};
	uint8_t b;
} led_fade_key_t;

typedef union {
	struct {
		uint8_t :1;
		uint8_t led_latch_6:1;
		uint8_t led_latch_5:1;
		uint8_t led_latch_4:1;
		uint8_t led_latch_3:1;
		uint8_t led_latch_2:1;
		uint8_t led_latch_1:1;
		uint8_t led_latch_0:1;
	};
	uint8_t b;
} led_latch_t;

typedef union {
	struct {
		uint8_t nthr:6;	
		uint8_t nhsyt:2; //Mult of 12.5%
	};
	uint8_t b;
} nthr_nhyst_t;

typedef union {
	struct {
		uint8_t high_time:4;
		uint8_t low_time:4;
	};
	uint8_t b;
} extend_pulse_time_t;

typedef union {
	struct {
		uint8_t ndrift:4;	// Mult of 320ms
		uint8_t nrd:4;	//Mult of 2.56s
	};
	uint8_t b;
} ndrift_nrd_t;

volatile static union {
	struct {
		device_mode_t device_mode;	//0
		guard_key_com_opts_t guard_key_com_opts;	//1
		dil_dht_t dil_dht;	//2
		pthr_physt_t pthr_physt;	//3
		pdrift_t pdrift;	//4
		prd_t prd;	//5
		lbl_t lbl;	//6
		aks_mask_t aks_mask;	//7-8
		detect_pwm_t detect_pwm[7];	//9-15
		led_det_hold_time_t led_det_hold_time;	//16
		led_fade_key_t led_fade_key;	//17
		led_latch_t led_latch;	//18
		nthr_nhyst_t nthr_nhyst[11];	//19-29
		extend_pulse_time_t extend_pulse_time;	//30
		ndrift_nrd_t ndrift_nrd[11];	//31-41
	} a;		
	uint8_t b[42];
} setups;


extern uint8_t AT42QT1110_calc_crc(uint8_t* data, uint8_t length);
extern void AT42QT1110_spi_read_timeout(uint8_t* data, uint32_t timeout_us);
extern void AT42QT1110_send_data(uint8_t* data, uint8_t length);
extern uint8_t AT42QT1110_send_cmd(uint8_t cmd);
extern void AT42QT1110_get_data(uint8_t cmd, uint8_t* data, uint8_t length);
extern uint32_t AT42QT1110_get_key(uint32_t key);
extern int32_t AT42QT1110_init(void);

#endif /* AT42QT1110_H_ */