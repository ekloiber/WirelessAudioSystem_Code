/** \addtogroup module_ehif_field_op Basic Operations with Automatic Field Endianess Conversion
 *
 * This code is for big-endian microcontrollers/compilers.
 *
 * @{
 */
#include "../cc85xx_ehif_basic_op.h"




/** \brief Performs a WRITE operation without endianess conversion (both big)
 *
 * \param[in]       length
 *     Number of bytes to be written (0 to 4095)
 * \param[in]       *pData
 *     Pointer to data buffer to be written
 * \param[in]       *pFieldSpec
 *     Ignored
 *
 * \return
 *     EHIF status word at start of WRITE operation (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifFieldWrite(uint16_t length, const uint8_t* pData, const int8_t* pFieldSpec) {
    return ehifWrite(length, pData);
} // ehifFieldWrite




/** \brief Performs a READ operation without endianess conversion (both big)
 *
 * \param[in]       length
 *     Number of bytes to be read (0 to 4095)
 * \param[out]      *pData
 *     Pointer to storage buffer for read data
 * \param[in]       *pFieldSpec
 *     Ignored
 *
 * \return
 *     EHIF status word at start of READ operation (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifFieldRead(uint16_t length, uint8_t* pData, const int8_t* pFieldSpec) {
    return ehifRead(length, pData);
} // ehifFieldRead




/** \brief Performs a READBC operation without endianess conversion (both big)
 *
 * The number of bytes read is the minimum of the length function parameter and the length field returned
 * by the READBC command.
 *
 * \param[in,out]   *pVarLength
 *     Upon function entry \a *pVarLength should indicate the size of \a pData[]. Upon function exit the
 *     value is changed to indicate the actual number of bytes read (0 to 4095)
 * \param[out]      *pData
 *     Pointer to storage buffer for read data
 * \param[in]       *pFieldSpec
 *     Ignored
 *
 * \return
 *     EHIF status word at start of READBC operation (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifFieldReadbc(uint16_t *pVarLength, uint8_t* pData, const int8_t* pFieldSpec) {
    return ehifReadbc(pVarLength, pData);
} // ehifFieldReadbc




/** \brief Performs a CMD_REQ operation without endianess conversion (both big)
 *
 * \param[in]   cmd
 *     Command type (0x00 to 0x3F, see \c EHIF_CMD_XXXXX definitions)
 * \param[in]   length
 *     Number of parameter bytes (0 to 255)
 * \param[in]   *pParam
 *     Pointer to command parameter buffer before endianess conversion
 * \param[in]   *pFieldSpec
 *     Ignored
 *
 * \return
 *     EHIF status word at start of CMD_REQ operation (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifFieldCmdReq(uint8_t cmd, uint8_t length, const uint8_t* pParam, const int8_t* pFieldSpec) {
    return ehifCmdReq(cmd, length, pParam);
} // ehifFieldCmdReq


//@}
