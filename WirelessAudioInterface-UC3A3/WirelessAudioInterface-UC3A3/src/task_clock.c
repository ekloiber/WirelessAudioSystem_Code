/*
 * task_clock.c
 *
 * Created: 4/3/2013 8:44:37 PM
 *  Author: Eva
 */ 

#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "task_clock.h"

//! Initializes clocks
void task_clock_init(void) {
	
	gpio_map_t CLOCK_GPIO_MAP =
	{
		{PIN_GCLK0,	FUNC_GCLK0},
		{PIN_GCLK1,	FUNC_GCLK1},
		{PIN_GCLK2,	FUNC_GCLK2},
		{PIN_GCLK3,	FUNC_GCLK3}
	};
	
	gpio_enable_module(CLOCK_GPIO_MAP, sizeof(CLOCK_GPIO_MAP)/sizeof(CLOCK_GPIO_MAP[0]));
#ifdef USE_GCLK_FOR_BCLK_WCLK
	pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK1, 0, 1, 1, 1);	//Default to 48/44.1kHz
	pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK2, 0, 1, 1, (BCLK_MULT/2)-1);	//BCLK / BCLK_MULT
#endif
	
	//MCLK is replica of OSC0 = 12MHz
	pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK3, 0, 0, 0, 0);
	
	return;
}

void task_clock_change_bclk_wclk(uint32_t fs) {

#ifdef USE_GCLK_FOR_BCLK_WCLK
	switch (fs)
	{
		case 44100:
			pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK1, 0, 1, 1, 1);
			pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK2, 0, 1, 1, (BCLK_MULT/2)-1);
		break;
			
		case 48000:
			pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK1, 0, 1, 1, 1);
			pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK2, 0, 1, 1, (BCLK_MULT/2)-1);
		break;
			
		case 88200:
			pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK1, 0, 1, 1, 0);
			pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK2, 0, 1, 1, (BCLK_MULT/4)-1);
		break;
			
		case 96000:
			pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK1, 0, 1, 1, 0);
			pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK2, 0, 1, 1, (BCLK_MULT/4)-1);
		break;
			
		case 132300:
			pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK1, 0, 1, 0, 0);
			pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK2, 0, 1, 1, (BCLK_MULT/8)-1);
		break;
			
		case 196000:
			pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK1, 0, 1, 0, 0);
			pm_gc_setup(&AVR32_PM, AVR32_PM_GCLK_GCLK2, 0, 1, 1, (BCLK_MULT/8)-1);
		break;
		
		default:
			//bad fs
		break;
	}
#endif

	//Not implemented
	
	return;
}