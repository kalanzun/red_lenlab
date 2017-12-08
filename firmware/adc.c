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


tADC adc0;
tADC adc1;

/*
inline void
StartPingPongADCuDMAChannel(uint32_t ui32ChannelStructIndex)
{
    ASSERT(!ADCQueueFull(&adc.adc_queue)); // TODO Error Handling

    uDMAChannelTransferSet(ui32ChannelStructIndex,
            UDMA_MODE_PINGPONG,
            (void *)(ADC0_BASE + ADC_O_SSFIFO0),
            ADCQueueAcquire(&adc.adc_queue)->payload, ADC_PAYLOAD_LENGTH);

}
*/
/*
inline void
StartBasicADCuDMAChannel(uint32_t ui32ChannelStructIndex, uint32_t time)
{
    tADCEvent *event;

    ASSERT(!ADCQueueFull(&adc.adc_queue)); // TODO Error Handling
    event = ADCQueueAcquire(&adc.adc_queue);

    *((uint32_t *) event->payload) = time;

    uDMAChannelTransferSet(ui32ChannelStructIndex,
            UDMA_MODE_BASIC,
            (void *)(ADC0_BASE + ADC_O_SSFIFO0),
            event->payload + 4, ADC_PAYLOAD_LENGTH - 4);
}
*/

//*****************************************************************************
//
// The interrupt handler for ADC0, sequence 0.  This interrupt will occur when
// a DMA transfer is complete using the first sequencer on ADC0.  It will also
// be triggered if the peripheral signals an error.
//
//*****************************************************************************
void
ADCIntHandler(tADC *self)
{
    //
    // Clear the ADC interrupt
    //
    ADCIntClear(self->adc_base, 0);

    if (self->pingpong)
    {
        if(!uDMAChannelSizeGet(self->udma_channel | UDMA_PRI_SELECT))
        {
            //DEBUG_PRINT("adc ping %d\n", self->count);
            //ASSERT(!self->pong_ready); // adc writes into this one at the moment
            //self->ping_ready = 1;
            if (self->enable) {
                ASSERT(!self->lock);
                self->read = 0;
                ADCLock(self);
            }
            uDMAChannelTransferSet(self->udma_channel | UDMA_PRI_SELECT,
                    UDMA_MODE_PINGPONG,
                    (void *)(self->adc_base + ADC_O_SSFIFO0),
                    self->ping, ADC_BUFFER_LENGTH);
            //ADCQueueWrite(&adc.adc_queue);
            //adc.pri_ready = 1;
            // Achtung: Beim Beenden leaken wir hier adc_queue.acquire
            //logger operation one shot DMA
            //StartPingPongADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT);
            // damit der interrupt das nächste Mal nach UDMA_ALT_SELECT springt muss das DMA hier neu konfiguriert werden, auch wenn dann nach ALT angehalten werden soll.
            //uDMAChannelEnable(UDMA_CHANNEL_ADC0); // TODO event. nicht nötig, bitte probieren
        }
        else if(!uDMAChannelSizeGet(self->udma_channel | UDMA_ALT_SELECT))
        {
            //DEBUG_PRINT("adc pong %d\n", self->count);
            //ASSERT(!self->ping_ready); // adc writes into this one at the moment
            //self->pong_ready = 1;
            if (self->enable) {
                ASSERT(!self->lock);
                self->read = 1;
                ADCLock(self);
            }
            uDMAChannelTransferSet(self->udma_channel | UDMA_ALT_SELECT,
                    UDMA_MODE_PINGPONG,
                    (void *)(self->adc_base + ADC_O_SSFIFO0),
                    self->pong, ADC_BUFFER_LENGTH);
            //if (self->count == 0) {
            //    ADCSequenceDisable(self->adc_base, 0);
            //}
            //ASSERT(!adc.pri_ready); // this one should be active now
            //ADCQueueWrite(&adc.adc_queue);
            //adc.alt_ready = 1;
            //logger operation one shot DMA
            //StartPingPongADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT);
            //uDMAChannelEnable(UDMA_CHANNEL_ADC0);
        }
        //else
        //{
            // normal ADC interrupt, not uDMA interrupt / what???
            // does happen if IntEnable is with ADCStart instead of ADCInit.
            //ASSERT(0);
        //}
    }
    else if (self->basic) // single shot
    {
        if(!uDMAChannelSizeGet(self->udma_channel | UDMA_PRI_SELECT))
        {
            //DEBUG_PRINT("SGN\n");
            //ADCQueueWrite(&adc.adc_queue);
            ADCSequenceDisable(self->adc_base, 0);
            self->basic = 0;
        }
        //else
        //{
            // normal ADC interrupt, not uDMA interrupt / what???
            // does happen if IntEnable is with ADCStart instead of ADCInit.
            //ASSERT(0);
        //}
    }
}

void
ADC0IntHandler(void)
{
    ADCIntHandler(&adc0);
}

void
ADC1IntHandler(void)
{
    ADCIntHandler(&adc1);
}



void
ADCStart(tADC *self)
{
    if (self->basic || self->pingpong)
        return;

    DEBUG_PRINT("ADCStart\n");

    self->pingpong = 1;

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
            self->ping, ADC_BUFFER_LENGTH);
    uDMAChannelTransferSet(self->udma_channel | UDMA_ALT_SELECT,
            UDMA_MODE_PINGPONG,
            (void *)(self->adc_base + ADC_O_SSFIFO0),
            self->pong, ADC_BUFFER_LENGTH);

    uDMAChannelEnable(self->udma_channel);
    ADCSequenceEnable(self->adc_base, 0);

    IntEnable(self->adc_int);
}

void
ADCStop(tADC *self)
{
    DEBUG_PRINT("ADCStop\n");

    if (self->pingpong) {
        // ADCStop cancels the current sequencer run, it will not deliver anymore data
        ADCSequenceDisable(self->adc_base, 0);
        self->pingpong = 0;
    }
}

void
ADCEnable(tADC *self)
{
    ADCUnlock(self);
    self->enable = 1;
}

void
ADCDisable(tADC *self)
{
    self->enable = 0;
}

void
ADCLock(tADC *self)
{
    self->lock = 1;
}

void
ADCUnlock(tADC *self)
{
    self->lock = 0;
}

uint16_t *
ADCGetBuffer(tADC *self)
{
    if (self->read)
        return self->pong;
    else
        return self->ping;
}

/*
void
ADCStartSingle(uint32_t time)
{
    if (adc.basic || adc.pingpong)
        return;

    adc.basic = 1;

    uDMAChannelAttributeDisable(UDMA_CHANNEL_ADC0,
        UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
        UDMA_ATTR_HIGH_PRIORITY |
        UDMA_ATTR_REQMASK);

    uDMAChannelControlSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
        UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
        UDMA_DST_INC_16 | UDMA_ARB_4);

    //StartBasicADCuDMAChannel(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT, time);

    uDMAChannelEnable(UDMA_CHANNEL_ADC0);
    ADCSequenceEnable(ADC0_BASE, 0);

    IntEnable(INT_ADC0SS0);
}
*/

inline void
ConfigureADC(void)
{
    //
    // Configure Pins
    //
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_1);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_2);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3);

    // Set the ADC Sequence to trigger always (that is 1 MHz)
    // and to generate an interrupt every 4 samples.
    // This is an arbitration size of 4 for the uDMA transfer
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_ALWAYS, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH7); // Pin PD0
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH6); // Pin PD1
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH7); // CH5 Pin PD2
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH6 | ADC_CTL_IE); // CH4 Pin PD3
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH6);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH6 | ADC_CTL_IE | ADC_CTL_END);

    /*  TODO: second ADC Module
    ROM_ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_ALWAYS, 0);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_CH2);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 1, ADC_CTL_CH2);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 2, ADC_CTL_CH2);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 3, ADC_CTL_CH2 | ADC_CTL_IE);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 4, ADC_CTL_CH2);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 5, ADC_CTL_CH2);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 6, ADC_CTL_CH2);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 7, ADC_CTL_CH2 | ADC_CTL_IE | ADC_CTL_END);
     */


    //
    // Enable the sequencer
    //
    ADCSequenceEnable(ADC0_BASE, 0);

    //ROM_ADCSequenceEnable(ADC1_BASE, 0);

    //
    // Enable sequencer interrupts
    //
    ADCIntEnable(ADC0_BASE, 0);
    //ROM_ADCIntEnable(ADC1_BASE, 0);
}


void
ADCInit(tADC *self)
{
    self->adc_base = ADC0_BASE;
    self->adc_int = INT_ADC0SS0;
    self->udma_channel = UDMA_CHANNEL_ADC0;
    self->pingpong = 0;
    self->basic = 0;
    self->ping_ready = 0;
    self->pong_ready = 0;
    self->enable = 0;
    self->lock = 0;
    self->read = 0;

    ConfigureADC();
}
