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

    if (self->adc_group->lock) return STATE_ERROR;

    ADCGroupSetHardwareOversample(self->adc_group, 1);

    LogSeqGroupEnable(&self->seq_group, interval);

    ADCGroupLock(self->adc_group);

    self->lock = true;

    return OK;
}


tError
LoggerStop(tLogger *self)
{
    if (!self->lock) return LOCK_ERROR;

    LogSeqGroupDisable(&self->seq_group);

    self->lock = false;

    ADCGroupUnlock(self->adc_group);

    return OK;
}


void
LoggerInit(tLogger *self, tADCGroup *adc_group)
{
    self->adc_group = adc_group;

    LogSeqGroupInit(&self->seq_group, adc_group);
}
