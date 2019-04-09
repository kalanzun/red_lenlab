/*
 * logger.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "logger.h"


void
ADC0SS3Handler(void)
{
    tLogSeq *self = &logger.seq_group.log_seq[0];

    LogSeqIntHandler(self);
}


void
ADC1SS3Handler(void)
{
    tLogSeq *self = &logger.seq_group.log_seq[1];

    LogSeqIntHandler(self);
}


tError
LoggerStart(tLogger *self, uint32_t interval)
{
    if (self->lock) return LOCK_ERROR;

    if (adc_group.lock) return ADC_ERROR;

    ADCGroupSetHardwareOversample(&adc_group, 1);

    LogSeqGroupEnable(&self->seq_group, interval);

    ADCGroupLock(&adc_group);

    self->lock = true;

    return OK;
}


tError
LoggerStop(tLogger *self)
{
    if (!self->lock) return LOCK_ERROR;

    LogSeqGroupDisable(&self->seq_group);

    self->lock = false;

    ADCGroupUnlock(&adc_group);

    return OK;
}


void
LoggerInit(tLogger *self)
{
    LogSeqGroupInit(&self->seq_group);
}
