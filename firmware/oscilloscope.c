/*
 * oscilloscope.c
 *
 *  Created on: 24.11.2018
 *      Author: Christoph
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

    if (!self->lock) return;

    if (OscSeqGroupReady(&self->seq_group)) {

        FOREACH_ADC {

            ring = &self->seq_group.osc_seq[i].ring;

            while(RingContent(ring)) {
                page = RingRead(ring);

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
