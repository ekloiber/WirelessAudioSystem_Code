/** \addtogroup module_ehif_hal_board
 *
 * @{
 */
#include <cc85xx_ehif_hal_board.h>
#include <cc85xx_ehif_hal_mcu.h>



/** Initializes the SPI interface
 */
void ehifIoInit(void) {

    // The pin mapping is as follows (se also included PDF)
    // - RESETn at P5.0
    // - EHIF interrupt at P5.1
    // - SCLK at P10.0
    // - CSn at P10.3
    // - MOSI at P10.4
    // - MISO at P10.5

    // Setup SPI interface: Synchronous master mode, MSB first, MOSI/MISO updated on falling edge of
    // SCLK, MOSI/MISO sampled on rising edge of SCLK
    UCA3CTL0  = UCMSB | UCMST | UCSYNC | UCCKPH;

    // Setup SPI interface: Use SMCLK divided by 4, normally 4 MHz
    UCA3CTL1  = UCSSEL1;
    UCA3BR0   = EHIF_MCU_SPEED_IN_MHZ / 4;
    UCA3BR1   = 0x00;

    // Configure SCLK, MOSI and MISO as peripheral mode pins with pull-up, CSn as GPIO mode with pull-up
    P10SEL    = 0x31;
    P10DIR    = 0x19;
    P10OUT   |= 0x38; // SCLK

    // Start with RESET_N activated
    P5OUT    &= 0xFE;
    P5DIR     = 0x01;

} // ehifIoInit


//@}
