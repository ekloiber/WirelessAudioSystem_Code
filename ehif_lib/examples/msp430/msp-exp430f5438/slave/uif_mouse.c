#include <msp430.h>
#include <stdint.h>
#include <string.h>
#include <cc85xx_ehif_defs.h>
#include "hal_board.h"
#include "hal_lcd.h"
#include "hal_int.h"
#include "hal_buttons.h"


uint32_t xMouseMoveDuration = 0;
uint32_t yMouseMoveDuration = 0;




void uifLcdPrintJoystickInfo(void) {
    halLcdPrintLine("L: Mouse left    ", 4, OVERWRITE_TEXT );
    halLcdPrintLine("R: Mouse right   ", 5, OVERWRITE_TEXT );
    halLcdPrintLine("U: Mouse up      ", 6, OVERWRITE_TEXT );
    halLcdPrintLine("D: Mouse down    ", 7, OVERWRITE_TEXT );
    halLcdPrintLine("C: Mouse L-button", 8, OVERWRITE_TEXT );
} // uifLcdPrintJoystickInfo




uint8_t uifPollFunc(EHIF_CMD_RC_SET_DATA_PARAM_T* pParam) {

    // Update mouse position and button state and send if changed
    uint8_t doRcSetData = 0;
    uint8_t buttonState = halButtonsPressed();
    if (buttonState & BUTTON_LEFT) { 
        pParam->mousePosX -= 1 + ((++xMouseMoveDuration) >> 5);
        doRcSetData = 1; 
    } else if (buttonState & BUTTON_RIGHT) {
        pParam->mousePosX += 1 + ((++xMouseMoveDuration) >> 5);
        doRcSetData = 1; 
    } else {
        xMouseMoveDuration = 0;
    }
    if (buttonState & BUTTON_UP) { 
        pParam->mousePosY -= 1 + ((++yMouseMoveDuration) >> 5);
        doRcSetData = 1; 
    } else if (buttonState & BUTTON_DOWN) { 
        pParam->mousePosY += 1 + ((++yMouseMoveDuration) >> 5);
        doRcSetData = 1; 
    } else {
        yMouseMoveDuration = 0;
    }
    uint8_t bvMouseButtons = ((buttonState & BUTTON_SELECT) != 0) << 0;
    if (bvMouseButtons != pParam->bvMouseButtons) {
        pParam->bvMouseButtons = bvMouseButtons;
        doRcSetData = 1;
    }
    
    // Set fixed parameters
    pParam->extSel = 1;
    
    return doRcSetData;
    
} // uifPollFunc