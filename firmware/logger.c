/*
 * logger.c
 *
 *  Created on: 24.11.2018
 *      Author: Christoph
 */

#include "logger.h"

#include "reply_handler.h"


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

    LogSeqGroupEnable(&self->seq_group, interval);

    self->lock = 1;

    return OK;
}


tError
LoggerStop(tLogger *self)
{
    if (!self->lock) return LOCK_ERROR;

    LogSeqGroupDisable(&self->seq_group);

    self->lock = 0;

    return OK;
}


void
LoggerMain(tLogger *self)
{
    uint32_t size;
    tEvent *reply;

    if (!self->lock) return;

    if (LogSeqGroupError(&self->seq_group)) {

        ASSERT(0); // TODO Logger error handling in release build

        LoggerStop(self);

    }

    else if (LogSeqGroupReady(&self->seq_group)) {

        reply = QueueAcquire(&reply_handler.reply_queue);

        EventSetReply(reply, LoggerData);
        EventSetType(reply, IntArray);

        size = sizeof(uint32_t) * ADC_GROUP_SIZE;
        ASSERT(size <= LENLAB_PACKET_BODY_LENGTH);
        EventSetBodyLength(reply, size);

        size = LogSeqGroupDataGet(&self->seq_group, (uint32_t *) (EventGetBody(reply)));
        ASSERT(size == ADC_GROUP_SIZE);

        QueueWrite(&reply_handler.reply_queue);

        LogSeqGroupRelease(&self->seq_group);

    }
}


void
LoggerInit(tLogger *self, tADCGroup *adc_group)
{
    self->adc_group = adc_group;

    LogSeqGroupInit(&self->seq_group, adc_group);

    self->lock = 0;
}
