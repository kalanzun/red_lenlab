/*
 * log_seq.h
 *
 */

#ifndef LOG_SEQ_H_
#define LOG_SEQ_H_


#include "adc.h"


typedef struct LogSeq {

    tADC *adc;

    uint32_t sequence_num;
    uint32_t sequence_int;
    uint32_t priority;

    volatile uint32_t count;

} tLogSeq;


typedef struct LogSeqGroup {

    tLogSeq log_seq[ADC_GROUP_SIZE];

    tADCGroup *adc_group;

} tLogSeqGroup;


inline void
LogSeqIntHandler(tLogSeq *self)
{
    ADCIntClear(self->adc->base, self->sequence_num);

    self->count = self->count + 1;
}


inline void
LogSeqEnable(tLogSeq *self)
{
    ADCSequenceEnable(self->adc->base, self->sequence_num);
    ADCIntEnable(self->adc->base, self->sequence_num); // Enable to generate direct ADC Interrupts, do not enable for DMA
    IntEnable(self->sequence_int); // Enable Interrupt in NVIC
}


inline void
LogSeqGroupEnable(tLogSeqGroup *self, uint16_t interval)
{
    // interval in ms

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
