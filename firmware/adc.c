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

//#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
//#include "driverlib/rom.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_adc.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
//#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
//#include "driverlib/uart.h"
#include "driverlib/gpio.h"
//#include "driverlib/timer.h"
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
    //
    // Clear the ADC interrupt
    //
    ADCIntClear(self->adc_base, 0);

    //DEBUG_PRINT("x\n");

    self->single = true;
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

bool
ADCReady()
{
    return adc.ready;
}

void
ADCRelease()
{
    adc.ready = 0;
}

tRing *
ADCGetRing(bool channel)
{
    if (channel)
        return &adc.adc1.ring;
    else
        return &adc.adc0.ring;
}

void
StartADCxSingle(tADCx* self)
{
    ADCSequenceConfigure(self->adc_base, 0, ADC_TRIGGER_TIMER, 0);
    ADCSequenceStepConfigure(self->adc_base, 0, 0, self->adc_channel | ADC_CTL_IE | ADC_CTL_END);

    ADCSequenceEnable(self->adc_base, 0);
    ADCIntEnable(self->adc_base, 0); // Enable to generate direct ADC Interrupts, do not enable for DMA
    IntEnable(self->adc_int); // Enable Interrupt in NVIC
}

bool
ADCSingle()
{
    return adc.adc0.single && adc.adc1.single;
}

void
ADCSingleRelease()
{
    adc.adc0.single = false;
    adc.adc1.single = false;
}

void
ADCxSingleGet(tADCx* self, uint32_t *value)
{
    ADCSequenceDataGet(self->adc_base, 0, value);
}

void
ADCSingleGet(uint32_t *channel0, uint32_t *channel1)
{
    ADCxSingleGet(&adc.adc0, channel0);
    ADCxSingleGet(&adc.adc1, channel1);
}

void
ADCStartSingle(uint32_t interval)
{
    uint32_t load_value;

    StartADCxSingle(&adc.adc0);
    StartADCxSingle(&adc.adc1);

    load_value = interval * (SysCtlClockGet() / 1000); // ms
    TimerLoadSet(TIMER1_BASE, TIMER_A, load_value);

    ADCSingleRelease();

    DEBUG_PRINT("TimerEnable\n");
    TimerEnable(TIMER1_BASE, TIMER_A);

    //ADCProcessorTrigger(adc.adc0.adc_base, 0);
    //ADCProcessorTrigger(adc.adc1.adc_base, 0);
}

void
StopADCx(tADCx *self)
{
    uDMAChannelDisable(self->udma_channel); // this one cancels uDMA immediately.
    ADCSequenceDisable(self->adc_base, 0);
    //ADCIntDisable(self->adc_base, 0);
}

void
ADCStop()
{
    TimerDisable(TIMER1_BASE, TIMER_A);

    StopADCx(&adc.adc0);
    StopADCx(&adc.adc1);
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
    adc.ready = 0;
    adc.error = 0;

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

    //uDMAChannelAssign(UDMA_CH24_ADC1_0);

    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_A, 80);
    TimerControlTrigger(TIMER1_BASE, TIMER_A, true);

    ConfigureADCx(&adc.adc0);
    ConfigureADCx(&adc.adc1);
}
