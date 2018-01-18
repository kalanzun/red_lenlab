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
    tPage *page;

    //
    // Clear the ADC interrupt
    //
    ADCIntClear(self->adc_base, 0);

    //DEBUG_PRINT("x\n");

    // ping_ and pong_enable are required, because uDMAChannelSizeGet
    // does return 0 at the end when the PRI channel is about to be disabled
    if(self->ping_enable && !uDMAChannelSizeGet(self->udma_channel | UDMA_PRI_SELECT))
    {
        // PRI is done, ALT is working this moment
        RingWrite(&self->ring);

        //DEBUG_PRINT("%d %d %d\n", self->ring.full, self->ring.acquire, self->ring.release);

        if (RingFull(&self->ring))
        {
            if (self->single)
            {
                // done
                self->ping_enable = 0;
                //DEBUG_PRINT("ping disable\n");
            }
            else
            {
                // error
                DEBUG_PRINT("Timing error: The firmware is too slow reading ADC data\n");
                adc.error = 1;
            }
        }

        else {
            // continue
            //DEBUG_PRINT("pri %d, %d\n", self->ring.length, self->ring.acquire);
            page = RingAcquire(&self->ring);

            uDMAChannelTransferSet(self->udma_channel | UDMA_PRI_SELECT,
                UDMA_MODE_PINGPONG,
                (void *)(self->adc_base + ADC_O_SSFIFO0),
                page->buffer+24, ADC_SAMPLES);
        }
    }

    else if(self->pong_enable && !uDMAChannelSizeGet(self->udma_channel | UDMA_ALT_SELECT))
    {
        // ALT is done, PRI is working this moment
        RingWrite(&self->ring);

        if (RingFull(&self->ring))
        {
            if (self->single)
            {
                // done
                self->pong_enable = 0;

                //DEBUG_PRINT("single\n");

                // disable ADC
                TimerDisable(TIMER1_BASE, TIMER_A);
                uDMAChannelDisable(self->udma_channel); // this one cancels uDMA immediately.
                ADCSequenceDisable(self->adc_base, 0);
                ADCIntDisable(self->adc_base, 0);

                // flag ready
                if (!adc.adc0.pong_enable && !adc.adc1.pong_enable) adc.ready = 1;
            }
            else
            {
                // error
                DEBUG_PRINT("Timing error: The firmware is too slow reading ADC data\n");
                adc.error = 1;
            }
        }

        else {
            // continue
            //DEBUG_PRINT("alt %d, %d\n", self->ring.length, self->ring.acquire);
            page = RingAcquire(&self->ring);

            uDMAChannelTransferSet(self->udma_channel | UDMA_ALT_SELECT,
                UDMA_MODE_PINGPONG,
                (void *)(self->adc_base + ADC_O_SSFIFO0),
                page->buffer+24, ADC_SAMPLES);
        }
    }
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
StartADCx(tADCx *self, tPage *ping, tPage *pong, uint32_t samplerate)
{
    ADCHardwareOversampleConfigure(self->adc_base, (1<<samplerate));

    uDMAChannelAttributeDisable(self->udma_channel,
        UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
        UDMA_ATTR_HIGH_PRIORITY |
        UDMA_ATTR_REQMASK);

    uDMAChannelControlSet(self->udma_channel | UDMA_PRI_SELECT,
        UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
        UDMA_DST_INC_16 | UDMA_ARB_4);

    uDMAChannelControlSet(self->udma_channel | UDMA_ALT_SELECT,
        UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
        UDMA_DST_INC_16 | UDMA_ARB_4);

    uDMAChannelTransferSet(self->udma_channel | UDMA_PRI_SELECT,
        UDMA_MODE_PINGPONG,
        (void *)(self->adc_base + ADC_O_SSFIFO0),
        ping->buffer+24, ADC_SAMPLES);

    self->ping_enable = 1;

    uDMAChannelTransferSet(self->udma_channel | UDMA_ALT_SELECT,
        UDMA_MODE_PINGPONG,
        (void *)(self->adc_base + ADC_O_SSFIFO0),
        pong->buffer+24, ADC_SAMPLES);

    self->pong_enable = 1;

    ADCSequenceEnable(self->adc_base, 0);
    uDMAChannelEnable(self->udma_channel);
    IntEnable(self->adc_int);
}

void
ADCStart(uint32_t length, bool single, uint32_t samplerate)
{
    tPage *ping0;
    tPage *pong0;
    tPage *ping1;
    tPage *pong1;

    //DEBUG_PRINT("ADCStart\n");

    RingAllocate(&adc.adc0.ring, length);
    RingAllocate(&adc.adc1.ring, length);

    ping0 = RingAcquire(&adc.adc0.ring);
    pong0 = RingAcquire(&adc.adc0.ring);

    ping1 = RingAcquire(&adc.adc1.ring);
    pong1 = RingAcquire(&adc.adc1.ring);

    if (single)
    {
        adc.adc0.single = 1;
        adc.adc1.single = 1;
    }
    else
    {
        adc.adc0.single = 0;
        adc.adc1.single = 0;
    }

    // Both ADC shall run exactly in sync
    // - A common timer triggers both ADCs
    // - The timer is disabled before a new measurement starts
    // - The sequencer and interrupts are disabled, too.
    //   DMA does not start in sync if spurious interrupts are around

    StartADCx(&adc.adc0, ping0, pong0, samplerate);
    StartADCx(&adc.adc1, ping1, pong1, samplerate);

    //DEBUG_PRINT("TimerEnable\n");

    TimerEnable(TIMER1_BASE, TIMER_A);

    // Drop the first two pages
    // The ring buffer is empty again
    ping0 = RingRead(&adc.adc0.ring);
    pong0 = RingRead(&adc.adc0.ring);

    ping1 = RingRead(&adc.adc1.ring);
    pong1 = RingRead(&adc.adc1.ring);

    RingRelease(&adc.adc0.ring);
    RingRelease(&adc.adc0.ring);

    RingRelease(&adc.adc1.ring);
    RingRelease(&adc.adc1.ring);

    //DEBUG_PRINT("ADCStart() done %d %d\n", adc.adc0.ring.acquire, adc.adc0.ring.release);
}

inline void
ADCSingle(uint32_t length, uint32_t samplerate)
{
    ADCStart(length, 1, samplerate);
}

inline void
ADCPingPong(uint32_t samplerate)
{
    ADCStart(2, 0, samplerate);
}

void
StopADCx(tADCx *self)
{
    uDMAChannelDisable(self->udma_channel); // this one cancels uDMA immediately.
    ADCSequenceDisable(self->adc_base, 0);
    ADCIntDisable(self->adc_base, 0);
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

    // Set the ADC Sequence
    // to generate an interrupt every 4 samples.
    // This is an arbitration size of 4 for the uDMA transfer
    ADCSequenceConfigure(self->adc_base, 0, ADC_TRIGGER_TIMER, 0);
    ADCSequenceStepConfigure(self->adc_base, 0, 0, self->adc_channel);
    ADCSequenceStepConfigure(self->adc_base, 0, 1, self->adc_channel);
    ADCSequenceStepConfigure(self->adc_base, 0, 2, self->adc_channel);
    ADCSequenceStepConfigure(self->adc_base, 0, 3, self->adc_channel | ADC_CTL_IE);
    ADCSequenceStepConfigure(self->adc_base, 0, 4, self->adc_channel);
    ADCSequenceStepConfigure(self->adc_base, 0, 5, self->adc_channel);
    ADCSequenceStepConfigure(self->adc_base, 0, 6, self->adc_channel);
    ADCSequenceStepConfigure(self->adc_base, 0, 7, self->adc_channel | ADC_CTL_IE | ADC_CTL_END);

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

    uDMAChannelAssign(UDMA_CH24_ADC1_0);

    ConfigureADCx(&adc.adc0);
    ConfigureADCx(&adc.adc1);
}
