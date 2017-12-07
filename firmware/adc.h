/*
 * adc.h
 *

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef ADC_H_
#define ADC_H_


#include "adc_queue.h"


typedef struct ADC {
    volatile bool pingpong;
    volatile bool basic;
    volatile bool ping_ready;
    volatile bool pong_ready;
    uint16_t ping[ADC_PAYLOAD_LENGTH];
    uint16_t pong[ADC_PAYLOAD_LENGTH];
} tADC;


extern tADC adc;


void ADCStart(void);
void ADCStop(void);

void ADCStartSingle(uint32_t time);

void ADCMain(void);

void ADCInit(void);


#endif /* ADC_H_ */
