/** \addtogroup module_ehif_field_op Basic Operations with Automatic Field Endianess Conversion
 *
 * This code is for little-endian microcontrollers/compilers.
 *
 * @{
 */
#include "../cc85xx_ehif_utils.h"
#include "../cc85xx_ehif_field_op.h"
#include "../cc85xx_ehif_basic_op.h"
#include <cc85xx_ehif_hal_mcu.h>
#include <cc85xx_ehif_hal_board.h>




/** \brief Transmits CMD_REQ / WRITE data fields with automatic endianess conversion (little to big)
 *
 * The function is limited by the number of bytes specified by the length field and by the the field
 * specification.
 *
 * \param[in]       length
 *     Number of bytes to transmit from \a *pData
 * \param[in]       *pData
 *     Pointer to storage buffer for parameters/data
 * \param[in]       *pFieldSpec
 *     Pointer to field specification list, using the following format (for each byte):
 *     - Positive value (msb is 0):
 *         - Bits 1:0 = Field size (1 = uint8_t, 2 = uint16_t, 3 = uint32_t)
 *         - Bits 7:2 = Repeat count (number of times this field size is repeated)
 *     - Negative value (msb is 1):
 *         - Bits 7:0 = Negative offset applied to \a pFieldSpec, creating an infinite loop until the
 *           number of bytes specified by \a length is reached
 */
static void ehifFieldTx(int16_t length, const uint8_t* pData, const int8_t* pFieldSpec) {

    // Until all the bytes have been consumed ...
    while (length > 0) {

        // Positive field spec value = Write field
        if (*pFieldSpec > 0) {

            // Bits 6:2 = repeat count (0 = one field, 1 = 2 fields and so on)
            uint16_t repeatCount = *pFieldSpec >> 2;
            do {

                // Bits 1:0 = field size shift (1 = 1 byte, 2 = 2 bytes, 3 = 4 bytes)
                uint8_t fieldSize = BV(((*pFieldSpec & 0x03) - 1) & 0x03);
                if (fieldSize > length) break;
                pData += fieldSize;
                switch (fieldSize) {
                case 4: // 32-bit
                    EHIF_SPI_TX(*(--pData));
                    EHIF_SPI_WAIT_TXRX();
                    EHIF_SPI_TX(*(--pData));
                    EHIF_SPI_WAIT_TXRX();
                case 2: // 16-bit
                    EHIF_SPI_TX(*(--pData));
                    EHIF_SPI_WAIT_TXRX();
                case 1: // 8-bit
                    EHIF_SPI_TX(*(--pData));
                    EHIF_SPI_WAIT_TXRX();
                    break;
                default:
                    return;
                }
                pData += fieldSize;
                length -= fieldSize;

            } while (repeatCount--);

            pFieldSpec++;

        // Zero field spec value = bail out. This should only happen due to host processor software bugs
        } else if (*pFieldSpec == 0) {
            return;

        // Negative field spec value = Go back in field specification
        } else {
            pFieldSpec += *pFieldSpec;
        }
    }

} // ehifFieldTx




/** \brief Receives READ(BC) data fields with automatic endianess conversion (from big to little)
 *
 * The function is limited by the number of bytes specified by the length field or by the the field
 * specification.
 *
 * \param[in]       length
 *     Number of bytes to receive into \a *pData
 * \param[in]       *pData
 *     Pointer to storage buffer for data
 * \param[in]       *pFieldSpec
 *     Pointer to field specification list, using the following format (for each byte):
 *     - Positive value (msb is 0):
 *         - Bits 1:0 = Field size (1 = uint8_t, 2 = uint16_t, 3 = uint32_t)
 *         - Bits 7:2 = Repeat count (number of times this field size is repeated)
 *     - Negative value (msb is 1):
 *         - Bits 7:0 = Negative offset applied to \a pFieldSpec, creating an infinite loop until the
 *           number of bytes specified by \a length is reached
 */
static void ehifFieldRx(uint16_t length, uint8_t* pData, const int8_t* pFieldSpec) {
    while (length) {

        // Positive field spec value = Write field
        if (*pFieldSpec > 0) {

            // Bits 6:2 = repeat count (0 = one field, 1 = 2 fields and so on)
            uint16_t repeatCount = *pFieldSpec >> 2;
            do {

                // Bits 1:0 = field size shift (1 = 1 byte, 2 = 2 bytes, 3 = 4 bytes)
                uint8_t fieldSize = BV(((*pFieldSpec & 0x03) - 1) & 0x03);
                if (fieldSize > length) break;
                pData += fieldSize;
                switch (fieldSize) {
                case 4: // 32-bit
                    EHIF_SPI_TX(0x00);
                    EHIF_SPI_WAIT_TXRX();
                    *(--pData) = EHIF_SPI_RX();
                    EHIF_SPI_TX(0x00);
                    EHIF_SPI_WAIT_TXRX();
                    *(--pData) = EHIF_SPI_RX();
                case 2: // 16-bit
                    EHIF_SPI_TX(*pData);
                    EHIF_SPI_WAIT_TXRX();
                    *(--pData) = EHIF_SPI_RX();
                case 1: // 8-bit
                    EHIF_SPI_TX(0x00);
                    EHIF_SPI_WAIT_TXRX();
                    *(--pData) = EHIF_SPI_RX();
                    break;
                default:
                    return;
                }
                pData += fieldSize;
                length -= fieldSize;

            } while (repeatCount--);

            pFieldSpec++;

        // Zero field spec value = bail out. This should normally not happen, but can for instance happen
        // if an electrical error corrupts the length value returned by READBC
        } else if (*pFieldSpec == 0) {
            return;

        // Negative field spec value = Go back in field specification
        } else {
            pFieldSpec += *pFieldSpec;
        }
    }
} // ehifFieldRx




/** \brief Performs a WRITE operation with automatic endianess conversion (little to big)
 *
 * \param[in]       length
 *     Number of bytes to be written (0 to 4095)
 * \param[in]       *pData
 *     Pointer to data buffer to be written
 * \param[in]       *pFieldSpec
 *     Pointer to field specification list, using the following format (for each byte):
 *     - Positive value (msb is 0):
 *         - Bits 1:0 = Field size (1 = uint8_t, 2 = uint16_t, 3 = uint32_t)
 *         - Bits 7:2 = Repeat count (number of times this field size is repeated)
 *     - Negative value (msb is 1):
 *         - Bits 7:0 = Negative offset applied to \a pFieldSpec, creating an infinite loop until the
 *           number of bytes specified by \a length is reached
 *
 * \return
 *     EHIF status word at start of WRITE operation (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifFieldWrite(uint16_t length, const uint8_t* pData, const int8_t* pFieldSpec) {

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
    ehifFieldTx(length, pData, pFieldSpec);
    EHIF_SPI_WAIT_TXRX();

    // End operation
    EHIF_SPI_END();
    return statusWord;

} // ehifFieldWrite




/** \brief Performs a READ operation with automatic endianess conversion (big to little)
 *
 * \param[in]       length
 *     Number of bytes to be read (0 to 4095)
 * \param[out]      *pData
 *     Pointer to storage buffer for read data
 * \param[in]       *pFieldSpec
 *     Pointer to field specification list, using the following format (for each byte):
 *     - Positive value (msb is 0):
 *         - Bits 1:0 = Field size (1 = uint8_t, 2 = uint16_t, 3 = uint32_t)
 *         - Bits 7:2 = Repeat count (number of times this field size is repeated)
 *     - Negative value (msb is 1):
 *         - Bits 7:0 = Negative offset applied to \a pFieldSpec, creating an infinite loop until the
 *           number of bytes specified by \a length is reached
 *
 * \return
 *     EHIF status word at start of READ operation (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifFieldRead(uint16_t length, uint8_t* pData, const int8_t* pFieldSpec) {

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
    ehifFieldRx(length, pData, pFieldSpec);

    // End operation
    EHIF_SPI_END();
    return statusWord;

} // ehifFieldRead




/** \brief Performs a READBC operation with automatic endianess conversion (big to little)
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
 *     Pointer to field specification list, using the following format (for each byte):
 *     - Positive value (msb is 0):
 *         - Bits 1:0 = Field size (1 = uint8_t, 2 = uint16_t, 3 = uint32_t)
 *         - Bits 7:2 = Repeat count (number of times this field size is repeated)
 *     - Negative value (msb is 1):
 *         - Bits 7:0 = Negative offset applied to \a pFieldSpec, creating an infinite loop until the
 *           number of bytes specified by \a length is reached
 *
 * \return
 *     EHIF status word at start of READBC operation (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifFieldReadbc(uint16_t *pVarLength, uint8_t* pData, const int8_t* pFieldSpec) {

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
    ehifFieldRx(length, pData, pFieldSpec);

    // End operation
    EHIF_SPI_END();
    return statusWord;

} // ehifFieldReadbc




/** \brief Performs a CMD_REQ operation with automatic endianess conversion (little to big)
 *
 * \param[in]   cmd
 *     Command type (0x00 to 0x3F, see \c EHIF_CMD_XXXXX definitions)
 * \param[in]   length
 *     Number of parameter bytes (0 to 255)
 * \param[in]   *pParam
 *     Pointer to command parameter buffer before endianess conversion
 * \param[in]   *pFieldSpec
 *     Pointer to field specification list, using the following format (for each byte):
 *     - Positive value (msb is 0):
 *         - Bits 1:0 = Field size (1 = uint8_t, 2 = uint16_t, 3 = uint32_t)
 *         - Bits 7:2 = Repeat count (number of times this field size is repeated)
 *     - Negative value (msb is 1):
 *         - Bits 7:0 = Negative offset applied to \a pFieldSpec, creating an infinite loop until the
 *           number of bytes specified by \a length is reached
 *
 * \return
 *     EHIF status word at start of CMD_REQ operation (see \c EHIF_EVT_XXXXX definitions)
 */
uint16_t ehifFieldCmdReq(uint8_t cmd, uint8_t length, const uint8_t* pParam, const int8_t* pFieldSpec) {

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
    ehifFieldTx(length, pParam, pFieldSpec);
    EHIF_SPI_WAIT_TXRX();

    // End operation
    EHIF_SPI_END();
    return statusWord;

} // ehifFieldCmdReq


//@}
