/*
 * osc_seq.h
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

#ifndef OSC_SEQ_H_
#define OSC_SEQ_H_


#include "adc.h"
#include "ring.h"


typedef struct OscSeq {

    tADC *adc;

    uint32_t sequence_num;
    uint32_t sequence_int;
    uint32_t priority;
    uint32_t udma_channel;

    volatile unsigned char ping_enable;
    volatile unsigned char pong_enable;

    tRing ring;

} tOscSeq;


typedef struct OscSeqGroup {

    tOscSeq osc_seq[ADC_GROUP_SIZE];

    tADCGroup *adc_group;

} tOscSeqGroup;


inline void
OscSeqDisable(tOscSeq *self)
{
    ADCTimerStop(self->adc->timer); // it does not hurt to call it twice

    IntDisable(self->sequence_int);
    uDMAChannelDisable(self->udma_channel); // this one cancels uDMA immediately.
    ADCSequenceDisable(self->adc->base, self->sequence_num);
}


inline void
OscSeqIntHandler(tOscSeq *self)
{
    tPage *page;

    ADCIntClear(self->adc->base, self->sequence_num);

    // ping_ and pong_enable are required, because uDMAChannelSizeGet
    // does return 0 at the end when the PRI channel is about to be disabled
    if(self->ping_enable && !uDMAChannelSizeGet(self->udma_channel | UDMA_PRI_SELECT)) {
        // PRI is done, ALT is working this moment
        RingWrite(&self->ring);

        if (RingFull(&self->ring)) {
            // done
            self->ping_enable = 0;
        }

        else {
            // continue
            page = RingAcquire(&self->ring);

            uDMAChannelTransferSet(self->udma_channel | UDMA_PRI_SELECT,
                UDMA_MODE_PINGPONG,
                (void *)(self->adc->base + ADC_O_SSFIFO0),
                page->buffer+24, ADC_SAMPLES);
        }
    }

    else if(self->pong_enable && !uDMAChannelSizeGet(self->udma_channel | UDMA_ALT_SELECT)) {
        // ALT is done, PRI is working this moment
        RingWrite(&self->ring);

        if (RingFull(&self->ring)) {
            // done
            self->pong_enable = 0;

            // disable ADC
            OscSeqDisable(self);
        }

        else {
            // continue
            page = RingAcquire(&self->ring);

            uDMAChannelTransferSet(self->udma_channel | UDMA_ALT_SELECT,
                UDMA_MODE_PINGPONG,
                (void *)(self->adc->base + ADC_O_SSFIFO0),
                page->buffer+24, ADC_SAMPLES);
        }
    }
}


inline void
OscSeqGroupAllocate(tOscSeqGroup *self, tPage *pages, uint32_t length)
{
    int i;

    FOREACH_ADC RingAllocate(&self->osc_seq[i].ring, pages + i*length, length);
}


inline unsigned char
OscSeqGroupReady(tOscSeqGroup *self)
{
    int i;
    unsigned char enable = 0;

    FOREACH_ADC enable |= self->osc_seq[i].ping_enable | self->osc_seq[i].pong_enable;

    return !enable;
}


inline void
OscSeqEnable(tOscSeq *self)
{
    tPage *page;

    // Both ADC shall run exactly in sync
    // - A common timer triggers both ADCs
    // - The timer is disabled before a new measurement starts
    // - The sequencer and interrupts are disabled, too.
    //   DMA does not start in sync if spurious interrupts are around

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

    page = RingAcquire(&self->ring);

    uDMAChannelTransferSet(self->adc->udma_channel | UDMA_PRI_SELECT,
        UDMA_MODE_PINGPONG,
        (void *)(self->adc->base + ADC_O_SSFIFO0),
        page->buffer+24, ADC_SAMPLES);

    self->ping_enable = 1;

    page = RingAcquire(&self->ring);

    uDMAChannelTransferSet(self->udma_channel | UDMA_ALT_SELECT,
        UDMA_MODE_PINGPONG,
        (void *)(self->adc->base + ADC_O_SSFIFO0),
        page->buffer+24, ADC_SAMPLES);

    self->pong_enable = 1;

    ADCSequenceEnable(self->adc->base, self->sequence_num);
    uDMAChannelEnable(self->udma_channel);
    IntEnable(self->sequence_int);
}


inline void
OscSeqGroupEnable(tOscSeqGroup *self)
{
    int i;
    FOREACH_ADC OscSeqEnable(&self->osc_seq[i]);

    ADCTimerStart(&self->adc_group->timer, 1);
}


inline void
ConfigureOscSeq(tOscSeq *self)
{
    self->sequence_num = 0;
    self->priority = 0;

    // Set the ADC Sequence
    // to generate an interrupt every 4 samples.
    // This is an arbitration size of 4 for the uDMA transfer
    ADCSequenceConfigure(self->adc->base, self->sequence_num, ADC_TRIGGER_TIMER, self->priority);
    ADCSequenceStepConfigure(self->adc->base, self->sequence_num, 0, self->adc->channel);
    ADCSequenceStepConfigure(self->adc->base, self->sequence_num, 1, self->adc->channel);
    ADCSequenceStepConfigure(self->adc->base, self->sequence_num, 2, self->adc->channel);
    ADCSequenceStepConfigure(self->adc->base, self->sequence_num, 3, self->adc->channel | ADC_CTL_IE);
    ADCSequenceStepConfigure(self->adc->base, self->sequence_num, 4, self->adc->channel);
    ADCSequenceStepConfigure(self->adc->base, self->sequence_num, 5, self->adc->channel);
    ADCSequenceStepConfigure(self->adc->base, self->sequence_num, 6, self->adc->channel);
    ADCSequenceStepConfigure(self->adc->base, self->sequence_num, 7, self->adc->channel | ADC_CTL_IE | ADC_CTL_END);
}


inline void
ConfigureOscSeq0(tOscSeq *self, tADCGroup *adc_group)
{
    self->adc = &adc_group->adc[0];
    self->sequence_int = INT_ADC0SS0;
    self->udma_channel = UDMA_CHANNEL_ADC0;

    ConfigureOscSeq(self);
}


inline void
ConfigureOscSeq1(tOscSeq *self, tADCGroup *adc_group)
{
    self->adc = &adc_group->adc[1];
    self->sequence_int = INT_ADC1SS0;
    self->udma_channel = 24; // UDMA_CHANNEL_ADC1 enthält einen falschen Wert?!

    ConfigureOscSeq(self);
}


inline void
OscSeqGroupInit(tOscSeqGroup *self, tADCGroup *adc_group)
{
    self->adc_group = adc_group;

    ConfigureOscSeq0(&self->osc_seq[0], self->adc_group);
    ConfigureOscSeq1(&self->osc_seq[1], self->adc_group);
}

#endif /* OSC_SEQ_H_ */
