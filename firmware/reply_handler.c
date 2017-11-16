/*
 * reply_handler.c
 *
 *  Created on: 27.10.2017
 *      Author: Christoph
 */


#include "reply_handler.h"


tReplyHandler reply_handler;


void
ReplyHandlerMain(void)
{

}


void
ReplyHandlerInit(void)
{
    QueueInit(&reply_handler.reply_queue);
}



