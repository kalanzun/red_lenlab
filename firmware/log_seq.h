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


#include "adc.h"


typedef struct LogSeq {

    tADC *adc;

    uint32_t sequence_num;
    uint32_t sequence_int;
    uint32_t priority;

    volatile bool ready;
    volatile bool error;
    volatile uint32_t count;

} tLogSeq;


typedef struct LogSeqGroup {

    tLogSeq log_seq[ADC_GROUP_SIZE];

} tLogSeqGroup;


inline void
LogSeqIntHandler(tLogSeq *self)
{
    ADCIntClear(self->adc->base, self->sequence_num);

    if (self->ready) self->error = 1;
    else self->ready = 1;
    self->count = self->count + 1;
}


inline bool
LogSeqGroupReady(tLogSeqGroup *self)
{
    unsigned int i;
    bool ready = 1;

    FOREACH_ADC ready &= self->log_seq[i].ready;

    return ready;
}


inline bool
LogSeqGroupError(tLogSeqGroup *self)
{
    unsigned int i;

    bool error = 0;
    FOREACH_ADC error |= self->log_seq[i].error;
    return error;
}


inline void
LogSeqGroupRelease(tLogSeqGroup *self)
{
    unsigned int i;

    FOREACH_ADC self->log_seq[i].ready = 0;
}


inline int32_t
LogSeqGroupDataGet(tLogSeqGroup *self, uint32_t *buffer)
{
    unsigned int i;
    int32_t count = 0;

    FOREACH_ADC count += ADCSequenceDataGet(self->log_seq[i].adc->base, self->log_seq[i].sequence_num, buffer + count);

    return count;
}


inline void
LogSeqEnable(tLogSeq *self)
{
    self->ready = 0;
    self->error = 0;
    self->count = 0;

    ADCSequenceEnable(self->adc->base, self->sequence_num);
    ADCIntEnable(self->adc->base, self->sequence_num); // Enable to generate direct ADC Interrupts, do not enable for DMA
    IntEnable(self->sequence_int); // Enable Interrupt in NVIC
}


inline void
LogSeqGroupEnable(tLogSeqGroup *self, uint32_t interval)
{
    // interval in ms

    int i;
    uint32_t buffer[8]; // max FIFO length of SS1

    // clear data FIFO
    LogSeqGroupDataGet(self, &buffer);

    FOREACH_ADC LogSeqEnable(&self->log_seq[i]);

    ADCTimerStart(&adc_group.timer, interval * 1000); // us
}


inline void
LogSeqDisable(tLogSeq *self)
{
    IntDisable(self->sequence_int);
    ADCIntDisable(self->adc->base, self->sequence_num);
    ADCSequenceDisable(self->adc->base, self->sequence_num);
}


inline void
LogSeqGroupDisable(tLogSeqGroup *self)
{
    int i;

    ADCTimerStop(&adc_group.timer);

    FOREACH_ADC LogSeqDisable(&self->log_seq[i]);
}


inline void
ConfigureLogSeq(tLogSeq *self, tADC *adc, uint32_t sequence_int)
{
    self->adc = adc;

    self->sequence_num = 1; // shall be different from osc_seq
    self->sequence_int = sequence_int;
    self->priority = 1; // shall be different from osc_seq

    ADCSequenceConfigure(self->adc->base, self->sequence_num, ADC_TRIGGER_TIMER, self->priority);
    //ADCSequenceStepConfigure(self->adc->base, self->sequence_num, 0, self->adc->channel | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceStepConfigure(self->adc->base, self->sequence_num, 0, self->adc->channel);
    ADCSequenceStepConfigure(self->adc->base, self->sequence_num, 1, self->adc->channel1 | ADC_CTL_IE | ADC_CTL_END);
}


inline void
LogSeqGroupInit(tLogSeqGroup *self)
{
    ConfigureLogSeq(&self->log_seq[0], &adc_group.adc[0], INT_ADC0SS1);
    ConfigureLogSeq(&self->log_seq[1], &adc_group.adc[1], INT_ADC1SS1);
}


#endif /* LOG_SEQ_H_ */
