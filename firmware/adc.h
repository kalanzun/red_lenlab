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


#define ADC_BUFFER_LENGTH 1000


typedef struct ADC {
    uint32_t adc_base;
    uint32_t adc_int;
    uint32_t udma_channel;
    volatile bool pingpong;
    volatile bool basic;
    volatile bool ping_ready;
    volatile bool pong_ready;
    volatile bool enable;
    volatile bool lock;
    volatile bool read;
    uint16_t ping[ADC_BUFFER_LENGTH];
    uint16_t pong[ADC_BUFFER_LENGTH];
} tADC;


extern tADC adc0;
extern tADC adc1;


void ADCEnable(tADC *self);
void ADCDisable(tADC *self);

void ADCLock(tADC *self);
void ADCUnlock(tADC *self);

uint16_t *ADCGetBuffer();


void ADCStart(tADC *self);
void ADCStop(tADC *self);

void ADCInit(tADC *self);


#endif /* ADC_H_ */
