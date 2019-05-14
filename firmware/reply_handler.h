/*
 * reply_handler.h
 *
 */

#ifndef REPLY_HANDLER_H_
#define REPLY_HANDLER_H_

// sends single USB packets


#include "event_queue.h"


typedef struct ReplyHandler {
    tQueue reply_queue;
} tReplyHandler;


extern tReplyHandler reply_handler;


inline void
ReplyHandlerInit(tReplyHandler *self)
{
    QueueInit(&self->reply_queue);
}


#endif /* REPLY_HANDLER_H_ */
