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
//#include "driverlib/debug.h"
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

    if (self->single) {
        if(self->ping_ready && !uDMAChannelSizeGet(self->udma_channel | UDMA_PRI_SELECT))
        {
            DEBUG_PRINT("ADCInt PRI\n");
            // pri is done, alt is working right now
            RingWrite(&adc.ring);
            if (RingFull(&adc.ring)) {
                // done
                self->ping_ready = 0;
            }
            else {
                // continue
                page = RingAcquire(&adc.ring);

                uDMAChannelTransferSet(self->udma_channel | UDMA_PRI_SELECT,
                        UDMA_MODE_PINGPONG,
                        (void *)(self->adc_base + ADC_O_SSFIFO0),
                        page->buffer+24, ADC_SAMPLES);
            }
        }
        else if(self->pong_ready && !uDMAChannelSizeGet(self->udma_channel | UDMA_ALT_SELECT))
        {
            DEBUG_PRINT("ADCInt ALT\n");
            // alt is done, pri is working right now
            RingWrite(&adc.ring);
            if (RingFull(&adc.ring)) {
                // done
                self->pong_ready = 0;
                uDMAChannelDisable(self->udma_channel); // this one cancels uDMA immediately.
                self->single = 0;
                if (!adc.adc0.single && !adc.adc1.single) adc.ready = 1;
            }
            else {
                // continue
                page = RingAcquire(&adc.ring);

                uDMAChannelTransferSet(self->udma_channel | UDMA_ALT_SELECT,
                        UDMA_MODE_PINGPONG,
                        (void *)(self->adc_base + ADC_O_SSFIFO0),
                        page->buffer+24, ADC_SAMPLES);
            }
        }
    }

    /*
    if(!uDMAChannelSizeGet(self->udma_channel | UDMA_PRI_SELECT))
    {
        self->ping_ready = 1;
        if (self->pong_ready) {
            DEBUG_PRINT("Timing error: The firmware is too slow reading ADC data\n");
            adc.enable = 0;
            adc.error = 1;
        }
        uDMAChannelTransferSet(self->udma_channel | UDMA_PRI_SELECT,
                UDMA_MODE_PINGPONG,
                (void *)(self->adc_base + ADC_O_SSFIFO0),
                self->ping, ADC_BUFFER_LENGTH);

        if (adc.adc0.ping_ready && adc.adc1.ping_ready) {
            if (adc.enable) {
                adc.pingpong = 0;
                adc.ready = 1;
            }
            else {
                adc.adc0.ping_ready = 0;
                adc.adc1.ping_ready = 0;
                if (adc.prepare) {
                    adc.enable = 1;
                    adc.prepare = 0;
                }
            }
        }
    }
    else if(!uDMAChannelSizeGet(self->udma_channel | UDMA_ALT_SELECT))
    {
        self->pong_ready = 1;
        if (self->ping_ready) {
            DEBUG_PRINT("Timing error: The firmware is too slow reading ADC data\n");
            adc.enable = 0;
            adc.error = 1;
        }
        uDMAChannelTransferSet(self->udma_channel | UDMA_ALT_SELECT,
                UDMA_MODE_PINGPONG,
                (void *)(self->adc_base + ADC_O_SSFIFO0),
                self->pong, ADC_BUFFER_LENGTH);

        if (adc.adc0.pong_ready && adc.adc1.pong_ready) {
            if (adc.enable) {
                adc.pingpong = 1;
                adc.ready = 1;
            }
            else {
                adc.adc0.pong_ready = 0;
                adc.adc1.pong_ready = 0;
                if (adc.prepare) {
                    adc.enable = 1;
                    adc.prepare = 0;
                }
            }
        }
    }
    else
    {
        // normal ADC interrupt, not uDMA interrupt / what???
        // does happen if IntEnable is with ADCStart instead of ADCInit.
        //ASSERT(0);
    }
    */
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
ADCEnable()
{
    if (!adc.prepare && !adc.enable && !adc.error)
        adc.prepare = 1;
}

void
ADCDisable()
{
    adc.enable = 0;
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
    if (adc.pingpong) {
        adc.adc0.pong_ready = 0;
        adc.adc1.pong_ready = 0;
    }
    else {
        adc.adc0.ping_ready = 0;
        adc.adc1.ping_ready = 0;
    }
}

uint16_t *
ADCGetBuffer(bool channel)
{
    return 0;
    /*
    if (adc.pingpong) {
        if (channel)
            return adc.adc1.pong;
        else
            return adc.adc0.pong;
    }
    else {
        if (channel)
            return adc.adc1.ping;
        else
            return adc.adc0.ping;
    }
    */
}

tRing *
ADCGetRing()
{
    return &adc.ring;
}

void
ADCSetDivider(uint8_t divider)
{
    ADCHardwareOversampleConfigure(adc.adc0.adc_base, 1 << divider);
    ADCHardwareOversampleConfigure(adc.adc1.adc_base, 1 << divider);
}

void
ConfigureADCx(tADCx* self)
{
    //
    // Configure Pins
    //
    GPIOPinTypeADC(self->gpio_base, self->gpio_pin);

    ADCHardwareOversampleConfigure(self->adc_base, 4); // 1 is too fast for the oscilloscope module

    // Set the ADC Sequence to trigger always (that is 1 MHz)
    // and to generate an interrupt every 4 samples.
    // This is an arbitration size of 4 for the uDMA transfer
    ADCSequenceConfigure(self->adc_base, 0, ADC_TRIGGER_ALWAYS, 0);
    ADCSequenceStepConfigure(self->adc_base, 0, 0, self->adc_channel);
    ADCSequenceStepConfigure(self->adc_base, 0, 1, self->adc_channel);
    ADCSequenceStepConfigure(self->adc_base, 0, 2, self->adc_channel);
    ADCSequenceStepConfigure(self->adc_base, 0, 3, self->adc_channel | ADC_CTL_IE);
    ADCSequenceStepConfigure(self->adc_base, 0, 4, self->adc_channel);
    ADCSequenceStepConfigure(self->adc_base, 0, 5, self->adc_channel);
    ADCSequenceStepConfigure(self->adc_base, 0, 6, self->adc_channel);
    ADCSequenceStepConfigure(self->adc_base, 0, 7, self->adc_channel | ADC_CTL_IE | ADC_CTL_END);

    //
    // Enable the sequencer
    //
    ADCSequenceEnable(self->adc_base, 0);

    //
    // Enable sequencer interrupts
    //
    ADCIntEnable(self->adc_base, 0);
}

void
StartADCx(tADCx* self, tPage* ping, tPage* pong)
{
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

    uDMAChannelTransferSet(self->udma_channel | UDMA_ALT_SELECT,
            UDMA_MODE_PINGPONG,
            (void *)(self->adc_base + ADC_O_SSFIFO0),
            pong->buffer+24, ADC_SAMPLES);

    uDMAChannelEnable(self->udma_channel);

    IntEnable(self->adc_int);
}

void
ADCInit()
{
    adc.pingpong = 0;
    adc.ready = 0;
    adc.enable = 0;
    adc.prepare = 0;
    adc.error = 0;

    adc.adc0.adc_base = ADC0_BASE;
    adc.adc0.adc_int = INT_ADC0SS0;
    adc.adc0.adc_channel = ADC_CTL_CH7;
    adc.adc0.gpio_base = GPIO_PORTD_BASE;
    adc.adc0.gpio_pin = GPIO_PIN_0;
    adc.adc0.udma_channel = UDMA_CHANNEL_ADC0;

    adc.adc0.ping_ready = 0;
    adc.adc0.pong_ready = 0;

    adc.adc1.adc_base = ADC1_BASE;
    adc.adc1.adc_int = INT_ADC1SS0;
    adc.adc1.adc_channel = ADC_CTL_CH6;
    adc.adc1.gpio_base = GPIO_PORTD_BASE;
    adc.adc1.gpio_pin = GPIO_PIN_1;
    adc.adc1.udma_channel = 24; // UDMA_CHANNEL_ADC1 enthält einen falschen Wert?!

    uDMAChannelAssign(UDMA_CH24_ADC1_0);

    adc.adc1.ping_ready = 0;
    adc.adc1.pong_ready = 0;

    ConfigureADCx(&adc.adc0);
    ConfigureADCx(&adc.adc1);

    //StartADCx(&adc.adc0);
    //StartADCx(&adc.adc1);
}

void
ADCSingle()
{
    tPage *ping0;
    tPage *pong0;
    tPage *ping1;
    tPage *pong1;

    ADCInit();

    RingAllocate(&adc.ring, MEMORY_LENGTH);

    ping0 = RingAcquire(&adc.ring);
    ping1 = RingAcquire(&adc.ring);

    pong0 = RingAcquire(&adc.ring);
    pong1 = RingAcquire(&adc.ring);

    adc.adc0.single = 1;
    adc.adc1.single = 1;

    adc.adc0.ping_ready = 1;
    adc.adc0.pong_ready = 1;

    adc.adc1.ping_ready = 1;
    adc.adc1.pong_ready = 1;

    StartADCx(&adc.adc0, ping0, pong0);
    StartADCx(&adc.adc1, ping1, pong1);

}
