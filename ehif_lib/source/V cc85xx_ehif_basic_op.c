/** \addtogroup module_ehif_basic_op Basic Operations
 *
 * @{
 */
#include "cc85xx_ehif_utils.h"
#include "cc85xx_ehif_basic_op.h"
#include <cc85xx_ehif_hal_mcu.h>
#include <cc85xx_ehif_hal_board.h>


/// Internal variable that registers timeout errors while waiting for CMD_REQ_READY to go active
static uint8_t waitReadyError = 0;




/** \brief Performs a GET_STATUS operation, and returns the EHIF status word
 *
 * \note The GET_STATUS operation ignores the CMD_REQ_RDY status.
 *
 * \return
 *     EHIF status word (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifGetStatus(void) {

    // Begin operation
    EHIF_SPI_BEGIN();

    // Send type/length, receive status word
    uint16_t statusWord;
    EHIF_SPI_TX(0x80);
    EHIF_SPI_WAIT_TXRX();
    statusWord = EHIF_SPI_RX() << 8;
    EHIF_SPI_TX(0x00);
    EHIF_SPI_WAIT_TXRX();
    statusWord |= EHIF_SPI_RX();

    // End operation
    EHIF_SPI_END();
    return statusWord;

} // ehifGetStatus




/** \brief Performs a WRITE operation
 *
 * \param[in]       length
 *     Number of bytes to be written (0 to 4095)
 * \param[in]       *pData
 *     Pointer to data buffer to be written
 *
 * \return
 *     EHIF status word at start of WRITE operation (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifWrite(uint16_t length, const uint8_t* pData) {

    // Begin operation
    EHIF_SPI_BEGIN();
    ehifWaitReady();

    // Send type/length, receive status word
    uint16_t statusWord;
    EHIF_SPI_TX(0x80 | ((length >> 8) & 0x0F));
    EHIF_SPI_WAIT_TXRX();
    statusWord = EHIF_SPI_RX() << 8;
    EHIF_SPI_TX(length & 0xFF);
    EHIF_SPI_WAIT_TXRX();
    statusWord |= EHIF_SPI_RX();

    // Send data
    while (length--) {
        EHIF_SPI_WAIT_TXRX();
        EHIF_SPI_TX(*(pData++));
    }
    EHIF_SPI_WAIT_TXRX();

    // End operation
    EHIF_SPI_END();
    return statusWord;

} // ehifWrite




/** \brief Performs a READ operation
 *
 * \param[in]       length
 *     Number of bytes to be read (0 to 4095)
 * \param[out]      *pData
 *     Pointer to storage buffer for read data
 *
 * \return
 *     EHIF status word at start of READ operation (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifRead(uint16_t length, uint8_t* pData) {

    // Begin operation
    EHIF_SPI_BEGIN();
    ehifWaitReady();

    // Send type/length, receive status word
    uint16_t statusWord;
    EHIF_SPI_TX(0x90 | ((length >> 8) & 0x0F));
    EHIF_SPI_WAIT_TXRX();
    statusWord = EHIF_SPI_RX() << 8;
    EHIF_SPI_TX(length & 0xFF);
    EHIF_SPI_WAIT_TXRX();
    statusWord |= EHIF_SPI_RX();

    // Receive data
    if (length--) {
        EHIF_SPI_TX(0x00);
        while (length--) {
            EHIF_SPI_WAIT_TXRX();
            *(pData++) = EHIF_SPI_RX();
            EHIF_SPI_TX(0x00);
        }
        EHIF_SPI_WAIT_TXRX();
        *(pData++) = EHIF_SPI_RX();
    }

    // End operation
    EHIF_SPI_END();
    return statusWord;

} // ehifRead




/** \brief Performs a READBC operation
 *
 * The number of bytes read is the minimum of the length function parameter and the length field returned
 * by the READBC command.
 *
 * \param[in,out]       *pVarLength
 *     Upon function entry \a *pVarLength should indicate the size of \a pData[]. Upon function exit the
 *     value is changed to indicate the actual number of bytes read (0 to 4095)
 * \param[out]          *pData
 *     Pointer to storage buffer for read data
 *
 * \return
 *     EHIF status word at start of READBC operation (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifReadbc(uint16_t *pVarLength, uint8_t* pData) {

    // Begin operation
    EHIF_SPI_BEGIN();
    ehifWaitReady();

    // Send type, receive status word
    uint16_t statusWord;
    EHIF_SPI_TX(0xA0);
    EHIF_SPI_WAIT_TXRX();
    statusWord = EHIF_SPI_RX() << 8;
    EHIF_SPI_TX(0x00);
    EHIF_SPI_WAIT_TXRX();
    statusWord |= EHIF_SPI_RX();

    // Receive length
    uint16_t length;
    EHIF_SPI_TX(0xA0);
    EHIF_SPI_WAIT_TXRX();
    length = EHIF_SPI_RX() << 8;
    EHIF_SPI_TX(0x00);
    EHIF_SPI_WAIT_TXRX();
    length |= EHIF_SPI_RX();

    // Constrain length
    if (length > *pVarLength) {
        length = *pVarLength;
    }
    *pVarLength = length;

    // Receive data
    if (length--) {
        EHIF_SPI_TX(0x00);
        while (length--) {
            EHIF_SPI_WAIT_TXRX();
            *(pData++) = EHIF_SPI_RX();
            EHIF_SPI_TX(0x00);
        }
        EHIF_SPI_WAIT_TXRX();
        *(pData++) = EHIF_SPI_RX();
    }

    // End operation
    EHIF_SPI_END();
    return statusWord;

} // ehifReadbc




/** \brief Performs a CMD_REQ operation
 *
 * \param[in]       cmd
 *     Command type (0x00 to 0x3F, see \c EHIF_CMD_XXXXX definitions)
 * \param[in]       length
 *     Number of parameter bytes (0 to 255)
 * \param[in]       *pParam
 *     Pointer to command parameter buffer
 *
 * \return
 *     EHIF status word at start of CMD_REQ operation (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifCmdReq(uint8_t cmd, uint8_t length, const uint8_t* pParam) {

    // Begin operation
    EHIF_SPI_BEGIN();
    ehifWaitReady();

    // Send type/command code/parameter length, receive status word
    uint16_t statusWord;
    EHIF_SPI_TX(0xC0 | cmd);
    EHIF_SPI_WAIT_TXRX();
    statusWord = EHIF_SPI_RX() << 8;
    EHIF_SPI_TX(length & 0xFF);
    EHIF_SPI_WAIT_TXRX();
    statusWord |= EHIF_SPI_RX();

    // Send parameters
    while(length--) {
        EHIF_SPI_WAIT_TXRX();
        EHIF_SPI_TX(*(pParam++));
    }
    EHIF_SPI_WAIT_TXRX();

    // End operation
    EHIF_SPI_END();
    return statusWord;

} // ehifCmdReq




/** \brief Performs a SET_ADDR operation
 *
 * This operation is only available in SPI bootloader mode. It has no effect in application mode.
 *
 * \param[in]       addr
 *     CC85XX memory address where the subsequent WRITE operation will store data
 *
 * \return
 *     EHIF status word at start of SET_ADDR operation (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifSetAddr(uint16_t addr) {

    // Begin operation
    EHIF_SPI_BEGIN();
    ehifWaitReady();

    // Send type/address
    uint16_t statusWord;
    EHIF_SPI_TX(0x00 | (HI8(addr) & 0x7F));
    EHIF_SPI_WAIT_TXRX();
    statusWord = EHIF_SPI_RX() << 8;
    EHIF_SPI_TX(LO8(addr) & 0xFF);
    EHIF_SPI_WAIT_TXRX();
    statusWord |= EHIF_SPI_RX();

    // End operation
    EHIF_SPI_END();
    return statusWord;

} // ehifSetAddr




/** \brief Performs RESETn pin-based SYS_RESET to enter the CC85XX application
 *
 * The timing requirements stated in the CC85XX Family User's Guide must be followed. This requires that:
 * - The \ref EHIF_DELAY_SPI_RESET_TO_CSN_HIGH constant is correctly defined
 *
 * Timing must be verified, using oscilloscope or logic analyzer, when porting to new microcontrollers
 * and/or compiler tools.
 */
void ehifSysResetPin(uint8_t waitReady) {

    // Ensure that MOSI is high at all times
    EHIF_SPI_FORCE_MOSI(1);

    // Perform SYS_RESET sequence
    EHIF_PIN_RESET_BEGIN();
    EHIF_SPI_BEGIN();
    EHIF_DELAY_MS(2);
    EHIF_PIN_RESET_END();
    EHIF_DELAY_US(4);
    EHIF_SPI_END();
    if (waitReady) {
        EHIF_DELAY_US(1);
        EHIF_SPI_BEGIN();
        ehifWaitReadyMs(100);
        EHIF_SPI_END();
    }

    // Return the MOSI pin to peripheral mode
    EHIF_SPI_RELEASE_MOSI();

} // ehifSysResetPin




/** \brief Performs RESETn pin-based BOOT_RESET to enter the CC85XX bootloader
 *
 * The timing requirements stated in the CC85XX Family User's Guide must be followed. This requires that:
 * - The \ref EHIF_DELAY_SPI_RESET_TO_CSN_HIGH constant is correctly defined
 *
 * Timing must be verified, using oscilloscope or logic analyzer, when porting to new microcontrollers
 * and/or compiler tools.
 */
void ehifBootResetPin(void) {

    // Ensure that MOSI is low at all times
    EHIF_SPI_FORCE_MOSI(0);

    // Perform SYS_RESET sequence
    EHIF_PIN_RESET_BEGIN();
    EHIF_SPI_BEGIN();
    EHIF_DELAY_MS(2);
    EHIF_ENTER_CRITICAL_SECTION();
    EHIF_PIN_RESET_END();
    EHIF_DELAY_US(4);
    EHIF_SPI_END();
    EHIF_DELAY_US(1);
    EHIF_SPI_BEGIN();
    EHIF_LEAVE_CRITICAL_SECTION();
    ehifWaitReadyMs(100);
    EHIF_SPI_END();

    // Return the MOSI pin to peripheral mode
    EHIF_SPI_RELEASE_MOSI();

} // ehifBootResetPin





/** \brief Performs SPI-based SYS_RESET to enter the CC85XX application
 *
 * The timing requirements stated in the CC85XX Family User's Guide must be followed. This requires that:
 * - The SPI interface runs at 2 MHz or more to be able to complete 12 SCLK cycles within 10 us
 * - The \ref EHIF_DELAY_SPI_RESET_TO_CSN_HIGH constant is correctly defined
 *
 * Timing must be verified, using oscilloscope or logic analyzer, when porting to new microcontrollers
 * and/or compiler tools.
 */
void ehifSysResetSpi(uint8_t waitReady) {

    // Perform SYS_RESET sequence
    EHIF_SPI_BEGIN();
    EHIF_DELAY_MS(2);
    EHIF_SPI_TX(0xBF);
    EHIF_SPI_WAIT_TXRX();
    EHIF_SPI_TX(0xFF);
    EHIF_SPI_WAIT_TXRX();
    if (EHIF_DELAY_SPI_RESET_TO_CSN_HIGH) {
        EHIF_DELAY_US(EHIF_DELAY_SPI_RESET_TO_CSN_HIGH);
    }
    EHIF_SPI_END();
    if (waitReady) {
        EHIF_DELAY_US(1);
        EHIF_SPI_BEGIN();
        ehifWaitReadyMs(100);
        EHIF_SPI_END();
    }

} // ehifSysResetSpi




/** \brief Performs SPI-based BOOT_RESET to enter the CC85XX bootloader
 *
 * The timing requirements stated in the CC85XX Family User's Guide must be followed. This requires that:
 * - The SPI interface runs at 2 MHz or more to be able to complete 12 SCLK cycles within 10 us
 * - The \ref EHIF_DELAY_SPI_RESET_TO_CSN_HIGH constant is correctly defined
 *
 * This function disables global interrupts in the timing critical part of the operation.
 *
 * Timing must be verified, using oscilloscope or logic analyzer, when porting to new microcontrollers
 * and/or compiler tools.
 */
void ehifBootResetSpi(void) {

    // Perform SYS_RESET sequence
    EHIF_SPI_BEGIN();
    EHIF_DELAY_MS(2);
    EHIF_ENTER_CRITICAL_SECTION();
    EHIF_SPI_TX(0xB0);
    EHIF_SPI_WAIT_TXRX();
    EHIF_SPI_TX(0x00);
    EHIF_SPI_WAIT_TXRX();
    if (EHIF_DELAY_SPI_RESET_TO_CSN_HIGH) {
        EHIF_DELAY_US(EHIF_DELAY_SPI_RESET_TO_CSN_HIGH);
    }
    EHIF_SPI_END();
    EHIF_DELAY_US(1);
    EHIF_SPI_BEGIN();
    EHIF_LEAVE_CRITICAL_SECTION();
    ehifWaitReadyMs(100);
    EHIF_SPI_END();

} // ehifBootResetSpi





/** \brief Internal function: Waits until EHIF is ready or the default 10 ms timeout expires
 *
 * The timeout mechanism ensures that code execution does not hang if the CC85XX does not respond.
 *
 * The function assumes that CSn is active.
 */
void ehifWaitReady(void) {
    uint16_t maxDelay = 5000;
    while (!EHIF_SPI_IS_CMDREQ_READY() && --maxDelay) {
        EHIF_DELAY_US(2);
    }
    if (!maxDelay) waitReadyError = 1;
} // ehifWaitReady




/** \brief Waits until EHIF is ready or the timeout (in milliseconds) expires
 *
 * The MISO pin level is checked every 10 us.
 *
 * \param[in]       timeout
 *     Timeout in milliseconds
 */
void ehifWaitReadyMs(uint16_t timeout) {
    EHIF_SPI_BEGIN();
    uint32_t maxDelay = ((uint32_t) timeout) * 100;
    while (!EHIF_SPI_IS_CMDREQ_READY() && --maxDelay) {
        EHIF_DELAY_US(10);
    }
    if (!maxDelay) waitReadyError = 1;
    EHIF_SPI_END();
} // ehifWaitReadyMs




/** \brief Indicates whether a timeout error has occurred and clears the error state
 *
 * This function should be polled and checked at regular intervals to make sure that CMD_REQ_READY has
 * not been violated (with or without generating \ref BV_EHIF_EVT_SPI_ERROR.
 *
 * \return
 *     Non-zero if CMD_REQ_READY violation has occurred, otherwise zero.
 */
uint8_t ehifGetWaitReadyError(void) {
    if (waitReadyError) {
        waitReadyError = 0;
        return 1;
    } else {
        return 0;
    }
} // ehifGetWaitReadyError


//@}
