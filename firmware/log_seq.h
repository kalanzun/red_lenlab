/*
 * log_seq.h
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

#ifndef LOG_SEQ_H_
#define LOG_SEQ_H_


#include "adc.h"


typedef struct LogSeq {

    tADC *adc;

    uint32_t sequence_num;
    uint32_t sequence_int;
    uint32_t priority;

    unsigned char ready;
    unsigned char error;

} tLogSeq;


typedef struct LogSeqGroup {

    tLogSeq log_seq[ADC_GROUP_SIZE];

    tADCGroup *adc_group;

} tLogSeqGroup;


inline void
LogSeqIntHandler(tLogSeq *self)
{
    ADCIntClear(self->adc->base, self->sequence_num);

    if (self->ready) self->error = 1;
    else self->ready = 1;
}


inline unsigned char
LogSeqGroupReady(tLogSeqGroup *self)
{
    int i;

    unsigned char ready = 0;
    FOREACH_ADC ready &= self->log_seq[i].ready;
    return ready;
}


inline unsigned char
LogSeqGroupError(tLogSeqGroup *self)
{
    int i;

    unsigned char error = 0;
    FOREACH_ADC error |= self->log_seq[i].error;
    return error;
}


inline void
LogSeqGroupRelease(tLogSeqGroup *self)
{
    int i;

    FOREACH_ADC self->log_seq[i].ready = 0;
}


inline int32_t
LogSeqGroupDataGet(tLogSeqGroup *self, uint32_t *buffer)
{
    int i;
    int32_t count = 0;

    FOREACH_ADC count += ADCSequenceDataGet(self->log_seq[i].adc->base, self->log_seq[i].sequence_num, buffer + count);

    return count;
}


inline void
LogSeqEnable(tLogSeq *self)
{
    self->ready = 0;
    self->error = 0;

    ADCSequenceEnable(self->adc->base, self->sequence_num);
    ADCIntEnable(self->adc->base, self->sequence_num); // Enable to generate direct ADC Interrupts, do not enable for DMA
    IntEnable(self->sequence_int); // Enable Interrupt in NVIC
}


inline void
LogSeqGroupEnable(tLogSeqGroup *self, uint32_t interval)
{
    int i;
    FOREACH_ADC LogSeqEnable(&self->log_seq[i]);

    ADCTimerStart(&self->adc_group->timer, interval);
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

    ADCTimerStop(&self->adc_group->timer);

    FOREACH_ADC LogSeqDisable(&self->log_seq[i]);
}


inline void
ConfigureLogSeq(tLogSeq *self, tADC *adc, uint32_t sequence_int)
{
    self->adc = adc;

    self->sequence_num = 3; // shall be different from osc_seq
    self->sequence_int = sequence_int;
    self->priority = 1; // shall be different from osc_seq

    ADCSequenceConfigure(self->adc->base, self->sequence_num, ADC_TRIGGER_TIMER, self->priority);
    ADCSequenceStepConfigure(self->adc->base, self->sequence_num, 0, self->adc->channel | ADC_CTL_IE | ADC_CTL_END);
}


inline void
LogSeqGroupInit(tLogSeqGroup *self, tADCGroup *adc_group)
{
    ConfigureLogSeq(&self->log_seq[0], &adc_group->adc[0], INT_ADC0SS3);
    ConfigureLogSeq(&self->log_seq[1], &adc_group->adc[1], INT_ADC1SS3);

    self->adc_group = adc_group;
}


#endif /* LOG_SEQ_H_ */
