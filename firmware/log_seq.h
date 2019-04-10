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

    ADCSequenceEnable(self->adc->base, self->sequence_num);
    ADCIntEnable(self->adc->base, self->sequence_num); // Enable to generate direct ADC Interrupts, do not enable for DMA
    IntEnable(self->sequence_int); // Enable Interrupt in NVIC
}


inline void
LogSeqGroupEnable(tLogSeqGroup *self, uint32_t interval)
{
    // interval in ms

    int i;
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

    self->sequence_num = 3; // shall be different from osc_seq
    self->sequence_int = sequence_int;
    self->priority = 1; // shall be different from osc_seq

    ADCSequenceConfigure(self->adc->base, self->sequence_num, ADC_TRIGGER_TIMER, self->priority);
    ADCSequenceStepConfigure(self->adc->base, self->sequence_num, 0, self->adc->channel | ADC_CTL_IE | ADC_CTL_END);
}


inline void
LogSeqGroupInit(tLogSeqGroup *self)
{
    ConfigureLogSeq(&self->log_seq[0], &adc_group.adc[0], INT_ADC0SS3);
    ConfigureLogSeq(&self->log_seq[1], &adc_group.adc[1], INT_ADC1SS3);
}


#endif /* LOG_SEQ_H_ */
