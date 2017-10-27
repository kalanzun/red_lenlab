/*
 * reply_handler.h
 *
 *  Created on: 27.10.2017
 *      Author: Christoph
 */

#ifndef REPLY_HANDLER_H_
#define REPLY_HANDLER_H_

// sends single USB packets


#include "event_queue.h"


typedef struct ReplyHandler {
    tQueue reply_queue;
} tReplyHandler;


// Singleton
extern tReplyHandler reply_handler;


void ReplyHandlerInit(void);

void ReplyHandlerMain(void);


#endif /* REPLY_HANDLER_H_ */
