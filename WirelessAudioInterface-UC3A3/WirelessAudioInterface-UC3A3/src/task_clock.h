/*
 * task_clock.h
 *
 * Created: 4/3/2013 8:44:49 PM
 *  Author: Eva
 */ 


#ifndef TASK_CLOCK_H_
#define TASK_CLOCK_H_

#define PIN_GCLK0	AVR32_PM_GCLK_0_2_PIN
#define FUNC_GCLK0	AVR32_PM_GCLK_0_2_FUNCTION
#define PIN_GCLK1	AVR32_PM_GCLK_1_1_PIN
#define FUNC_GCLK1	AVR32_PM_GCLK_1_1_FUNCTION
#define PIN_GCLK2	AVR32_PM_GCLK_2_PIN
#define FUNC_GCLK2	AVR32_PM_GCLK_2_FUNCTION
#define PIN_GCLK3	AVR32_PM_GCLK_3_PIN
#define FUNC_GCLK3	AVR32_PM_GCLK_3_FUNCTION

#define MCLK_MULT	256		//MCLK = MCLK_MULT * FS
#define BCLK_MULT	64		//BCLK = BCLK_MULT * FS	(2 x 32-bit words)

extern void task_clock_init(void);
extern void task_clock_change_bclk_wclk(uint32_t fs);

#endif /* TASK_CLOCK_H_ */