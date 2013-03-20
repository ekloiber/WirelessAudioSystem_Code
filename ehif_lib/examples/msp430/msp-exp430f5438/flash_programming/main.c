#include <msp430.h>
#include <stdint.h>
#include <cc85xx_ehif_utils.h>
#include <cc85xx_ehif_basic_op.h>
#include <cc85xx_ehif_hal_board.h>
#include <cc85xx_ehif_hal_mcu.h>
#include <cc85xx_ehif_bootloader.h>
#include <cc85xx_ehif_bootloader.h>
#include "hal_board.h"
#include "hal_lcd.h"
#include "hal_int.h"
#include "hal_buttons.h"




uint16_t eraseProgVerifyFlash(const uint8_t* pFlashImage) {
    
    // Extract information from the image
    uint32_t imageSize = (pFlashImage[0x1E] << 8) | pFlashImage[0x1F];
    const uint8_t* pExpectedCrcVal = pFlashImage + imageSize;

    // Enter the SPI bootloader
    ehifBootResetPin();
    uint16_t status = ehifBlUnlockSpi();
    if (status != EHIF_BL_SPI_LOADER_READY) return status;

    // Erase current flash contents
    status = ehifBlFlashMassErase();
    if (status != EHIF_BL_ERASE_DONE) return status;

    // For each 1 kB flash page ...
    for (uint16_t offset = 0x0000; offset < 0x8000; offset += 0x0400) {

        // Bail out when the entire image has been programmed (it is normally less than 32 kB)
        if (offset >= imageSize) break;

        // Write the page data to RAM
        ehifSetAddr(0x6000);
        ehifWrite(0x0400, pFlashImage + offset);

        // Program the page
        status = ehifBlFlashPageProg(0x6000, 0x8000 + offset);
        if (status != EHIF_BL_PROG_DONE) return status;
    }

    // Verify the flash contents by performing CRC-32 check. Also compare the calculated CRC with the one 
    // in the image to make sure that we've actually programmed it and not just verified what was already
    // in the flash memory
    uint8_t pActualCrcVal[sizeof(uint32_t)];
    status = ehifBlFlashVerify(imageSize, pActualCrcVal);
    for (int n = 0; n < sizeof(pActualCrcVal); n++) {
        if (pActualCrcVal[n] != pExpectedCrcVal[n]) {
            status = EHIF_BL_VERIFY_FAILED;
        }
    }
    
    // Exit the SPI bootloader (not waiting for EHIF CMD_REQ_RDY since this will interfere with button 
    // functionality on the CSn pin in autonomous operation)
    ehifSysResetPin(false);
  
    return status;
    
} // eraseProgVerifyFlash




extern const uint8_t pMasterImage[];
extern const uint8_t pSlaveImage[];
#define IMAGE_COUNT 2

const uint8_t* pSelFlashImage;




void selectImage(uint8_t selNext) {
    static int16_t selIndex = 0;
    
    // Change image index
    selIndex += (selNext ? 1 : -1);
    if (selIndex < 0) selIndex = 0;
    if (selIndex >= IMAGE_COUNT) selIndex = IMAGE_COUNT - 1;
    
    // Select image
    switch (selIndex) {
    case 0: pSelFlashImage = pMasterImage; break;
    case 1: pSelFlashImage = pSlaveImage; break;
    }
    
    // Update LCD display
    char pLine[18] = "Image:           ";
    pLine[7] = '0' + selIndex;
    halLcdPrintLine(pLine, 6, OVERWRITE_TEXT );
    
} // selectImage
    
    


uint8_t pollButtons(void) {
    static uint32_t lastButtonDuration = 0;
    static uint8_t lastButton = 0x00;
    uint8_t currButton = halButtonsPressed();
    if (currButton == lastButton) {
        lastButtonDuration++;
    } else {
        lastButton = currButton;
        lastButtonDuration = 0;
    }
    if (lastButtonDuration == 10) {
        return lastButton & BUTTON_ALL;
    } else {
        return 0x00;
    }
} // pollButtons
    
    
    
            
int main(void) {
    uint16_t status;
    
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    
    // Initialize the MCU and board peripherals
    halBoardInit();
    halBoardStartXT1();	
    halBoardSetSystemClock(SYSCLK_16MHZ);
    halButtonsInit(BUTTON_ALL);
    halLcdInit();
    halLcdBackLightInit();
    halLcdSetContrast(90);
    halLcdSetBackLight(10);
    halLcdClearScreen();
    
    halLcdPrintLine("CC85XX FLASH-PROG", 0, OVERWRITE_TEXT );
    halLcdPrintLine("                 ", 1, OVERWRITE_TEXT );
    halLcdPrintLine("U: Select image 0", 2, OVERWRITE_TEXT );
    halLcdPrintLine("D: Select image 1", 3, OVERWRITE_TEXT );
    halLcdPrintLine("R: Run programmer", 4, OVERWRITE_TEXT );
    halLcdPrintLine("                 ", 5, OVERWRITE_TEXT );
    halLcdPrintLine("Image: 0         ", 6, OVERWRITE_TEXT );
    halLcdPrintLine("Status: Idle     ", 7, OVERWRITE_TEXT );
    halLcdPrintLine("Result: None     ", 8, OVERWRITE_TEXT );
      
    // Initialize EHIF IO
    ehifIoInit();
    
    // Initialize image selection
    pSelFlashImage = pMasterImage;

    // Main loop
    while (1) {
        
        // Wait 10 ms
        __delay_cycles(160000);
        
        // Perform action according to button events (debouncing with 100 ms delay)
        switch (pollButtons()) {
        case BUTTON_UP:
            selectImage(0);
            break;
        case BUTTON_DOWN:
            selectImage(1);
            break;
        case BUTTON_RIGHT:
            halLcdPrintLine("Status: Running  ", 7, OVERWRITE_TEXT );
            status = eraseProgVerifyFlash(pSelFlashImage);
            switch (status) {
            case EHIF_BL_VERIFY_OK:         halLcdPrintLine("Result: SUCCESS  ", 8, OVERWRITE_TEXT ); break;
            case EHIF_BL_VERIFY_FAILED:     halLcdPrintLine("Result: VerifyErr", 8, OVERWRITE_TEXT ); break;
            case EHIF_BL_PROG_FAILED:       halLcdPrintLine("Result: ProgErr  ", 8, OVERWRITE_TEXT ); break;
            case EHIF_BL_ERASE_FAILED:      halLcdPrintLine("Result: EraseErr ", 8, OVERWRITE_TEXT ); break;
            case EHIF_BL_SPI_LOADER_LOCKED: halLcdPrintLine("Result: UnlockErr", 8, OVERWRITE_TEXT ); break;
            default:                        halLcdPrintLine("Result: OtherErr ", 8, OVERWRITE_TEXT ); break;
            }
            halLcdPrintLine("Status: Idle     ", 7, OVERWRITE_TEXT );
            break;
        }
        
    }
    
} // main
