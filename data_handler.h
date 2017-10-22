/*
 * data_handler.h
 *
 *  Created on: 22.10.2017
 *      Author: Christoph
 */

#ifndef DATA_HANDLER_H_
#define DATA_HANDLER_H_

// handles sending of USB packets directly or via uDMA.


#include "packet_queue.h"


typedef struct {
    tPacketQueue command_queue;
} tReplyHandler;



void CommandHandlerInit(tCommandHandler *self);

void CommandHandlerMain(tCommandHandler *self);



void DataHandlerSendMessage(const char* text);

#endif /* DATA_HANDLER_H_ */
