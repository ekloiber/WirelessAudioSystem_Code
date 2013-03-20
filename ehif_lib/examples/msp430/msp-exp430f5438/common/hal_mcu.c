/******************************************************************************
    Filename: hal_mcu.c

    Copyright 2007 Texas Instruments, Inc.
******************************************************************************/


/*******************************************************************************
 * INCLUDES
 */

#include "hal_types.h"
#include "hal_defs.h"
#include "hal_board.h"
#include "hal_mcu.h"



/*******************************************************************************
 * PUBLIC FUNCTIONS
 */


/******************************************************************************
* @fn  halMcuWaitUs
*
* @brief
* Busy wait function. Waits the specified number of microseconds. Use
* assumptions about number of clock cycles needed for the various instructions.
*
* NB! This function is highly dependent on architecture and compiler!
*
* NB! This function has not been calibrated for MSP
*
* Parameters:
*
* @param  uint16    usec
*                   Number of microseconds to wait.
*
* @return void
*
******************************************************************************/
#pragma optimize=none
void halMcuWaitUs(uint16 usec)
{
    usec >>= 1;         // Divide by 2 to achieve better resolution

    while(usec > 1)     // compare: ~13 cycles
    {
        asm("NOP");     // 1 cycle/NOP
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");

        usec--;         // decr: ~10 cycles
    }
}


/******************************************************************************
* @fn  halMcuWaitMs
*
* @brief
* Busy wait function. Waits the specified number of milliseconds. Use
* assumptions about number of clock cycles needed for the various instructions.
*
* NB! This function is highly dependent on architecture and compiler!
*
* Parameters:
*
* @param  uint16    msec
*                   Number of milliseconds to wait.
*
* @return void
*
******************************************************************************/
void halMcuWaitMs(uint16 msec)
{
    while(msec-- > 0)
    {
        halMcuWaitUs(1000);
    }
}

