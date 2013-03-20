/** \addtogroup module_ehif_basic_op Basic Operations
 *
 * \brief Implements basic SPI operations, pin and SPI-based reset routines and waiting/timeout utilities
 *
 * @{
 */
#ifndef CC85XX_EHIF_BASIC_OP_H_
#define CC85XX_EHIF_BASIC_OP_H_

#include <stdint.h>


//-------------------------------------------------------------------------------------------------------
// Function prototypes
uint16_t ehifGetStatus(void);
uint16_t ehifWrite(uint16_t length, const uint8_t* pData);
uint16_t ehifRead(uint16_t length, uint8_t* pData);
uint16_t ehifReadbc(uint16_t *pLength, uint8_t* pData);
uint16_t ehifCmdReq(uint8_t cmd, uint8_t length, const uint8_t* pParam);
uint16_t ehifSetAddr(uint16_t addr);
void ehifSysResetPin(uint8_t waitReady);
void ehifSysResetSpi(uint8_t waitReady);
void ehifBootResetPin(void);
void ehifBootResetSpi(void);
void ehifWaitReady(void);
void ehifWaitReadyMs(uint16_t timeout);
uint8_t ehifGetWaitReadyError(void);
//-------------------------------------------------------------------------------------------------------


#endif
//@}
