/** \addtogroup module_ehif_cmd_exec Command Execution Framework
 *
 * @{
 */
#include "cc85xx_ehif_utils.h"
#include "cc85xx_ehif_cmd_exec.h"
#include <cc85xx_ehif_defs.h>
#include <cc85xx_ehif_field_op.h>
#include <cc85xx_ehif_hal_mcu.h>
#include <cc85xx_ehif_hal_board.h>




/// Adds n 8-bit fields
#define F8(n)   ((((n) - 1) << 2) | 1)
/// Adds n 16-bit fields
#define F16(n)  ((((n) - 1) << 2) | 2)
/// Adds n 32-bit fields
#define F32(n)  ((((n) - 1) << 2) | 3)




/** \brief Executes EHIF commands without data phase, with automatic endianess conversion
 *
 * This function supports the following EHIF commands (see \ref ehifCmdExecWithRead(),
 * \ref ehifCmdExecWithReadbc() and \ref ehifCmdExecWithWrite() for other EHIF commands):
 * - \ref EHIF_CMD_EHC_EVT_CLR
 * - \ref EHIF_CMD_EHC_EVT_MASK
 * - \ref EHIF_CMD_NWM_DO_JOIN
 * - \ref EHIF_CMD_NWM_ACH_SET_USAGE
 * - \ref EHIF_CMD_NWM_CONTROL_ENABLE
 * - \ref EHIF_CMD_NWM_CONTROL_SIGNAL
 * - \ref EHIF_CMD_NWM_SET_RF_CH_MASK
 * - \ref EHIF_CMD_RC_SET_DATA
 * - \ref EHIF_CMD_PM_SET_STATE
 * - \ref EHIF_CMD_VC_SET_VOLUME
 * - \ref EHIF_CMD_CAL_SET_DATA
 * - \ref EHIF_CMD_NVS_SET_DATA
 * - \ref EHIF_CMD_RFT_TXPER
 * - \ref EHIF_CMD_RFT_TXTST_PN
 * - \ref EHIF_CMD_RFT_TXTST_CW
 * - \ref EHIF_CMD_RFT_RXTST_CONT
 * - \ref EHIF_CMD_RFT_NWKSIM
 * - \ref EHIF_CMD_AT_GEN_TONE
 * - \ref EHIF_CMD_IOTST_OUTPUT
 *
 * The function waits for command completion at the start (for at most 10 ms), but not at the end.
 *
 * \param[in]       cmd
 *     One of the command IDs listed above, EHIF_CMD_XXXXX
 * \param[in]       cmdLength
 *     Command parameter length, in most cases sizeof(EHIF_CMD_XXXXX_PARAM_T)
 * \param[in]       *pCmdParam
 *     Pointer to the command parameter structure, EHIF_CMD_XXXXX_PARAM_T
 */
void ehifCmdExec(uint8_t cmd, uint8_t cmdLength, const void* pCmdParam) {

    // Locate endianess conversion specification for CMD_REQ
    const int8_t* pFieldSpec = NULL;
    switch (cmd) {
    case EHIF_CMD_EHC_EVT_CLR:        { static const int8_t pF[] = { F8(1), 0 };                 pFieldSpec = pF; } break;
    case EHIF_CMD_EHC_EVT_MASK:       { static const int8_t pF[] = { F8(2), 0 };                 pFieldSpec = pF; } break;
    case EHIF_CMD_NWM_DO_JOIN:        { static const int8_t pF[] = { F16(1), F32(4), 0 };        pFieldSpec = pF; } break;
    case EHIF_CMD_NWM_ACH_SET_USAGE:  { static const int8_t pF[] = { F8(16), 0 };                pFieldSpec = pF; } break;
    case EHIF_CMD_NWM_CONTROL_ENABLE: { static const int8_t pF[] = { F8(2), 0 };                 pFieldSpec = pF; } break;
    case EHIF_CMD_NWM_CONTROL_SIGNAL: { static const int8_t pF[] = { F8(2), 0 };                 pFieldSpec = pF; } break;
    case EHIF_CMD_NWM_SET_RF_CH_MASK: { static const int8_t pF[] = { F32(1), 0 };                pFieldSpec = pF; } break;
    case EHIF_CMD_RC_SET_DATA:        { static const int8_t pF[] = { F8(9), F16(2), 0 };         pFieldSpec = pF; } break;
    case EHIF_CMD_PM_SET_STATE:       { static const int8_t pF[] = { F8(1), 0 };                 pFieldSpec = pF; } break;
    case EHIF_CMD_VC_SET_VOLUME:      { static const int8_t pF[] = { F32(1), 0 };                pFieldSpec = pF; } break;
    case EHIF_CMD_CAL_SET_DATA:       { static const int8_t pF[] = { F8(1), F32(1), 0 };         pFieldSpec = pF; } break;
    case EHIF_CMD_NVS_SET_DATA:       { static const int8_t pF[] = { F8(1), F32(1), 0 };         pFieldSpec = pF; } break;
    case EHIF_CMD_RFT_TXPER:          { static const int8_t pF[] = { F16(1), F32(1), F8(2), 0 }; pFieldSpec = pF; } break;
    case EHIF_CMD_RFT_TXTST_PN:       { static const int8_t pF[] = { F8(3), 0 };                 pFieldSpec = pF; } break;
    case EHIF_CMD_RFT_TXTST_CW:       { static const int8_t pF[] = { F8(3), 0 };                 pFieldSpec = pF; } break;
    case EHIF_CMD_RFT_RXTST_CONT:     { static const int8_t pF[] = { F8(2), 0 };                 pFieldSpec = pF; } break;
    case EHIF_CMD_RFT_NWKSIM:         { static const int8_t pF[] = { F16(4), F8(6), 0 };         pFieldSpec = pF; } break;
    case EHIF_CMD_AT_GEN_TONE:        { static const int8_t pF[] = { F8(3), F16(1), 0 };         pFieldSpec = pF; } break;
    case EHIF_CMD_IOTST_OUTPUT:       { static const int8_t pF[] = { F32(2), 0 };                pFieldSpec = pF; } break;
    default: return;
    }

    // Send CMD_REQ
    ehifFieldCmdReq(cmd, cmdLength, (const uint8_t*) pCmdParam, pFieldSpec);

} // ehifCmdExec




/** \brief Executes EHIF commands with READ data phase, with automatic endianess conversion
 *
 * This function supports the following EHIF commands (see \ref ehifCmdExec(),
 * \ref ehifCmdExecWithReadbc() and \ref ehifCmdExecWithWrite() for other EHIF commands):
 * - \ref EHIF_CMD_DI_GET_DEVICE_INFO
 * - \ref EHIF_CMD_DI_GET_CHIP_INFO
 * - \ref EHIF_CMD_VC_GET_VOLUME
 * - \ref EHIF_CMD_PS_RF_STATS
 * - \ref EHIF_CMD_PS_AUDIO_STATS
 * - \ref EHIF_CMD_RC_GET_DATA
 * - \ref EHIF_CMD_PM_GET_DATA
 * - \ref EHIF_CMD_CAL_GET_DATA
 * - \ref EHIF_CMD_IO_GET_PIN_VAL
 * - \ref EHIF_CMD_NVS_GET_DATA
 * - \ref EHIF_CMD_RFT_RXPER - split execution required
 * - \ref EHIF_CMD_RFT_RXTST_RSSI
 * - \ref EHIF_CMD_AT_DET_TONE
 * - \ref EHIF_CMD_IOTST_INPUT
 *
 * The function waits for command completion at the start (for at most 10 ms). \ref EHIF_CMD_RFT_RXPER
 * must be split due to long execution time, i.e. \ref ehifCmdExecWithRead() must be executed first with
 * \a execSel = \ref EHIF_EXEC_CMD and then with \a execSel = \ref EHIF_EXEC_DATA when the output is
 * ready. Use ehifWaitReadyMs() or other suitable waiting/timeout mechanism between the execution phases.
 *
 * \param[in]       execSel
 *     Selects whether to execute the command phase (\ref EHIF_EXEC_CMD), data phase
 *     (\ref EHIF_EXEC_DATA) or both (\ref EHIF_EXEC_ALL). See above for more information.
 * \param[in]       cmd
 *     One of the command IDs listed above, EHIF_CMD_XXXXX
 * \param[in]       cmdLength
 *     Command parameter length, in most cases sizeof(EHIF_CMD_XXXXX_PARAM_T)
 * \param[in]       *pCmdParam
 *     Pointer to the command parameter structure, EHIF_CMD_XXXXX_PARAM_T
 * \param[in]       dataLength
 *     Read data length, in most cases sizeof(EHIF_CMD_XXXXX_DATA_T)
 * \param[out]      *pReadData
 *     Pointer to the read data structure, EHIF_CMD_XXXXX_DATA_T
 */
void ehifCmdExecWithRead(uint8_t execSel, uint8_t cmd, uint8_t cmdLength, const void* pCmdParam, uint16_t dataLength, void* pReadData) {

    // Execute command phase?
    if (execSel & EHIF_EXEC_CMD) {
        const int8_t* pFieldSpec;

        // Locate endianess conversion specification for CMD_REQ
        switch (cmd) {
        case EHIF_CMD_DI_GET_DEVICE_INFO: { static const int8_t pF[] = { 0 };                        pFieldSpec = pF; } break;
        case EHIF_CMD_DI_GET_CHIP_INFO:   { static const int8_t pF[] = { F16(1), 0 };                pFieldSpec = pF; } break;
        case EHIF_CMD_VC_GET_VOLUME:      { static const int8_t pF[] = { F8(1), 0 };                 pFieldSpec = pF; } break;
        case EHIF_CMD_PS_RF_STATS:        { static const int8_t pF[] = { 0 };                        pFieldSpec = pF; } break;
        case EHIF_CMD_PS_AUDIO_STATS:     { static const int8_t pF[] = { 0 };                        pFieldSpec = pF; } break;
        case EHIF_CMD_RC_GET_DATA:        { static const int8_t pF[] = { F8(1), 0 };                 pFieldSpec = pF; } break;
        case EHIF_CMD_PM_GET_DATA:        { static const int8_t pF[] = { 0 };                        pFieldSpec = pF; } break;
        case EHIF_CMD_CAL_GET_DATA:       { static const int8_t pF[] = { 0 };                        pFieldSpec = pF; } break;
        case EHIF_CMD_IO_GET_PIN_VAL:     { static const int8_t pF[] = { 0 };                        pFieldSpec = pF; } break;
        case EHIF_CMD_NVS_GET_DATA:       { static const int8_t pF[] = { F8(1), 0 };                 pFieldSpec = pF; } break;
        case EHIF_CMD_RFT_RXPER:          { static const int8_t pF[] = { F16(1), F32(2), F8(1), 0 }; pFieldSpec = pF; } break;
        case EHIF_CMD_RFT_RXTST_RSSI:     { static const int8_t pF[] = { F8(1), 0 };                 pFieldSpec = pF; } break;
        case EHIF_CMD_AT_DET_TONE:        { static const int8_t pF[] = { F8(1), 0 };                 pFieldSpec = pF; } break;
        case EHIF_CMD_IOTST_INPUT:        { static const int8_t pF[] = { F32(1), 0 };                pFieldSpec = pF; } break;
        default: return;
        }

        // Send CMD_REQ
        ehifFieldCmdReq(cmd, cmdLength, (const uint8_t*) pCmdParam, pFieldSpec);
    }

    // Execute data phase?
    if (execSel & EHIF_EXEC_DATA) {
        const int8_t* pFieldSpec;

        // Locate endianess conversion specification for READ
        switch (cmd) {
        case EHIF_CMD_DI_GET_DEVICE_INFO: { static const int8_t pF[] = { F32(3), 0 };                         pFieldSpec = pF; } break;
        case EHIF_CMD_DI_GET_CHIP_INFO:   { static const int8_t pF[] = { F16(2), F32(4), F16(2), 0 };         pFieldSpec = pF; } break;
        case EHIF_CMD_VC_GET_VOLUME:      { static const int8_t pF[] = { F16(1), 0 };                         pFieldSpec = pF; } break;
        case EHIF_CMD_PS_RF_STATS:        { static const int8_t pF[] = { F32(5), F8(2), F16(21), 0 };         pFieldSpec = pF; } break;
        case EHIF_CMD_PS_AUDIO_STATS:     { static const int8_t pF[] = { F32(3), F16(1), F8(2), F16(2), -1 }; pFieldSpec = pF; } break;
        case EHIF_CMD_RC_GET_DATA:        { static const int8_t pF[] = { F8(9), F16(2), 0 };                  pFieldSpec = pF; } break;
        case EHIF_CMD_PM_GET_DATA:        { static const int8_t pF[] = { F32(3), F16(1), 0 };                 pFieldSpec = pF; } break;
        case EHIF_CMD_CAL_GET_DATA:       { static const int8_t pF[] = { F32(1), 0 };                         pFieldSpec = pF; } break;
        case EHIF_CMD_IO_GET_PIN_VAL:     { static const int8_t pF[] = { F32(1), 0 };                         pFieldSpec = pF; } break;
        case EHIF_CMD_NVS_GET_DATA:       { static const int8_t pF[] = { F32(1), 0 };                         pFieldSpec = pF; } break;
        case EHIF_CMD_RFT_RXPER:          { static const int8_t pF[] = { F32(6), F16(3), F32(1), F8(1), -3 }; pFieldSpec = pF; } break;
        case EHIF_CMD_RFT_RXTST_RSSI:     { static const int8_t pF[] = { F8(1), 0 };                          pFieldSpec = pF; } break;
        case EHIF_CMD_AT_DET_TONE:        { static const int8_t pF[] = { F16(2), 0 };                         pFieldSpec = pF; } break;
        case EHIF_CMD_IOTST_INPUT:        { static const int8_t pF[] = { F32(1), 0 };                         pFieldSpec = pF; } break;
        default: return;
        }

        // Send READ
        ehifFieldRead(dataLength, (uint8_t*) pReadData, pFieldSpec);
    }

} // ehifCmdExecWithRead




/** \brief Executes EHIF commands with READBC data phase, with automatic endianess conversion
 *
 * The number of bytes read is the minimum of the length function parameter and the length field returned
 * by the READBC command.
 *
 * This function supports the following EHIF commands (see \ref ehifCmdExec(),
 * \ref ehifCmdExecWithReadbc() and \ref ehifCmdExecWithWrite() for other EHIF commands):
 * - \ref EHIF_CMD_NWM_DO_SCAN - split execution required
 * - \ref EHIF_CMD_NWM_GET_STATUS_M
 * - \ref EHIF_CMD_NWM_GET_STATUS_S
 * - \ref EHIF_CMD_DSC_RX_DATAGRAM
 *
 * The function waits for command completion at the start (for at most 10 ms). \ref EHIF_CMD_NWM_DO_SCAN
 * must be split due to long execution time, i.e. \ref ehifCmdExecWithRead() must be executed first with
 * \a execSel = \ref EHIF_EXEC_CMD and then with \a execSel = \ref EHIF_EXEC_DATA when the output is
 * ready. Use ehifWaitReadyMs() or other suitable waiting/timeout mechanism between the execution phases.
 *
 * \param[in]       execSel
 *     Selects whether to execute the command phase (\ref EHIF_EXEC_CMD), data phase
 *     (\ref EHIF_EXEC_DATA) or both (\ref EHIF_EXEC_ALL). See above for more information.
 * \param[in]       cmd
 *     One of the command IDs listed above, EHIF_CMD_XXXXX
 * \param[in]       cmdLength
 *     Command parameter length, in most cases sizeof(EHIF_CMD_XXXXX_PARAM_T)
 * \param[in]       *pCmdParam
 *     Pointer to the command parameter structure, EHIF_CMD_XXXXX_PARAM_T
 * \param[in,out]   *pDataVarLength
 *     Upon function entry \a *pDataVarLength should indicate the size of \a pReadData[]. Upon function
 *     exit the value is changed to indicate the actual number of bytes read (0 to 4095)
 * \param[out]      *pReadbcData
 *     Pointer to the read data structure, EHIF_CMD_XXXXX_DATA_T
 */
void ehifCmdExecWithReadbc(uint8_t execSel, uint8_t cmd, uint8_t cmdLength, const void* pCmdParam, uint16_t* pDataVarLength, void* pReadbcData) {

    // Execute command phase?
    if (execSel & EHIF_EXEC_CMD) {
        const int8_t* pFieldSpec;

        // Locate endianess conversion specification for CMD_REQ
        switch (cmd) {
        case EHIF_CMD_NWM_DO_SCAN:        { static const int8_t pF[] = { F16(1), F32(3), F8(2), 0 }; pFieldSpec = pF; } break;
        case EHIF_CMD_NWM_GET_STATUS_M:   { static const int8_t pF[] = { 0 };                        pFieldSpec = pF; } break;
        case EHIF_CMD_NWM_GET_STATUS_S:   { static const int8_t pF[] = { 0 };                        pFieldSpec = pF; } break;
        case EHIF_CMD_DSC_RX_DATAGRAM:    { static const int8_t pF[] = { 0 };                        pFieldSpec = pF; } break;
        default: return;
        }

        // Send CMD_REQ
        ehifFieldCmdReq(cmd & 0x3F, cmdLength, (const uint8_t*) pCmdParam, pFieldSpec);
    }

    // Execute data phase?
    if (execSel & EHIF_EXEC_DATA) {
        const int8_t* pFieldSpec;

        // Locate endianess conversion specification for READBC
        switch (cmd) {
        case EHIF_CMD_NWM_DO_SCAN:        { static const int8_t pF[] = { F32(3), F8(1), F16(1), F8(9), F16(2), -5 }; pFieldSpec = pF; } break;
        case EHIF_CMD_NWM_GET_STATUS_M:   { static const int8_t pF[] = { F8(1), F16(2), F32(3), F16(1), F8(2), -3 }; pFieldSpec = pF; } break;
        case EHIF_CMD_NWM_GET_STATUS_S:   { static const int8_t pF[] = { F32(3), F8(1), F16(1), F8(9), F16(3), -5 }; pFieldSpec = pF; } break;
        case EHIF_CMD_DSC_RX_DATAGRAM:    { static const int8_t pF[] = { F8(1), F32(1), F8(1), -1 };                 pFieldSpec = pF; } break;
        default: return;
        }

        // Send READBC
        ehifFieldReadbc(pDataVarLength, (uint8_t*) pReadbcData, pFieldSpec);
    }

} // ehifCmdExecWithReadbc




/** \brief Executes EHIF commands with WRITE data phase, with automatic endianess conversion
 *
 * This function supports the following EHIF commands (see \ref ehifCmdExec(),
 * \ref ehifCmdExecWithReadbc() and \ref ehifCmdExecWithWrite() for other EHIF commands):
 * - \ref EHIF_CMD_DSC_TX_DATAGRAM
 *
 * The function waits for command completion at the start (for at most 10 ms).
 *
 * \param[in]       execSel
 *     Selects whether to execute the command phase (\ref EHIF_EXEC_CMD), data phase
 *     (\ref EHIF_EXEC_DATA) or both (\ref EHIF_EXEC_ALL).
 * \param[in]       cmd
 *     One of the command IDs listed above, EHIF_CMD_XXXXX
 * \param[in]       cmdLength
 *     Command parameter length, in most cases sizeof(EHIF_CMD_XXXXX_PARAM_T)
 * \param[in]       *pCmdParam
 *     Pointer to the command parameter structure, EHIF_CMD_XXXXX_PARAM_T
 * \param[in]       dataLength
 *     Write data length, in most cases sizeof(EHIF_CMD_XXXXX_DATA_T)
 * \param[in]       *pWriteData
 *     Pointer to the write data structure, EHIF_CMD_XXXXX_DATA_T
 */
void ehifCmdExecWithWrite(uint8_t execSel, uint8_t cmd, uint8_t cmdLength, const void* pCmdParam, uint16_t dataLength, const void* pWriteData) {

    // Execute command phase?
    if (execSel & EHIF_EXEC_CMD) {
        const int8_t* pFieldSpec;

        // Locate endianess conversion specification for CMD_REQ
        switch (cmd) {
        case EHIF_CMD_DSC_TX_DATAGRAM:    { static const int8_t pF[] = { F8(1), F32(1), 0 }; pFieldSpec = pF; } break;
        default: return;
        }

        // Send CMD_REQ
        ehifFieldCmdReq(cmd, cmdLength, (const uint8_t*) pCmdParam, pFieldSpec);
    }

    // Execute data phase?
    if (execSel & EHIF_EXEC_DATA) {
        const int8_t* pFieldSpec;

        // Locate endianess conversion specification for WRITE
        switch (cmd) {
        case EHIF_CMD_DSC_TX_DATAGRAM:    { static const int8_t pF[] = { F8(1), -1 }; pFieldSpec = pF; } break;
        default: return;
        }

        // Send WRITE
        ehifFieldWrite(dataLength, (const uint8_t*) pWriteData, pFieldSpec);
    }

} // ehifCmdExecWithWrite


//@}
