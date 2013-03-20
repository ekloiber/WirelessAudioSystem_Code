/** \addtogroup module_ehif_field_op Basic Operations with Automatic Field Endianess Conversion
 * \ingroup module_ehif_cmd_exec
 *
 * \brief Performs WRITE, READ, READBC and CMD_REQ with field endianess conversion for the
 *        \ref module_ehif_cmd_exec.
 *
 * The documentation is for the little-endian version. See \ref section_ehif_cmd_exec_endianess for
 * further details.
 *
 * @{
 */
#ifndef CC85XX_EHIF_FIELD_OP_H_
#define CC85XX_EHIF_FIELD_OP_H_

#include <stdint.h>


//-------------------------------------------------------------------------------------------------------
// Function prototypes
uint16_t ehifFieldWrite(uint16_t length, const uint8_t* pData, const int8_t* pFieldSpec);
uint16_t ehifFieldRead(uint16_t length, uint8_t* pData, const int8_t* pFieldSpec);
uint16_t ehifFieldReadbc(uint16_t *pLength, uint8_t* pData, const int8_t* pFieldSpec);
uint16_t ehifFieldCmdReq(uint8_t cmd, uint8_t length, const uint8_t* pParam, const int8_t* pFieldSpec);
//-------------------------------------------------------------------------------------------------------


#endif
//@}
