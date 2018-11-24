/*
 * oscilloscope.c
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

#include "oscilloscope.h"

#include "logger.h"
#include "reply_handler.h"
#include "usb_device.h"


#define MEMORY_LENGTH 22


tPage memory[MEMORY_LENGTH];


void
ADC0SS0Handler(void)
{
    tOscSeq *self = &oscilloscope.seq_group.osc_seq[0];

    OscSeqIntHandler(self);
}


void
ADC1SS0Handler(void)
{
    tOscSeq *self = &oscilloscope.seq_group.osc_seq[1];

    OscSeqIntHandler(self);
}


tError
OscilloscopeStart(tOscilloscope *self, uint32_t samplerate)
{
    if (self->lock) return LOCK_ERROR;

    if (logger.lock) return STATE_ERROR;

    // 2 rings of 10 pages each
    OscSeqGroupAllocate(&self->seq_group, memory, 10);

    OscSeqGroupEnable(&self->seq_group);

    self->lock = 1;

    return OK;
}


tError
OscilloscopeStop(tOscilloscope *self)
{
    if (!self->lock) return LOCK_ERROR;

    self->lock = 0;

    return OK;
}


void
OscilloscopeMain(tOscilloscope *self)
{
    int i;
    tRing *ring;
    tPage *page;
    tRingIter iter;

    if (!self->lock) return;

    if (OscSeqGroupReady(&self->seq_group)) {

        //DEBUG_PRINT("OscilloscopeData\n");

        FOREACH_ADC {

            ring = &self->seq_group.osc_seq[i].ring;

            for (RingIterInit(&iter, ring); iter.content; RingIterNext(&iter))
            {
                page = RingIterGet(&iter);

                page->buffer[0] = OscilloscopeData; // reply
                page->buffer[1] = ShortArray; // type
                page->buffer[2] = i; // channel
                page->buffer[3] = 0; // last
            }

        }

        page->buffer[3] = 255; // last packet

        USBDeviceSendInterleaved(&self->seq_group.osc_seq[0].ring, &self->seq_group.osc_seq[1].ring);
        OscilloscopeStop(self);

    }
}


void
OscilloscopeInit(tOscilloscope *self, tADCGroup *adc_group)
{
    self->adc_group = adc_group;

    OscSeqGroupInit(&self->seq_group, adc_group);

    self->lock = 0;
}
