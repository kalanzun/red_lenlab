/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ADC_H_
#define ADC_H_


#include "inc/hw_adc.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/udma.h"

#include "adc_timer.h"


#define ADC_GROUP_SIZE 2
#define FOREACH_ADC for (i = 0; i < ADC_GROUP_SIZE; ++i)


typedef struct ADC {

    uint32_t base;
    uint32_t channel;
    uint32_t channel1;
    uint32_t gpio_base;
    uint32_t gpio_pin;

} tADC;


typedef struct ADCGroup {

    tADC adc[ADC_GROUP_SIZE];

    tADCTimer timer;

    unsigned char lock;

} tADCGroup;


extern tADCGroup adc_group;


inline void
ADCGroupLock(tADCGroup *self)
{
    self->lock = 1;
}


inline void
ADCGroupUnlock(tADCGroup *self)
{
    self->lock = 0;
}


inline void
ADCGroupSetHardwareOversample(tADCGroup *self, uint32_t samplerate)
{
    int i;

    FOREACH_ADC ADCHardwareOversampleConfigure(self->adc[i].base, 1<<samplerate);
}


inline void
ConfigureADC(tADC *self)
{
    GPIOPinTypeADC(self->gpio_base, self->gpio_pin);

    ADCPhaseDelaySet(self->base, ADC_PHASE_0);
    ADCHardwareOversampleConfigure(self->base, 1);
}


inline void
ConfigureADC0(tADC *self)
{
    self->base = ADC0_BASE;
    self->channel = ADC_CTL_CH7; // Pin PD0
    self->channel1 = ADC_CTL_CH5; // Pin PD2
    self->gpio_base = GPIO_PORTD_BASE;
    self->gpio_pin = GPIO_PIN_0;

    ConfigureADC(self);
}


inline void
ConfigureADC1(tADC *self)
{
    self->base = ADC1_BASE;
    self->channel = ADC_CTL_CH6; // Pin PD1
    self->channel1 = ADC_CTL_CH4; // Pin PD3
    self->gpio_base = GPIO_PORTD_BASE;
    self->gpio_pin = GPIO_PIN_1;

    uDMAChannelAssign(UDMA_CH24_ADC1_0);

    ConfigureADC(self);
}


inline void
ADCGroupInit(tADCGroup *self)
{
    ConfigureADC0(&self->adc[0]);
    ConfigureADC1(&self->adc[1]);

    ADCTimerInit(&self->timer);

    self->lock = 0;
}


#endif /* ADC_H_ */
