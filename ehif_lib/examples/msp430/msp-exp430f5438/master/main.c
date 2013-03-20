#include <msp430.h>
#include <stdint.h>
#include <string.h>
#include <cc85xx_ehif_utils.h>
#include <cc85xx_ehif_defs.h>
#include <cc85xx_ehif_basic_op.h>
#include <cc85xx_ehif_hal_board.h>
#include <cc85xx_ehif_hal_mcu.h>
#include <cc85xx_ehif_cmd_exec.h>
#include "hal_board.h"
#include "hal_lcd.h"
#include "hal_int.h"
#include "hal_buttons.h"


// Shared parameter/data memory for most EHIF commands (to save RAM and avoid using stack)
EHIF_CMD_PARAM_T ehifCmdParam;
EHIF_CMD_DATA_T  ehifCmdData;


// CC85XX states
#define CC85XX_STATE_OFF            0
#define CC85XX_STATE_INACTIVE       1
#define CC85XX_STATE_ACTIVE         2

// Converts from volume in whole dB's to the resolution used by CC85XX
#define DB_TO_VOL(x)                (((int16_t) (x)) << 3)




void initParam(void) {
    memset(&ehifCmdParam, 0x00, sizeof(ehifCmdParam));
} // initParam




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
    uint8_t  currState;
    uint8_t  targetState;
    uint16_t readbcLength;
    uint8_t  errorCheckInterval = 100;
    uint16_t pairingDurationLeft = 0;

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

    halLcdPrintLine("  CC85XX MASTER  ", 0, OVERWRITE_TEXT );
    halLcdPrintLine("                 ", 1, OVERWRITE_TEXT );
    halLcdPrintLine("S1: Power toggle ", 2, OVERWRITE_TEXT );
    halLcdPrintLine("S2: Pairing start", 3, OVERWRITE_TEXT );
    halLcdPrintLine("U: Volume up     ", 4, OVERWRITE_TEXT );
    halLcdPrintLine("D: Volume down   ", 5, OVERWRITE_TEXT );
    halLcdPrintLine("                 ", 6, OVERWRITE_TEXT );
    halLcdPrintLine("                 ", 7, OVERWRITE_TEXT );
    halLcdPrintLine("                 ", 8, OVERWRITE_TEXT );

    // Initialize EHIF IO
    ehifIoInit();

    // Reset into the application
    ehifSysResetPin(true);
    currState = CC85XX_STATE_INACTIVE;
    targetState = CC85XX_STATE_ACTIVE;

    // Get the initial volume setting from non-volatile storage
    // The difference between currVolume and prevVolume will trigger an update below
    initParam();
    ehifCmdParam.nvsGetData.index = 0;
    ehifCmdExecWithRead(EHIF_EXEC_ALL, EHIF_CMD_NVS_GET_DATA, 
                        sizeof(EHIF_CMD_NVS_GET_DATA_PARAM_T), &ehifCmdParam, 
                        sizeof(EHIF_CMD_NVS_GET_DATA_DATA_T), &ehifCmdData);
    int16_t currVolume = (int16_t) ((uint16_t) ehifCmdData.nvsGetData.data);
    int16_t prevVolume = ~currVolume;

    // Main loop
    while (1) {

        // Wait 10 ms
        EHIF_DELAY_MS(10);

        // Perform action according to edge-triggered button events (debouncing with 100 ms delay)
        switch (pollButtons()) {

        // POWER TOGGLE
        case BUTTON_S1:
            if (currState == CC85XX_STATE_OFF) {
                targetState = CC85XX_STATE_ACTIVE;
            } else {
                targetState = CC85XX_STATE_OFF;
            }
            break;

        // PAIRING TRIGGER
        case BUTTON_S2:
            if (currState != CC85XX_STATE_OFF) {

                // Activate the pairing signal
                initParam();
                ehifCmdParam.nwmControlSignal.wmPairSignal = 1;
                ehifCmdExec(EHIF_CMD_NWM_CONTROL_SIGNAL, sizeof(EHIF_CMD_NWM_CONTROL_SIGNAL_PARAM_T), &ehifCmdParam);

                // Set the timeout after which pairing signaling is disabled (10 seconds)
                pairingDurationLeft = 1000;
            }
            break;

        // MUTE TOGGLE
        case BUTTON_SELECT:
            if (currState != CC85XX_STATE_OFF) {
                initParam();
                ehifCmdParam.vcSetVolume.muteOp = 3; // Toggle
                ehifCmdExec(EHIF_CMD_VC_SET_VOLUME, sizeof(EHIF_CMD_VC_SET_VOLUME_PARAM_T), &ehifCmdParam);
            }
            break;

        // VOLUME DOWN
        case BUTTON_DOWN:
            // Change the target volume by -3 dB and let the update procedure below handle it
            currVolume -= DB_TO_VOL(3);
            break;

        // VOLUME UP
        case BUTTON_UP:
            // Change the target volume by +3 dB and let the update procedure below handle it
            currVolume += DB_TO_VOL(3);
            break;
        }

        // Run the state machine
        if (currState != targetState) {

            if (currState == CC85XX_STATE_OFF) {
                // HANDLE POWER ON

                // Ensure known state (power state 5)
                ehifSysResetPin(true);
                currState = CC85XX_STATE_INACTIVE;

            } else if (targetState == CC85XX_STATE_OFF) {
                // HANDLE POWER OFF

                // Ensure known state (power state 5)
                ehifSysResetPin(true);
                currState = CC85XX_STATE_INACTIVE;

                // Save the current volume setting to non-volatile storage
                initParam();
                ehifCmdParam.nvsSetData.index = 0;
                ehifCmdParam.nvsSetData.data = (uint16_t) currVolume;
                ehifCmdExec(EHIF_CMD_NVS_SET_DATA, sizeof(EHIF_CMD_NVS_SET_DATA_PARAM_T), &ehifCmdParam);
                
                // Set power state 0
                initParam();
                ehifCmdParam.pmSetState.state = 0;
                ehifCmdExec(EHIF_CMD_PM_SET_STATE, sizeof(EHIF_CMD_PM_SET_STATE_PARAM_T), &ehifCmdParam);
                currState = CC85XX_STATE_OFF;

            } else if (targetState == CC85XX_STATE_ACTIVE) {
                // HANDLE NETWORK ACTIVATION

                // Enable network maintenance
                initParam();
                ehifCmdParam.nwmControlEnable.wmEnable = 1;
                ehifCmdExec(EHIF_CMD_NWM_CONTROL_ENABLE, sizeof(EHIF_CMD_NWM_CONTROL_ENABLE_PARAM_T), &ehifCmdParam);
                currState = CC85XX_STATE_ACTIVE;

                // Trigger volume update
                prevVolume = ~currVolume;
            }

        } else {

            // Only OFF and ACTIVE are target states. In the OFF state we do nothing, so only need to
            // handle the ACTIVE state.
            if (currState == CC85XX_STATE_ACTIVE) {

                // Perform error checking at 10 ms * 100 = 1 second intervals:
                // - No timeouts or SPI errors shall have occurred
                // - NWM_GET_STATUS should indicate that we're active
                if (--errorCheckInterval == 0) {
                    errorCheckInterval = 100;

                    // Get the network status, which is the first byte
                    uint8_t nwkStatus;
                    readbcLength = 1;
                    ehifCmdExecWithReadbc(EHIF_EXEC_ALL, EHIF_CMD_NWM_GET_STATUS_M, 0, NULL, &readbcLength, &nwkStatus);

                    // Check for errors
                    uint16_t status = ehifGetStatus();
                    if (ehifGetWaitReadyError() || (status & BV_EHIF_EVT_SPI_ERROR) || (nwkStatus == 0)) {

                        // The device is in an unknown state -> restart everything
                        ehifSysResetPin(true);
                        currState = CC85XX_STATE_INACTIVE;
                    }
                }

                // Limit the new volume setting to range -51 dB to 0 dB (-51 is 1 dB below minimum volume
                // of -50 dB, causing soft-muting, and is also a multiple of 3 dB, which is the used
                // increment size)
                if (currVolume < DB_TO_VOL(-51)) currVolume = -51;
                if (currVolume > DB_TO_VOL(0)) currVolume = 0;

                // Update the remote volume control if needed
                if (currVolume != prevVolume) {
                    initParam();
                    ehifCmdParam.vcSetVolume.setOp = 1; // Absolute
                    ehifCmdParam.vcSetVolume.value = currVolume;
                    ehifCmdExec(EHIF_CMD_VC_SET_VOLUME, sizeof(EHIF_CMD_VC_SET_VOLUME_PARAM_T), &ehifCmdParam);
                }
                
                // Exit pairing mode when the timeout expires (when the countdown goes from 1 to 0)
                if (pairingDurationLeft && !--pairingDurationLeft) {
                    
                    // Dectivate the pairing signal
                    initParam();
                    ehifCmdParam.nwmControlSignal.wmPairSignal = 0;
                    ehifCmdExec(EHIF_CMD_NWM_CONTROL_SIGNAL, sizeof(EHIF_CMD_NWM_CONTROL_SIGNAL_PARAM_T), &ehifCmdParam);
                }
            }
        }
    }

} // main
