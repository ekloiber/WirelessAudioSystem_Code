/** \addtogroup module_ehif_bootloader Bootloader Commands
 *
 * \brief Provides access to bootloader specific EHIF commands and defines relevant EHIF status words
 *
 *
 * \section section_ehif_bootloader_algorithm Programming Algorithm
 * The following code erases, programs and verifies the CC85XX flash memory, using a flash image stored
 * in the host processor's code memory. This code can also be found in the Flash Programming code
 * example.
 *
 * If the entire image cannot be stored in the host processor, it can instead be transferred to the host
 * processor in smaller pieces, 1 kB at a time at the start of the page loop, and the image information
 * is extracted when programming the first and last flash pages.
 *
 * \code
 * uint16_t eraseProgVerifyFlash(const uint8_t* pFlashImage) {
 *
 *     // Extract information from the image
 *     uint32_t imageSize = (pFlashImage[0x1E] << 8) | pFlashImage[0x1F];
 *     const uint8_t* pExpectedCrcVal = pFlashImage + imageSize;
 *
 *     // Enter the SPI bootloader
 *     ehifBootResetPin();
 *     uint16_t status = ehifBlUnlockSpi();
 *     if (status != EHIF_BL_SPI_LOADER_READY) return status;
 *
 *     // Erase current flash contents
 *     status = ehifBlFlashMassErase();
 *     if (status != EHIF_BL_ERASE_DONE) return status;
 *
 *     // For each 1 kB flash page ...
 *     for (uint16_t offset = 0x0000; offset < 0x8000; offset += 0x0400) {
 *
 *         // Bail out when the entire image has been programmed (it is normally less than 32 kB)
 *         if (offset >= imageSize) break;
 *
 *         // Write the page data to RAM
 *         ehifSetAddr(0x6000);
 *         ehifWrite(0x0400, pFlashImage + offset);
 *
 *         // Program the page
 *         status = ehifBlFlashPageProg(0x6000, 0x8000 + offset);
 *         if (status != EHIF_BL_PROG_DONE) return status;
 *     }
 *
 *     // Verify the flash contents by performing CRC-32 check. Also compare the calculated CRC with the one
 *     // in the image to make sure that we've actually programmed it and not just verified what was already
 *     // in the flash memory
 *     uint8_t pActualCrcVal[sizeof(uint32_t)];
 *     status = ehifBlFlashVerify(imageSize, pActualCrcVal);
 *     for (int n = 0; n < sizeof(pActualCrcVal); n++) {
 *         if (pActualCrcVal[n] != pExpectedCrcVal[n]) {
 *             status = EHIF_BL_VERIFY_FAILED;
 *         }
 *     }
 *
 *     // Exit the SPI bootloader (not waiting for EHIF CMD_REQ_RDY since this will interfere with button
 *     // functionality on the CSn pin in autonomous operation)
 *     ehifSysResetPin(false);
 *
 *     return status;
 *
 * } // eraseProgVerifyFlash
 * \endcode
 *
 * @{
 */
#ifndef CC85XX_EHIF_BOOTLOADER_H_
#define CC85XX_EHIF_BOOTLOADER_H_

#include <stdint.h>


//-------------------------------------------------------------------------------------------------------
/// \name SPI Bootloader EHIF Status Words
//@{

#define EHIF_BL_SPI_LOADER_UNLOCK  0x8021 ///< SPI bootloader is locked, waiting for BL_UNLOCK_SPI
#define EHIF_BL_SPI_LOADER_READY   0x8020 ///< SPI bootloader is ready for the first command
#define EHIF_BL_SPI_LOADER_LOCKED  0x0022 ///< SPI bootloader (one-shot) unlocking (BL_UNLOCK_SPI) failed
#define EHIF_BL_ERASE_WORKING      0x0002 ///< Flash mass erase in progress
#define EHIF_BL_ERASE_DONE         0x8003 ///< Flash mass erase completed successfully
#define EHIF_BL_ERASE_FAILED       0x8004 ///< Flash mass erase failed
#define EHIF_BL_PROG_WORKING       0x000A ///< Flash page programming in progress
#define EHIF_BL_PROG_DONE          0x800B ///< Flash page programming completed successfully
#define EHIF_BL_PROG_FAILED        0x800C ///< Flash page programming failed
#define EHIF_BL_VERIFY_WORKING     0x000D ///< Flash verification in progress
#define EHIF_BL_VERIFY_OK          0x800E ///< Flash verification completed successfully
#define EHIF_BL_VERIFY_FAILED      0x800F ///< Flash verification failed

//@}
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
// Function prototypes
uint16_t ehifBlUnlockSpi(void);
uint16_t ehifBlFlashMassErase(void);
uint16_t ehifBlFlashPageProg(uint16_t ramAddr, uint16_t flashAddr);
uint16_t ehifBlFlashVerify(uint16_t byteCount, uint8_t* pCrcVal);
//-------------------------------------------------------------------------------------------------------


#endif
//@}
