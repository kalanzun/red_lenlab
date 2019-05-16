/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2019 Christoph Simon and the Lenlab developer team
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

#include <stdbool.h>
#include <stdint.h>

#include "oscilloscope.h"

#include "driverlib/debug.h"

#include "lenlab_protocol.h"
#include "reply_handler.h"


void
ADC0SS0Handler(void)
{
    tOscSeq *self = &osc_seq_group.osc_seq[0];

    OscSeqIntHandler(self);
}


void
ADC1SS0Handler(void)
{
    tOscSeq *self = &osc_seq_group.osc_seq[1];

    OscSeqIntHandler(self);
}


tError
OscilloscopeStart(tOscilloscope *self, uint32_t samplerate)
{
    if (self->lock) return LOCK_ERROR;

    if (adc_group.lock) return ADC_ERROR;

    if (memory.lock) return MEMORY_ERROR;

    self->lock = 1;

    ADCGroupLock(&adc_group);

    ADCGroupSetHardwareOversample(&adc_group, samplerate);

    // 2 rings of 10 pages each (osc_seq want's an even number)
    OscSeqGroupAllocate(&osc_seq_group, 10);

    OscSeqGroupEnable(&osc_seq_group);

    return OK;
}


tError
OscilloscopeLinearTestData(tOscilloscope *self)
{
    unsigned int i, j, k;
    tRing *ring;
    tPage *page;
    uint16_t *short_buffer;

    if (self->lock) return LOCK_ERROR;

    if (adc_group.lock) return ADC_ERROR;

    if (memory.lock) return MEMORY_ERROR;

    self->lock = 1;

    ADCGroupLock(&adc_group);

    // 2 rings of 10 pages each (osc_seq want's an even number)
    OscSeqGroupAllocate(&osc_seq_group, 10);

    // write linear test data into the DMA buffer
    FOREACH_ADC {
        ring = &osc_seq_group.osc_seq[i].ring;
        j = 0;
        while (!RingFull(ring)) {
            page = RingAcquire(ring);
            short_buffer = (uint16_t *) &page->buffer;

            for (k = 0; k < OSCILLOSCOPE_SAMPLES; k++) {
                short_buffer[2 * OSCILLOSCOPE_OFFSET + k] = k + (j * OSCILLOSCOPE_SAMPLES);
            }

            RingWrite(ring);
            j++;
        }
    }

    return OK;
}


tError
OscilloscopeStop(tOscilloscope *self)
{
    if (!self->lock) return LOCK_ERROR;

    self->lock = 0;

    OscSeqGroupDisable(&osc_seq_group);

    ADCGroupUnlock(&adc_group);

    return OK;
}


void
OscilloscopeMain(tOscilloscope *self, bool enable_reply)
{
    unsigned int i;
    tEvent *reply;
    tRing *ring;
    tPage *page;
    uint8_t *head;
    tRingIter iter;

    if (!self->lock) return;

    if (OscSeqGroupReady(&osc_seq_group)) {

        FOREACH_ADC {

            ring = &osc_seq_group.osc_seq[i].ring;

            for (RingIterInit(&iter, ring); iter.content; RingIterNext(&iter))
            {
                page = RingIterGet(&iter);
                head = (uint8_t *) page->buffer;
                head[0] = OscilloscopeData; // reply
                head[1] = ShortArray; // type
                head[2] = i; // channel
                head[3] = 0; // last
            }

        }

        head[3] = 255; // last packet

        // Note: It does not call RingFree, when enable_reply is false
        if (enable_reply) {
            // usb_device will call RingFree when done
            FOREACH_ADC {
                reply = QueueAcquire(&reply_handler.reply_queue);
                EventSetRing(reply, &osc_seq_group.osc_seq[i].ring);
                QueueWrite(&reply_handler.reply_queue);
            }
        }

        OscilloscopeStop(self);

    }
}


void
OscilloscopeInit(tOscilloscope *self)
{
    self->lock = 0;
}
