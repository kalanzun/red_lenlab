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

#include <stdbool.h>
#include <stdint.h>

#include "trigger.h"

#include "reply_handler.h"


tError
TriggerStart(tTrigger *self, uint32_t log2oversamples)
{
    uint32_t i;

    if (self->lock) return LOCK_ERROR;

    if (adc_group.lock) return ADC_ERROR;

    if (memory.lock) return MEMORY_ERROR;

    self->lock = 1;

    for (i = 0; i < TRIGGER_FILTER_LENGTH; i++) self->filter[i] = 0;
    self->index = 0;
    self->state = 0;

    self->wait = 0;
    self->active = 0;
    self->save = 0;
    self->count = 0;
    self->post_count = 0;

    // 18 memory pages
    RingAllocate(&self->ring, 18);

    ADCGroupLock(&adc_group);

    ADCGroupSetHardwareOversample(&adc_group, log2oversamples);

    // TODO
    // Ich brauche einen pingpong Modus. W�hrend pong den zweiten Puffer schreibt, verarbeitet Main den ersten Puffer
    // Allerdings ist der erste Puffer auch bereits bei der DMA Transaction gemeldet.
    // ODER: DMA h�rt automatisch auf und Main muss jeweils am Ende der Verarbeitung die Transaction programmieren
    // Wenn das ausreichend schnell passiert l�uft DMA weiter.

    // 2 rings of 2 memory pages each
    OscSeqGroupAllocate(&osc_seq_group, 2);

    OscSeqGroupEnable(&osc_seq_group);

    return OK;
}


tError
TriggerStop(tTrigger *self)
{
    if (!self->lock) return LOCK_ERROR;

    self->lock = 0;

    OscSeqGroupDisable(&osc_seq_group); // stop ping pong DMA

    OscSeqGroupFree(&osc_seq_group);

    ADCGroupUnlock(&adc_group);

    return OK;
}


inline int8_t
delta(uint16_t previous, uint16_t next)
{
    int16_t delta = next - previous;
    if (delta > 127)
        return 127;
    else if (delta < -127)
        return -127;
    else
        return (int8_t) delta;
}


void
TriggerMain(tTrigger *self, bool enable_reply)
{
    unsigned int i;

    tEvent *reply;
    tPage *page;

    uint8_t *head;
    uint16_t *short_head;

    tPage *adc_page0;
    tPage *adc_page1;

    uint16_t *buffer0;
    uint16_t *buffer1;

    uint16_t state0, state1;
    int8_t *data;

    if (!self->lock) return;

    if (!OscSeqGroupRingContent(&osc_seq_group)) return;

    adc_page0 = RingRead(&osc_seq_group.osc_seq[0].ring);
    adc_page1 = RingRead(&osc_seq_group.osc_seq[1].ring);

    buffer0 = (uint16_t *) (adc_page0->buffer + OSCILLOSCOPE_OFFSET);
    buffer1 = (uint16_t *) (adc_page1->buffer + OSCILLOSCOPE_OFFSET);

    if (RingFull(&self->ring)) {
        RingRead(&self->ring);
        RingRelease(&self->ring);
    }

    page = RingAcquire(&self->ring);
    head = (uint8_t *) page->buffer;
    short_head = (uint16_t *) page->buffer;

    head[0] = OscilloscopeData;
    head[1] = ByteArray;
    head[2] = 0;
    head[3] = 0;

    state0 = buffer0[0] >> 2;
    state1 = buffer1[0] >> 2;

    short_head[2] = 0; // trigger value
    short_head[3] = 0; // empty
    short_head[4] = state0;
    short_head[5] = state1;

    data = (int8_t *) (page->buffer + OSCILLOSCOPE_OFFSET);

    if (self->count == 9) {
        self->wait = 1;
    }

    for (i = 1; i < OSCILLOSCOPE_SAMPLES; ++i) // OSCILLOSCOPE_SAMPLES
    {
        state0 += data[2*i] = delta(state0, buffer0[i] >> 2);
        state1 += data[2*i+1] = delta(state1, buffer1[i] >> 2);

        if (self->count < 9 || self->wait || self->active) {
            self->state -= self->filter[self->index];
            self->state += state0;
            self->filter[self->index] = state0;
            self->index = (self->index + 1) % TRIGGER_FILTER_LENGTH;
        }

        // pre trigger, wait for signal to be below the trigger level
        if (self->wait && (self->state < (TRIGGER_FILTER_LENGTH*512))) {
            self->wait = 0;
            self->active = 1;
        }

        // trigger, wait for signal to cross the trigger level
        if (self->active && (self->state > (TRIGGER_FILTER_LENGTH*512))) {
            self->active = 0;
            self->save = 1;
            if (i < 4) {
                // trigger happened during the end of the last packet
                ++self->post_count;
                short_head[2] = OSCILLOSCOPE_SAMPLES - (4 - i); // trigger value
            }
            else {
                short_head[2] = i - 4; // trigger value
            }
        }
    }

    OscSeqGroupRelease(&osc_seq_group); // RingRelease
    OscSeqGroupPingPong(&osc_seq_group); // Reconfigure DMA

    // trigger timeout
    if ((self->wait || self->active) && self->count == 100) {
        self->wait = 0;
        self->active = 0;
        self->save = 1;
    }

    self->count++;
    RingWrite(&self->ring);

    if (self->save) {

        if (self->post_count == 9) { // half of 18
            head[3] = 255; // mark this the last package
            // Note: It does not call RingFree, when enable_usb is false
            if (enable_reply) {
                // usb_device will call RingFree when done
                reply = QueueAcquire(&reply_handler.reply_queue);
                EventSetRing(reply, &self->ring);
                QueueWrite(&reply_handler.reply_queue);
            }
            TriggerStop(self);
        }

        self->post_count++;
    }

}


tError
TriggerLinearTestData(tTrigger *self)
{
    unsigned int i, k;
    tRing *ring;
    tPage *page;
    uint16_t *short_buffer;

    if (self->lock) return LOCK_ERROR;

    if (adc_group.lock) return ADC_ERROR;

    if (memory.lock) return MEMORY_ERROR;

    self->lock = 1;

    for (i = 0; i < TRIGGER_FILTER_LENGTH; i++) self->filter[i] = 0;
    self->index = 0;
    self->state = 0;

    self->wait = 0;
    self->active = 0;
    self->save = 0;
    self->count = 0;
    self->post_count = 0;

    // 18 memory pages
    RingAllocate(&self->ring, 18);

    ADCGroupLock(&adc_group);

    // 2 rings of 2 memory pages each
    OscSeqGroupAllocate(&osc_seq_group, 2);

    FOREACH_ADC {
        osc_seq_group.osc_seq[i].ping_enable = 1;
        osc_seq_group.osc_seq[i].pong_enable = 1;

        page = RingAcquire(&osc_seq_group.osc_seq[i].ring);
    }

    while(self->lock) {
        // write linear test data into the DMA buffer
        FOREACH_ADC {
            ring = &osc_seq_group.osc_seq[i].ring;
            page = ring->acquire > 0 ? ring->pages + ring->acquire - 1 : ring->pages + ring->length - 1;
            short_buffer = (uint16_t *) (page->buffer + OSCILLOSCOPE_OFFSET);

            for (k = 0; k < OSCILLOSCOPE_SAMPLES / 2; k++) {
                short_buffer[k] = 16 * k;
            }
            for (; k < OSCILLOSCOPE_SAMPLES; k++) {
                short_buffer[k] = (16 * OSCILLOSCOPE_SAMPLES / 2) - (16 * (k - OSCILLOSCOPE_SAMPLES / 2));
            }

            RingWrite(ring);

            osc_seq_group.osc_seq[i].ping_enable = 0; // DMA done
        }

        TriggerMain(self, true); // will reset self->lock at some point
        // calls RingAcquire
    }

    return OK;
}


void
TriggerInit(tTrigger *self)
{
    self->lock = 0;
}
