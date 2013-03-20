/** \addtogroup module_ehif_defs External Host Interface Definitions
 *
 * \brief EHIF constants and command/data phase data type definitions
 *
 *
 * \section section_ehif_defs_overview Overview
 * The following items are defined here:
 * - SPI status word events
 * - SPI command type IDs
 * - Data structure types for CMD_REQ parameters and READ/READBC/WRITE data
 * - Buffer union types of all CMD_REQ parameter structures and all READ/READBC/WRITE data structures
 * - Defined remote control commands
 *
 * The structure type definitions documented here have been written for the little-endian IAR compiler
 * for MSP430 and 8051, This means that a sequence of bit-fields that fit within a fundamental data type
 * (e.g. uint32_t) are declared in reverse order.
 *
 * @{
 */
#ifndef EHIF_DEFS_H_
#define EHIF_DEFS_H_

#include <stdint.h>


//-------------------------------------------------------------------------------------------------------
/// \name EHIF Status Word Bits
//@{

#define BV_EHIF_STAT_CMD_REQ_RDY      BV(15)  ///< Indicates that EHIF is ready for (another) command
#define BV_EHIF_STAT_CONNECTED        BV(8)   ///< Indicates one or more network connections
#define BV_EHIF_EVT_DSC_RX_AVAIL      BV(7)   ///< Bit indicationg whether RX FIFO for the DSC is empty
#define BV_EHIF_EVT_DSC_TX_AVAIL      BV(6)   ///< Bit indicationg whether TX FIFO for the DSC is empty
#define BV_EHIF_EVT_DSC_RESET         BV(5)   ///< Bit indicationg that the DSC has been reset
#define BV_EHIF_EVT_SPI_ERROR         BV(4)   ///< Error on SPI interface
#define BV_EHIF_EVT_VOL_CHG           BV(3)   ///< Some operation or event caused a volume change
#define BV_EHIF_EVT_PS_CHG            BV(2)   ///< Power state has changed
#define BV_EHIF_EVT_NWK_CHG           BV(1)   ///< Network state has changed
#define BV_EHIF_EVT_SR_CHG            BV(0)   ///< Sample rate has changed

//@}
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
/// \name SPI Command Type IDs
//@{
//                                    0x00
//                                    0x01
//                                    0x02
//                                    0x03
#define EHIF_CMD_DSC_TX_DATAGRAM      0x04    ///< Command ID for DSC_TX_DATAGRAM
#define EHIF_CMD_DSC_RX_DATAGRAM      0x05    ///< Command ID for DSC_RX_DATAGRAM
//                                    0x06
//                                    0x07
#define EHIF_CMD_NWM_DO_SCAN          0x08    ///< Command ID for NWM_DO_SCAN
#define EHIF_CMD_NWM_DO_JOIN          0x09    ///< Command ID for NWM_DO_JOIN
#define EHIF_CMD_NWM_GET_STATUS_M     0x0A    ///< Command ID for NWM_GET_STATUS (protocol master version)
#define EHIF_CMD_NWM_GET_STATUS_S     0x8A    ///< Command ID for NWM_GET_STATUS (protocol slave version)
#define EHIF_CMD_NWM_ACH_SET_USAGE    0x0B    ///< Command ID for NWM_ACH_SET_USAGE
#define EHIF_CMD_NWM_CONTROL_ENABLE   0x0C    ///< Command ID for NWM_CONTROL_ENABLE
#define EHIF_CMD_NWM_CONTROL_SIGNAL   0x0D    ///< Command ID for NWM_CONTROL_SIGNAL
#define EHIF_CMD_NWM_SET_RF_CH_MASK   0x0E    ///< Command ID for NWM_SET_RF_CH_MASK
//                                    0x0F
#define EHIF_CMD_PS_RF_STATS          0x10    ///< Command ID for PS_RF_STATS
#define EHIF_CMD_PS_AUDIO_STATS       0x11    ///< Command ID for PS_AUDIO_STATS
#define EHIF_CMD_RFT_RXPER            0x12    ///< Command ID for RFT_RXPER
#define EHIF_CMD_RFT_TXPER            0x13    ///< Command ID for RFT_TXPER
#define EHIF_CMD_RFT_TXTST_PN         0x14    ///< Command ID for RFT_TXTST_PN
#define EHIF_CMD_RFT_TXTST_CW         0x15    ///< Command ID for RFT_TXTST_CW
#define EHIF_CMD_VC_GET_VOLUME        0x16    ///< Command ID for VC_GET_VOLUME
#define EHIF_CMD_VC_SET_VOLUME        0x17    ///< Command ID for VC_SET_VOLUME
//                                    0x18
#define EHIF_CMD_EHC_EVT_CLR          0x19    ///< Command ID for EHC_EVT_CLR
#define EHIF_CMD_EHC_EVT_MASK         0x1A    ///< Command ID for EHC_EVT_MASK
//                                    0x1B
#define EHIF_CMD_PM_SET_STATE         0x1C    ///< Command ID for PM_SET_STATE
#define EHIF_CMD_PM_GET_DATA          0x1D    ///< Command ID for PM_GET_DATA
#define EHIF_CMD_DI_GET_DEVICE_INFO   0x1E    ///< Command ID for DI_GET_DEVICE_INFO
#define EHIF_CMD_DI_GET_CHIP_INFO     0x1F    ///< Command ID for DI_GET_CHIP_INFO
#define EHIF_CMD_AT_GEN_TONE          0x20    ///< Command ID for AT_GEN_TONE
#define EHIF_CMD_AT_DET_TONE          0x21    ///< Command ID for AT_DET_TONE
#define EHIF_CMD_IOTST_INPUT          0x22    ///< Command ID for IOTST_INPUT
#define EHIF_CMD_IOTST_OUTPUT         0x23    ///< Command ID for IOTST_OUTPUT
#define EHIF_CMD_SDP_INJECT_ERROR     0x24    ///< Command ID for SDP_INJECT_ERROR (internal test command)
#define EHIF_CMD_RFT_RXTST_CONT       0x25    ///< Command ID for RFT_RXTST_CONT
#define EHIF_CMD_RFT_RXTST_RSSI       0x26    ///< Command ID for RFT_RXTST_RSSI
#define EHIF_CMD_RFT_NWKSIM           0x27    ///< Command ID for RFT_NWKSIM
#define EHIF_CMD_CAL_SET_DATA         0x28    ///< Command ID for CAL_SET_DATA
#define EHIF_CMD_CAL_GET_DATA         0x29    ///< Command ID for CAL_GET_DATA
#define EHIF_CMD_IO_GET_PIN_VAL       0x2A    ///< Command ID for IO_GET_PIN_VAL
#define EHIF_CMD_NVS_GET_DATA         0x2B    ///< Command ID for NVS_GET_DATA
#define EHIF_CMD_NVS_SET_DATA         0x2C    ///< Command ID for NVS_SET_DATA
#define EHIF_CMD_RC_SET_DATA          0x2D    ///< Command ID for RC_SET_DATA
#define EHIF_CMD_RC_GET_DATA          0x2E    ///< Command ID for RC_GET_DATA

//@}
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// Command parameter and command data structures, listed by command ID
#pragma pack(1)

/// Command parameters for \ref EHIF_CMD_DSC_TX_DATAGRAM
typedef struct {
    uint8_t  connReset               : 1;  ///< Reset data side-channel connection
    uint8_t  reserved0               : 7;  ///< Reserved
    uint32_t addr;                         ///< Datagram destination address (device ID)
} EHIF_CMD_DSC_TX_DATAGRAM_PARAM_T;

/// Command parameters for \ref EHIF_CMD_NWM_DO_SCAN
typedef struct {
    uint16_t scanTo                  : 12; ///< Scan operation timeout in units of 10 ms
    uint16_t scanMax                 : 4;  ///< Maximum number of unique scan results to return
    uint32_t mfctId;                       ///< Manufacturer ID filter (must match protocol master if non-zero)
    uint32_t prodIdMask;                   ///< Product ID filter mask (defines \c prodIdRef bits required to match)
    uint32_t prodIdRef;                    ///< Product ID filter reference (must match protocol master if non-zero)
    uint8_t  reqPairingSignal        : 1;  ///< Require pairing signal from the protocol master to match
    uint8_t  reserved0               : 7;  ///< Reserved
    int8_t   reqRssi;                      ///< Minimum required RSSI to match, in dBm (-128 = disabled)
} EHIF_CMD_NWM_DO_SCAN_PARAM_T;

/// Command parameters for \ref EHIF_CMD_NWM_DO_SCAN
typedef struct {
    uint32_t deviceId;                     ///< Network ID (0: not connected, nwk. info invalid)
    uint32_t mfctId;                       ///< Manufacturer ID of WPM (0: device failed filtering)
    uint32_t prodId;                       ///< Product/family ID of WPM
    uint8_t  reserved0               : 1;  ///< Reserved
    uint8_t  wpmAllowsJoin           : 1;  ///< WPM allows additional slaves to join
    uint8_t  wpmPairSignal           : 1;  ///< WPM signalling user-initiated pairing
    uint8_t  wpmMfctFilt             : 1;  ///< WPM only accepts slaves with matching manufacturer ID
    uint8_t  wpmDscEn                : 1;  ///< WPM supports data side-channel
    uint8_t  wpmPowerState           : 3;  ///< Power state of WPM
    uint16_t achSupport;                   ///< Bitmask of audio channels supported in network
    uint8_t  pAchInfo[8];                  ///< Audio channel information (4 bits/logical channel)
    int8_t   rssi;                         ///< Mean RSSI of packets from WPM
    uint16_t smplRate                : 12; ///< Sample rate in 25 Hz increments
    uint16_t reserved1               : 4;  ///< Reserved
    uint16_t latency                 : 12; ///< Audio latency in number of samples
    uint16_t reserved2               : 4;  ///< Reserved
} EHIF_CMD_NWM_DO_SCAN_DATA_T;

/// Command parameters for \ref EHIF_CMD_NWM_DO_JOIN
typedef struct {
    uint16_t joinTo                  : 15; ///< Join operation timeout in units of 10 ms
    uint16_t reserved0               : 1;  ///< Reserved
    uint32_t deviceId;                     ///< Device ID filter (0x00000000 = leave, 0xFFFFFFFF = pairing enabled WPMs)
    uint32_t mfctId;                       ///< Manufacturer ID filter (must match protocol master if non-zero)
    uint32_t prodIdMask;                   ///< Product ID filter mask (defines \c prodIdRef bits required to match)
    uint32_t prodIdRef;                    ///< Product ID filter reference (must match protocol master if non-zero)
} EHIF_CMD_NWM_DO_JOIN_PARAM_T;

/// Command data for \ref EHIF_CMD_NWM_GET_STATUS_M for protocol masters
typedef struct {
    uint8_t  nwkState                : 4;  ///< Current WPM network state
    uint8_t  wpsCount                : 4;  ///< Number of protocol slaves currently connected (0 to 6)
    uint16_t smplRate                : 12; ///< Current sample rate in 25 Hz increments
    uint16_t tsPeriod                : 4;  ///< Timeslot period=(2+tsPeriod/4) ms
    uint16_t bvAchUsedOverall;             ///< Bitmask of which audio channels are used overall
    struct {
        uint32_t devId;                    ///< Device ID of WPS
        uint32_t mfctId;                   ///< Manufacturer ID of WPS
        uint32_t prodId;                   ///< Product/family ID of WPS
        uint16_t bvAchUsedByWps;           ///< Bitmask of audio channels used by WPS
        uint8_t  tsMissCount;              ///< Number of timeslots since slave seen
        uint8_t  dscEn               : 1;  ///< WPS supports data side-channel
        uint8_t  spSlot              : 3;  ///< Index of WPS slave slot used (1-7)
        uint8_t  reserved0           : 4;  ///< Reserved
    } pWpsInfo[6];
} EHIF_CMD_NWM_GET_STATUS_MASTER_DATA_T;

/// Command data for \ref EHIF_CMD_NWM_GET_STATUS_S for protocol slaves
typedef struct {
    uint32_t nwkId;                        ///< Network ID (0: not connected, nwk. info invalid)
    uint32_t wpmMfctId;                    ///< Manufacturer ID of WPM (0: device failed filtering)
    uint32_t wpmProdId;                    ///< Product/family ID of WPM
    uint8_t  smplStmpValid           : 1;  ///< Flag indicating if samplestamps below are valid
    uint8_t  wpmAllowsJoin           : 1;  ///< WPM allows additional slaves to join
    uint8_t  wpmSignalsPairing       : 1;  ///< WPM signalling user-initiated pairing
    uint8_t  doMfctIdFiltering       : 1;  ///< WPM only accepts slaves with matching manufacturer ID
    uint8_t  wpmDscEn                : 1;  ///< WPM supports data side-channel
    uint8_t  wpmPowerState           : 3;  ///< Power state of WPM
    uint16_t bvAchSupportedByWpm;          ///< Bitmask of audio channels supported in network
    uint8_t  achInfo[8];                   ///< Audio channel information (4 bits/logical channel)
    int8_t   meanRssi;                     ///< Mean RSSI of packets from WPM
    uint16_t smplRate                : 12; ///< Sample rate in 25 Hz increments
    uint16_t tsPeriod                : 4;  ///< Timeslot period = (2 + tsPeriod / 4) ms
    uint16_t audioLatency            : 12; ///< Audio latency in number of samples
    uint16_t nwkState                : 4;  ///< Current WPS network state
    uint16_t bvAchUsedByMe;                ///< Bitmask for audio channels used by WPS
} EHIF_CMD_NWM_GET_STATUS_SLAVE_DATA_T;

/// Command parameters for \ref EHIF_CMD_NWM_ACH_SET_USAGE
typedef struct {
    uint8_t  pAchUsage[16];                ///< One AIF DMA index for each logical channel (0xFF = channel unused)
} EHIF_CMD_NWM_ACH_SET_USAGE_PARAM_T;

/// Command parameters for \ref EHIF_CMD_NWM_CONTROL_ENABLE
typedef struct {
    uint8_t  reserved0;                    ///< Reserved
    uint8_t  wmEnable                : 1;  ///< Network maintenance control: TRUE to enable, FALSE to disable
    uint8_t  reserved1               : 7;  ///< Reserved
} EHIF_CMD_NWM_CONTROL_ENABLE_PARAM_T;

/// Command parameters for \ref EHIF_CMD_NWM_CONTROL_SIGNAL
typedef struct {
    uint8_t  reserved0;                    ///< Reserved
    uint8_t  wmPairSignal            : 1;  ///< Pairing signal control: TRUE to enable, FALSE to disable
    uint8_t  reserved1               : 7;  ///< Reserved
} EHIF_CMD_NWM_CONTROL_SIGNAL_PARAM_T;

/// Command parameters for \ref EHIF_CMD_NWM_SET_RF_CH_MASK
typedef struct {
    uint32_t rfChMask                : 20; ///< New dynamic RF channel mask to be used by WASPM
    uint32_t reserved0               : 12; ///< Reserved
} EHIF_CMD_NWM_SET_RF_CH_MASK_PARAM_T;

/// Command data for \ref EHIF_CMD_PS_RF_STATS
typedef struct {
    uint32_t timeslotCount;                ///< Number of timeslots
    uint32_t pktRxCount;                   ///< Number of attempted packet receptions (excl. SJP)
    uint32_t pktRxErrCount;                ///< Number of packets that timed out or were received with header error
    uint32_t sliceRxTxCount;               ///< Number of slices received or transmitted
    uint32_t sliceRxErrCount;              ///< Number of slices received with errors
    uint8_t  nwkJoinCount;                 ///< Number of network joins
    uint8_t  nwkDropCount;                 ///< Number of network drops
    uint16_t afhSwapCount;                 ///< Number of times the AFH algorithm swapped out an active channel
    uint16_t pAfhChUsageCount[20];         ///< For each RF channel: Number of timeslots the channel has been used
} EHIF_CMD_PS_RF_STATS_DATA_T;

/// Command data for \ref EHIF_CMD_PS_AUDIO_STATS
typedef struct {
    uint32_t smplProcessedCountDiv16;      ///< Number of samples processed (divided by 16)
    uint32_t smplConcealedCountDiv16;      ///< Number of samples where error concealment was active (divided by 16)
    uint32_t smplMutedCountDiv16;          ///< Number of samples that were muted (divided by 16)
    uint16_t muteEventCount;               ///< Number of mute events that have occurred (count once per mute event)
    uint8_t  activeChannelCount;           ///< Number of channels that pChannelStats is valid for
    uint8_t  reserved0;                    ///< Reserved for struct member alignment
    struct {
        uint16_t peakValue;                ///< Maximum absolute peak value (no decay)
        uint16_t filtMeanValue;            ///< Mean absolute value (filtered)
    } pChannelStats[6];                    ///< Sample statistics for each audio channel (0 to activeChannelCount-1)
} EHIF_CMD_PS_AUDIO_STATS_DATA_T;

/// Command parameters for \ref EHIF_CMD_RFT_RXPER
typedef struct {
    uint16_t cycleCount;                   ///< Number of RF channel cycles
    uint32_t nwkId;                        ///< Network ID
    uint32_t timeout;                      ///< Timeout for synchronization with transmitter at startup in number of units of 10 ms
    uint8_t  rfChannel;                    ///< 0 = Use configured RF channel mask, 1-18 = Use specified single RF channel
} EHIF_CMD_RFT_RXPER_PARAM_T;

/// Command data for \ref EHIF_CMD_RFT_RXPER
typedef struct {
    uint32_t cycleCount;                   ///< Actual number of RF channel cycles
    uint32_t rfChannelCount;               ///< Number of RF channels used
    uint32_t allOkTotal;                   ///< Sum of pChannelResults[].allOkCount
    uint32_t syncErrTotal;                 ///< Sum of pChannelResults[].syncErrCount
    uint32_t hdrErrTotal;                  ///< Sum of pChannelResults[].hdrErrCount
    uint32_t sliceErrTotal;                ///< Sum of pChannelResults[].sliceErrCount
    struct {
        uint16_t allOkCount;               ///< Number of packets received with no errors at all
        uint16_t syncErrCount;             ///< Number of packet timeouts (no valid synchronization word received)
        uint16_t hdrErrCount;              ///< Number of packets with header error
        int32_t  rssiMean            : 8;  ///< Mean RSSI for packets with OK header
        uint32_t sliceErrCount       : 24; ///< Total number of slice errors in packets with OK header (16 slices per packet)
        uint8_t  rssiStdDev;               ///< Standard deviation of RSSI values for packets with OK header
    } pChannelStats[20];                   ///< Results for each RF channel (0 to 19)
} EHIF_CMD_RFT_RXPER_DATA_T;

/// Command parameters for \ref EHIF_CMD_RFT_TXPER
typedef struct {
    uint16_t cycleCount;                   ///< Number of RF channel cycles
    uint32_t nwkId;                        ///< Network ID
    int8_t   txPower;                      ///< Target TX output power in dBm
    uint8_t  rfChannel;                    ///< 0 = Use configured RF channel mask, 1-18 = Use specified single RF channel
} EHIF_CMD_RFT_TXPER_PARAM_T;

/// Command parameters for \ref EHIF_CMD_RFT_TXTST_PN
typedef struct {
    uint8_t  enable                  : 1;  ///< Enable/disable output of continuous PN sequence
    uint8_t  reserved0               : 7;  ///< Reserved
    uint8_t  rfFreq;                       ///< RF frequency to transmit at
    int8_t   txPower;                      ///< Target ouptut power in dBm
} EHIF_CMD_RFT_TXTST_PN_PARAM_T;

/// Command parameters for \ref EHIF_CMD_RFT_TXTST_CW
typedef struct {
    uint8_t  enable                  : 1;  ///< Enable/disable output of continuous tone
    uint8_t  reserved0               : 7;  ///< Reserved
    uint8_t  rfFreq;                       ///< RF frequency to transmit at
    int8_t   txPower;                      ///< Target ouptut power in dBm
} EHIF_CMD_RFT_TXTST_CW_PARAM_T;

/// Command parameters for \ref EHIF_CMD_VC_GET_VOLUME
typedef struct {
    uint8_t  isLocalOrChannelOffset  : 1;  ///< Master: global/local Slave: volume/channelOffset
    uint8_t  isInVol                 : 1;  ///< Output volume/input volume
    uint8_t  logChannel              : 4;  ///< Specifies logical channel when requesting channel offset
    uint8_t  reserved0               : 2;  ///< Reserved (set to 0b00)
} EHIF_CMD_VC_GET_VOLUME_PARAM_T;

/// Command data for \ref EHIF_CMD_VC_GET_VOLUME
typedef struct {
    int16_t volume;                        ///< Volume setting of device
} EHIF_CMD_VC_GET_VOLUME_DATA_T;

/// Command parameters for \ref EHIF_CMD_VC_SET_VOLUME
typedef struct {
    int32_t  value                   : 11; ///< Set operation value (absolute or relative)
    uint32_t reserved1               : 5;  ///< Reserved (set to 0b00000)
    uint32_t logChannel              : 4;  ///< Logical channel, valid when log. channel offset
    uint32_t setOp                   : 2;  ///< None/Absolute/relative/log. channel offset
    uint32_t muteOp                  : 2;  ///< None/mute/unmute/mute toggle
    uint32_t isLocal                 : 1;  ///< Slave global/local (currently only relevant for master)
    uint32_t isInVol                 : 1;  ///< Output volume/input volume
    uint32_t source                  : 2;  ///< Operation source (set to 0b00 = EHIF)
    uint32_t reserved0               : 4;  ///< Reserved (set to 0b0000)
} EHIF_CMD_VC_SET_VOLUME_PARAM_T;

/// Command parameters for \ref EHIF_CMD_EHC_EVT_CLR
typedef struct {
    uint8_t  clearedEvents;                ///< Bit mask of EHIF SPI status word event flags to clear
} EHIF_CMD_EHC_EVT_CLR_PARAM_T;

/// Command parameters for \ref EHIF_CMD_EHC_EVT_MASK
typedef struct {
    uint8_t  irqGioLevel             : 1;  ///< Event interrupt pin active level
    uint8_t  reserved0               : 7;  ///< Reserved
    uint8_t  eventFilter;                  ///< Bit mask of EHIF SPI status word event flags to generate interrupt
} EHIF_CMD_EHC_EVT_MASK_PARAM_T;

/// Command parameters for \ref EHIF_CMD_PM_SET_STATE
typedef struct {
    uint8_t  state;                        ///< New power state
} EHIF_CMD_PM_SET_STATE_PARAM_T;

/// Command parameters for EHIF_CMD_PM_GET_DATA_DATA_T
typedef struct {
    uint32_t inSilenceDuration;            ///< Time the local audio inputs have been silent in units of 10 ms
    uint32_t outSilenceDuration;           ///< Time the local audio outputs have been silent in units of 10 ms
    uint32_t nwkInactivityDuration;        ///< Time without network connection in units of 10 ms
    uint16_t vbatVoltage;                  ///< Last measured VBAT voltage in millivolts (0 when disabled)
} EHIF_CMD_PM_GET_DATA_DATA_T;

/// Command data for \ref EHIF_CMD_DI_GET_DEVICE_INFO
typedef struct {
    uint32_t deviceId;                     ///< Device ID specified
    uint32_t mfctId;                       ///< Manufacturer Id specified by costumer
    uint32_t prodId;                       ///< Product Id specified by costumer
} EHIF_CMD_DI_GET_DEVICE_INFO_DATA_T;

/// Command parameters for \ref EHIF_CMD_DI_GET_CHIP_INFO
typedef struct {
    uint16_t ignored;                      ///< Ignored
} EHIF_CMD_DI_GET_CHIP_INFO_PARAM_T;

/// Command data for \ref EHIF_CMD_DI_GET_CHIP_INFO
typedef struct {
    uint16_t famId;                        ///< Chip Family ID (0x2505)
    uint16_t siRev;                        ///< Silicon revision (as read from analog hardware)
    uint32_t romRev;                       ///< ROM image revision number
    uint32_t nvmRev;                       ///< NVM image revision number
    uint32_t reserved0;                    ///< ROM image size
    uint32_t nvmSize;                      ///< NVM image size
    uint16_t chipId;                       ///< Chip ID (ie. 0x8520 for the first device)
    uint16_t chipCaps;                     ///< Chip capabilites
} EHIF_CMD_DI_GET_CHIP_INFO_DATA_T;

/// Command parameters for \ref EHIF_CMD_AT_GEN_TONE
typedef struct {
    uint8_t  logChannel;                   ///< Logical channel to generate tone on
    uint8_t  reserved0;                    ///< Reserved for future extensions, set to 0
    uint8_t  amplitude;                    ///< Amplitude of tone to generate
    uint16_t freq;                         ///< Frequency of tone to generate
} EHIF_CMD_AT_GEN_TONE_PARAM_T;

/// Command parameters for \ref EHIF_CMD_AT_DET_TONE
typedef struct {
    uint8_t  logChannel;                   ///< Logical channel to activate tone detector for
} EHIF_CMD_AT_DET_TONE_PARAM_T;

/// Command data for \ref EHIF_CMD_AT_DET_TONE
typedef struct {
    uint16_t amplitude;                    ///< Estimated amplitude of tone
    uint16_t freq;                         ///< Estimated frequency of tone
} EHIF_CMD_AT_DET_TONE_DATA_T;

/// Command parameters for \ref EHIF_CMD_IOTST_INPUT
typedef union {
    uint32_t pinMask;                      ///< Pin mask for reading IO pin values
    struct {
        uint32_t gioxSel             : 15; ///< One bit for GIO1 to GIO15: 1 to read, 0 to ignore
        uint32_t reserved0           : 1;  ///< Reserved (set to 0)
        uint32_t usbSel              : 1;  ///< Read USB data lines if set
        uint32_t reserved1           : 15; ///< Reserved (set to 0)
    };
} EHIF_CMD_IOTST_INPUT_PARAM_T;

/// Command data for \ref EHIF_CMD_IOTST_INPUT
typedef union {
    uint32_t pinVal;                       ///< Pin mask defining IO pin values
    struct {
        uint32_t gioxVal             : 15; ///< One bit for GIO1 to GIO15: 1 to read, 0 to revert to original state
        uint32_t reserved1           : 1;  ///< Reserved for future extensions
        uint32_t usbNVal             : 1;  ///< Value of USB_N if usbPnSel
        uint32_t usbPVal             : 1;  ///< Value of USB_P if usbPnSel
        uint32_t usbDiffVal          : 1;  ///< Value to output differentially if usbDiffSel
        uint32_t reserved0           : 13; ///< Reserved for future extensions
    };
} EHIF_CMD_IOTST_INPUT_DATA_T;

/// Command parameters for \ref EHIF_CMD_IOTST_OUTPUT
typedef union {
    struct {
        uint32_t pinMask;                  ///< Pin mask for setting IO pin values
        uint32_t pinVal;                   ///< Pin mask defining IO pin values
    };
    struct {
        // pinMask
        uint32_t gioxSel             : 15; ///< One bit for GIO1 to GIO15: 1 to modify, 0 to revert to original state
        uint32_t reserved1           : 1;  ///< Reserved for future extensions
        uint32_t usbPnSel            : 1;  ///< Write usbPnVal data if set
        uint32_t usbDiffSel          : 1;  ///< Write usbDiffVal data if set
        uint32_t reserved0           : 14; ///< Reserved for future extensions
        // pinVal
        uint32_t gioxVal             : 15; ///< One bit for GIO1 to GIO15: Output level
        uint32_t reserved3           : 1;  ///< Reserved for future extensions
        uint32_t usbPnVal            : 2;  ///< Value to output on USB_P/USB_N if usbPnSel
        uint32_t usbDiffVal          : 1;  ///< Value to output differentially if usbDiffSel
        uint32_t reserved2           : 13; ///< Reserved for future extensions
    };
} EHIF_CMD_IOTST_OUTPUT_PARAM_T;

/// Command parameters for \ref EHIF_CMD_RFT_RXTST_CONT
typedef struct {
    uint8_t  enable                  : 1;  ///< Enable/disable continuous receive
    uint8_t  reserved0               : 7;  ///< Reserved
    uint8_t  rfFreq;                       ///< RF frequency to receive at
} EHIF_CMD_RFT_RXTST_CONT_PARAM_T;

/// Command parameters for \ref EHIF_CMD_RFT_RXTST_RSSI
typedef struct {
    uint8_t  rfFreq;                       ///< Frequency to retreive RSSI value for
} EHIF_CMD_RFT_RXTST_RSSI_PARAM_T;

/// Command data for \ref EHIF_CMD_RFT_RXTST_RSSI
typedef struct {
    int8_t   rssi;                         ///< Measured RSSI (received signal strength) value in dBm
} EHIF_CMD_RFT_RXTST_RSSI_DATA_T;

/// Command parameters for \ref EHIF_CMD_RFT_NWKSIM
typedef struct {
    uint16_t tsPeriod;                     ///< Timeslot period [microseconds]
    uint16_t txMinDuration;                ///< Master packet minimum duration [microseconds]
    uint16_t txMaxDuration;                ///< Master packet maximum duration [microseconds]
    uint16_t rxDuration;                   ///< RX duration per SP slot [microseconds]
    uint8_t  spSlotCount;                  ///< Number of SP slots
    int8_t   txPower;                      ///< Target TX output power [dBm]
    uint8_t  pActiveChannels[4];           ///< List of active channels
} EHIF_CMD_RFT_NWKSIM_PARAM_T;

/// Command parameters for \ref EHIF_CMD_CAL_SET_DATA
typedef struct {
    uint8_t  writeKey;                     ///< Write key
    int32_t  txPower                 : 8;  ///< TX power in dBm
    int32_t  reserved0               : 24; ///< Reserved for future extensions
} EHIF_CMD_CAL_SET_DATA_PARAM_T;

/// Command data for \ref EHIF_CMD_CAL_GET_DATA
typedef struct {
    uint32_t calInvalid              : 1;  ///< Cleared if override values have been written
    int32_t  res0                    : 23; ///< Reserved
    int32_t  txPower                 : 8;  ///< Overrides configured TX output power target [dBm]
} EHIF_CMD_CAL_GET_DATA_DATA_T;

/// Command data for \ref EHIF_CMD_IO_GET_PIN_VAL
typedef struct {
    uint32_t pinVal                  : 15; ///< Pin mask defining IO pin values (bit 0 = GIO1, bit 14 = GIO15)
    uint32_t reserved0               : 17; ///< Reserved for future extensions
} EHIF_CMD_IO_GET_PIN_VAL_DATA_T;

/// Command parameters for \ref EHIF_CMD_NVS_GET_DATA
typedef struct {
    uint8_t  index                   : 1;  ///< Index of 32-bit data slot to read (autonomous slave: 0 = network ID, host-controlled master/slave: 0/1 = custom data)
    uint8_t  reserved0               : 7;  ///< Reserved for future extensions
} EHIF_CMD_NVS_GET_DATA_PARAM_T;

/// Command data for \ref EHIF_CMD_NVS_GET_DATA
typedef struct {
    uint32_t data;                         ///< Read data (0xFFFFFFFF if the storage is not previously used)
} EHIF_CMD_NVS_GET_DATA_DATA_T;

/// Command parameters for \ref EHIF_CMD_NVS_SET_DATA
typedef struct {
    uint8_t  index                   : 1;  ///< Index of 32-bit data slot to write (autonomous slave: 0 = network ID, host-controlled master/slave: 0/1 = custom data)
    uint8_t  reserved0               : 7;  ///< Reserved for future extensions
    uint32_t data;                         ///< Data to be written (NB: Writing 0xFFFFFFFF to slot 0 will be ignored)
} EHIF_CMD_NVS_SET_DATA_PARAM_T;

/// Command parameters for \ref EHIF_CMD_RC_SET_DATA
typedef struct {
    uint8_t  rcCmdCount              : 3;  ///< Number of transmitted proprietary command codes following the keyboard codes
    uint8_t  rcKeyCount              : 3;  ///< Number of transmitted keyboard codes at start of pRcCmds[] (pRcCmds[0] = modifiers)
    uint8_t  extSel                  : 2;  ///< Extension selection (part after pRcCmds[]): 0 = none, 1 = mouse
    uint8_t  pRcCmds[7];                   ///< Active keyboard codes (rcKeyCount) followed by active proprietary command codes (rcCmdCount)
    uint8_t  bvMouseButtons;               ///< Bit-vector specifying pressed mouse buttons (left, right etc.)
    uint16_t mousePosX;                    ///< Mouse horizontal position
    uint16_t mousePosY;                    ///< Mouse vertical position
} EHIF_CMD_RC_SET_DATA_PARAM_T;

/// Command parameters for \ref EHIF_CMD_RC_GET_DATA
typedef struct {
    uint8_t  spSlot;                       ///< Index of WPS slave slot to return state for (\ref EHIF_CMD_NWM_GET_STATUS_MASTER_DATA_T.pWpsInfo[n].spSlot)
} EHIF_CMD_RC_GET_DATA_PARAM_T;

/// Command data for \ref EHIF_CMD_RC_GET_DATA
typedef struct {
    uint8_t  rcCmdCount              : 3;  ///< Number of transmitted proprietary command codes following the keyboard codes
    uint8_t  rcKeyCount              : 3;  ///< Number of transmitted keyboard codes at start of pRcCmds[] (pRcCmds[0] = modifiers)
    uint8_t  extSel                  : 2;  ///< Extension selection (part after pRcCmds[]): 0 = none, 1 = mouse
    uint8_t  pRcCmds[7];                   ///< Active keyboard codes (rcKeyCount) followed by active proprietary command codes (rcCmdCount)
    uint8_t  bvMouseButtons;               ///< Bit-vector specifying pressed mouse buttons (left, right etc.)
    uint16_t mousePosX;                    ///< Mouse horizontal position
    uint16_t mousePosY;                    ///< Mouse vertical position
} EHIF_CMD_RC_GET_DATA_DATA_T;

//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// EHIF command buffers

/// SPI command parameter buffer type
typedef union {
    EHIF_CMD_DI_GET_CHIP_INFO_PARAM_T      diGetChipId;
    EHIF_CMD_PM_SET_STATE_PARAM_T          pmSetState;
    EHIF_CMD_EHC_EVT_MASK_PARAM_T          ehcEvtMask;
    EHIF_CMD_EHC_EVT_CLR_PARAM_T           ehcEvtClr;
    EHIF_CMD_VC_SET_VOLUME_PARAM_T         vcSetVolume;
    EHIF_CMD_VC_GET_VOLUME_PARAM_T         vcGetVolume;
    EHIF_CMD_NWM_DO_SCAN_PARAM_T           nwmDoScan;
    EHIF_CMD_NWM_DO_JOIN_PARAM_T           nwmDoJoin;
    EHIF_CMD_NWM_ACH_SET_USAGE_PARAM_T     nwmAchSetUsage;
    EHIF_CMD_NWM_CONTROL_ENABLE_PARAM_T    nwmControlEnable;
    EHIF_CMD_NWM_CONTROL_SIGNAL_PARAM_T    nwmControlSignal;
    EHIF_CMD_NWM_SET_RF_CH_MASK_PARAM_T    nwmSetRfChMask;
    EHIF_CMD_RFT_TXTST_CW_PARAM_T          rftTxtstCw;
    EHIF_CMD_RFT_TXTST_PN_PARAM_T          rftTxtstPn;
    EHIF_CMD_RFT_TXPER_PARAM_T             rftTxPer;
    EHIF_CMD_RFT_RXPER_PARAM_T             rftRxPer;
    EHIF_CMD_DSC_TX_DATAGRAM_PARAM_T       dscTxDatagram;
    EHIF_CMD_AT_GEN_TONE_PARAM_T           atGenTone;
    EHIF_CMD_AT_DET_TONE_PARAM_T           atDetTone;
    EHIF_CMD_IOTST_INPUT_PARAM_T           iotstInput;
    EHIF_CMD_IOTST_OUTPUT_PARAM_T          iotstOutput;
    EHIF_CMD_RFT_RXTST_CONT_PARAM_T        rftRxtstCont;
    EHIF_CMD_RFT_RXTST_RSSI_PARAM_T        rftRxtstRssi;
    EHIF_CMD_RFT_NWKSIM_PARAM_T            rftNwksim;
    EHIF_CMD_CAL_SET_DATA_PARAM_T          calSetData;
    EHIF_CMD_NVS_GET_DATA_PARAM_T          nvsGetData;
    EHIF_CMD_NVS_SET_DATA_PARAM_T          nvsSetData;
    EHIF_CMD_RC_SET_DATA_PARAM_T           rcSetData;
    EHIF_CMD_RC_GET_DATA_PARAM_T           rcGetData;
} EHIF_CMD_PARAM_T;

/// SPI command R/W data buffer type
typedef union {
    EHIF_CMD_DI_GET_CHIP_INFO_DATA_T       diGetChipInfo;
    EHIF_CMD_DI_GET_DEVICE_INFO_DATA_T     diGetDeviceInfo;
    EHIF_CMD_PM_GET_DATA_DATA_T            pmGetData;
    EHIF_CMD_VC_GET_VOLUME_DATA_T          vcGetVolume;
    EHIF_CMD_NWM_GET_STATUS_SLAVE_DATA_T   nwmGetStatusSlave;
    EHIF_CMD_NWM_GET_STATUS_MASTER_DATA_T  nwmGetStatusMaster;
    EHIF_CMD_AT_DET_TONE_DATA_T            atDetTone;
    EHIF_CMD_IOTST_INPUT_DATA_T            iotstInput;
    EHIF_CMD_RFT_RXTST_RSSI_DATA_T         rftRxtstRssi;
    EHIF_CMD_CAL_GET_DATA_DATA_T           calGetData;
    EHIF_CMD_PS_RF_STATS_DATA_T            psRfStats;
    EHIF_CMD_NVS_GET_DATA_DATA_T           nvsGetData;
    EHIF_CMD_IO_GET_PIN_VAL_DATA_T         ioGetPinVal;
    EHIF_CMD_RC_GET_DATA_DATA_T            rcGetData;
} EHIF_CMD_DATA_T;

//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
/// \name Remote Control Commands
//@{

#define EHIF_RC_CMD_OUT_VOL_MUTE_TOGGLE 1  ///< Output volume mute
#define EHIF_RC_CMD_OUT_VOL_INCR        2  ///< Output volume increment
#define EHIF_RC_CMD_OUT_VOL_DECR        3  ///< Output volume increment
#define EHIF_RC_CMD_IN_VOL_MUTE_TOGGLE  4  ///< Input volume mute
#define EHIF_RC_CMD_NWK_STANDBY_ENABLE  5  ///< Network standby enable
#define EHIF_RC_CMD_NWK_STANDBY_DISABLE 6  ///< Network standby disable
#define EHIF_RC_CMD_NWK_STANDBY_TOGGLE  7  ///< Network standby toggle
#define EHIF_RC_CMD_PLAY_PAUSE_TOGGLE   16 ///< Play/Pause
#define EHIF_RC_CMD_SCAN_NEXT_TRACK     17 ///< Scan next track
#define EHIF_RC_CMD_SCAN_PREV_TRACK     18 ///< Scan previous track
#define EHIF_RC_CMD_FAST_FORWARD        19 ///< Fast forward
#define EHIF_RC_CMD_REWIND              20 ///< Rewind

//@}
//-------------------------------------------------------------------------------------------------------


#endif

//@}
