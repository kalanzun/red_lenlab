/*
 * adc.h
 *
 *  Created on: 23.10.2017
 *      Author: Christoph
 */

#ifndef ADC_H_
#define ADC_H_


#include "adc_queue.h"


typedef struct ADC {
    volatile bool pingpong;
    volatile bool basic;
    tADCQueue adc_queue;
} tADC;


extern tADC adc;


void ADCStart(void);
void ADCStop(void);

void ADCStartSingle(uint32_t time);

void ADCMain(void);

void ADCInit(void);


#endif /* ADC_H_ */
