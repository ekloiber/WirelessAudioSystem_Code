/** \addtogroup module_ehif_hal_board HAL: Board Specific Definitions and Routines
 * \ingroup module_ehif_mcu
 *
 * \brief Defines board specific constants, macros and functions
 *
 * \section section_ehif_hal_board_overview Overview
 * The following items are defined here:
 * - MCU clock speed
 * - Time constants that depend on MCU clock speed and SPI interface configuration
 * - Fundamental SPI operations
 * - Fundamental pin operations
 * - EHIF event interrupt handling
 *
 * @{
 */
#ifndef CC85XX_EHIF_HAL_BOARD_H_
#define CC85XX_EHIF_HAL_BOARD_H_

#include <msp430.h>


//-------------------------------------------------------------------------------------------------------
/// \name Clock Speed and Delay Definitions
//@{

/// Specify the MCU clock speed used in number if MHz (e.g. 16 for 16 MHz)
#define EHIF_MCU_SPEED_IN_MHZ               16

/// Delay in us between SYS_RESET or BOOT_RESET SPI byte transfers and CSn high afterwards (0 = none)
#define EHIF_DELAY_SPI_RESET_TO_CSN_HIGH    2

//@}
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
/// \name SPI Interface Macros
//@{

/// Activates CSn, starting an SPI operation
#define EHIF_SPI_BEGIN()                    st( P10OUT &= ~0x08; )

/// Non-zero when EHIF is ready, zero when EHIF is not ready
#define EHIF_SPI_IS_CMDREQ_READY()          (P10IN & 0x20)

/// Transmits a single byte
#define EHIF_SPI_TX(x)                      st( UCA3TXBUF = (x); )

/// Waits for completion of \ref EHIF_SPI_TX() (no timeout required!)
#define EHIF_SPI_WAIT_TXRX()                st( while (UCA3STAT & UCBUSY); )

/// The received byte after completing the last \ref EHIF_SPI_TX()
#define EHIF_SPI_RX()                       (UCA3RXBUF)

/// Deactivates CSn, ending an SPI operation
#define EHIF_SPI_END()                      st( P10OUT |= 0x08; )

/// Forces the MOSI pin to the specified level
#define EHIF_SPI_FORCE_MOSI(x)              st( P10SEL &= ~0x10; if (x) { P10OUT |= 0x10; } else { P10OUT &= ~0x10; } )

/// Ends forcing of the MOSI pin started by \ref EHIF_SPI_FORCE_MOSI()
#define EHIF_SPI_RELEASE_MOSI()             st( P10SEL |= 0x10; )

//@}
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
/// \name Reset Interface Macros
//@{

/// Activates RESETn, starting pin reset
#define EHIF_PIN_RESET_BEGIN()              st( P5OUT &= ~0x01; )

/// Deactivates RESETn, ending pin reset
#define EHIF_PIN_RESET_END()                st( P5OUT |= 0x01; )

//@}
//-------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------
/// \name Event Interrupt
//@{

/// Non-zero when the EHIF interrupt is active, zero when the EHIF interrupt is active
#define EHIF_INTERRUPT_IS_ACTIVE()          !(P5IN & 0x02)

//@}
//-------------------------------------------------------------------------------------------------------


void ehifIoInit(void);


#endif
//@}
