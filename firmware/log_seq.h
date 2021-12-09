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

#ifndef LOG_SEQ_H_
#define LOG_SEQ_H_


#include "driverlib/debug.h"

#include "adc_group.h"


#define LOG_SEQ_SEQUENCE_NUM 1 // shall be different from osc_seq
#define LOG_SEQ_PRIORITY 1 // shall be different from osc_seq


struct LogSeq {
    volatile bool ready;
    volatile bool error;
    volatile uint32_t count;
};


struct LogSeqGroup {
    struct LogSeq log_seq[GROUP_SIZE];
};


extern struct LogSeqGroup log_seq_group;


// struct LogSeq *log;
#define FOREACH_LOG for (log = log_seq_group.log_seq; log != log_seq_group.log_seq + GROUP_SIZE; ++log)


inline void
LogSeqIntHandler(struct LogSeq *self)
{
    if (self->ready) self->error = true;
    else self->ready = true;

    self->count = self->count + 1;
}


inline bool
LogSeqGroupReady(void)
{
    struct LogSeq *log;
    bool ready = true;

    FOREACH_LOG ready &= log->ready;

    return ready;
}


inline bool
LogSeqGroupError(void)
{
    struct LogSeq *log;
    bool error = false;

    FOREACH_LOG error |= log->error;

    return error;
}


inline void
LogSeqGroupRelease(void)
{
    struct LogSeq *log;

    FOREACH_LOG log->ready = false;
}


inline int
LogSeqGroupDataGet(uint32_t *buffer)
{
    const struct ADC *adc;
    int count = 0; // sample count

    FOREACH_ADC count += ADCSequenceDataGet(adc->base, LOG_SEQ_SEQUENCE_NUM, buffer + count);

    return count;
}


inline void
LogSeqEnable(struct LogSeq *self)
{
    self->ready = false;
    self->error = false;
    self->count = 0;
}


inline void
LogSeqGroupEnable(uint32_t interval)
{
    const struct ADC *adc;
    struct LogSeq *log;
    int count;
    uint32_t buffer[8]; // max FIFO length of SS1

    // clear data FIFO
    count = LogSeqGroupDataGet(buffer);
    ASSERT(count == 0 || count == 8);

    FOREACH_LOG LogSeqEnable(log);

    FOREACH_ADC {
        ADCSequenceEnable(adc->base, LOG_SEQ_SEQUENCE_NUM);
        ADCIntEnable(adc->base, LOG_SEQ_SEQUENCE_NUM); // Enable to generate direct ADC Interrupts, do not enable for DMA
    }

    IntEnable(INT_ADC0SS1); // Enable Interrupt in NVIC
    IntEnable(INT_ADC1SS1); // Enable Interrupt in NVIC

    // interval in ms
    ADCTimerStart(interval * 1000); // us
}


inline void
LogSeqGroupDisable(void)
{
    const struct ADC *adc;

    ADCTimerStop();

    IntDisable(INT_ADC0SS1);
    IntDisable(INT_ADC1SS1);

    FOREACH_ADC {
        ADCIntDisable(adc->base, LOG_SEQ_SEQUENCE_NUM);
        ADCSequenceDisable(adc->base, LOG_SEQ_SEQUENCE_NUM);
    }
}


inline void
LogSeqGroupInit(void)
{
    const struct ADC *adc;

    FOREACH_ADC {
        ADCSequenceConfigure(adc->base, LOG_SEQ_SEQUENCE_NUM, ADC_TRIGGER_TIMER, LOG_SEQ_PRIORITY);
        ADCSequenceStepConfigure(adc->base, LOG_SEQ_SEQUENCE_NUM, 0, adc->channel);
        ADCSequenceStepConfigure(adc->base, LOG_SEQ_SEQUENCE_NUM, 1, adc->channel_1 | ADC_CTL_IE | ADC_CTL_END);
    }
}


#endif /* LOG_SEQ_H_ */
