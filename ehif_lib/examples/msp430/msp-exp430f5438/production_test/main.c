#include <msp430.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <cc85xx_ehif_utils.h>
#include <cc85xx_ehif_defs.h>
#include <cc85xx_ehif_basic_op.h>
#include <cc85xx_ehif_hal_board.h>
#include <cc85xx_ehif_hal_mcu.h>
#include <cc85xx_ehif_cmd_exec.h>
#include <cc85xx_ehif_bootloader.h>
#include "hal_board.h"
#include "hal_lcd.h"
#include "hal_int.h"
#include "hal_buttons.h"
#include "UserExperienceGraphics.h"


// Shared parameter/data memory for most EHIF commands (to save RAM and avoid using stack)
EHIF_CMD_PARAM_T ehifCmdParam;
EHIF_CMD_DATA_T  ehifCmdData;


// Production test states
#define IDLE                        0
#define SELECT_IMAGE                1
#define FLASH_PROD_TEST_IMAGE       2
#define FLASH_PROG_FAILED           3
#define D3_TEST                     4
#define ROCKER_UP_TEST              5
#define ROCKER_DOWN_TEST            6
#define D2_TEST                     7
#define TONE_GEN_TEST               8
#define TONE_DET_TEST               9
#define TX_CW_TEST_LO               10
#define TX_CW_TEST_MID              11
#define TX_CW_TEST_HIGH             12
#define FLASH_APP_IMAGE             13
#define PROD_TEST_DONE              14


// Application roles
#define MASTER                      0
#define SLAVE                       1


// Global variables
bool_t shortButtonTimer;
bool_t rockerDownPressed;
bool_t newRockerDownPress;
bool_t newRockerDownPressPossible;
bool_t rockerUpPressed;
bool_t newRockerUpPress;
bool_t newRockerUpPressPossible;
uint16_t toggleLED;
uint32_t rockerUpDebounce;
uint32_t rockerDownDebounce;

extern const uint8_t pHeadsetMasterPtest[];
extern const uint8_t pHeadsetSlavePtest[];
extern const uint8_t pHeadsetMasterApplication[];
extern const uint8_t pHeadsetSlaveApplication[];
const uint8_t* pSelFlashImage;



void initParam(void) {
    memset(&ehifCmdParam, 0x00, sizeof(ehifCmdParam));
} // initParam


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
    if (shortButtonTimer && lastButtonDuration == 5 || !shortButtonTimer && lastButtonDuration == 500) {
        if (lastButtonDuration == 5) lastButtonDuration = 501;
        return lastButton & BUTTON_ALL;
    } else {
        return 0x00;
    }
} // pollButtons



void detectRockerPresses(uint8_t currState) {
    // Read value on GIO1 (Rocker UP) and GIO3 (Rocker DOWN)
    initParam();
    ehifCmdParam.iotstInput.pinMask = 0x00000005;
    ehifCmdExecWithRead(EHIF_EXEC_ALL, EHIF_CMD_IOTST_INPUT, 
            sizeof(EHIF_CMD_IOTST_INPUT_PARAM_T), &ehifCmdParam,
            sizeof(EHIF_CMD_IOTST_INPUT_DATA_T), &ehifCmdData);    
    
    // Clear any new rocker presses
    newRockerDownPress = false;
    newRockerUpPress = false;

    
    // Check if rocker DOWN is pressed
    if ((ehifCmdData.iotstInput.pinVal & 0x00000004) == 0x00000000) {
        // Shifting in a 0 on the LSB
        rockerDownDebounce <<= 1;
    } else {
        // Shifting in a 1 on the LSB
        rockerDownDebounce <<= 1;
        rockerDownDebounce |= 0x00000001;
    }
//    if (rockerDownDebounce == 0x00000000)
    if ((!shortButtonTimer && rockerDownDebounce == 0x00000000) || (shortButtonTimer && (rockerDownDebounce & 0x00000FFF) == 0x00000000))
        rockerDownPressed = true;
//    else if (rockerDownDebounce == 0xFFFFFFFF) {
    else if ((!shortButtonTimer && rockerDownDebounce == 0xFFFFFFFF) || (shortButtonTimer && (rockerDownDebounce & 0x00000FFF) == 0x00000FFF)) {
        rockerDownPressed = false;
        newRockerDownPressPossible = true;
    }

    // Check that the rocker DOWN press we found (if any) is a new press
    if (rockerDownPressed && newRockerDownPressPossible) {
        newRockerDownPress = true;
        newRockerDownPressPossible = false;
    }
    
    
    // Check if rocker UP is pressed
    if ((ehifCmdData.iotstInput.pinVal & 0x00000001) == 0x00000000) {
        // Shifting in a 0 on the LSB
        rockerUpDebounce <<= 1;
    } else {
        // Shifting in a 1 on the LSB
        rockerUpDebounce <<= 1;
        rockerUpDebounce |= 0x00000001;
    }
    if (rockerUpDebounce == 0x00000000)
        rockerUpPressed = true;
    else if (rockerUpDebounce == 0xFFFFFFFF) {
        rockerUpPressed = false;
        newRockerUpPressPossible = true;
    }

    // Check that the rocker UP press we found (if any) is a new press
    if (rockerUpPressed && newRockerUpPressPossible) {
        newRockerUpPress = true;
        newRockerUpPressPossible = false;
    }    
    
    
    // Set D2 back to it's current flashing state, otherwise it will not flash... 
    // For both IOTST_OUTPUT and IOTST_INPUT the family user's guide mentions that 
    // "The pins will revert to their original states when their respective <PIN>_SEL is released".
    // We do IOTST_INPUT commands continuously to listen for rocker presses this command leaves the pins in a pulled-up
    // state, so the flashing we do with the toggleLED variable will not be visible for the test operator.
    if (currState == D2_TEST) {
        initParam();
        ehifCmdParam.iotstOutput.pinMask = 0x0800;
        ehifCmdParam.iotstOutput.pinVal = toggleLED;
        ehifCmdExec(EHIF_CMD_IOTST_OUTPUT, sizeof(EHIF_CMD_IOTST_OUTPUT_PARAM_T), &ehifCmdParam);        
    }
    
} // detectRockerPresses




int __low_level_init(void) {
    // Prevent data memory initialization that triggers the watchdog timer
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    return 1;
}




int main(void) {
    uint8_t currState = IDLE;
    uint8_t targetState = SELECT_IMAGE;
    uint32_t counter = 0;
    bool_t resetForCWComplete;
    uint8 applicationRole;
    uint16_t status;
        
    // Initialize global variables
    shortButtonTimer = false;
    rockerDownPressed = false;
    newRockerDownPress = false;
    newRockerDownPressPossible = true;
    rockerUpPressed = false;
    newRockerUpPress = false;
    newRockerUpPressPossible = true;    
    rockerUpDebounce = 0xFFFFFFFF;
    rockerDownDebounce = 0xFFFFFFFF;
    toggleLED = 0x0800;

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

    // Initialize EHIF IO
    ehifIoInit();
    
    
    
    // Main loop
    while (1) {
    
        // Perform action according to edge-triggered button events
        switch (pollButtons()) {
            
        // BUTTON S1 (TEST FAILED and image selection)
        case BUTTON_S1:
            switch (currState) {
            
            case SELECT_IMAGE:
                pSelFlashImage = pHeadsetMasterPtest;
                applicationRole = MASTER;
                targetState = FLASH_PROD_TEST_IMAGE;
                break;
                
            case D3_TEST:
                halLcdPrintLine("D3...........FAIL", 0, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 1, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 2, OVERWRITE_TEXT );
                targetState = PROD_TEST_DONE;
                break;
                
            case ROCKER_UP_TEST:
                halLcdPrintLine("Rocker up....FAIL", 1, OVERWRITE_TEXT );
                targetState = PROD_TEST_DONE;
                break;
                
            case ROCKER_DOWN_TEST:
                halLcdPrintLine("Rocker down..FAIL", 2, OVERWRITE_TEXT );
                targetState = PROD_TEST_DONE;
                break;
                
            case D2_TEST:
                halLcdPrintLine("D2 flashing..FAIL", 3, OVERWRITE_TEXT );
                
                // Make sure we turn off the LED before leaving this state.
                initParam();
                ehifCmdParam.iotstOutput.pinMask = 0x0800;
                ehifCmdParam.iotstOutput.pinVal = 0x0800;
                ehifCmdExec(EHIF_CMD_IOTST_OUTPUT, sizeof(EHIF_CMD_IOTST_OUTPUT_PARAM_T), &ehifCmdParam);
                
                targetState = PROD_TEST_DONE;
                break;
                
            case TONE_GEN_TEST:
                halLcdPrintLine("Tone test....FAIL", 4, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 5, OVERWRITE_TEXT );
                
                // Make sure we stop the 1kHz sinus in both channel 0 and channel 1
                initParam();
                ehifCmdParam.atGenTone.logChannel = 0x00;
                ehifCmdParam.atGenTone.reserved0 = 0x00;
                ehifCmdParam.atGenTone.amplitude = 0x00;
                ehifCmdParam.atGenTone.freq = 0x0064;                
                ehifCmdExec(EHIF_CMD_AT_GEN_TONE, sizeof(EHIF_CMD_AT_GEN_TONE_PARAM_T), &ehifCmdParam);
                ehifCmdParam.atGenTone.logChannel = 0x01;
                ehifCmdExec(EHIF_CMD_AT_GEN_TONE, sizeof(EHIF_CMD_AT_GEN_TONE_PARAM_T), &ehifCmdParam);
                
                targetState = PROD_TEST_DONE;
                break;
                
            case TONE_DET_TEST:
                halLcdPrintLine("Tone test....FAIL", 4, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 5, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 6, OVERWRITE_TEXT );
                targetState = PROD_TEST_DONE;
                break;
                
            case TX_CW_TEST_LO:
                halLcdPrintLine("CW LO........FAIL", 5, OVERWRITE_TEXT );
                
                // Disable CW
                ehifCmdParam.rftTxtstCw.enable = 0x00;
                ehifCmdExec(EHIF_CMD_RFT_TXTST_CW, sizeof(EHIF_CMD_RFT_TXTST_CW_PARAM_T), &ehifCmdParam);
                
                targetState = PROD_TEST_DONE;
                break;
                
            case TX_CW_TEST_MID:
                halLcdPrintLine("CW MID.......FAIL", 6, OVERWRITE_TEXT );
                
                // Disable CW
                ehifCmdParam.rftTxtstCw.enable = 0x00;
                ehifCmdExec(EHIF_CMD_RFT_TXTST_CW, sizeof(EHIF_CMD_RFT_TXTST_CW_PARAM_T), &ehifCmdParam);
                
                targetState = PROD_TEST_DONE;
                break;
                
            case TX_CW_TEST_HIGH:
                halLcdPrintLine("CW HIGH......FAIL", 7, OVERWRITE_TEXT );
                
                // Disable CW
                ehifCmdParam.rftTxtstCw.enable = 0x00;
                ehifCmdExec(EHIF_CMD_RFT_TXTST_CW, sizeof(EHIF_CMD_RFT_TXTST_CW_PARAM_T), &ehifCmdParam);
                
                targetState = PROD_TEST_DONE;
                break;
            
            }
            break;

        // BUTTON S2 (Image selection, retry programming when failed, restart test when finnished, passing D3 test)
        case BUTTON_S2:
            switch (currState) {
            
            case SELECT_IMAGE:
                pSelFlashImage = pHeadsetSlavePtest;
                applicationRole = SLAVE;
                targetState = FLASH_PROD_TEST_IMAGE;
                break;
                
            case FLASH_PROG_FAILED:
                targetState = SELECT_IMAGE;
                break;
                
            case D3_TEST:
                targetState = ROCKER_UP_TEST;
                break;
                
            case PROD_TEST_DONE:
                targetState = SELECT_IMAGE;
                break;
                
            }
            break;
            
        } // switch (pollButtons())
    

    
        // Run the state machine
        if (currState != targetState) {
            
            // We are entering a new state/test. In here we should mainly update the LCD display and change the currState variable.
            // EHIF commands for TONE_GEN_TEST and PROD_TEST_DONE as well as flash programming are executed here as well.
            
            if (targetState == SELECT_IMAGE) {
                halLcdPrintLine("Image selection  ", 0, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 1, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 2, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 3, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 4, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 5, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 6, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 7, OVERWRITE_TEXT );
                halLcdPrintLine("S1:M         S2:S", 8, OVERWRITE_TEXT );
                currState = SELECT_IMAGE;
                
            } else if (targetState == FLASH_PROD_TEST_IMAGE) {
                
                halLcdPrintLine("                 ", 0, OVERWRITE_TEXT );
                halLcdPrintLine("       ...       ", 4, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 8, OVERWRITE_TEXT );
                status = eraseProgVerifyFlash(pSelFlashImage);
                
                switch (status) {
                case EHIF_BL_VERIFY_OK:         halLcdPrintLine("Result: SUCCESS",   0, OVERWRITE_TEXT ); break;
                case EHIF_BL_VERIFY_FAILED:     halLcdPrintLine("Result: VerifyErr", 0, OVERWRITE_TEXT ); break;
                case EHIF_BL_PROG_FAILED:       halLcdPrintLine("Result: ProgErr  ", 0, OVERWRITE_TEXT ); break;
                case EHIF_BL_ERASE_FAILED:      halLcdPrintLine("Result: EraseErr ", 0, OVERWRITE_TEXT ); break;
                case EHIF_BL_SPI_LOADER_LOCKED: halLcdPrintLine("Result: UnlockErr", 0, OVERWRITE_TEXT ); break;
                default:                        halLcdPrintLine("Result: OtherErr ", 0, OVERWRITE_TEXT ); break;
                }
                
                if (status == EHIF_BL_VERIFY_OK)
                    targetState = D3_TEST;
                else
                    targetState = FLASH_PROG_FAILED;
                
                currState = FLASH_PROD_TEST_IMAGE;
                
            } else if (targetState == FLASH_PROG_FAILED) {
                halLcdPrintLine("                 ", 1, OVERWRITE_TEXT );
                halLcdPrintLine("An error occurred", 2, OVERWRITE_TEXT );
                halLcdPrintLine("during program or", 3, OVERWRITE_TEXT );
                halLcdPrintLine("verify of the    ", 4, OVERWRITE_TEXT );
                
                if (currState == FLASH_PROD_TEST_IMAGE)
                halLcdPrintLine("prodTest image   ", 5, OVERWRITE_TEXT );
                else
                halLcdPrintLine("application image", 5, OVERWRITE_TEXT );
                
                halLcdPrintLine("                 ", 6, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 7, OVERWRITE_TEXT );
                halLcdPrintLine("         S2:Retry", 8, OVERWRITE_TEXT );

                currState = FLASH_PROG_FAILED;
                
            }
            else if (targetState == D3_TEST) {
                halLcdPrintLine("Connect the USB  ", 0, OVERWRITE_TEXT );
                halLcdPrintLine("cable. D3 should ", 1, OVERWRITE_TEXT );
                halLcdPrintLine("be lit.          ", 2, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 3, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 4, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 5, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 6, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 7, OVERWRITE_TEXT );
                halLcdPrintLine("S1:FAIL   S2:PASS", 8, OVERWRITE_TEXT );
                currState = D3_TEST;
                
            } else if (targetState == ROCKER_UP_TEST) {
                // Reset CC85xx.. This is the first time we are going to interract with the CC85xx via SPI.
                ehifSysResetPin(true);
                halLcdPrintLine("D3.............OK", 0, OVERWRITE_TEXT );
                halLcdPrintLine("Rocker up...     ", 1, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 2, OVERWRITE_TEXT );
                halLcdPrintLine("S1:FAIL          ", 8, OVERWRITE_TEXT );
                currState = ROCKER_UP_TEST;

            } else if (targetState == ROCKER_DOWN_TEST) {                
                halLcdPrintLine("Rocker up......OK", 1, OVERWRITE_TEXT );
                halLcdPrintLine("Rocker down...   ", 2, OVERWRITE_TEXT );                
                currState = ROCKER_DOWN_TEST;
                
            } else if (targetState == D2_TEST) {
                halLcdPrintLine("Rocker down....OK", 2, OVERWRITE_TEXT );
                halLcdPrintLine("D2 flashing...   ", 3, OVERWRITE_TEXT );
                halLcdPrintLine("S1:FAIL     PASS ", 8, OVERWRITE_TEXT );               
                halLcdImage(IMG_ARROW_DOWN, 1, 10, 8*16, 12*8);                
                currState = D2_TEST;
                
                // Performing the IOTST_OUTPUT stalls the CPU for some time and we need to lower the lastButtonDuration requirement for buttonpresses.
                shortButtonTimer = true;
                
            } else if (targetState == TONE_GEN_TEST) {
                // Generate a 1kHz sinus in both channel 0 and channel 1
                initParam();
                ehifCmdParam.atGenTone.logChannel = 0x00;
                ehifCmdParam.atGenTone.reserved0 = 0x00;
                ehifCmdParam.atGenTone.amplitude = 0x01;
                ehifCmdParam.atGenTone.freq = 0x0064;                
                ehifCmdExec(EHIF_CMD_AT_GEN_TONE, sizeof(EHIF_CMD_AT_GEN_TONE_PARAM_T), &ehifCmdParam);
                ehifCmdParam.atGenTone.logChannel = 0x01;
                ehifCmdExec(EHIF_CMD_AT_GEN_TONE, sizeof(EHIF_CMD_AT_GEN_TONE_PARAM_T), &ehifCmdParam);
                
                halLcdPrintLine("D2 flashing....OK", 3, OVERWRITE_TEXT );
                halLcdPrintLine("Tone test...     ", 4, OVERWRITE_TEXT );
                halLcdPrintLine("1kHz playing...  ", 5, OVERWRITE_TEXT );
                halLcdPrintLine("S1:FAIL     PASS ", 8, OVERWRITE_TEXT );               
                halLcdImage(IMG_ARROW_DOWN, 1, 10, 8*16, 12*8);                
                currState = TONE_GEN_TEST;
                
            } else if (targetState == TONE_DET_TEST) {
                halLcdPrintLine("D2 flashing....OK", 3, OVERWRITE_TEXT );
                halLcdPrintLine("Tone test...     ", 4, OVERWRITE_TEXT );
                halLcdPrintLine("Freq L   :     Hz", 5, OVERWRITE_TEXT );
                halLcdPrintLine("Freq R   :     Hz", 6, OVERWRITE_TEXT );
                halLcdPrintLine("S1:FAIL     PASS ", 8, OVERWRITE_TEXT );               
                halLcdImage(IMG_ARROW_DOWN, 1, 10, 8*16, 12*8);
                currState = TONE_DET_TEST;
                
                // Performing the tone detection stalls the CPU for some time and we need to lower the lastButtonDuration requirement for buttonpresses.
                shortButtonTimer = true;
                
            } else if (targetState == TX_CW_TEST_LO) {
                // We are about to do a reset since we are going to use the RFT_TXTST_CW command. 
                // This variabel must be false for the reset to happen.
                resetForCWComplete = false;
                
                halLcdPrintLine("Tone test......OK", 4, OVERWRITE_TEXT );
                halLcdPrintLine("CW LO...         ", 5, OVERWRITE_TEXT );
                halLcdPrintLine("                 ", 6, OVERWRITE_TEXT );
                currState = TX_CW_TEST_LO;
                
                // We can now return to the normal lastButtonDuration requirement for buttonpresses.
                shortButtonTimer = false;
        
            } else if (targetState == TX_CW_TEST_MID) {
                // We are about to do a reset since we are going to use the RFT_TXTST_CW command. 
                // This variabel must be false for the reset to happen.
                resetForCWComplete = false;
                
                halLcdPrintLine("CW LO..........OK", 5, OVERWRITE_TEXT );
                halLcdPrintLine("CW MID...        ", 6, OVERWRITE_TEXT );
                currState = TX_CW_TEST_MID;
            
            } else if (targetState == TX_CW_TEST_HIGH) {
                // We are about to do a reset since we are going to use the RFT_TXTST_CW command. 
                // This variabel must be false for the reset to happen.
                resetForCWComplete = false;
                
                halLcdPrintLine("CW MID.........OK", 6, OVERWRITE_TEXT );
                halLcdPrintLine("CW HIGH...       ", 7, OVERWRITE_TEXT );  
                currState = TX_CW_TEST_HIGH;
                
            } 
            
            else if (targetState == FLASH_APP_IMAGE) {
                // Disable CW
                ehifCmdParam.rftTxtstCw.enable = 0x00;
                ehifCmdExec(EHIF_CMD_RFT_TXTST_CW, sizeof(EHIF_CMD_RFT_TXTST_CW_PARAM_T), &ehifCmdParam);
                
                if (applicationRole == MASTER)
                    pSelFlashImage = pHeadsetMasterApplication;
                else
                    pSelFlashImage = pHeadsetSlaveApplication;
                
                status = eraseProgVerifyFlash(pSelFlashImage);
                
                if (status == EHIF_BL_VERIFY_OK)
                    targetState = PROD_TEST_DONE;
                else
                {
                    switch (status) {
                    case EHIF_BL_VERIFY_FAILED:     halLcdPrintLine("Result: VerifyErr", 0, OVERWRITE_TEXT ); break;
                    case EHIF_BL_PROG_FAILED:       halLcdPrintLine("Result: ProgErr  ", 0, OVERWRITE_TEXT ); break;
                    case EHIF_BL_ERASE_FAILED:      halLcdPrintLine("Result: EraseErr ", 0, OVERWRITE_TEXT ); break;
                    case EHIF_BL_SPI_LOADER_LOCKED: halLcdPrintLine("Result: UnlockErr", 0, OVERWRITE_TEXT ); break;
                    default:                        halLcdPrintLine("Result: OtherErr ", 0, OVERWRITE_TEXT ); break;
                    }
                    targetState = FLASH_PROG_FAILED;
                }
                
                currState = FLASH_APP_IMAGE;
                
            }
            
            else if (targetState == PROD_TEST_DONE) {
                ehifSysResetSpi(true);
                halLcdPrintLine("Restart test: S2 ", 8, OVERWRITE_TEXT );
                currState = PROD_TEST_DONE;
                
                // We can now return to the normal lastButtonDuration requirement for buttonpresses (if needed).
                shortButtonTimer = false;
            }

        }
        else {
            
            // currState is equal to targetState...
            
            if (currState == ROCKER_UP_TEST) {
                if (newRockerUpPress) {
                    targetState = ROCKER_DOWN_TEST;                    
                }
                
            } else if (currState == ROCKER_DOWN_TEST) {
                if (newRockerDownPress) {
                    targetState = D2_TEST;
                }
                
            } else if (currState == D2_TEST) {
                // Flash LED D2
                if (counter <= 0)
                {
                  // Set the D2 blinking speed
                  counter = 1000;
                  
                  toggleLED ^= 0x0800;
                  
                  // Set the pin value of GIO12 (connected to D2)
                  initParam();
                  ehifCmdParam.iotstOutput.pinMask = 0x0800;
                  ehifCmdParam.iotstOutput.pinVal = toggleLED;
                  ehifCmdExec(EHIF_CMD_IOTST_OUTPUT, sizeof(EHIF_CMD_IOTST_OUTPUT_PARAM_T), &ehifCmdParam);
                }
                else
                  counter--;
                
                // Detect rockerpress for PASS
                if (newRockerDownPress) {
                    if (applicationRole == MASTER)
                        targetState = TONE_DET_TEST;
                    else
                        targetState = TONE_GEN_TEST;
                    
                }
                
            } else if (currState == TONE_GEN_TEST) {
                // Detect rockerpress for PASS
                if (newRockerDownPress) {
                    targetState = TX_CW_TEST_LO;                    
                }
                
            } else if (currState == TONE_DET_TEST) {                                
                static char freq[5];
                
                // Detect tone on channel 0 and update LCD
                initParam();
                ehifCmdParam.atDetTone.logChannel = 0x00;
                ehifCmdExecWithRead(EHIF_EXEC_ALL, EHIF_CMD_AT_DET_TONE, 
                        sizeof(EHIF_CMD_AT_DET_TONE_PARAM_T), &ehifCmdParam,
                        sizeof(EHIF_CMD_AT_DET_TONE_DATA_T), &ehifCmdData);
                
                sprintf(freq,"%5u",10 * ehifCmdData.atDetTone.freq);
                halLcdPrintLineCol(freq, 5, 10, OVERWRITE_TEXT) ;
                
                // Detect tone on channel 1 and update LCD
                initParam();
                ehifCmdParam.atDetTone.logChannel = 0x01;
                ehifCmdExecWithRead(EHIF_EXEC_ALL, EHIF_CMD_AT_DET_TONE, 
                        sizeof(EHIF_CMD_AT_DET_TONE_PARAM_T), &ehifCmdParam,
                        sizeof(EHIF_CMD_AT_DET_TONE_DATA_T), &ehifCmdData);
                
                sprintf(freq,"%5u",10 * ehifCmdData.atDetTone.freq);
                halLcdPrintLineCol(freq, 6, 10, OVERWRITE_TEXT) ;
                
                // Detect rockerpress for PASS
                if (newRockerDownPress) {
                    targetState = TX_CW_TEST_LO;                    
                }
                
            } else if (currState == TX_CW_TEST_LO) {
                // A reset is required before and after the use of RFT_TXTST_CW. Since we are using the rocker down switch as a next button, 
                // and this production test is meant for both the master and slave headset boards, we have an issue with reset on the master... 
                // The preloaded demo master has no buttons assigned to GIO1 and GIO3, so if the rocker button is pressed while we are doing a reset
                // it will suddenly be a pull-down instead of a pull-up on this pin after the reset. That's why we have to wait for a release of 
                // the rocker button before we perform the reset. The resetForCWComplete makes sure we don't enter this if statement more 
                // than once.
                if (newRockerDownPressPossible && !resetForCWComplete) {
                    // Reset CC85xx.. A reset is required before the use of RFT_TXTST_CW.
                    ehifSysResetPin(true);
                    resetForCWComplete = true;

                    // Output CW on 2406 MHz
                    initParam();
                    ehifCmdParam.rftTxtstCw.enable = 0x01;
                    ehifCmdParam.rftTxtstCw.reserved0 = 0x00;
                    ehifCmdParam.rftTxtstCw.rfFreq = 88;
                    ehifCmdParam.rftTxtstCw.txPower = 5;
                    ehifCmdExec(EHIF_CMD_RFT_TXTST_CW, sizeof(EHIF_CMD_RFT_TXTST_CW_PARAM_T), &ehifCmdParam);
                    
                }
                
                // Detect rockerpress for PASS
                if (newRockerDownPress) {
                    targetState = TX_CW_TEST_MID;
                }
                
            } else if (currState == TX_CW_TEST_MID) {
                // See above (currState == TX_CW_TEST_LO) for an explanation for this if...
                if (newRockerDownPressPossible && !resetForCWComplete) {
                    // Reset CC85xx.. A reset is required before the use of RFT_TXTST_CW.
                    ehifSysResetPin(true);
                    resetForCWComplete = true;
                    
                    // Output CW on 2442 MHz
                    ehifCmdParam.rftTxtstCw.rfFreq = 124;
                    ehifCmdExec(EHIF_CMD_RFT_TXTST_CW, sizeof(EHIF_CMD_RFT_TXTST_CW_PARAM_T), &ehifCmdParam);
                    
                }
                
                // Detect rockerpress for PASS
                if (newRockerDownPress) {
                    targetState = TX_CW_TEST_HIGH;                    
                }
                
            } else if (currState == TX_CW_TEST_HIGH) {
                // See above (currState == TX_CW_TEST_LO) for an explanation for this if...
                if (newRockerDownPressPossible && !resetForCWComplete) {
                    // Reset CC85xx.. A reset is required before the use of RFT_TXTST_CW.
                    ehifSysResetPin(true);
                    resetForCWComplete = true;
                    
                    // Output CW on 2474 MHz
                    ehifCmdParam.rftTxtstCw.rfFreq = 156;
                    ehifCmdExec(EHIF_CMD_RFT_TXTST_CW, sizeof(EHIF_CMD_RFT_TXTST_CW_PARAM_T), &ehifCmdParam);
                    
                }
                
                // Detect rockerpress for PASS
                if (newRockerDownPress) {
                    halLcdPrintLine("CW HIGH........OK", 7, OVERWRITE_TEXT );
//                    targetState = PROD_TEST_DONE;
                    targetState = FLASH_APP_IMAGE;
                }
                
            }
            
            
            // Not necessary to detect rocker presses in all the states
            if (currState > D3_TEST && currState < PROD_TEST_DONE)
                detectRockerPresses(currState);
        }
    
    } // while (1)
    

} // main
