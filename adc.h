/*
 * adc.h
 *
 *  Created on: 23.10.2017
 *      Author: Christoph
 */

#ifndef ADC_H_
#define ADC_H_


extern volatile uint32_t adc_interrupt_counter;

void ADCInit(void);

void ADCStart(void);


#endif /* ADC_H_ */
