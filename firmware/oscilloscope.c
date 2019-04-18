/*
 * oscilloscope.c
 *
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
                short_buffer[2 + k] = k + (j * OSCILLOSCOPE_SAMPLES);
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
