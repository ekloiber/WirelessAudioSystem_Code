/*
 * task_ADC.c
 *
 * Created: 4/4/2013 9:11:59 PM
 *  Author: Eva
 */ 

#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "adc.h"
#include "power_clocks_lib.h"
#include "task_ADC.h"

void task_adc_init(void) {
	
	static const gpio_map_t ADC_GPIO_MAP = {
		{PIN_AUDIO, FUNC_AUDIO}
	};
	
	gpio_enable_module(ADC_GPIO_MAP, sizeof(ADC_GPIO_MAP)/sizeof(ADC_GPIO_MAP[0]));
	
	//ADC configured for ADC clock / 16 
	AVR32_ADC.mr |= 0xF << AVR32_ADC_MR_PRESCAL_OFFSET;
	
	adc_configure(&AVR32_ADC);
	
	return;
}

void task_adc_clear(void) {
	uint32_t i;
	
	adc_data.average = 0;
	adc_data.buffer_count = 0;
	adc_data.latest_val = 0;
	
	for (i = 0; i < NUM_AVERAGE_SAMPLES; i++) {
		adc_data.buffer[i] = 0;
	}
	
	return;
}

void task_adc_enable(void) {
	task_adc_clear();
	
	adc_enable(&AVR32_ADC, ADC_CHAN_AUDIO);
	
	return;
}

void task_adc_disable(void) {
	task_adc_clear();
	
	adc_disable(&AVR32_ADC, ADC_CHAN_AUDIO);
	
	return;
}

//Returns the adc value on a scale of 0 to ADC_MAX_VALUE/2

uint32_t task_adc_get_abs_val(void) {
	uint32_t adc_val;
	adc_start(&AVR32_ADC);
	
	adc_val = adc_get_value(&AVR32_ADC, ADC_CHAN_AUDIO);
	
	if (adc_val < (ADC_MAX_VALUE / 2)) adc_val = (ADC_MAX_VALUE / 2) - adc_val;
	else adc_val = adc_val - (ADC_MAX_VALUE / 2);
	
	return  adc_val;
}

//Computes average of NUM_AVERAGE_SAMPLES samples

uint32_t task_adc_update(void) {
	uint32_t i;
	uint32_t sum = 0;
	
	adc_data.latest_val = task_adc_get_abs_val();
	
	adc_data.buffer[adc_data.buffer_count] = adc_data.latest_val;
	
	if (adc_data.buffer_count < (NUM_AVERAGE_SAMPLES - 1)) adc_data.buffer_count++;
	else adc_data.buffer_count = 0;
	
	for (i = 0; i < NUM_AVERAGE_SAMPLES; i++)
	{
		sum += adc_data.buffer[i];
	}
	
	adc_data.average = sum / NUM_AVERAGE_SAMPLES;
	
	return adc_data.average;
}

