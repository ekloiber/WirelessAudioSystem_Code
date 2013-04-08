/*
 * task_LEDs.c
 *
 * Created: 4/3/2013 4:18:28 PM
 *  Author: Eva
 */ 

#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "tc.h"
#include "task_LEDs.h"
#include "task_ADC.h"

#define INCR_LUT(var) ((var < (SINE_LUT_SIZE-1)) ? (var++) : (var=0))

//! Initialize the LED timer/counter PWM function
void task_leds_init(void) {
	
	static const gpio_map_t LED_GPIO_MAP =
	{
		{PIN_RED_TOP,	FUNC_RED_TOP},
		{PIN_GREEN_TOP, FUNC_GREEN_TOP},
		{PIN_BLUE_TOP,	FUNC_BLUE_TOP},
		{PIN_RED_BOT,	FUNC_RED_BOT},
		{PIN_GREEN_BOT, FUNC_GREEN_BOT},
		{PIN_BLUE_BOT,	FUNC_BLUE_BOT}
	};
	
	gpio_enable_module(LED_GPIO_MAP, sizeof(LED_GPIO_MAP)/sizeof(LED_GPIO_MAP[0]));
	
	// Set-up:
	// - TIOA turns on when Counter = RA, off when Counter = RC
	// - TIOB turns on when Counter = RB, off when Counter = RC
	// - Clock is fPBA / 8 = 8.25MHz
	
	tc_waveform_opt_t waveform_opt_0 = {
		.channel = 0,
		
		.bswtrg = TC_EVT_EFFECT_NOOP,
		.beevt = TC_EVT_EFFECT_NOOP,
		.bcpc = TC_EVT_EFFECT_TOGGLE,
		.bcpb = TC_EVT_EFFECT_TOGGLE,
		
		.aswtrg = TC_EVT_EFFECT_NOOP,
		.aeevt = TC_EVT_EFFECT_NOOP,
		.acpc = TC_EVT_EFFECT_TOGGLE,
		.acpa = TC_EVT_EFFECT_TOGGLE,
		
		.wavsel = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
		
		.enetrg = false,
		.eevt = TC_EXT_EVENT_SEL_XC0_OUTPUT,
		.eevtedg = TC_SEL_NO_EDGE,
		.cpcdis = false,
		.cpcstop = false,
		
		.burst = TC_BURST_NOT_GATED,
		.clki = TC_CLOCK_RISING_EDGE,
		.tcclks = TC_CLOCK_SOURCE_TC3,
	};
	
	tc_waveform_opt_t waveform_opt_1 = {
		.channel = 1,
		
		.bswtrg = TC_EVT_EFFECT_NOOP,
		.beevt = TC_EVT_EFFECT_NOOP,
		.bcpc = TC_EVT_EFFECT_TOGGLE,
		.bcpb = TC_EVT_EFFECT_TOGGLE,
		
		.aswtrg = TC_EVT_EFFECT_NOOP,
		.aeevt = TC_EVT_EFFECT_NOOP,
		.acpc = TC_EVT_EFFECT_TOGGLE,
		.acpa = TC_EVT_EFFECT_TOGGLE,
	
		.wavsel = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
		
		.enetrg = false,
		.eevt = TC_EXT_EVENT_SEL_XC0_OUTPUT,
		.eevtedg = TC_SEL_NO_EDGE,
		.cpcdis = false,
		.cpcstop = false,
		
		.burst = TC_BURST_NOT_GATED,
		.clki = TC_CLOCK_RISING_EDGE,
		.tcclks = TC_CLOCK_SOURCE_TC3,
	};
	
	tc_waveform_opt_t waveform_opt_2 = {
		.channel = 2,
		
		.bswtrg = TC_EVT_EFFECT_NOOP,
		.beevt = TC_EVT_EFFECT_NOOP,
		.bcpc = TC_EVT_EFFECT_TOGGLE,
		.bcpb = TC_EVT_EFFECT_TOGGLE,
		
		.aswtrg = TC_EVT_EFFECT_NOOP,
		.aeevt = TC_EVT_EFFECT_NOOP,
		.acpc = TC_EVT_EFFECT_TOGGLE,
		.acpa = TC_EVT_EFFECT_TOGGLE,
		
		.wavsel = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
		
		.enetrg = false,
		.eevt = TC_EXT_EVENT_SEL_XC0_OUTPUT,
		.eevtedg = TC_SEL_NO_EDGE,
		.cpcdis = false,
		.cpcstop = false,
		
		.burst = TC_BURST_NOT_GATED,
		.clki = TC_CLOCK_RISING_EDGE,
		.tcclks = TC_CLOCK_SOURCE_TC3,
	};
	
	tc_select_external_clock(TC_RED_TOP, CHAN_RED_TOP, CLK_RED_TOP);
	tc_select_external_clock(TC_GREEN_TOP, CHAN_GREEN_TOP, CLK_GREEN_TOP);
	tc_select_external_clock(TC_BLUE_TOP, CHAN_BLUE_TOP, CLK_BLUE_TOP);
	tc_select_external_clock(TC_RED_BOT, CHAN_RED_BOT, CLK_RED_BOT);
	tc_select_external_clock(TC_GREEN_BOT, CHAN_GREEN_BOT, CLK_GREEN_BOT);
	tc_select_external_clock(TC_BLUE_BOT, CHAN_BLUE_BOT, CLK_BLUE_BOT);
	
	tc_init_waveform(&AVR32_TC0, &waveform_opt_0);
	tc_init_waveform(&AVR32_TC0, &waveform_opt_1);
	tc_init_waveform(&AVR32_TC0, &waveform_opt_2);
	tc_init_waveform(&AVR32_TC1, &waveform_opt_0);
	tc_init_waveform(&AVR32_TC1, &waveform_opt_1);
	tc_init_waveform(&AVR32_TC1, &waveform_opt_2);
	
	tc_write_rc(TC_RED_TOP, CHAN_RED_TOP, 0xFFFF);
	tc_write_rc(TC_GREEN_TOP, CHAN_GREEN_TOP, 0xFFFF);
	tc_write_rc(TC_BLUE_TOP, CHAN_BLUE_TOP, 0xFFFF);
	tc_write_rc(TC_RED_BOT, CHAN_RED_BOT, 0xFFFF);
	tc_write_rc(TC_GREEN_BOT, CHAN_GREEN_BOT, 0xFFFF);
	tc_write_rc(TC_BLUE_BOT, CHAN_BLUE_BOT, 0xFFFF);
	
	task_adc_init();
	
	return;
}

//! Set the RGB values of the LEDs
/*!
	\param red_top the duty cycle of the top red LED scaled by 0xFFFF
	\param green_top the duty cycle of the top green LED scaled by 0xFFFF
	\param blue_top the duty cycle of the top blue LED scaled by 0xFFFF
	\param red_bot the duty cycle of the bottom red LED scaled by 0xFFFF
	\param green_bot the duty cycle of the bottom green LED scaled by 0xFFFF
	\param blue_bot the duty cycle of the bottom blue LED scaled by 0xFFFF
	\sa task_leds_set_RGB_hex()
*/
void task_leds_set_RGB(uint16_t red_top, uint16_t green_top, uint16_t blue_top, uint16_t red_bot, uint16_t green_bot, uint16_t blue_bot) {
	
	tc_write_rb(TC_RED_TOP, CHAN_RED_TOP, 0xFFFF-red_top);
	tc_write_rb(TC_GREEN_TOP, CHAN_GREEN_TOP, 0xFFFF-green_top);
	tc_write_rb(TC_BLUE_TOP, CHAN_BLUE_TOP, 0xFFFF-blue_top);
	
	tc_write_ra(TC_RED_BOT, CHAN_RED_BOT, 0xFFFF-red_bot);
	tc_write_ra(TC_GREEN_BOT, CHAN_GREEN_BOT, 0xFFFF-green_bot);
	tc_write_ra(TC_BLUE_BOT, CHAN_BLUE_BOT, 0xFFFF-blue_bot);
	
	return;
}

//! Set the RGB values of the LEDs based on a (HTML-esque) hex code e.g. FFFF00 = yellow
/*!
	\param hex_top the RGB hex value for the top LEDs
	\param hex_bot the RGB hex value for the bottom LEDs
	\sa task_leds_set_RGB()
*/
void task_leds_set_RGB_hex(uint32_t hex_top, uint32_t hex_bot) {
	uint16_t red_top = ((hex_top & 0x00FF0000) >> 16) * 0x101;
	uint16_t blue_top = ((hex_top & 0x0000FF00) >> 8) * 0x101;
	uint16_t green_top = (hex_top & 0x000000FF) * 0x101;
	uint16_t red_bot = ((hex_bot & 0x00FF0000) >> 16) * 0x101;
	uint16_t blue_bot = ((hex_bot & 0x0000FF00) >> 8) * 0x101;
	uint16_t green_bot = (hex_bot & 0x000000FF) * 0x101;
	
	task_leds_set_RGB(red_top, green_top, blue_top, red_bot, green_bot, blue_bot);
	
	return;
}

//! Starts the LED PWM
void task_leds_start(void) {
	tc_start(TC_RED_TOP, CHAN_RED_TOP);
	tc_start(TC_GREEN_TOP, CHAN_GREEN_TOP);
	tc_start(TC_BLUE_TOP, CHAN_BLUE_TOP);
	tc_start(TC_RED_BOT, CHAN_RED_BOT);
	tc_start(TC_GREEN_BOT, CHAN_GREEN_BOT);
	tc_start(TC_BLUE_BOT, CHAN_BLUE_BOT);
	
	//Each led is pi/3 out of phase
	red_top_index = 0;
	green_top_index = SINE_LUT_SIZE / 6;
	blue_top_index = 2 * (SINE_LUT_SIZE / 6);
	red_bottom_index = 3 * (SINE_LUT_SIZE / 6);
	green_bottom_index = 4 * (SINE_LUT_SIZE / 6);
	blue_bottom_index = 5 * (SINE_LUT_SIZE / 6);
	
	task_adc_enable();
	
	return;
}

//! Updates the LEDs with audio average * LFO
void task_leds_update(void) {
	
	uint32_t adc_val = task_adc_update();
	
	task_leds_set_RGB(	adc_val*sine_lookup[red_top_index], 
						adc_val*sine_lookup[green_top_index],
						adc_val*sine_lookup[blue_top_index],
						adc_val*sine_lookup[red_bottom_index],
						adc_val*sine_lookup[green_bottom_index],
						adc_val*sine_lookup[blue_bottom_index]);
	
	INCR_LUT(red_top_index);
	INCR_LUT(green_top_index);
	INCR_LUT(blue_top_index);
	INCR_LUT(red_bottom_index);
	INCR_LUT(green_bottom_index);
	INCR_LUT(blue_bottom_index);
	
	return;
}