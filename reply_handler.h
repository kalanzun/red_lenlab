/*
 * reply_handler.h
 *
 *  Created on: 22.10.2017
 *      Author: Christoph
 */

#ifndef REPLY_HANDLER_H_
#define REPLY_HANDLER_H_


#include "packet_queue.h"


typedef struct {
    tPacketQueue reply_queue;
} tReplyHandler;



void ReplyHandlerInit(tReplyHandler *self);

void ReplyHandlerMain(tReplyHandler *self);


#endif /* REPLY_HANDLER_H_ */
