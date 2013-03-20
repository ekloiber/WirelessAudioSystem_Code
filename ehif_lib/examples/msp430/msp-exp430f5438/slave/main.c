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

// Scan data (not included in EHIF_CMD_DATA_T due to size)
EHIF_CMD_NWM_DO_SCAN_DATA_T ehifNwmDoScanData;

// Have the remote control data in a separate variable because mouse operation depends on previous values
EHIF_CMD_RC_SET_DATA_PARAM_T ehifRcSetDataParam;


// User interface functions
void uifLcdPrintJoystickInfo(void);
uint8_t uifPollFunc(EHIF_CMD_RC_SET_DATA_PARAM_T* pParam);


// CC85XX states
#define CC85XX_STATE_OFF            0
#define CC85XX_STATE_ALONE          1
#define CC85XX_STATE_PAIRING        2
#define CC85XX_STATE_ACTIVE         3




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
    uint32_t nwkId;
    uint8_t  errorCheckInterval = 100;

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

    halLcdPrintLine("  CC85XX SLAVE   ", 0, OVERWRITE_TEXT );
    halLcdPrintLine("                 ", 1, OVERWRITE_TEXT );
    halLcdPrintLine("S1: Power toggle ", 2, OVERWRITE_TEXT );
    halLcdPrintLine("S2: Pairing start", 3, OVERWRITE_TEXT );
    uifLcdPrintJoystickInfo();

    // Wipe remote control information
    memset(&ehifRcSetDataParam, 0x00, sizeof(ehifRcSetDataParam));

    // Initialize EHIF IO
    ehifIoInit();

    // Reset into the application
    ehifSysResetPin(true);
    currState = CC85XX_STATE_ALONE;
    targetState = CC85XX_STATE_ACTIVE;

    // Get the last used network ID from CC85XX non-volatile storage
    initParam();
    ehifCmdParam.nvsGetData.index = 0;
    ehifCmdExecWithRead(EHIF_EXEC_ALL, EHIF_CMD_NVS_GET_DATA, 
                        sizeof(EHIF_CMD_NVS_GET_DATA_PARAM_T), &ehifCmdParam, 
                        sizeof(EHIF_CMD_NVS_GET_DATA_DATA_T), &ehifCmdData);
    nwkId = ehifCmdData.nvsGetData.data;

    // Handle illegal default network IDs that may occur first time after programming
    if ((nwkId == 0x00000000) || (nwkId == 0xFFFFFFFF)) {
        nwkId = 0xFFFFFFFE;
    }

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
                targetState = CC85XX_STATE_PAIRING;
            }
            break;
        }

        // Run the state machine
        if (currState != targetState) {

            if (currState == CC85XX_STATE_OFF) {
                // HANDLE POWER ON

                // Ensure known state (power state 5)
                ehifSysResetPin(true);
                currState = CC85XX_STATE_ALONE;

            } else if (targetState == CC85XX_STATE_OFF) {
                // HANDLE POWER OFF

                // Ensure known state (power state 5)
                ehifSysResetPin(true);
                currState = CC85XX_STATE_ALONE;

                // Set power state 0
                ehifCmdParam.pmSetState.state = 0;
                ehifCmdExec(EHIF_CMD_PM_SET_STATE, sizeof(EHIF_CMD_PM_SET_STATE_PARAM_T), &ehifCmdParam);
                currState = CC85XX_STATE_OFF;

            } else if (targetState == CC85XX_STATE_PAIRING) {
                // HANDLE PAIRING

                // Let the last executed EHIF command complete, with 5 second timeout
                ehifWaitReadyMs(5000);

                // Disconnect if currently connected
                if (ehifGetStatus() & BV_EHIF_STAT_CONNECTED) {
                    initParam();
                    // All parameters should be zero
                    ehifCmdExec(EHIF_CMD_NWM_DO_JOIN, sizeof(EHIF_CMD_NWM_DO_JOIN_PARAM_T), &ehifCmdParam);
                }

                // Search for one protocol master with pairing signal enabled for 10 seconds
                initParam();
                ehifCmdParam.nwmDoScan.scanTo           = 1000;
                ehifCmdParam.nwmDoScan.scanMax          = 1;
                ehifCmdParam.nwmDoScan.reqPairingSignal = 1;
                ehifCmdParam.nwmDoScan.reqRssi          = -128;
                ehifCmdExecWithReadbc(EHIF_EXEC_CMD, EHIF_CMD_NWM_DO_SCAN, 
                                      sizeof(EHIF_CMD_NWM_DO_SCAN_PARAM_T), &ehifCmdParam, 
                                      NULL, NULL);

                // Fetch network information once ready
                ehifWaitReadyMs(12000);
                readbcLength = sizeof(ehifNwmDoScanData);
                ehifCmdExecWithReadbc(EHIF_EXEC_DATA, EHIF_CMD_NWM_DO_SCAN, 
                                      0, NULL, 
                                      &readbcLength, &ehifNwmDoScanData);

                // If found ...
                if (readbcLength == sizeof(EHIF_CMD_NWM_DO_SCAN_DATA_T)) {

                    // Update the network ID to be used next
                    nwkId = ehifNwmDoScanData.deviceId;

                    // Place the new network ID in CC85XX non-volatile storage
                    initParam();
                    ehifCmdParam.nvsSetData.index = 0;
                    ehifCmdParam.nvsSetData.data  = ehifNwmDoScanData.deviceId;
                    ehifCmdExec(EHIF_CMD_NVS_SET_DATA, sizeof(EHIF_CMD_NVS_SET_DATA_PARAM_T), &ehifCmdParam);
                }

                // Done
                currState = CC85XX_STATE_ALONE;
                targetState = CC85XX_STATE_ACTIVE;

            } else if (targetState == CC85XX_STATE_ACTIVE) {

                // We're disconnected. Proceed only if EHIF is ready, so that power toggle and pairing
                // buttons can still be operated
                uint16_t status = ehifGetStatus();
                if (status & BV_EHIF_STAT_CMD_REQ_RDY) {

                    // Perform join operation first and then activate audio channels. We're using remote
                    // volume control
                    if (!(status & BV_EHIF_STAT_CONNECTED)) {

                        // Enable disconnection notification to avoid unnecessary EHIF activity while active
                        initParam();
                        ehifCmdParam.ehcEvtClr.clearedEvents = BV_EHIF_EVT_NWK_CHG;
                        ehifCmdExec(EHIF_CMD_EHC_EVT_CLR, sizeof(EHIF_CMD_EHC_EVT_CLR_PARAM_T), &ehifCmdParam);
                        initParam();
                        ehifCmdParam.ehcEvtMask.irqGioLevel = 0;
                        ehifCmdParam.ehcEvtMask.eventFilter = BV_EHIF_EVT_NWK_CHG;
                        ehifCmdExec(EHIF_CMD_EHC_EVT_MASK, sizeof(EHIF_CMD_EHC_EVT_MASK_PARAM_T), &ehifCmdParam);

                        // Not connected: Start JOIN operation
                        initParam();
                        ehifCmdParam.nwmDoJoin.joinTo = 100;
                        ehifCmdParam.nwmDoJoin.deviceId = nwkId;
                        ehifCmdExec(EHIF_CMD_NWM_DO_JOIN, sizeof(EHIF_CMD_NWM_DO_JOIN_PARAM_T), &ehifCmdParam);

                    } else {
                        // Connected: Subscribe to audio channels (0xFF = unused)
                        memset(&ehifCmdParam, 0xFF, sizeof(EHIF_CMD_NWM_ACH_SET_USAGE_PARAM_T));
                        ehifCmdParam.nwmAchSetUsage.pAchUsage[0] = 0; // Front left  -> I2S LEFT
                        ehifCmdParam.nwmAchSetUsage.pAchUsage[1] = 1; // Front right -> I2S RIGHT
                        ehifCmdExec(EHIF_CMD_NWM_ACH_SET_USAGE, sizeof(EHIF_CMD_NWM_ACH_SET_USAGE_PARAM_T), &ehifCmdParam);
                        currState = CC85XX_STATE_ACTIVE;
                    }
                }
            }

        } else {

            // Only OFF and ACTIVE are permanent target states. SCAN is only a temporary target state.
            // In the OFF state we do nothing, so only need to handle the ACTIVE state.
            if (currState == CC85XX_STATE_ACTIVE) {

                // Detect network disconnection without generating noise on the SPI interface
                if (EHIF_INTERRUPT_IS_ACTIVE()) {
                    currState = CC85XX_STATE_ALONE;
                }

                // Perform error checking at 10 ms * 100 = 1 second intervals:
                // - No timeouts or SPI errors shall have occurred
                // - We should be connected unless disconnection has been signalized
                if (--errorCheckInterval == 0) {
                    errorCheckInterval = 100;
                    uint16_t status = ehifGetStatus();
                    if (ehifGetWaitReadyError() || (status & BV_EHIF_EVT_SPI_ERROR) ||
                        (!(status & BV_EHIF_STAT_CONNECTED) && !(status & BV_EHIF_EVT_NWK_CHG))) {

                        // The device is in an unknown state -> restart everything
                        ehifSysResetPin(true);
                        currState = CC85XX_STATE_ALONE;
                    }
                }

                // If the network connection is up and running...
                if (currState == CC85XX_STATE_ACTIVE) {

                    // Send remote control commands (mouse or play control, depending on which uif file
                    // is included in the build)
                    if (uifPollFunc(&ehifRcSetDataParam)) {
                        ehifCmdExec(EHIF_CMD_RC_SET_DATA, sizeof(EHIF_CMD_RC_SET_DATA_PARAM_T), &ehifRcSetDataParam);
                    }
                }
            }
        }
    }

} // main
