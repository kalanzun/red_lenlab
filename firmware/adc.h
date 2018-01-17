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


#include "ring.h"


#define ADC_SAMPLES 500


typedef struct ADCx {
    uint32_t adc_base;
    uint32_t adc_int;
    uint32_t adc_channel;
    uint32_t gpio_base;
    uint32_t gpio_pin;
    uint32_t udma_channel;

    volatile bool ping_ready;
    volatile bool pong_ready;
    volatile bool single;

    tPage *ping;
    tPage *pong;
} tADCx;


typedef struct ADC {
    volatile bool pingpong;
    volatile bool ready;
    volatile bool enable;
    volatile bool prepare;
    volatile bool error;

    tADCx adc0;
    tADCx adc1;

    tRing ring;
} tADC;


void ADCSetDivider(uint8_t divider);

void ADCStart();
void ADCStop();
bool ADCReady();
uint16_t *ADCGetBuffer(bool channel);
void ADCRelease();

void ADCSingle();
tRing *ADCGetRing();

void ADCInit();

#endif /* ADC_H_ */
