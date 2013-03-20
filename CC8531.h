#ifndef _CC8531_H_INCLUDED
#define _CC8531_H_INCLUDED

#include <stdio.h>
#include <Arduino.h>
#include <avr/pgmspace.h>

#define u8 uint8_t
#define s8 int8_t
#define u16 uint16_t
#define s16 int16_t
#define u32 uint32_t

//Chip select for CC8531
#define SS_CC8531 A2

#define SELECT_CC8531() digitalWrite(SS_CC8531, LOW)
#define DESELECT_CC8531() digitalWrite(SS_CC8531, HIGH)

#define RESET_DELAY_US 3

//Error codes
#define BL_SPI_LOADER_UNLOCK  0x8021 // SPI bootloader is locked, waiting for BL_UNLOCK_SPI
#define BL_SPI_LOADER_READY   0x8020 // SPI bootloader is ready for the first command
#define BL_SPI_LOADER_LOCKED  0x0022 // SPI bootloader (one-shot) unlocking (BL_UNLOCK_SPI) failed
#define BL_ERASE_WORKING      0x0002 // Flash mass erase in progress
#define BL_ERASE_DONE         0x8003 // Flash mass erase completed successfully
#define BL_ERASE_FAILED       0x8004 // Flash mass erase failed
#define BL_PROG_WORKING       0x000A // Flash page programming in progress
#define BL_PROG_DONE          0x800B // Flash page programming completed successfully
#define BL_PROG_FAILED        0x800C // Flash page programming failed
#define BL_VERIFY_WORKING     0x000D // Flash verification in progress
#define BL_VERIFY_OK          0x800E // Flash verification completed successfully
#define BL_VERIFY_FAILED      0x800F // Flash verification failed

//Intel hex records
#define DATA_RECORD 0x00
#define EOF_RECORD 0x01


//EHIF status word
typedef union {
	struct {
		u8 EVT_SR_CHG:1;	//Audio sample rate has changed
		u8 EVT_NWK_CHG:1;	//Master: slave has joined/left network
							//Slave: join attempt failed or disconnected
		u8 EVT_PS_CHG:1;	//Power state of device changed
		u8 EVT_VOL_CHG:1;	//Volume for device changed
		u8 EVT_SPI_ERROR:1;	//Invalid SPI cmd/param or unexpected RW ops
		u8 EVT_DSC_RESET:1;	//Data side channel was reset
		u8 EVT_DSC_TX_AVAIL:1;	//Set when there is space for new datagrams
								//in data side channel transmission queue
		u8 EVT_DSC_RX_AVAIL:1;	//Set when the data side channel reception queue
								//contains one or more datagrams
		u8 WASP_CONN:1;		//Master: Set if audio network contains ≥1 slave
							//Slave: Set if connected to an audio network
		u8 PWR_STATE:3;		//Current device power stage
							//5 Active, 4 Low power, 3 Local standby
							//2 Network standby, 0 Off
		u8 :3;
		u8 CMDREQ_RDY:1;	//Indicates if EHIF is ready for SPI ops
	} B;
	u16 nStatus; 
} StatusWord_t;

//Chip Info
typedef struct {
	u16 FAMILY_ID:16;	//Hardware family ID, 0x2505

	u8 :8;

	u8 SIL_MIN_REV:4;	//Silicon minor revision
	u8 SIL_MAJ_REV:4;	//Silicon major revision

	u8 ROM_MIN_REV:8;	//ROM minor revision
	u8 ROM_MAJ_REV:8;	//ROM major revision
	u16 ROM_TYPE:16;	//ROM image type 0xB007
	
	u8 FW_PATCH_REV:8;	//Firmware patch revision
	u8 FW_MIN_REV:4;	//Firmware minor revision
	u8 FW_MAJ_REV:4;	//Firmware major revision
	u16 FW_TYPE:16;		//Firmware type 0xFFFF (unprog), 0x2505 (prog)
	
	u8:8;
	u8:8;
	u8:8;
	u8:8;
	u8:8;

	u32 FW_IMAGE_SIZE:32;	//Size of firmware image in bytes
	u16 CHIP_ID:16;		//ID of silicon device
	u16 CHIP_CAPS:16	//Chip capabilities
} Chip_info_t;

//Device Info
typedef struct {
	u32 DEVICE_ID:32;	//Unique device ID
	u32 MANF_ID:32;		//Unique manufacturer ID
	u32 PROD_ID:32;		//Product/family ID
} Device_info_t;

//Event Interrupt Mask
typedef struct {
	u8:8;
	u8 MSK_SR_CHG:1;	//If set, event will contribute to interrupt pin output
	u8 MSK_NWK_CHG:1;
	u8 MSK_PS_CHG:1;
	u8 MSK_VOL_CHG:1;
	u8 MSK_SPI_ERROR:1;
	u8 MSK_DSC_RESET:1;	
	u8 MSK_DSC_TX_AVAIL:1;
	u8 MSK_DSC_RX_AVAIL:1;
	u8 EHIF_IRQ_POL:1;	//Determines polarity of interrupt pin 0=active low
} EHIF_mask_t;

//Event Flags
typedef struct {
	u8 MSK_SR_CHG:1;	//If set, clears any flag
	u8 MSK_NWK_CHG:1;
	u8 MSK_PS_CHG:1;
	u8 MSK_VOL_CHG:1;
	u8 MSK_SPI_ERROR:1;
	u8 MSK_DSC_RESET:1;
	u8:2;
} EHIF_flags_t;

//Scan Parameters
typedef struct {
	u16 SCAN_TO:12;		//Timeout of scan operation (x10ms)
	u8 SCAN_MAX:4;		//Max number of networks to locate before ending scan
	u32 MANF_ID:32;		//Manufacturer ID filtering criteria (0=all IDs)
	u32 PROD_MASK_ID:32;	//PROD_ID&PROD_MASK_ID == PROD_ID_REF&PROD_ID_MASK
	u32 PROD_ID_REF:32;		//0 disables filtering
	u8 REQ_WPM_PAIR_SIGNAL:1;	//Require protocol master to signal pairing
	u8:7;
	s8 REQ_RSSI;	//Required RSSI minimum value of the first received packet
					//Signed 2's complement 0=0dBm, set to -128dbM to disable
} Scan_param_t;

//Scan Results
typedef struct {
	u32 DEVICE_ID:32;	//Network ID of current network (0 = no network)
	u32 MANF_ID:32;		//ID of master 0 = not connected
	u32 PROD_ID:32;		//ID of master 0 = not connected
	u8:1;
	u8 WPM_ALLOWS_JOIN:1;	//Master can accept slaves onto network
	u8 WPM_PAIR_SIGNAL:1;	//Master is signaling pair attempt
	u8 WPM_MFCT_FILT:1;	//Master only accepts slaves with matching MANF_ID
	u8 WPM_DSC_EN:1;	//Master supports data side channel
	u8 WPM_PM_STATE:3;	//Master's power state 
	u16 ACH_SUPPORT:16;	//Which audio channels are supported
	u8 ACH_FORMAT_1:3;	//Format 0 Unused, 1 PCM16, 2 PCME24, 4 SLAC, 5 PCMLF
	u8 ACH_ACTIVE_1:1;	//Audio channel used in network
	u8 ACH_FORMAT_0:3;
	u8 ACH_ACTIVE_0:1;
	u8 ACH_FORMAT_3:3;
	u8 ACH_ACTIVE_3:1;
	u8 ACH_FORMAT_2:3;
	u8 ACH_ACTIVE_2:1;
	u8 ACH_FORMAT_5:3;
	u8 ACH_ACTIVE_5:1;
	u8 ACH_FORMAT_4:3;
	u8 ACH_ACTIVE_4:1;
	u8 ACH_FORMAT_7:3;
	u8 ACH_ACTIVE_7:1;
	u8 ACH_FORMAT_6:3;
	u8 ACH_ACTIVE_6:1;
	u8 ACH_FORMAT_9:3;
	u8 ACH_ACTIVE_9:1;
	u8 ACH_FORMAT_8:3;
	u8 ACH_ACTIVE_8:1;
	u8 ACH_FORMAT_11:3;
	u8 ACH_ACTIVE_11:1;
	u8 ACH_FORMAT_10:3;
	u8 ACH_ACTIVE_10:1;
	u8 ACH_FORMAT_13:3;
	u8 ACH_ACTIVE_13:1;
	u8 ACH_FORMAT_12:3;
	u8 ACH_ACTIVE_12:1;
	u8 ACH_FORMAT_15:3;
	u8 ACH_ACTIVE_15:1;
	u8 ACH_FORMAT_14:3;
	u8 ACH_ACTIVE_14:1;
	s8 RSSI:8;	//Averaged RSSI of packets recv'd from master during scan operation
	
	u16 SAMPLE_RATE:12;	//Sample rate (x25Hz) as reported by master
	u16:4;
	u16 LATENCY:12;	//Audio latency (in #samples)
	u16:4;
} Scan_results_t;

//Join Parameters
typedef struct {
	u16 JOIN_TO:12;		//Timeout (x10ms)
	u16:1;
	u32 DEVICE_ID:32;	//Network ID of network to join (0 = leave network)
	u32 MANF_ID:32;		//Manufacturer ID filtering criteria (0 = ignore)
	u32 PROD_ID_MASK:32;	//Product ID filtering criteria (see scan param)
	u32 PROD_ID_REF:32;
} Join_param_t;

//Status Results - Slave
typedef struct {
	u32 DEVICE_ID:32;	//Network ID of current network (0 = no network)
	u32 MANF_ID:32;		//ID of master 0 = not connected
	u32 PROD_ID:32;		//ID of master 0 = not connected
	u8:1;
	u8 WPM_ALLOWS_JOIN:1;	//Master can accept slaves onto network
	u8 WPM_PAIR_SIGNAL:1;	//Master is signaling pair attempt
	u8 WPM_MFCT_FILT:1;	//Master only accepts slaves with matching MANF_ID
	u8 WPM_DSC_EN:1;	//Master supports data side channel
	u8 WPM_PM_STATE:3;	//Master's power state 
	u16 ACH_SUPPORT:16;	//Which audio channels are supported
	u8 ACH_FORMAT_1:3;	//Format 0 Unused, 1 PCM16, 2 PCME24, 4 SLAC, 5 PCMLF
	u8 ACH_ACTIVE_1:1;	//Audio channel used in network
	u8 ACH_FORMAT_0:3;
	u8 ACH_ACTIVE_0:1;
	u8 ACH_FORMAT_3:3;
	u8 ACH_ACTIVE_3:1;
	u8 ACH_FORMAT_2:3;
	u8 ACH_ACTIVE_2:1;
	u8 ACH_FORMAT_5:3;
	u8 ACH_ACTIVE_5:1;
	u8 ACH_FORMAT_4:3;
	u8 ACH_ACTIVE_4:1;
	u8 ACH_FORMAT_7:3;
	u8 ACH_ACTIVE_7:1;
	u8 ACH_FORMAT_6:3;
	u8 ACH_ACTIVE_6:1;
	u8 ACH_FORMAT_9:3;
	u8 ACH_ACTIVE_9:1;
	u8 ACH_FORMAT_8:3;
	u8 ACH_ACTIVE_8:1;
	u8 ACH_FORMAT_11:3;
	u8 ACH_ACTIVE_11:1;
	u8 ACH_FORMAT_10:3;
	u8 ACH_ACTIVE_10:1;
	u8 ACH_FORMAT_13:3;
	u8 ACH_ACTIVE_13:1;
	u8 ACH_FORMAT_12:3;
	u8 ACH_ACTIVE_12:1;
	u8 ACH_FORMAT_15:3;
	u8 ACH_ACTIVE_15:1;
	u8 ACH_FORMAT_14:3;
	u8 ACH_ACTIVE_14:1;
	s8 RSSI:8;	//Averaged RSSI of packets recv'd from master during scan operation
	
	u16 SAMPLE_RATE:12;	//Sample rate (x25Hz) as reported by master
	u16:4;
	u16 LATENCY:12;	//Audio latency (in #samples)
	u16 NWK_STATUS:4;	//Network status 0 = no connection, 8/13 = connected
						//other = searching/tracking/joining
	u16 ACH_USED:16;	//Which audio channels are being used by the salve
} Slave_status_t;

//Status Results - Master
typedef struct {
	u8 NWK_STATUS:4;	//Network status of master 0 = No network, other = maintaining network
	u8:4;
	u16 SAMPLE_RATE:12;	//Sample rate (x25Hz)
	u16:4;
	u16 ACH_USED:16;	//Which audio channels are being used by slaves

	struct {
		u32 DEVICE_ID:32;	//IDs of slave
		u32 MANF_ID:32;
		u32 PROD_ID:32;
		u16 ACH_USED:16;	//Which audio channels slave is consuming
		u8:8;
		u8 WPS_DSC_EN:1;	//Protocol slave supports data side channel
		u8 SLAVE_SHORT_ID:3;
		u8:4;
	} slaveStatus[6];

} Master_status_t;

//Audio Channel Configuration
typedef struct {
	u8 FRONT_PRIMARY_LEFT;		//0-MAX configures the audio interface channel for
	u8 FRONT_PRIMARY_RIGHT;		//the logical audio channel. MAX=max# of local audio
	u8 REAR_SECONDARY_LEFT;		//interface channels
	u8 REAR_SECONDARY_RIGHT;	//0xFF = channel not in use
	u8 FRONT_CENTER;
	u8 SUBWOOFER;
	u8 SIDE_LEFT;
	u8 SIDE_RIGHT;
	u8 USER_DEFINED_0;
	u8 USER_DEFINED_1;
	u8 INPUT_LEFT;
	u8 INPUT_RIGHT;
	u8 MICROPHONE_0;
	u8 MICROPHONE_1;
	u8 MICROPHONE_2;
	u8 MICROPHONE_3;
} Audio_chan_t;

//RF Channel Mask
typedef struct {
	u32:1;
	u32 RF_CH_MASK:18;	//Enabled (1) or disabled (0) RF channels 1-18
	u32:13;
} Wireless_chan_mask_t;

//Remote Control Data
typedef union {
	struct {
		u8 CMD_COUNT:3;		//Number of active bytes in CMD_STATE
		u8 KEYB_COUNT:3;	//Number of active bytes in KEYB_STATE
		u8 EXT_SEL:2;		//Contents of bytes 8-12 - 0 None, 1 Mouse, 2,3 Error
		union {				
			u8 KEYB_STATE;	//[0] is modifier bits (ctrl/shift/alt), Bytes 0..KEYB_COUNT
							//others are US keyboard codes 
			u8 CMD_STATE;	//Active remote control command IDs, Bytes KEYB_COUNT+1..KEYB_COUNT+CMD_COUNT
			u8 FILL;		//Filler bits, write 0x00, Bytes KEYB_COUNT+CMD_COUNT+1
		} DATA[7];	
		u8 MOUSE_BUTTONS:8;		//EXT_SEL=1: bit vector specifying pressed mouse buttons
		u16 MOUSE_POSITION_X:16;	//EXT_SEL=1: Mouse horizontal position
		u16 MOUSE_POSITION_Y:16;	//EXT_SEL=1: Mouse vertical position
	} PRE_DEFINED;
	struct {
		u8 ZERO:6;		//Write 0
		u8 DATA_SEL:2;	//Specifies contents of bytes 1:12- 0/1/3 do not use, 2 HID report raw data
		u8 DATA[12];	//HID report data or other RC info
	} CUSTOM;
} RC_data_t;

//Data Side Channel Data (tx)
typedef struct {
	u8 CONN_RESET:1;	//Resets/opens DSC conection. Set in first datagram after network established
	u8:7;
	u32 ADDR:32;	//32b device ID of datagram destination
} DSC_tx_data_t;

//Data Side Channel  Params (rx)
typedef struct {
	DSC_data_T DSCdata;
	u8* data;
} DSC_rx_data_t

//Power Management Set State
typedef PM_set_state_t u8;	//6,7 Reserved, 5 active, 4 low power, 3 local standby
							//2 network standby, 1 Reserved, 0 off

//Power Management Get State
typedef struct {
	u32 IN_SILENCE_TIME;	//Period of time with analog/digital silence on all input channels (x10ms)
	u32 OUT_SILENCE_TIME;	//Time with digital silence on all local output channels (x10ms)
	u32 NWK_INACTIVITY_TIME;	//Time without any network connections (x10ms)
	u16 VBAT_VOLTAGE;	//Last measured battery voltage (mV)
} PM_state_get_t;

//Volume Set 
typedef struct {
	u8 IS_LOCAL:1;	//0 Remote, 1 Local
	u8 IS_IN_VOL:1;	//0 Output vol, 1 Input vol
	u8:6;
	u8 LOG_CHANNEL:4;	//Logical channel (SET_OPT=3)
	u8 SET_OP:2;	//0 None, 1 Absolute, 2 Relative, 3 Logical Channel offset
	u8 MUTE_OP:2;	//0 None, 1 Mute, 2 Unmute, 3 Toggle
	s16 VOLUME:16;	//Volume (x0.125dB)
} Volume_set_t;

//Volume Get
typedef union {
	struct {
		u8 IS_LOCAL:1;	//0 Remote, 1 Local
		u8 IS_IN_VOL:1;	//0 Output vol, 1 Input vol
		u8:6;
	} MASTER;
	struct {
		u8 IS_CHANNEL OFFSET:1;	//0 Volume setting, 1 Mono/logical channel offset
		u8 IS_IN_VOL:1;	//0 Output vol, 1 Input vol
		u8 LOG_CHANNEL:4;	//Logical channel
		u8:2;
	} SLAVE;
} Volume_get_t;

//Volume Data
typedef Volume_data_t s16;	//Volume (x0.125dB)

//Audio Statistics Data
typedef struct {
	u16 CH_PEAK_VALUE;	//Peak absolute value
	u16 CH_MEAN_VALUE;	//Mean absolute value (filtered)
} Audio_stat_data_t;

//Audio Statistics
typedef struct {
	u32 SMPL_PROCESS_COUNT16;	//Number of processed samples/16 contributing 
								//to the below counter values
	u32 SMPL_CONCEAL_COUNT16;	//Number of samples/16 where error concealment was active
	u32 SMPL_MUTED_COUNT16;	//Number of muted samples/16 where muting was active
	u8 CH_COUNT;	//Number of channels in the peak and mean value stats
	u8:8;
	Audio_stat_data_t* DATA;
} Audio_stat_t;

//RF Statistics Data
typedef struct {
	u32 TIMESLOT_COUNT;	//Number of timeslots contributing to statistics
	u32 RX_PKT_COUNT;	//Number of packet receptions attempted
	u32 RX_PKT_FAIL_COUNT;	//Number of packet receptions failed
	u32 RX_SLICE_COUNT;	//S:Slices received, M:Slices transmitted
	u32 RX_SLICE_ERR_COUNT;	//S:Slices received w/errors, M:N/A
	u8 NWK_JOIN_COUNT;	//S:#successful network joins, M:#successful slave joins
	u8 NWK_DROP_COUNT;	//S:#network drops, M:#slave drops
	u16 AFH_SWAP_COUNT;	//S:N/A, M:#times adaptive frequency hopping swapped out an active channel
	u16* AFH_CH_USAGE;	//Number of times RF channel is used
} RF_stat_t;

//GIO Data
typedef struct {
	u8:8;
	u8:8;
	u8 GIO9:1;	//Value of GIOx
	u8 GIO10:1;
	u8 GIO11:1;
	u8 GIO12:1;
	u8 GIO13:1;
	u8 GIO14:1;
	u8 GIO15:1;
	u8:1;
	u8 GIO1:1;
	u8 GIO2:1;
	u8 GIO3:1;
	u8 GIO4:1;
	u8 GIO5:1;
	u8 GIO6:1;
	u8 GIO7:1;
	u8 GIO8:1;
} GIO_data_t;

//Flash Parameters
typedef struct {
	u8 SLOT_INDEX:2;	//Index of 32-bit data slot to read
	u8:6;
} Flash_param_t;

//Flash Data
typedef struct {
	u8 SLOT_INDEX:2;	//Index of 32-bit data slot to read
	u8:6;
	u8 DATA[4];	//Data to be writen
} Flash_data_t;

//RF TX Test
typedef struct {
	u8 ENABLE:1;	//Enable/disable transmission
	u8:7;
	u8 FREQ_OFFSET;	//RF frequency = 2318+offset MHz
	s8 TX_POW;	//TX power in dBm
} RF_tx_test_t;

//RF RX test
typedef struct {
	u8 ENABLE:1;
	u8:7;
	u8 FREQ_OFFSET;
} RF_rx_test_t;

//RF TX Error Rate
typedef struct {
	u16 CYCLE_COUNT;	//Number of times to cycle through the RF channel space
	u32 NWK_ID;	//Network ID to be used by the test
	s8 TX_POW;	//Target TX power in dBm
	u8 RF_CHANNEL;	//0 Use application channel, 1-18 use single RF channel
} RF_tx_error_t;

//RF RX Error Rate Parameters
typedef struct {
	u16 CYCLE_COUNT;	//Number of times to cycle through RF channel space
	u32 NWK_ID;	//Network ID to be used by the test
	u32 TIMEOUT;	//Timeout for syncing with transmitter at startup (x10ms)
	u8 RF_CHANNEL;	//0 Use application channel, 1-18 use single RF channel
} RF_rx_error_param_t;

//RF RX Error Rate Data
typedef struct {
	u32 CYCLE_COUNT;	//Actual number of RF channel cycles
	u32 RF_CHANNEL_COUNT;	//Number of RF channels used
	u32 ALL_OK_TOTAL;	//Totals of x_x_COUNT
	u32 SYNC_ERR_TOTAL;
	u32 HEADER_ERR_TOTAL;
	u32 SLICE_ERR_TOTAL;
	struct {
		u16 ALL_OK_COUNT;	//# packets with no error
		u16 SYNC_ERR_COUNT;	//# packets with timeouts
		u16 HEADER_ERR_COUNT;	//# packets with header error
		u32 SLICE_ERR_COUNT:24;	//# slice errors in packets with ok header
		u8 RSSI_MEAN;	//Mean RSSI for packets with ok header
		u8 RSS_STD_DEV;	//Standard deviation of RSSI
	} DATA[20];
} RF_rx_error_data_t;

//Network Sim Parameters
typedef struct {
	u16 TIMESLOT_PERIOD;	//Timeslot period in us
	u16 TX_MIN_DURATION;	//Min length of tx packet in us
	u16 TX_MAX_DURATION;	//Max length of tx packet in us
	u16 RX_DURATION;	//Duration of one rx period in us
	u8 SP_COUNT;	//Number of slaves to enable rx for
	s8 TX_POW;	//Target TX power in dBm
	u8 ACTIVE_RF_CH_LIST[4];	//Active channels (must be low->high)
} Network_sim_t;

//Audio Tone Generation
typedef struct {
	u8 CHANNEL;	//Channel index (0..X-1) for X channels master->slave 
				//(10..10+Y-1) for Y channels slave->master
	u8:8;
	u8 AMP_TONE;	//16-bit sample amplitude value divided by 256
	u16 FREQ_TONE;	//Frequency x10Hz
} Audio_tone_t;

//Audio Tone Detection
typedef struct {
	u16 AMP_TONE;	//Estimated amplitude
	u16 FREQ_TONE;	//Estimated frequency (x10Hz)
} Audio_det_t;

//IO Input Test Parameters
typedef struct {
	u8:8;

	u8 XANT_USB_SEL:1;	//Read XANTP/USBP and XANTN/USBN if set
	u8:7;

	u8 GIO9_SEL:1;	//Read GIOx if set
	u8 GI10_SEL:1;
	u8 GI11_SEL:1;
	u8 GI12_SEL:1;
	u8 GI13_SEL:1;
	u8 GI14_SEL:1;
	u8 GI15_SEL:1;
	u8:1;

	u8 GIO1_SEL:1;
	u8 GIO2_SEL:1;
	u8 GIO3_SEL:1;
	u8 GIO4_SEL:1;
	u8 GIO5_SEL:1;
	u8 GIO6_SEL:1;
	u8 GIO7_SEL:1;
	u8 GIO8_SEL:1;
} IOTest_param_t;

//IO Input Test Data
typedef struct {
	u8:8;
	
	u8 XANTN_USBN_VAL:1;	//Value of x
	u8 XANTP_USBP_VAL:1;
	u8 USB_DIFF_VAL:1;	//Differential value of USB/XANT
	u8:5;

	u8 GIO9_VAL:1;	//Value of GIOx
	u8 GI10_VAL:1;
	u8 GI11_VAL:1;
	u8 GI12_VAL:1;
	u8 GI13_VAL:1;
	u8 GI14_VAL:1;
	u8 GI15_VAL:1;
	u8:1;

	u8 GIO1_VAL:1;
	u8 GIO2_VAL:1;
	u8 GIO3_VAL:1;
	u8 GIO4_VAL:1;
	u8 GIO5_VAL:1;
	u8 GIO6_VAL:1;
	u8 GIO7_VAL:1;
	u8 GIO8_VAL:1;
} IOTest_data_t;

//IO Output Test Parameters
typedef struct {
	u8:8;

	u8 XANT_USB_PN_SEL:1;	//Write XANT/USB if set using _P_VAL and _N_VAL
	u8 XANT_USB_DIFF_SEL:1;	//Write XANT/USB if set using XANT_USB_DIFF_VAL
	u8:6;
	
	u8 GIO9_SEL:1;	//Write GIOx if set
	u8 GI10_SEL:1;
	u8 GI11_SEL:1;
	u8 GI12_SEL:1;
	u8 GI13_SEL:1;
	u8 GI14_SEL:1;
	u8 GI15_SEL:1;
	u8:1;

	u8 GIO1_SEL:1;
	u8 GIO2_SEL:1;
	u8 GIO3_SEL:1;
	u8 GIO4_SEL:1;
	u8 GIO5_SEL:1;
	u8 GIO6_SEL:1;
	u8 GIO7_SEL:1;
	u8 GIO8_SEL:1;

	u8:8;

	u8 XANT_USB_N_VAL:1;	//Value of x
	u8 XANT_USB_P_VAL:1;
	u8 XANT_USB_DIFF_VAL:1;	//Differential value of USB/XANT
	u8:5;

	u8 GIO9_VAL:1;	//Value of GIOx
	u8 GI10_VAL:1;
	u8 GI11_VAL:1;
	u8 GI12_VAL:1;
	u8 GI13_VAL:1;
	u8 GI14_VAL:1;
	u8 GI15_VAL:1;
	u8:1;

	u8 GIO1_VAL:1;
	u8 GIO2_VAL:1;
	u8 GIO3_VAL:1;
	u8 GIO4_VAL:1;
	u8 GIO5_VAL:1;
	u8 GIO6_VAL:1;
	u8 GIO7_VAL:1;
	u8 GIO8_VAL:1;
} IOTest_output_t;

class CC8531Class {
private:
	u8 waitReadyError;
	void waitReady();
	void waitReadyMs(u16);
	StatusWord_t writeWord(u8, u8);
	StatusWord_t setAddr(u16);
	StatusWord_t cmdReq(u8, u8, u8*);
	StatusWord_t write(u16, u8*);
	StatusWord_t read(u16, u8*);
	StatusWord_t readBC(u8*, u16*);
	StatusWord_t getStatus();
	void sysReset(u8);
	void bootReset();
	u8 hexVal(u8);
	u8 readByte(File);
	u16 readInt(File);
public:
	void begin();
	StatusWord_t flashProgram(u8*);
	StatusWord_t flashHex(File);
	u8 getWaitReadyError();

	class BootloaderClass {
	public:
		StatusWord_t unlock();
		StatusWord_t flashMassErase();
		StatusWord_t flashPageProg(u16, u16*);
		StatusWord_t flashVerify(u16, u8*);
	} Bootloader;

	class InfoClass {
	public:
		StatusWord_t getChipInfo(Chip_info_t*);
		StatusWord_t getDeviceInfo(Device_info_t*);
	} Info;

	class EHIFCtrlClass {
	public:
		StatusWord_t confInterruptMask(EHIF_mask_t*);
		StatusWord_t clearEventFlags(EHIF_flags_t*);
	} EHIFCtrl;

	class NetworkClass {
	public:
		StatusWord_t scan(Scan_param_t*, u8*);
		StatusWord_t join(Join_param_t*);
		StatusWord_t getStatusSlave(u8*);
		StatusWord_t confAudioChan(Audio_chan_t*);
		StatusWord_t enableNetworkControl();
		StatusWord_t disableNetworkControl();
		StatusWord_t enablePairingSignal();
		StatusWord_t disablePairingSignal();
		StatusWord_t setChanMask(Wireless_chan_mask_t*);
	} Network;

	class RCClass {
	public:
		StatusWord_t setData(RC_data_t*);
		StatusWord_t getData(RC_data_t*, u8*);
	} RC;

	class DSCClass {
	public:
		StatusWord_t txData(DSC_tx_data_t*, u16, u8*);
		StatusWord_t rxData(DSC_rx_data_t*, u16*;
	} DSC;

	class PowerClass {
	public:
		StatusWord_t setPower(PM_set_state_t*);
		StatusWord_t getPower(PM_state_get_t*);
	} Power;

	class VolumeClass {
	public:
		StatusWord_t setVolume(Volume_set_t*);
		StatusWord_t getVolume(Volume_get_t*, Volume_data_t*);
	} Volume;

	class StatisticsClass {
	public:
		StatusWord_t audioStats(Audio_stat_t*, u16*);
		StatusWord_t rfStats(RF_stat_t*, u16*);
	} Statistics;

	class UtilityClass {
	public:
		StatusWord_t getGIO(GIO_data_t*);
		StatusWord_t getFlash(Flash_param_t*, u8*);
		StatusWord_t setFlash(Flash_data_t*);
	} Utility;

	class RFTestClass {
	public:
		StatusWord_t txWave(RF_tx_test_t*);
		StatusWord_t txRand(RF_tx_test_t*);
		StatusWord_t rxWave(RF_rx_test_t*);
		StatusWord_t rxRSSI(u8*, s8*);
		StatusWord_t txError(RF_tx_error_t*);
		StatusWord_t rxError(RF_rx_error_param_t*, RF_rx_error_data_t*);
		StatusWord_t networkSim(Network_sim_t*);	
	} RFTest;

	class AudioTestClass {
	public:
		StatusWord_t genTone(Audio_tone_t*);
		StatusWord_t detectTone(u8*, Audio_det_t*);
	} AudioTest;

	class IOTestClass {
	public:
		StatusWord_t input(IOTest_param_t*, IOTest_data_t*);
		StatusWord_t output(IOTest_output_t*);
	} IOTest;
};

extern CC8531Class CC8531;

#endif