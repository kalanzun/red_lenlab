/*
 * trigger.c
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

#include "trigger.h"

#include "reply_handler.h"
#include "usb_device.h"


tError
TriggerStart(tTrigger *self, uint32_t samplerate)
{
    uint32_t i;

    if (self->lock) return LOCK_ERROR;

    if (self->oscilloscope->adc_group->lock) return STATE_ERROR;
    // ADCGroup is free, so oscilloscope->memory is available

    for (i = 0; i < TRIGGER_FILTER_LENGTH; i++) self->filter[i] = 0;
    self->index = 0;
    self->state = 0;

    self->wait = 0;
    self->active = 0;
    self->save = 0;
    self->count = 0;
    self->post_count = 0;

    ADCGroupSetHardwareOversample(self->oscilloscope->adc_group, samplerate);

    // TODO
    // Ich brauche einen pingpong Modus. Während pong den zweiten Puffer schreibt, verarbeitet Main den ersten Puffer
    // Allerdings ist der erste Puffer auch bereits bei der DMA Transaction gemeldet.
    // ODER: DMA hört automatisch auf und Main muss jeweils am Ende der Verarbeitung die Transaction programmieren
    // Wenn das ausreichend schnell passiert läuft DMA weiter.

    // 18 memory pages
    RingAllocate(&self->ring, self->oscilloscope->memory, 18);

    // 2 rings of 2 memory pages each
    OscSeqGroupAllocate(&self->oscilloscope->seq_group, self->oscilloscope->memory, 2);

    OscSeqGroupEnable(&self->oscilloscope->seq_group);

    ADCGroupLock(self->oscilloscope->adc_group);

    self->lock = 1;

    return OK;
}


tError
TriggerStop(tTrigger *self)
{
    if (!self->lock) return LOCK_ERROR;

    self->lock = 0;

    ADCGroupUnlock(self->oscilloscope->adc_group);

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
TriggerMain(tTrigger *self)
{
    int i;
    tPage *page;

    tPage *adc_page0;
    tPage *adc_page1;

    uint16_t *buffer0;
    uint16_t *buffer1;

    uint16_t state0, state1;
    int8_t *data;

    if (!self->lock) return;

    if (!OscSeqGroupRingContent(&self->oscilloscope->seq_group)) return;

    adc_page0 = RingRead(&self->oscilloscope->seq_group.osc_seq[0].ring);
    adc_page1 = RingRead(&self->oscilloscope->seq_group.osc_seq[1].ring);

    buffer0 = (uint16_t *) (adc_page0->buffer + 24);
    buffer1 = (uint16_t *) (adc_page1->buffer + 24);

    if (RingFull(&self->ring)) {
        RingRead(&self->ring);
        RingRelease(&self->ring);
    }

    page = RingAcquire(&self->ring);

    page->buffer[0] = OscilloscopeData;
    page->buffer[1] = ByteArray;
    page->buffer[2] = 0;
    page->buffer[3] = 0;

    state0 = buffer0[0] >> 2;
    state1 = buffer1[0] >> 2;
    *(uint16_t *) (page->buffer + 4) = state0;
    *(uint16_t *) (page->buffer + 6) = state1;
    *(uint16_t *) (page->buffer + 8) = 0; // trigger value

    data = (int8_t *) (page->buffer + 12);

    if (self->count == 8) {
        self->wait = 1;
    }

    for (i = 1; i < ADC_SAMPLES; ++i)
    {
        data[2*i] = delta(state0, buffer0[i] >> 2);
        state0 += data[2*i];
        data[2*i+1] = delta(state1, buffer1[i] >> 2);
        state1 += data[2*i+1];

        if (self->count == 7 || self->wait || self->active) {
            self->state -= self->filter[self->index];
            self->state += state0;
            self->filter[self->index] = state0;
            self->index = (self->index + 1) % TRIGGER_FILTER_LENGTH;
        }

        // pre trigger, wait for signal to be below the trigger level
        if (self->wait && (self->state < (8*512))) {
            self->wait = 0;
            self->active = 1;
        }

        // trigger, wait for signal to cross the trigger level
        if (self->active && (self->state > (8*512))) {
            self->active = 0;
            self->save = 1;
            *(uint16_t *) (page->buffer + 8) = i;
        }
    }

    OscSeqGroupRelease(&self->oscilloscope->seq_group); // RingRelease
    OscSeqGroupPingPong(&self->oscilloscope->seq_group); // Reconfigure DMA

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
            page->buffer[3] = 255; // mark this the last package
            OscSeqGroupDisable(&self->oscilloscope->seq_group); // stop ping pong DMA
            USBDeviceSend(&self->ring);
            TriggerStop(self);
        }

        self->post_count++;
    }

}


void
TriggerInit(tTrigger *self, tOscilloscope *oscilloscope)
{
    self->oscilloscope = oscilloscope;

    self->lock = 0;
}
