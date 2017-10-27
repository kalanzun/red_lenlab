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
    tADCQueue adc_queue;
} tADC;


// Singleton
extern tADC adc;


void ADCStart(void);

void ADCMain(void);

void ADCInit(void);


#endif /* ADC_H_ */
