/*
 * oscilloscope.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "oscilloscope.h"

#include "driverlib/debug.h"


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

    if (self->adc_group->lock) return STATE_ERROR;

    if (self->memory->lock) return STATE_ERROR;

    MemoryLock(self->memory);

    ADCGroupLock(self->adc_group);

    self->lock = 1;

    ADCGroupSetHardwareOversample(self->adc_group, samplerate);

    // 2 rings of 10 pages each
    OscSeqGroupAllocate(&self->seq_group, self->memory->pages, 10);

    OscSeqGroupEnable(&self->seq_group);

    return OK;
}


tError
OscilloscopeStop(tOscilloscope *self)
{
    if (!self->lock) return LOCK_ERROR;

    self->lock = 0;

    ADCGroupUnlock(self->adc_group);

    MemoryUnlock(self->memory);

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

        ADCTimerStop(&self->adc_group->timer); // DMA disables itself, the timer is left running

        //DEBUG_PRINT("OscilloscopeData\n");

        FOREACH_ADC {

            ring = &self->seq_group.osc_seq[i].ring;

            for (RingIterInit(&iter, ring); iter.content; RingIterNext(&iter))
            {
                page = RingIterGet(&iter);
                /*
                page->buffer[0] = OscilloscopeData; // reply
                page->buffer[1] = ShortArray; // type
                page->buffer[2] = i; // channel
                page->buffer[3] = 0; // last
                */
            }

        }

        page->buffer[3] = 255; // last packet

        //USBDeviceSendInterleaved(&self->seq_group.osc_seq[0].ring, &self->seq_group.osc_seq[1].ring);

        OscilloscopeStop(self);

    }
}


void
OscilloscopeInit(tOscilloscope *self, tMemory *memory, tADCGroup *adc_group)
{
    self->memory = memory;

    self->adc_group = adc_group;

    OscSeqGroupInit(&self->seq_group, adc_group);

    self->lock = 0;
}
