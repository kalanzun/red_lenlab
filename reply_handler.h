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


#define REPLY_TYPE_BOOL 1
#define REPLY_TYPE_UINT8 2
#define REPLY_TYPE_UINT16 3
#define REPLY_TYPE_UINT32 4
#define REPLY_TYPE_STRING 5


typedef struct ReplyHandler {
    tQueue reply_queue;
} tReplyHandler;


// Singleton
extern tReplyHandler reply_handler;


void ReplyHandlerInit(void);

void ReplyHandlerMain(void);


#endif /* REPLY_HANDLER_H_ */
