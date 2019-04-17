/*
 * trigger.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "trigger.h"

#include "lenlab_protocol.h"
#include "reply_handler.h"


#define TRIGGER_OFFSET 1


tError
TriggerStart(tTrigger *self, uint32_t samplerate)
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

    ADCGroupSetHardwareOversample(&adc_group, samplerate);

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

    head[0] = OscilloscopeData;
    head[1] = ByteArray;
    head[2] = 0;
    head[3] = 0;

    state0 = buffer0[0] >> 2;
    state1 = buffer1[0] >> 2;
    /*
    *(uint16_t *) (page->buffer + 4) = state0;
    *(uint16_t *) (page->buffer + 6) = state1;
    *(uint16_t *) (page->buffer + 8) = 0; // trigger value
    */

    data = (int8_t *) (page->buffer + TRIGGER_OFFSET);

    if (self->count == 8) {
        self->wait = 1;
    }

    for (i = 1; i < OSCILLOSCOPE_SAMPLES; ++i) // OSCILLOSCOPE_SAMPLES
    {
        state0 += data[2*i] = delta(state0, buffer0[i] >> 2);
        state1 += data[2*i+1] = delta(state1, buffer1[i] >> 2);

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
            //*(uint16_t *) (page->buffer + 2) = i;
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


void
TriggerInit(tTrigger *self)
{
    self->lock = 0;
}
