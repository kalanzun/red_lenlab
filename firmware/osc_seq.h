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

#ifndef OSC_SEQ_H_
#define OSC_SEQ_H_


#include "driverlib/debug.h"

#include "adc_group.h"
#include "page_handler.h"


#define OSC_SEQ_SEQUENCE_NUM 0 // shall be different from log_seq
#define OSC_SEQ_PRIORITY 0 // shall be different from log_seq
#define OSC_SEQ_FIFO_OFFSET ADC_O_SSFIFO0 // for sequence number 0

#define OSCILLOSCOPE_OFFSET 24
#define OSCILLOSCOPE_SAMPLES 500


struct OscSeq {
    const struct ADC * const adc;
    struct Ring * const page_queue;

    volatile bool ping_enable;
    volatile bool pong_enable;
};


struct OscSeqGroup {
    struct OscSeq osc_seq[GROUP_SIZE];
};


extern struct OscSeqGroup osc_seq_group;


// struct OscSeq *osc;
#define FOREACH_OSC for (osc = osc_seq_group.osc_seq; osc != osc_seq_group.osc_seq + GROUP_SIZE; ++osc)


inline void
OscSeqGroupDisable(void)
{
    const struct ADC *adc;

    ADCTimerStop();

    IntDisable(INT_ADC0SS0);
    IntDisable(INT_ADC1SS0);

    FOREACH_ADC {
        uDMAChannelDisable(adc->udma_channel); // this one cancels uDMA immediately.
        ADCSequenceDisable(adc->base, OSC_SEQ_SEQUENCE_NUM);
    }
}


inline void
OscSeqPingPong(struct OscSeq *self)
{
    uint8_t *page = RingAcquire(self->page_queue);

    if (!self->ping_enable) {
        ASSERT(self->pong_enable); // pong still runs, else we are too late here

        uDMAChannelTransferSet(self->adc->udma_channel | UDMA_PRI_SELECT,
            UDMA_MODE_PINGPONG,
            (void *) (self->adc->base + OSC_SEQ_FIFO_OFFSET),
            page + OSCILLOSCOPE_OFFSET, OSCILLOSCOPE_SAMPLES);

        self->ping_enable = true;
    }

    if (!self->pong_enable) {
        ASSERT(self->ping_enable); // ping still runs, else we are too late here

        uDMAChannelTransferSet(self->adc->udma_channel | UDMA_ALT_SELECT,
            UDMA_MODE_PINGPONG,
            (void *) (self->adc->base + OSC_SEQ_FIFO_OFFSET),
            page + OSCILLOSCOPE_OFFSET, OSCILLOSCOPE_SAMPLES);

        self->pong_enable = true;
    }
}


inline void
OscSeqIntHandler(struct OscSeq *self)
{
    // ping_ and pong_enable are required, because uDMAChannelSizeGet
    // does return 0 at the end when the PRI channel is about to be disabled
    if(self->ping_enable && !uDMAChannelSizeGet(self->adc->udma_channel | UDMA_PRI_SELECT)) {
        // PRI is done, ALT is working this moment
        RingWrite(self->page_queue);

        self->ping_enable = false;

        ASSERT(self->pong_enable);
        // the program did not reconfigure DMA, DMA did need the configuration just a moment ago

        // Automatic ping pong if the ring is not full.
        if (self->page_queue->has_space) {
            OscSeqPingPong(self);
        }
    }

    else if(self->pong_enable && !uDMAChannelSizeGet(self->adc->udma_channel | UDMA_ALT_SELECT)) {
        // ALT is done, PRI is working this moment
        RingWrite(self->page_queue);

        self->pong_enable = false;

        if (self->ping_enable) {
            ASSERT(self->page_queue->has_space); // assume an even number of pages

            OscSeqPingPong(self);
        }
        // else the program did not reconfigure DMA, DMA did need the configuration just a moment ago
        // the main loop shall detect it and disable DMA
    }
}


inline void
OscSeqGroupPingPong(void)
{
    struct OscSeq *osc;

    FOREACH_OSC OscSeqPingPong(osc);
}


inline bool
OscSeqGroupReady(void)
{
    struct OscSeq *osc;
    bool enable = false;

    FOREACH_OSC enable |= osc->ping_enable | osc->pong_enable;

    return !enable;
}


inline void
OscSeqEnable(struct OscSeq *self)
{
    // Both ADC shall run exactly in sync
    // - A common timer triggers both ADCs
    // - The timer is disabled before a new measurement starts
    // - The sequencer and interrupts are disabled, too.
    //   DMA does not start in sync if spurious interrupts are around

    uDMAChannelAttributeDisable(self->adc->udma_channel,
        UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
        UDMA_ATTR_HIGH_PRIORITY |
        UDMA_ATTR_REQMASK);

    uDMAChannelControlSet(self->adc->udma_channel | UDMA_PRI_SELECT,
        UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
        UDMA_DST_INC_16 | UDMA_ARB_4);

    uDMAChannelControlSet(self->adc->udma_channel | UDMA_ALT_SELECT,
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
}


inline void
OscSeqGroupEnable(void)
{
    const struct ADC *adc;
    struct OscSeq *osc;

    FOREACH_OSC OscSeqEnable(osc);

    FOREACH_ADC {
        ADCSequenceEnable(adc->base, OSC_SEQ_SEQUENCE_NUM);
        ADCIntDisable(adc->base, OSC_SEQ_SEQUENCE_NUM);
        uDMAChannelEnable(adc->udma_channel);
    }

    IntEnable(INT_ADC0SS0);
    IntEnable(INT_ADC1SS0);

    ADCTimerStart(1); // us
}


inline void
OscSeqGroupInit(void)
{
    const struct ADC *adc;
    int i;
    uint32_t config;

    FOREACH_ADC {
        ADCSequenceConfigure(adc->base, OSC_SEQ_SEQUENCE_NUM, ADC_TRIGGER_TIMER, OSC_SEQ_PRIORITY);
        for (i = 0; i < 8; ++i) {
            config = adc->channel;
            if (i == 3 || i == 7) config |= ADC_CTL_IE;
            if (i == 7) config |= ADC_CTL_END;
            ADCSequenceStepConfigure(adc->base, OSC_SEQ_SEQUENCE_NUM, i, config);
        }
    }
}


#endif /* OSC_SEQ_H_ */
