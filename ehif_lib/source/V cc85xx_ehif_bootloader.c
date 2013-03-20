/** \addtogroup module_ehif_bootloader Bootloader Commands
 *
 * @{
 */
#include "cc85xx_ehif_bootloader.h"
#include "cc85xx_ehif_basic_op.h"
#include "cc85xx_ehif_utils.h"
#include <cc85xx_ehif_hal_mcu.h>
#include <cc85xx_ehif_hal_board.h>




/** \brief Unlocks the SPI bootloader
 *
 * Bootloader unlocking shall be performed as the very first operation after BOOT_RESET. Otherwise, the
 * SPI bootloader will be locked and another BOOT_RESET is required to retry.
 *
 * \note This command is only available in bootloader mode.
 *
 * \return
 *     One of the following:
 *     - \ref EHIF_BL_SPI_LOADER_READY - Bootloader unlocking succeeded
 *     - \ref EHIF_BL_SPI_LOADER_LOCKED - Bootloader unlocking failed. Make sure that
 *     - Other: EHIF operation failed (CC85XX is not in bootloader mode, incorrect MCU clock speed,
 *       CC85XX crystal problems etc.)
 */
uint16_t ehifBlUnlockSpi(void) {

    // Prepare CMD_REQ parameters
    static const uint8_t pParams[4] = {
        0x25, 0x05, 0xB0, 0x07 // KEY
    };

    // Send BL_UNLOCK_SPI
    ehifCmdReq(0x00, sizeof(pParams), pParams);

    // Wait for completion and return status
    ehifWaitReadyMs(1);
    return ehifGetStatus();

} // ehifBlUnlockSpi




/** \brief Erases all flash contents
 *
 * \note This command is only available in bootloader mode.
 *
 * \return
 *     One of the following:
 *     - \ref EHIF_BL_ERASE_DONE - Flash mass erase succeeded
 *     - \ref EHIF_BL_ERASE_FAILED - Flash mass erase failed (electrical error)
 *     - Other: EHIF operation failed (CC85XX is not in bootloader mode, incorrect MCU clock speed,
 *       CC85XX crystal problems etc.)
 */
uint16_t ehifBlFlashMassErase(void) {

    // Prepare CMD_REQ parameters
    static const uint8_t pParams[4] = {
        0x25, 0x05, 0x13, 0x37 // KEY
    };

    // Send BL_FLASH_MASS_ERASE
    ehifCmdReq(0x03, sizeof(pParams), pParams);

    // Wait for completion and return status
    ehifWaitReadyMs(25);
    return ehifGetStatus();

} // ehifBlFlashMassErase




/** \brief Programs a single 1 kB flash page using data written to RAM
 *
 * \note This command is only available in bootloader mode.
 *
 * \param[in]   ramAddr
 *     The address value used with SET_ADDR when using WRITE to transfer flash page data to CC85XX RAM
 * \param[out]  flashAddr
 *     Flash page address: 0x8000 + (0x0400 * "flash page index, 0-31")
 *
 * \return
 *     One of the following:
 *     - \ref EHIF_BL_PROG_DONE - Flash page programming succeeded
 *     - \ref EHIF_BL_PROG_FAILED - Flash page programming failed (electrical error)
 *     - Other: EHIF operation failed (CC85XX is not in bootloader mode, incorrect MCU clock speed,
 *       CC85XX crystal problems etc.)
 */
uint16_t ehifBlFlashPageProg(uint16_t ramAddr, uint16_t flashAddr) {

    // Prepare CMD_REQ parameters
    uint8_t pParams[10] = {
        HI8(ramAddr), LO8(ramAddr),     // RAM_ADDR
        HI8(flashAddr), LO8(flashAddr), // FLASH_ADDR
        0x01, 0x00,                     // DWORD_COUNT
        0x25, 0x05, 0x13, 0x37          // KEY
    };

    // Send BL_FLASH_PAGE_PROG
    ehifCmdReq(0x07, sizeof(pParams), pParams);

    // Wait for completion and return status
    ehifWaitReadyMs(10);
    return ehifGetStatus();

} // ehifBlFlashPageProg




/** \brief Verifies current flash content using a CRC-32 calculation
 *
 * The function only verifies that the flash content has a valid CRC-32, not that it matches the last
 * attempted programming. To do that, the returned CRC (\a pCrcVal) should be checked against the four
 * last bytes in the programmed image (i.e. the last four bytes programmed, \a byteCount bytes after the
 * start of the image).
 *
 * \note This command is only available in bootloader mode.
 *
 * \param[in]   byteCount
 *     Size of the flash image / offset of CRC-32.
 * \param[out]  *pCrcVal
 *     Pointer to a byte array of 4 bytes, where the actual CRC value is stored
 *
 * \return
 *     One of the following:
 *     - \ref EHIF_BL_VERIFY_OK - Flash verification succeeded (CC85XX contains image with valid CRC)
 *     - \ref EHIF_BL_VERIFY_FAILED - Flash verification failed (CC85XX contains image with invalid CRC)
 *     - Other: EHIF operation failed (CC85XX is not in bootloader mode etc.)
 */
uint16_t ehifBlFlashVerify(uint16_t byteCount, uint8_t* pCrcVal) {

    // Prepare CMD_REQ parameters
    uint8_t pParams[8] = {
        0x00, 0x00, 0x80, 0x00,                    // DATA_ADDR
        0x00, 0x00, HI8(byteCount), LO8(byteCount) // BYTE_COUNT
    };

    // Send BL_FLASH_VERIFY
    ehifCmdReq(0x0F, sizeof(pParams), pParams);

    // Get CRC and return status
    ehifWaitReadyMs(15);
    return ehifRead(4, pCrcVal);

} // ehifBlFlashVerify


//@}
