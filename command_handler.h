/*
 * command_handler.h
 *
 *  Created on: 22.10.2017
 *      Author: Christoph
 */

#ifndef COMMAND_HANDLER_H_
#define COMMAND_HANDLER_H_

// receives USB packets and parses the commands


#include "event_queue.h"


typedef struct {
    tQueue command_queue;
} tCommandHandler;


// Singleton
extern tCommandHandler command_handler;


void CommandHandlerInit(void);

void CommandHandlerMain(void);


#endif /* COMMAND_HANDLER_H_ */
