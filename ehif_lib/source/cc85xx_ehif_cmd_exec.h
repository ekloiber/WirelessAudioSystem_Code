/** \addtogroup module_ehif_cmd_exec Command Execution Framework
 *
 * \brief Execution framework for all EHIF commands except bootloader
 *
 *
 * \section section_ehif_cmd_exec_overview Overview
 * The EHIF command execution framework provides the entire EHIF command set, and is based on passing
 * parameter and data structures between the host processor application and the framework functions.
 * Structure type definitions for command phase and data phase are found in the \ref module_ehif_defs as
 * \c EHIF_CMD_XXXXX_PARAM_T and \c EHIF_CMD_XXXXX_DATA_T, where \c XXXXX is the name of the EHIF
 * command, e.g. \c VC_SET_VOLUME.
 *
 * The EHIF command set is divided into four different classes, and a command is executed as follows:
 * - The host processor prepares the parameter structure (for the CMD_REQ operation)
 * - Depending on the type of data phase (check function documentation for which commands they support):
 *     - For commands with <b>no data phase</b>, the host processor:
 *         - Calls \ref ehifCmdExec()
 *     - For commands with <b>outgoing fixed-size data phase </b>, the host processor:
 *         - Calls \ref ehifCmdExecWithRead()
 *         - Interprets the data structure (from the READ operation)
 *     - For commands with <b>outgoing variable-size data phase</b>, the host processor:
 *         - Calls \ref ehifCmdExecWithReadbc()
 *         - Interprets and sanity checks variable length value (from the READBC operation)
 *         - Interprets the data structure (from the READBC operation)
 *     - For commands with <b>ingoing data phase</b>, the host processor:
 *         - Prepares the data structure (for the WRITE operation)
 *         - Calls \ref ehifCmdExecWithWrite()
 *
 * For example, the following code executes NVS_GET_DATA to get the stored network ID:
 * \code
 * EHIF_CMD_PARAM_T ehifCmdParam;
 * EHIF_CMD_DATA_T  ehifCmdData;
 *
 * memset(&ehifCmdParam, 0x00, sizeof(EHIF_CMD_NVS_GET_DATA_PARAM_T));
 * ehifCmdParam.nvsGetData.index = 0;
 * ehifCmdExecWithRead(EHIF_EXEC_ALL, EHIF_CMD_NVS_GET_DATA,
 *                     sizeof(EHIF_CMD_NVS_GET_DATA_PARAM_T), &ehifCmdParam,
 *                     sizeof(EHIF_CMD_NVS_GET_DATA_DATA_T), &ehifCmdData);
 * uint32_t nwkId = ehifCmdData.nvsGetData.data;
 * \endcode
 *
 *
 * \section section_ehif_cmd_exec_endianess Endianess Handling
 * The EHIF command format is big-endian, meaning that values that span over multiple bytes are
 * transferred most significant byte (MSB) first and least significant byte (LSB) last. Many, if not
 * most, popular microcontrollers are little-endian, meaning that the LSB is stored at the lower memory
 * address and MSB at the higher memory address. This means that data structures cannot be transferred
 * directly, byte by byte, between host processor memory and the SPI interface.
 *
 * The main purpose if the command execution framework is to solve this issue, and so it comes in two
 * different versions, one for little-endian microcontrollers/compilers and another for big-endian:
 * - For little-endian microcontrollers/compilers:
 *     - Include the little-endian version of cc85xx_ehif_field_op.c
 *     - Used by application examples written in IAR for MSP430 and 8051
 * - For big-endian microcontrollers/compilers
 *     - Include the big-endian version of cc85xx_ehif_field_op.c
 *
 * For ease of use and improved code readability, the structure definitions utilize bit-fields. The
 * disadvantage of doing so is that bit-field handling is not specified in the C standard, and is
 * therefore compiler specific. The structure definitions therefore also come in two versions:
 * - With bit-fields within a data container (uint8_t, uint16_t or uint32_t) reversed:
 *     - Include the little-endian version of cc85xx_ehif_defs.h
 *     - Used by application examples written in IAR for MSP430 and 8051
 * - With bit-fields in normal big-endian order:
 *     - Include the little-endian version of cc85xx_ehif_defs.h
 *
 * Please refer to the compiler documentation for details on endianess and bit-field handling when using
 * other than the mentioned compiler and microcontrollers.
 *
 *
 * \section section_ehif_cmd_exec_long_time Commands With Long Execution Time
 * EHIF commands that take more than 10 ms to complete must be split and executed in two stages, with an
 * application-specific timeout in between. This is for instance the case for NWM_DO_SCAN, which can take
 * many seconds to complete when doing network pairing:
 * \code
 * EHIF_CMD_PARAM_T ehifCmdParam;
 * EHIF_CMD_DATA_T  ehifCmdData;
 *
 * // Search for one protocol master with pairing signal enabled for 10 seconds
 * memset(&ehifCmdParam, 0x00, sizeof(EHIF_CMD_NWM_DO_SCAN_PARAM_T));
 * ehifCmdParam.nwmDoScan.scanTo           = 1000;
 * ehifCmdParam.nwmDoScan.scanMax          = 1;
 * ehifCmdParam.nwmDoScan.reqPairingSignal = 1;
 * ehifCmdParam.nwmDoScan.reqRssi          = -128;
 * ehifCmdExecWithReadbc(EHIF_EXEC_CMD, EHIF_CMD_NWM_DO_SCAN,
 *                       sizeof(EHIF_CMD_NWM_DO_SCAN_PARAM_T),
 *                       &ehifCmdParam, NULL, NULL);
 *
 * // Fetch network information once ready
 * ehifWaitReadyMs(12000); // 10 seconds + some margin
 * readbcLength = sizeof(ehifNwmDoScanData);
 * ehifCmdExecWithReadbc(EHIF_EXEC_DATA, EHIF_CMD_NWM_DO_SCAN,
 *                       0, NULL,
 *                       &readbcLength, &ehifNwmDoScanData);
 * \endcode
 *
 *
 * \section section_ehif_cmd_exec_error Timeouts and Error Handling
 * When experiencing electrical errors (e.g. due to unstable power supply) or production testing fails
 * (e.g. due to PCB production faults), the host processor application could potentially get stuck in
 * infinite loops while waiting for CMD_REQ_READY from CC85XX. To avoid this, the execution framework
 * implements timeout mechanisms. The \ref ehifGetWaitReadyError() function indicates whether a violation
 * has occurred.
 *
 * The host processor is responsible for error detection and handling. Routine error checking should as a
 * minimum include:
 * - Polling the EHIF status word at regular intervals to check for \ref BV_EHIF_EVT_SPI_ERROR
 * - Calling ehifGetWaitReadyError() at regular intervals to check for CMD_REQ_READY violation due to
 *   timeout error. In most cases this will also generate \ref BV_EHIF_EVT_SPI_ERROR
 *
 * @{
 */
#ifndef CC85XX_EHIF_CMD_EXEC_H_
#define CC85XX_EHIF_CMD_EXEC_H_

#include <stdint.h>


//-------------------------------------------------------------------------------------------------------
/// \name Execution Selection
/// Possible values of the \c execSel argument for the ehifCmdExecWithXxxxx() functions
//@{

/// Execute the command phase only
#define EHIF_EXEC_CMD       BV(1)
/// Execute the data phase only
#define EHIF_EXEC_DATA      BV(2)
/// Execute command and data phases
#define EHIF_EXEC_ALL       (EHIF_EXEC_CMD | EHIF_EXEC_DATA)

//@}
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// Function prototypes
void ehifCmdExec(uint8_t cmd, uint8_t cmdLength, const void* pCmdParam);
void ehifCmdExecWithRead(uint8_t execSel, uint8_t cmd, uint8_t cmdLength, const void* pCmdParam, uint16_t dataLength, void* pReadData);
void ehifCmdExecWithReadbc(uint8_t execSel, uint8_t cmd, uint8_t cmdLength, const void* pCmdParam, uint16_t* pDataVarLength, void* pReadbcData);
void ehifCmdExecWithWrite(uint8_t execSel, uint8_t cmd, uint8_t cmdLength, const void* pCmdParam, uint16_t dataLength, const void* pWriteData);
//-------------------------------------------------------------------------------------------------------


#endif
//@}
