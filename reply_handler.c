/*
 * reply_handler.c
 *
 *  Created on: 22.10.2017
 *      Author: Christoph
 */

#include <stdbool.h>
#include <stdint.h>
#include "debug.h"
#include "reply_handler.h"


void
ReplyHandlerInit(tReplyHandler *self)
{
    PacketQueueInit(&self->reply_queue);
}


void
ReplyHandlerMain(tReplyHandler *self)
{
}



