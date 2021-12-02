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

#include "adc.h"


#define LOG_SEQ_SEQUENCE_NUM 1 // shall be different from osc_seq
#define LOG_SEQ_PRIORITY 1 // shall be different from osc_seq

#define LOG_SEQ_ADC0_INT INT_ADC0SS1
#define LOG_SEQ_ADC1_INT INT_ADC1SS1


struct LogSeq {
    volatile bool ready;
    volatile bool error;
    volatile uint32_t count;
};


struct LogSeqGroup {
    struct LogSeq log_seq[2];
};


extern struct LogSeqGroup log_seq_group;


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
    bool ready = true;

    ready &= log_seq_group.log_seq[0].ready;
    ready &= log_seq_group.log_seq[1].ready;

    return ready;
}


inline bool
LogSeqGroupError(void)
{
    bool error = false;

    error |= log_seq_group.log_seq[0].error;
    error |= log_seq_group.log_seq[1].error;

    return error;
}


inline void
LogSeqGroupRelease(void)
{
    log_seq_group.log_seq[0].ready = false;
    log_seq_group.log_seq[1].ready = false;
}


inline int
LogSeqGroupDataGet(uint32_t *buffer)
{
    int count = 0; // sample count

    count += ADCSequenceDataGet(ADC0_BASE, LOG_SEQ_SEQUENCE_NUM, buffer + count);
    count += ADCSequenceDataGet(ADC1_BASE, LOG_SEQ_SEQUENCE_NUM, buffer + count);

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
    int count;
    uint32_t buffer[8]; // max FIFO length of SS1

    // clear data FIFO
    count = LogSeqGroupDataGet(buffer);
    ASSERT(count == 0 || count == 8);

    LogSeqEnable(&log_seq_group.log_seq[0]);
    LogSeqEnable(&log_seq_group.log_seq[1]);

    ADCSequenceEnable(ADC0_BASE, LOG_SEQ_SEQUENCE_NUM);
    ADCIntEnable(ADC0_BASE, LOG_SEQ_SEQUENCE_NUM); // Enable to generate direct ADC Interrupts, do not enable for DMA
    IntEnable(LOG_SEQ_ADC0_INT); // Enable Interrupt in NVIC

    ADCSequenceEnable(ADC1_BASE, LOG_SEQ_SEQUENCE_NUM);
    ADCIntEnable(ADC1_BASE, LOG_SEQ_SEQUENCE_NUM); // Enable to generate direct ADC Interrupts, do not enable for DMA
    IntEnable(LOG_SEQ_ADC1_INT); // Enable Interrupt in NVIC

    // interval in ms
    ADCTimerStart(interval * 1000); // us
}


inline void
LogSeqGroupDisable(void)
{
    ADCTimerStop();

    IntDisable(LOG_SEQ_ADC0_INT);
    ADCIntDisable(ADC0_BASE, LOG_SEQ_SEQUENCE_NUM);
    ADCSequenceDisable(ADC0_BASE, LOG_SEQ_SEQUENCE_NUM);

    IntDisable(LOG_SEQ_ADC1_INT);
    ADCIntDisable(ADC1_BASE, LOG_SEQ_SEQUENCE_NUM);
    ADCSequenceDisable(ADC1_BASE, LOG_SEQ_SEQUENCE_NUM);
}


inline void
LogSeqGroupInit(void)
{
    ADCSequenceConfigure(ADC0_BASE, LOG_SEQ_SEQUENCE_NUM, ADC_TRIGGER_TIMER, LOG_SEQ_PRIORITY);
    ADCSequenceStepConfigure(ADC0_BASE, LOG_SEQ_SEQUENCE_NUM, 0, ADC0_CHANNEL0);
    ADCSequenceStepConfigure(ADC0_BASE, LOG_SEQ_SEQUENCE_NUM, 1, ADC0_CHANNEL1 | ADC_CTL_IE | ADC_CTL_END);

    ADCSequenceConfigure(ADC1_BASE, LOG_SEQ_SEQUENCE_NUM, ADC_TRIGGER_TIMER, LOG_SEQ_PRIORITY);
    ADCSequenceStepConfigure(ADC1_BASE, LOG_SEQ_SEQUENCE_NUM, 0, ADC1_CHANNEL0);
    ADCSequenceStepConfigure(ADC1_BASE, LOG_SEQ_SEQUENCE_NUM, 1, ADC1_CHANNEL1 | ADC_CTL_IE | ADC_CTL_END);
}


#endif /* LOG_SEQ_H_ */
