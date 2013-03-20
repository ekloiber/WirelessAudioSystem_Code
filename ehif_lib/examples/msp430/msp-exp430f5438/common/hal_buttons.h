/*******************************************************************************
    Filename: hal_buttons.h

    Copyright 2008 Texas Instruments, Inc.
***************************************************************************/
#ifndef HAL_BUTTONS_H
#define HAL_BUTTONS_H

#define BUTTON_PORT_DIR   P2DIR
#define BUTTON_PORT_SEL   P2SEL
#define BUTTON_PORT_OUT   P2OUT
#define BUTTON_PORT_REN   P2REN
#define BUTTON_PORT_IE    P2IE
#define BUTTON_PORT_IES   P2IES
#define BUTTON_PORT_IFG   P2IFG
#define BUTTON_PORT_IN    P2IN

#define BUTTON_SELECT     BIT3
#define BUTTON_DOWN       BIT5
#define BUTTON_UP         BIT4
#define BUTTON_RIGHT      BIT2
#define BUTTON_LEFT       BIT1 
#define BUTTON_S1         BIT6 
#define BUTTON_S2         BIT7 
#define BUTTON_ALL        0xFE

/*-------------------------------------------------------------
 *                  Function Prototypes 
 * ------------------------------------------------------------*/ 
void halButtonsInit(unsigned char buttonsMask);
unsigned char halButtonsPressed(void);
void halButtonsInterruptEnable(unsigned char buttonIntEnableMask);
void halButtonsInterruptDisable(unsigned char buttonIntEnableMask);
void halButtonsShutDown();

#endif
