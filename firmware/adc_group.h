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

#ifndef ADC_GROUP_H_
#define ADC_GROUP_H_


#include "inc/hw_adc.h"
#include "inc/hw_ints.h"

#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/udma.h"

#include "adc_timer.h"


#define GROUP_SIZE 2

#define FOREACH(element, array) for (element = array; element != array + GROUP_SIZE; ++element)


struct ADC {
    uint32_t base;
    uint32_t channel;
    uint32_t channel_1;
    uint32_t gpio_base;
    uint32_t gpio_pin;
    uint32_t gpio_pin_1;
    uint32_t udma_channel;
};


struct ADCGroup {
    struct ADC adc[GROUP_SIZE];
};


static const struct ADCGroup adc_group = {
    .adc = {
        {
            .base = ADC0_BASE,
            .channel = ADC_CTL_CH7, // Pin PD0
            .channel_1 = ADC_CTL_CH5, // Pin PD2
            .gpio_base = GPIO_PORTD_BASE,
            .gpio_pin = GPIO_PIN_0,
            .gpio_pin_1 = GPIO_PIN_2,
            .udma_channel = UDMA_CHANNEL_ADC0
        },
        {
            .base = ADC1_BASE,
            .channel = ADC_CTL_CH6, // Pin PD1
            .channel_1 = ADC_CTL_CH4, // Pin PD3
            .gpio_base = GPIO_PORTD_BASE,
            .gpio_pin = GPIO_PIN_1,
            .gpio_pin_1 = GPIO_PIN_3,
            .udma_channel = 24 // UDMA_CHANNEL_ADC1 is wrong?!
        }
    },
};


#define FOREACH_ADC(element) FOREACH(element, adc_group.adc)


inline void
ADCGroupSetHardwareOversample(uint32_t log2oversamples)
{
    const struct ADC *adc;

    // samplerate is 1 MHz / (2**log2oversamples)
    uint32_t factor = log2oversamples > 0 ? 1 << log2oversamples : 0;
    // a factor of 0 disables oversampling

    FOREACH_ADC(adc) ADCHardwareOversampleConfigure(adc->base, factor);
}


inline void
ADCGroupInit(void)
{
    const struct ADC *adc;

    FOREACH_ADC(adc) {
        GPIOPinTypeADC(adc->gpio_base, adc->gpio_pin);
        GPIOPinTypeADC(adc->gpio_base, adc->gpio_pin_1);

        ADCPhaseDelaySet(adc->base, ADC_PHASE_0);
        ADCHardwareOversampleConfigure(adc->base, 0);
    }

    uDMAChannelAssign(UDMA_CH24_ADC1_0);

    // ADCTimer
    ADCTimerInit();
}


#endif /* ADC_GROUP_H_ */
