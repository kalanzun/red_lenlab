/*
 * osc_seq.h
 *
 */

#ifndef OSC_SEQ_H_
#define OSC_SEQ_H_


#include "adc.h"
#include "ring.h"

#include "driverlib/debug.h"


// number of samples and offset in a memory page
#define OSCILLOSCOPE_SAMPLES 510
#define OSCILLOSCOPE_OFFSET 1


typedef struct OscSeq {

    tADC *adc;

    uint32_t sequence_num;
    uint32_t sequence_int;
    uint32_t priority;
    uint32_t udma_channel;

    volatile bool ping_enable;
    volatile bool pong_enable;

    tRing ring;

} tOscSeq;


typedef struct OscSeqGroup {

    tOscSeq osc_seq[ADC_GROUP_SIZE];

} tOscSeqGroup;


extern tOscSeqGroup osc_seq_group;


inline void
OscSeqDisable(tOscSeq *self)
{
    IntDisable(self->sequence_int);
    uDMAChannelDisable(self->udma_channel); // this one cancels uDMA immediately.
    ADCSequenceDisable(self->adc->base, self->sequence_num);
}


inline void
OscSeqGroupDisable(tOscSeqGroup *self)
{
    int i;

    ADCTimerStop(&adc_group.timer);

    FOREACH_ADC OscSeqDisable(&self->osc_seq[i]);
}


inline void
OscSeqPingPong(tOscSeq *self)
{
    tPage *page = RingAcquire(&self->ring);

    if (!self->ping_enable) {

        ASSERT(self->pong_enable); // pong still runs, else we are too late here

        uDMAChannelTransferSet(self->udma_channel | UDMA_PRI_SELECT,
            UDMA_MODE_PINGPONG,
            (void *)(self->adc->base + ADC_O_SSFIFO0), // sequence_num == 0 only
            page->buffer + OSCILLOSCOPE_OFFSET, OSCILLOSCOPE_SAMPLES);

        self->ping_enable = 1;
    }
    if (!self->pong_enable) {

        ASSERT(self->ping_enable); // ping still runs, else we are too late here

        uDMAChannelTransferSet(self->udma_channel | UDMA_ALT_SELECT,
            UDMA_MODE_PINGPONG,
            (void *)(self->adc->base + ADC_O_SSFIFO0), // sequence_num == 0 only
            page->buffer + OSCILLOSCOPE_OFFSET, OSCILLOSCOPE_SAMPLES);

        self->pong_enable = 1;
    }
}


inline void
OscSeqIntHandler(tOscSeq *self)
{
    ADCIntClear(self->adc->base, self->sequence_num);

    // ping_ and pong_enable are required, because uDMAChannelSizeGet
    // does return 0 at the end when the PRI channel is about to be disabled
    if(self->ping_enable && !uDMAChannelSizeGet(self->udma_channel | UDMA_PRI_SELECT)) {
        // PRI is done, ALT is working this moment
        RingWrite(&self->ring);

        self->ping_enable = 0;

        ASSERT(self->pong_enable);
        // the program did not reconfigure DMA, DMA did need the configuration just a moment ago

        // Automatic ping pong if the ring is not full.
        if (!RingFull(&self->ring)) {
            OscSeqPingPong(self);
        }

        // You may also call PingPong from Main after releasing a page of the ring.
        // There is time until ALT is finished as well
    }

    else if(self->pong_enable && !uDMAChannelSizeGet(self->udma_channel | UDMA_ALT_SELECT)) {
        // ALT is done, PRI is working this moment
        RingWrite(&self->ring);

        self->pong_enable = 0;

        if (!self->ping_enable) {
            // the program did not reconfigure DMA, DMA did need the configuration just a moment ago
            // OscSeqDisable(self);
            // Main shall disable osc_seq later on
        }

        // Automatic ping pong if the ring is not full.
        else if (!RingFull(&self->ring)) {
            OscSeqPingPong(self);
        }

        // You may also call PingPong from Main after releasing a page of the ring.
        // There is time until PRI is finished as well
    }
}


inline void
OscSeqGroupAllocate(tOscSeqGroup *self, uint32_t length)
{
    int i;

    FOREACH_ADC RingAllocate(&self->osc_seq[i].ring, length);
}


inline void
OscSeqGroupRelease(tOscSeqGroup *self)
{
    int i;

    FOREACH_ADC RingRelease(&self->osc_seq[i].ring);
}


inline void
OscSeqGroupPingPong(tOscSeqGroup *self)
{
    int i;

    FOREACH_ADC OscSeqPingPong(&self->osc_seq[i]);
}


inline unsigned char
OscSeqGroupReady(tOscSeqGroup *self)
{
    int i;
    unsigned char enable = 0;

    FOREACH_ADC enable |= self->osc_seq[i].ping_enable | self->osc_seq[i].pong_enable;

    return !enable;
}


inline unsigned char
OscSeqGroupRingContent(tOscSeqGroup *self)
{
    int i;
    unsigned char content = 1;

    FOREACH_ADC content &= RingContent(&self->osc_seq[i].ring);

    return content;
}


inline void
OscSeqEnable(tOscSeq *self)
{
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

    // configure ping
    self->ping_enable = 0;
    self->pong_enable = 1;
    OscSeqPingPong(self);

    // configure pong
    self->pong_enable = 0;
    OscSeqPingPong(self);

    ASSERT(self->ping_enable);
    ASSERT(self->pong_enable);

    ADCSequenceEnable(self->adc->base, self->sequence_num);
    ADCIntDisable(self->adc->base, self->sequence_num);
    uDMAChannelEnable(self->udma_channel);
    IntEnable(self->sequence_int);
}


inline void
OscSeqGroupEnable(tOscSeqGroup *self)
{
    int i;
    FOREACH_ADC OscSeqEnable(&self->osc_seq[i]);

    ADCTimerStart(&adc_group.timer, 1);
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
ConfigureOscSeq0(tOscSeq *self)
{
    self->adc = &adc_group.adc[0];
    self->sequence_int = INT_ADC0SS0;
    self->udma_channel = UDMA_CHANNEL_ADC0;

    ConfigureOscSeq(self);
}


inline void
ConfigureOscSeq1(tOscSeq *self)
{
    self->adc = &adc_group.adc[1];
    self->sequence_int = INT_ADC1SS0;
    self->udma_channel = 24; // UDMA_CHANNEL_ADC1 is wrong?!

    ConfigureOscSeq(self);
}


inline void
OscSeqGroupInit(tOscSeqGroup *self)
{
    ConfigureOscSeq0(&self->osc_seq[0]);
    ConfigureOscSeq1(&self->osc_seq[1]);
}

#endif /* OSC_SEQ_H_ */
