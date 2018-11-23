/*
 * adc.c
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

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_adc.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/usb.h"
#include "driverlib/udma.h"

#include "adc.h"
#include "usb_device.h"
#include "debug.h"


// channel: pin
// ADC_CTL_CH7: PD0
// ADC_CTL_CH6: PD1
// ADC_CTL_CH5: PD2
// ADC_CTL_CH4: PD3


tADC adc;


void
ADCxIntHandler(tADCx *self)
{
    ADCIntClear(self->adc_base, 0);

    self->logger_ready = true;
}


void
ADC0IntHandler(void)
{
    ADCxIntHandler(&adc.adc0);
}


void
ADC1IntHandler(void)
{
    ADCxIntHandler(&adc.adc1);
}


void
ADCxLoggerStart(tADCx* self)
{
    ADCSequenceConfigure(self->adc_base, 0, ADC_TRIGGER_TIMER, 0);
    ADCSequenceStepConfigure(self->adc_base, 0, 0, self->adc_channel | ADC_CTL_IE | ADC_CTL_END);

    ADCSequenceEnable(self->adc_base, 0);
    ADCIntEnable(self->adc_base, 0); // Enable to generate direct ADC Interrupts, do not enable for DMA
    IntEnable(self->adc_int); // Enable Interrupt in NVIC
}


uint32_t
ADCLoggerStart(uint32_t interval)
{
    uint32_t load_value;

    if (adc.lock)
        return 1;

    adc.lock = true;

    ADCxLoggerStart(&adc.adc0);
    ADCxLoggerStart(&adc.adc1);

    load_value = interval * (SysCtlClockGet() / 1000); // ms
    TimerLoadSet(TIMER1_BASE, TIMER_A, load_value);

    ADCLoggerRelease();

    TimerEnable(TIMER1_BASE, TIMER_A);

    return 0;
}

bool
ADCLoggerReady()
{
    return adc.adc0.logger_ready && adc.adc1.logger_ready;
}


void
ADCxLoggerGet(tADCx* self, uint32_t *value)
{
    ADCSequenceDataGet(self->adc_base, 0, value);
}


void
ADCLoggerGet(uint32_t *channel0, uint32_t *channel1)
{
    ADCxLoggerGet(&adc.adc0, channel0);
    ADCxLoggerGet(&adc.adc1, channel1);
}


void
ADCLoggerRelease()
{
    adc.adc0.logger_ready = false;
    adc.adc1.logger_ready = false;
}


void
ADCxLoggerStop(tADCx *self)
{
    ADCSequenceDisable(self->adc_base, 0);
    ADCIntDisable(self->adc_base, 0);
    IntDisable(self->adc_int);
}


void
ADCLoggerStop()
{
    TimerDisable(TIMER1_BASE, TIMER_A);

    ADCxLoggerStop(&adc.adc0);
    ADCxLoggerStop(&adc.adc1);

    adc.lock = false;
}


void
ConfigureADCx(tADCx* self)
{
    //
    // Configure Pins
    //
    GPIOPinTypeADC(self->gpio_base, self->gpio_pin);

    ADCPhaseDelaySet(self->adc_base, ADC_PHASE_0);

    ADCHardwareOversampleConfigure(self->adc_base, 1);
}

void
ADCInit()
{
    adc.lock = false;

    //adc.ready = 0;
    //adc.error = 0;

    adc.adc0.adc_base = ADC0_BASE;
    adc.adc0.adc_int = INT_ADC0SS0;
    adc.adc0.adc_channel = ADC_CTL_CH7;
    adc.adc0.gpio_base = GPIO_PORTD_BASE;
    adc.adc0.gpio_pin = GPIO_PIN_0;
    adc.adc0.udma_channel = UDMA_CHANNEL_ADC0;

    adc.adc1.adc_base = ADC1_BASE;
    adc.adc1.adc_int = INT_ADC1SS0;
    adc.adc1.adc_channel = ADC_CTL_CH6;
    adc.adc1.gpio_base = GPIO_PORTD_BASE;
    adc.adc1.gpio_pin = GPIO_PIN_1;
    adc.adc1.udma_channel = 24; // UDMA_CHANNEL_ADC1 enthält einen falschen Wert?!

    uDMAChannelAssign(UDMA_CH24_ADC1_0);

    ConfigureADCx(&adc.adc0);
    ConfigureADCx(&adc.adc1);

    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerControlTrigger(TIMER1_BASE, TIMER_A, true);
}
