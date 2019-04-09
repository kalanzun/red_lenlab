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
    tOscSeq *self = &oscilloscope.seq_group->osc_seq[0];

    OscSeqIntHandler(self);
}


void
ADC1SS0Handler(void)
{
    tOscSeq *self = &oscilloscope.seq_group->osc_seq[1];

    OscSeqIntHandler(self);
}


tError
OscilloscopeStart(tOscilloscope *self, uint32_t samplerate)
{
    if (self->lock) return LOCK_ERROR;

    if (self->seq_group->adc_group->lock) return ADC_ERROR;

    if (memory.acquire) return MEMORY_ERROR;

    self->lock = 1;

    ADCGroupLock(self->seq_group->adc_group);

    ADCGroupSetHardwareOversample(self->seq_group->adc_group, samplerate);

    // 2 rings of 10 pages each (osc_seq want's an even number)
    OscSeqGroupAllocate(self->seq_group, 10);

    OscSeqGroupEnable(self->seq_group);

    return OK;
}


tError
OscilloscopeStop(tOscilloscope *self)
{
    if (!self->lock) return LOCK_ERROR;

    self->lock = 0;

    ADCGroupUnlock(self->seq_group->adc_group);

    MemoryRelease(&memory);

    return OK;
}


void
OscilloscopeMain(tOscilloscope *self)
{
    // TODO not tested yet
    int i;
    tRing *ring;
    tPage *page;
    tRingIter iter;

    if (!self->lock) return;

    if (OscSeqGroupReady(self->seq_group)) {

        FOREACH_ADC {

            ring = &self->seq_group->osc_seq[i].ring;

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

        OscilloscopeStop(self); // TODO memory release should wait until USB is done?

    }
}


void
OscilloscopeInit(tOscilloscope *self, tOscSeqGroup *seq_group)
{
    self->seq_group = seq_group;

    self->lock = 0;
}
