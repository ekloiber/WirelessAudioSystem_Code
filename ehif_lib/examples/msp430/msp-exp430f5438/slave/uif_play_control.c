#include <msp430.h>
#include <stdint.h>
#include <string.h>
#include <cc85xx_ehif_defs.h>
#include "hal_board.h"
#include "hal_lcd.h"
#include "hal_int.h"
#include "hal_buttons.h"




void uifLcdPrintJoystickInfo(void) {
    halLcdPrintLine("L: Skip previous ", 4, OVERWRITE_TEXT );
    halLcdPrintLine("R: Skip next     ", 5, OVERWRITE_TEXT );
    halLcdPrintLine("U: Volume up     ", 6, OVERWRITE_TEXT );
    halLcdPrintLine("D: Volume down   ", 7, OVERWRITE_TEXT );
    halLcdPrintLine("C: Play - Pause  ", 8, OVERWRITE_TEXT );
} // uifLcdPrintJoystickInfo




uint8_t uifPollFunc(EHIF_CMD_RC_SET_DATA_PARAM_T* pParam) {
    static uint8_t prevButtonState = 0x00;

    // Update remote control commands if the button state has changed
    uint8_t buttonState = halButtonsPressed();
    if (buttonState != prevButtonState) {
        prevButtonState = buttonState;
        
        uint8_t rcCmdCount = 0;
        if (buttonState & BUTTON_LEFT) {
            pParam->pRcCmds[rcCmdCount++] = EHIF_RC_CMD_SCAN_PREV_TRACK;
        }
        if (buttonState & BUTTON_RIGHT) {
            pParam->pRcCmds[rcCmdCount++] = EHIF_RC_CMD_SCAN_NEXT_TRACK;
        }
        if (buttonState & BUTTON_UP) {
            pParam->pRcCmds[rcCmdCount++] = EHIF_RC_CMD_OUT_VOL_INCR;
        }
        if (buttonState & BUTTON_DOWN) {
            pParam->pRcCmds[rcCmdCount++] = EHIF_RC_CMD_OUT_VOL_DECR;
        }
        if (buttonState & BUTTON_SELECT) {
            pParam->pRcCmds[rcCmdCount++] = EHIF_RC_CMD_PLAY_PAUSE_TOGGLE;
        }
        pParam->rcCmdCount = rcCmdCount;
        return 1;
    } else { 
        return 0;
    }
    
} // uifPollFunc