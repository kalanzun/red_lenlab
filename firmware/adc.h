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


//ADC0_BASE
#define ADC0_CHANNEL0 ADC_CTL_CH7 // Pin PD0
#define ADC0_CHANNEL1 ADC_CTL_CH5 // Pin PD2
#define ADC0_GPIO_BASE GPIO_PORTD_BASE
#define ADC0_GPIO_PIN0 GPIO_PIN_0
#define ADC0_GPIO_PIN1 GPIO_PIN_2


//ADC1_BASE
#define ADC1_CHANNEL0 ADC_CTL_CH6 // Pin PD1
#define ADC1_CHANNEL1 ADC_CTL_CH4 // Pin PD3
#define ADC1_GPIO_BASE GPIO_PORTD_BASE
#define ADC1_GPIO_PIN0 GPIO_PIN_1
#define ADC1_GPIO_PIN1 GPIO_PIN_3


inline void
ADCGroupSetHardwareOversample(uint32_t log2oversamples)
{
    // samplerate is 1 MHz / (2**log2oversamples)
    uint32_t factor = log2oversamples > 0 ? 1 << log2oversamples : 0;
    // a factor of 0 disables oversampling

    ADCHardwareOversampleConfigure(ADC0_BASE, factor);
    ADCHardwareOversampleConfigure(ADC1_BASE, factor);
}


inline void
ADCGroupInit(void)
{
    // ADC0
    GPIOPinTypeADC(ADC0_GPIO_BASE, ADC0_GPIO_PIN0);
    GPIOPinTypeADC(ADC0_GPIO_BASE, ADC0_GPIO_PIN1);

    ADCPhaseDelaySet(ADC0_BASE, ADC_PHASE_0);
    ADCHardwareOversampleConfigure(ADC0_BASE, 0);

    // ADC1
    GPIOPinTypeADC(ADC1_GPIO_BASE, ADC1_GPIO_PIN0);
    GPIOPinTypeADC(ADC1_GPIO_BASE, ADC1_GPIO_PIN1);

    ADCPhaseDelaySet(ADC1_BASE, ADC_PHASE_0);
    ADCHardwareOversampleConfigure(ADC1_BASE, 0);

    uDMAChannelAssign(UDMA_CH24_ADC1_0);

    // ADCTimer
    ADCTimerInit();
}


#endif /* ADC_H_ */
