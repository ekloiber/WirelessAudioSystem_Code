/*
 * task_ADC.h
 *
 * Created: 4/4/2013 9:12:10 PM
 *  Author: Eva
 */ 


#ifndef TASK_ADC_H_
#define TASK_ADC_H_

#define PIN_AUDIO			AVR32_ADC_AD_7_PIN
#define FUNC_AUDIO			AVR32_ADC_AD_7_FUNCTION
#define ADC_CHAN_AUDIO		AVR32_ADC_CH7
#define ADC_OFFSET_AUDIO	AVR32_ADC_CH7_OFFSET
#define ADC_MASK_AUDIO		AVR32_ADC_CH7_MASK
#define ADC_SIZE_AUDIO		AVR32_ADC_CH7_SIZE

#define NUM_AVERAGE_SAMPLES		10

static volatile struct {
	uint32_t buffer[NUM_AVERAGE_SAMPLES];
	uint32_t buffer_count;
	uint32_t latest_val;
	uint32_t average;
} adc_data;

extern void task_adc_init(void);
extern void task_adc_clear(void);
extern void task_adc_enable(void);
extern void task_adc_disable(void);
extern uint32_t task_adc_get_abs_val(void);
extern uint32_t task_adc_update(void);

#endif /* TASK_ADC_H_ */