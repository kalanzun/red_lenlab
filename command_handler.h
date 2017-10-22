/*
 * command_handler.h
 *
 *  Created on: 22.10.2017
 *      Author: Christoph
 */

#ifndef COMMAND_HANDLER_H_
#define COMMAND_HANDLER_H_


#include "packet_queue.h"


typedef struct {
    tPacketQueue command_queue;
} tCommandHandler;


void CommandHandlerInit(tCommandHandler *self);

void CommandHandlerMain(tCommandHandler *self);


#endif /* COMMAND_HANDLER_H_ */
