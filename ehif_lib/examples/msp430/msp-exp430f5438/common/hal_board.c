/** 
 * @file  hal_board.c
 * 
 * Copyright 2008 Texas Instruments, Inc.
******************************************************************************/
#include "hal_msp-exp430f5438.h"

/*-------------------------------------------------------------------------*/
/**********************************************************************//**
 * @brief  Increments the VCore setting.
 * 
 * @param  level The target VCore setting
 * 
 * @return none
 *************************************************************************/
static void halBoardSetVCoreUp (unsigned char level)
{
  // Open PMM module registers for write access 
  PMMCTL0_H = 0xA5;                         
  
  // Set SVS/M high side to new level
  SVSMHCTL = (SVSMHCTL & ~(SVSHRVL0*3 + SVSMHRRL0)) | \
             (SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level); 
  
  // Set SVM new Level    
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;       
  // Set SVS/M low side to new level
  SVSMLCTL = (SVSMLCTL & ~(SVSMLRRL_3)) | (SVMLE + SVSMLRRL0 * level);
   
  while ((PMMIFG & SVSMLDLYIFG) == 0);      // Wait till SVM is settled (Delay)
  PMMCTL0_L = PMMCOREV0 * level;            // Set VCore to x
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);        // Clear already set flags
  
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);     // Wait till level is reached
  
  // Set SVS/M Low side to new level
  SVSMLCTL = (SVSMLCTL & ~(SVSLRVL0*3 + SVSMLRRL_3)) | \
             (SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level);
  
  // Lock PMM module registers from write access
  PMMCTL0_H = 0x00;                         
}

/**********************************************************************//**
 * @brief  Decrements the VCore setting.
 * 
 * @param  level The target VCore.  
 * 
 * @return none
 *************************************************************************/
static void halBoardSetVCoreDown(unsigned char level)
{
  // Open PMM module registers for write access
  PMMCTL0_H = 0xA5;                         
  
  // Set SVS/M low side to new level
  SVSMLCTL = (SVSMLCTL & ~(SVSLRVL0*3 + SVSMLRRL_3)) | \
             (SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level); 
  
  while ((PMMIFG & SVSMLDLYIFG) == 0);      // Wait till SVM is settled (Delay)
  PMMCTL0_L = (level * PMMCOREV0);          // Set VCore to new level 
  // Lock PMM module registers for write access
  
  PMMCTL0_H = 0x00;                         
}

/**********************************************************************//**
 * @brief  Get function for the DCORSEL, VCORE, and DCO multiplier settings 
 *         that map to a given clock speed. 
 * 
 * @param  systemClockSpeed Target DCO frequency - SYSCLK_xxMHZ.
 * 
 * @param  setDcoRange      Pointer to the DCO range select bits.
 * 
 * @param  setVCore         Pointer to the VCore level bits. 
 * 
 * @param  setMultiplier    Pointer to the DCO multiplier bits. 
 *
 * @return none
 ************************************************************************/
static void halBoardGetSystemClockSettings(unsigned char systemClockSpeed, 
                                    unsigned char *setDcoRange,
                                    unsigned char *setVCore,
                                    unsigned int  *setMultiplier)
{
  switch (systemClockSpeed)
  {
  case SYSCLK_1MHZ: 
    *setDcoRange = DCORSEL_1MHZ;
    *setVCore = VCORE_1MHZ;
    *setMultiplier = DCO_MULT_1MHZ;
    break;
  case SYSCLK_4MHZ: 
    *setDcoRange = DCORSEL_4MHZ;
    *setVCore = VCORE_4MHZ;
    *setMultiplier = DCO_MULT_4MHZ;
    break;
  case SYSCLK_8MHZ: 
    *setDcoRange = DCORSEL_8MHZ;
    *setVCore = VCORE_8MHZ;
    *setMultiplier = DCO_MULT_8MHZ;
    break;
  case SYSCLK_12MHZ: 
    *setDcoRange = DCORSEL_12MHZ;
    *setVCore = VCORE_12MHZ;
    *setMultiplier = DCO_MULT_12MHZ;
    break;
  case SYSCLK_16MHZ: 
    *setDcoRange = DCORSEL_16MHZ;
    *setVCore = VCORE_16MHZ;
    *setMultiplier = DCO_MULT_16MHZ;
    break;
  case SYSCLK_20MHZ: 
    *setDcoRange = DCORSEL_20MHZ;
    *setVCore = VCORE_20MHZ;
    *setMultiplier = DCO_MULT_20MHZ;
    break;
  case SYSCLK_25MHZ: 
    *setDcoRange = DCORSEL_25MHZ;
    *setVCore = VCORE_25MHZ;
    *setMultiplier = DCO_MULT_25MHZ;
    break;
  }	
}

/*-------------------------------------------------------------------------*/
/**********************************************************************//**
 * @brief  Set function for the PMM core voltage (PMMCOREV) setting
 * 
 * @param  level Target VCore setting 
 * 
 * @return none
 *************************************************************************/
void halBoardSetVCore(unsigned char level)
{
  unsigned char actLevel;

/* F5438 VCore level is set to level 2 and should never change. 
 * F5438A begins at default level zero and should change according to frequency 
 */ 
  if(Get_Device_Type() == F5438A)  
  {
    do {
      actLevel = PMMCTL0_L & PMMCOREV_3;
      if (actLevel < level) 
        halBoardSetVCoreUp(++actLevel);               // Set VCore (step by step)
      if (actLevel > level) 
        halBoardSetVCoreDown(--actLevel);             // Set VCore (step by step)
    }while (actLevel != level);  
  }
}

/**********************************************************************//**
 * @brief  Disables all supply voltage supervision and monitoring. 
 * 
 * @param  none
 * 
 * @return none
 *************************************************************************/
void halBoardDisableSVS(void)
{
  // Open PMM module registers for write access
  PMMCTL0_H = 0xA5;              
             
  SVSMLCTL &= ~( SVMLE + SVSLE + SVSLFP + SVMLFP );   // Disable Low side SVM
  SVSMHCTL &= ~( SVMHE + SVSHE + SVSHFP + SVMHFP );   // Disable High side SVM
  PMMCTL1 = PMMREFMD;
  
  // Lock PMM module registers for write access
  PMMCTL0_H = 0x00;                         	
}

/**********************************************************************//**
 * @brief  Enables all supply voltage supervision and monitoring
 * 
 * @param  none
 * 
 * @return none
 *************************************************************************/
void halBoardEnableSVS(void)
{
  // Open PMM module registers for write access
  PMMCTL0_H = 0xA5;                         
  
  /*-----------
   * NOTE: To attain the expected < 6 us wakeup from LPM modes, the following
   * two lines must be commented out due to the fact that the PMM will hold
   * the CPU until the reference is fully settled.  
   *----------*/
  SVSMHCTL &= ~(SVSHFP+SVMHFP);             // Disable full-performance mode 
  SVSMLCTL &= ~(SVSLFP+SVMLFP);             // Disable full-performance mode
  SVSMLCTL |= ( SVMLE + SVSLE);             // Enable Low side SVM
  SVSMHCTL |= ( SVMHE + SVSHE);             // Enable High side SVM
  PMMCTL1 &= ~PMMREFMD; 
  
  // Lock PMM module registers for write access  
  PMMCTL0_H = 0x00;                         
}

/**********************************************************************//**
 * @brief  Initialization routine for XT1. 
 * 
 * Sets the necessary internal capacitor values and loops until all 
 * ocillator fault flags remain cleared. 
 * 
 * @param  none
 * 
 * @return none
 *************************************************************************/
void halBoardStartXT1(void)
{
  // Set up XT1 Pins to analog function, and to lowest drive	
  P7SEL |= 0x03;                            
  UCSCTL6 |= XCAP_3 ;                       // Set internal cap values
  
  while(SFRIFG1 & OFIFG) {                  // Check OFIFG fault flag
    while ( (SFRIFG1 & OFIFG))              // Check OFIFG fault flag
    {    
      // Clear OSC fault flags 
      UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT1HFOFFG + XT2OFFG);
      SFRIFG1 &= ~OFIFG;                    // Clear OFIFG fault flag
    }
    UCSCTL6 &= ~(XT1DRIVE1_L+XT1DRIVE0);    // Reduce the drive strength 
  }
}

/**********************************************************************//**
 * @brief  Set function for MCLK frequency.
 * 
 * @param  systemClockSpeed Intended frequency of operation - SYSCLK_xxMHZ.
 * 
 * @return none
 *************************************************************************/
void halBoardSetSystemClock(unsigned char systemClockSpeed)
{
  unsigned char setDcoRange, setVCore;
  unsigned int  setMultiplier;

  halBoardGetSystemClockSettings( systemClockSpeed, &setDcoRange,  \
                                  &setVCore, &setMultiplier);
  	
  halBoardSetVCore( setVCore );   

  __bis_SR_register(SCG0);                  // Disable the FLL control loop    
  UCSCTL0 = 0x00;                           // Set lowest possible DCOx, MODx
  UCSCTL1 = setDcoRange;                    // Select suitable range
  
  UCSCTL2 = setMultiplier + FLLD_1;         // Set DCO Multiplier
  UCSCTL4 = SELA__XT1CLK | SELS__DCOCLKDIV  |  SELM__DCOCLKDIV ;
  __bic_SR_register(SCG0);                  // Enable the FLL control loop
  
  // Loop until XT1,XT2 & DCO fault flag is cleared
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
  
  // Worst-case settling time for the DCO when the DCO range bits have been 
  // changed is n x 32 x 32 x f_FLL_reference. See UCS chapter in 5xx UG 
  // for optimization.
  // 32 x 32 x / f_FLL_reference (32,768 Hz) = .03125 = t_DCO_settle
  // t_DCO_settle / (1 / 25 MHz) = 781250 = counts_DCO_settle
  __delay_cycles(781250);  
}

/**********************************************************************//**
 * @brief  Initializes ACLK, MCLK, SMCLK outputs on P11.0, P11.1, 
 *         and P11.2, respectively.
 * 
 * @param  none
 * 
 * @return none
 *************************************************************************/
void halBoardOutputSystemClock(void)
{
  P11DIR |= 0x07;
  P11SEL |= 0x07;                           
}

/**********************************************************************//**
 * @brief  Stops the output of ACLK, MCLK, SMCLK on P11.0, P11.1, and P11.2.
 * 
 * @param  none
 * 
 * @return none
 *************************************************************************/
void halBoardStopOutputSystemClock(void)
{  
  P11OUT &= ~0x07;
  P11DIR |= 0x07;	
  P11SEL &= ~0x07;                 
}

/**********************************************************************//**
 * @brief  Initializes all GPIO configurations. 
 * 
 * @param  none
 * 
 * @return none
 *************************************************************************/
void halBoardInit(void)
{  
  //Tie unused ports
  PAOUT  = 0;
  PADIR  = 0xFFFF;  
  PASEL  = 0;
  PBOUT  = 0;  
  PBDIR  = 0xFFFF;
  PBSEL  = 0;
  PCOUT  = 0;    
  PCDIR  = 0xFFFF;
  PCSEL  = 0;  
  PDOUT  = 0;  
  PDDIR  = 0xFFFF;
  PDSEL  = 0;  
  PEOUT  = 0;  
  PEDIR  = 0xFEFF;                          // P10.0 to USB RST pin, 
                                            // ...if enabled with J5
  PESEL  = 0;  
  P11OUT = 0;
  P11DIR = 0xFF;
  PJOUT  = 0;    
  PJDIR  = 0xFF;
  P11SEL = 0;
}
